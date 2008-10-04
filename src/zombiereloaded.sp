/**
 * ====================
 *   Zombie:Reloaded
 *   File: zombiereloaded.sp
 *   Author: Greyscale
 * ==================== 
 */

#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <hacks>

#undef REQUIRE_PLUGIN
#include <market>

#define VERSION "2.5.1"

#include "zr/zombiereloaded"
#include "zr/global"
#include "zr/cvars"
#include "zr/translation"
#include "zr/offsets"
#include "zr/ambience"
#include "zr/classes"
#include "zr/models"
#include "zr/overlays"
#include "zr/zombie"
#include "zr/menu"
#include "zr/sayhooks"
#include "zr/weaponrestrict"
#include "zr/damagecontrol"
#include "zr/commands"
#include "zr/event"

public Plugin:myinfo =
{
    name = "Zombie:Reloaded", 
    author = "Greyscale", 
    description = "Infection/survival style gameplay", 
    version = VERSION, 
    url = ""
};

public bool:AskPluginLoad(Handle:myself, bool:late, String:error[], err_max)
{
    CreateGlobals();
    
    return true;
}

public OnPluginStart()
{
    LoadTranslations("common.phrases.txt");
    LoadTranslations("zombiereloaded.phrases.txt");
    
    // ======================================================================
    
    ZR_PrintToServer("Plugin loading");
    
    // ======================================================================
    
    HookEvents();
    HookChatCmds();
    CreateCvars();
    HookCvars();
    CreateCommands();
    HookCommands();
    FindOffsets();
    SetupGameData();
    InitWeaponRestrict();
    InitDmgControl();
    
    // ======================================================================
    
    market = LibraryExists("market");
    
    // ======================================================================
    
    CreateConVar("gs_zombiereloaded_version", VERSION, "[ZR] Current version of this plugin", FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY);
    
    // ======================================================================
    
    ZR_PrintToServer("Plugin loaded");
}

public OnPluginEnd()
{
    ZREnd();
}

public OnLibraryRemoved(const String:name[])
{
	if (StrEqual(name, "market"))
	{
		market = false;
	}
}
 
public OnLibraryAdded(const String:name[])
{
	if (StrEqual(name, "market"))
	{
		market = true;
	}
}

public OnMapStart()
{
    MapChangeCleanup();
    
    LoadModelData();
    LoadDownloadData();
}

public OnConfigsExecuted()
{
    FindMapSky();
    
    LoadClassData();
    LoadAmbienceData();
    
    decl String:mapconfig[PLATFORM_MAX_PATH];
    
    GetCurrentMap(mapconfig, sizeof(mapconfig));
    Format(mapconfig, sizeof(mapconfig), "sourcemod/zombiereloaded/%s.cfg", mapconfig);
    
    decl String:path[PLATFORM_MAX_PATH];
    Format(path, sizeof(path), "cfg/%s", mapconfig);
    
    if (FileExists(path))
    {
        ServerCommand("exec %s", mapconfig);
    }
}

public OnClientPutInServer(client)
{
    pClass[client] = 0;
    gBlockMotherInfect[client] = false;
    
    bZVision[client] = !IsFakeClient(client);
    
    new bool:zhp = GetConVarBool(gCvars[CVAR_ZHP_DEFAULT]);
    dispHP[client] = zhp;
    
    ClientHookUse(client);
    ClientHookAttack(client);
    
    FindClientDXLevel(client);
    
    for (new x = 0; x < MAXTIMERS; x++)
    {
        tHandles[client][x] = INVALID_HANDLE;
    }
}

public OnClientDisconnect(client)
{
    ClientUnHookUse(client);
    ClientUnHookAttack(client);
    
    PlayerLeft(client);
    
    for (new x = 0; x < MAXTIMERS; x++)
    {
        if (tHandles[client][x] != INVALID_HANDLE)
        {
            CloseHandle(tHandles[client][x]);
            tHandles[client][x] = INVALID_HANDLE;
        }
    }
}

MapChangeCleanup()
{
    ClearArray(restrictedWeapons);
    
    tRound = INVALID_HANDLE;
    tInfect = INVALID_HANDLE;
    tAmbience = INVALID_HANDLE;
}

ZREnd()
{
    TerminateRound(3.0, Game_Commencing);
        
    UnhookCvars();
    UnhookEvents();
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        for (new y = 0; y < MAXTIMERS; y++)
        {
            if (tHandles[x][y] != INVALID_HANDLE)
            {
                CloseHandle(tHandles[x][y]);
                tHandles[x][y] = INVALID_HANDLE;
            }
        }
    }
}