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

#define VERSION "2.5.1.13"

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
#include "zr/zradmin"
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
    CreateConVar("zombie_version", VERSION, "Zombie:Reloaded Version", FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY);
    CreateConVar("zombie_enabled", "1", "Not synced with zr_enable", FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY);
    
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
    maxclients = GetMaxClients();
    MapChangeCleanup();
    
    LoadModelData();
    LoadDownloadData();
    
    /* Reset to default class if class selection saving is disabled. */
    if (!GetConVarBool(gCvars[CVAR_CLASSES_SAVE]))
    {
        new i;
        for (i = 1; i <= MAXPLAYERS; i++)
        {
            pClass[i] = GetDefaultClassIndex();
        }
    }
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
    pClass[client] = GetDefaultClassIndex();
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
    
    RefreshList();
}

public OnClientDisconnect(client)
{
    ClientUnHookUse(client);
    ClientUnHookAttack(client);
    
    PlayerLeft(client);
    
    new debug_val = GetConVarInt(gCvars[CVAR_DEBUG]);
    new String:debug_msg[64];
    
    for (new x = 0; x < MAXTIMERS; x++)
    {
        if (tHandles[client][x] != INVALID_HANDLE)
        {
            if (debug_val > 1)
            {
                Format(debug_msg, sizeof(debug_msg), "OnClientDisconnect - Killing timer %i with handle %x.", x, tHandles[client][x]);
                ZR_DebugPrintToConsole(0, debug_msg);
            }
            KillTimer(tHandles[client][x]);
            tHandles[client][x] = INVALID_HANDLE;
        }
    }
    
    RefreshList();
}

MapChangeCleanup()
{
    ClearArray(restrictedWeapons);
    
    tRound = INVALID_HANDLE;
    tInfect = INVALID_HANDLE;
    tAmbience = INVALID_HANDLE;
    
    for (new client = 1; client <= maxclients; client++)
    {
        for (new x = 0; x < MAXTIMERS; x++)
        {
            if (tHandles[client][x] != INVALID_HANDLE)
            {
                tHandles[client][x] = INVALID_HANDLE;
            }
        }
    }
}

ZREnd()
{
    TerminateRound(3.0, Game_Commencing);
        
    UnhookCvars();
    UnhookEvents();
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientConnected(x) || !IsClientInGame(x))
        {
            continue;
        }
        
        for (new y = 0; y < MAXTIMERS; y++)
        {
            if (tHandles[x][y] != INVALID_HANDLE)
            {
                KillTimer(tHandles[x][y]);
                tHandles[x][y] = INVALID_HANDLE;
            }
        }
    }
}