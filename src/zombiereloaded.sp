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

#define VERSION "3.0-dev"

#include "zr/zombiereloaded"
#include "zr/global"
#include "zr/cvars"
#include "zr/translation"
#include "zr/offsets"
#include "zr/ambience"
#include "zr/models"
#include "zr/overlays"
#include "zr/playerclasses/playerclasses"
#include "zr/antistick"
#include "zr/anticamp"
#include "zr/teleport"
#include "zr/zombie"
#include "zr/menu"
#include "zr/sayhooks"

// Weapons
#include "zr/weapons/weapons"

// Hitgroups
#include "zr/hitgroups"

// Knockback
#include "zr/knockback"

// Spawn protect
#include "zr/spawnprotect"

// ZHP
#include "zr/zhp"

#include "zr/zadmin"
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
    
    // Weapons
    WeaponsInit();
    
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
    
    // Forward event to modules.
    ClassLoad();
    WeaponsLoad();
    HitgroupsLoad();
    Anticamp_Startup();
}

public OnMapEnd()
{
    Anticamp_Disable();
}

public OnConfigsExecuted()
{
    decl String:mapconfig[PLATFORM_MAX_PATH];
    
    GetCurrentMap(mapconfig, sizeof(mapconfig));
    Format(mapconfig, sizeof(mapconfig), "sourcemod/zombiereloaded/%s.cfg", mapconfig);
    
    decl String:path[PLATFORM_MAX_PATH];
    Format(path, sizeof(path), "cfg/%s", mapconfig);
    
    if (FileExists(path))
    {
        ServerCommand("exec %s", mapconfig);
        
        if (LogFlagCheck(LOG_CORE_EVENTS))
        {
            LogMessage("Executed map config file: %s", mapconfig);
        }
    }
    
    FindMapSky();
    LoadAmbienceData();
}

public OnClientPutInServer(client)
{
    gBlockMotherInfect[client] = false;
    gKilledByWorld[client] = false; 
    
    bZVision[client] = !IsFakeClient(client);
    
    // Forward event to modules.
    ClassClientInit(client);
    WeaponsClientInit(client);
    SpawnProtectClientInit(client);
    ZHPClientInit(client);
    
    if (!IsFakeClient(client)) AmbienceStart(client);
    
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
    ClientUnHookAttack(client);
    
    PlayerLeft(client);
    
    // Forward event to modules.
    ClassOnClientDisconnect(client);
    WeaponsOnClientDisconnect(client);
    ZTeleResetClient(client);
    AmbienceStop(client);
    
    for (new x = 0; x < MAXTIMERS; x++)
    {
        if (tHandles[client][x] != INVALID_HANDLE)
        {
            KillTimer(tHandles[client][x]);
            tHandles[client][x] = INVALID_HANDLE;
        }
    }
    
    RefreshList();
}

MapChangeCleanup()
{
    tRound = INVALID_HANDLE;
    tInfect = INVALID_HANDLE;
    AmbienceStopAll();
    AntiStickReset();
    
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
    
    // TODO: Disable all modules! Teleport, ambience, overlays, antistick, etc.
    
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