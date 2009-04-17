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

// Core include.
#include "zr/zombiereloaded"

// External api (not done)
//#include "zr/global"

// Cvars (core)
#include "zr/cvars"

// Translations (core)
#include "zr/translation"

// Offsets (core)
#include "zr/offsets"

// Models (core)
#include "zr/models"

// Round end (core)
#include "zr/roundend"

// Class system (module)
#include "zr/playerclasses/playerclasses"

#include "zr/anticamp"
#include "zr/teleport"
#include "zr/zombie"
#include "zr/menu"
#include "zr/sayhooks"

// Weapons (module)
#include "zr/weapons/weapons"

// Sound effects (module)
#include "zr/soundeffects/soundeffects"

// Antistick (module)
#include "zr/antistick"

// Hitgroups (module)
#include "zr/hitgroups"

// Knockback (module)
#include "zr/knockback"

// Spawn protect (module)
#include "zr/spawnprotect"

// Respawn (module)
#include "zr/respawn"

// Napalm (module)
#include "zr/napalm"

// ZHP (module)
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
    // Todo: External API
    //CreateGlobals();
    
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
    
    g_bMarket = LibraryExists("market");
    
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
		g_bMarket = false;
	}
}
 
public OnLibraryAdded(const String:name[])
{
	if (StrEqual(name, "market"))
	{
		g_bMarket = true;
	}
}

public OnMapStart()
{
    MapChangeCleanup();
    
    LoadModelData();
    LoadDownloadData();
    
    // Forward event to modules.
    RoundEndOnMapStart();
    ClassLoad();
    WeaponsLoad();
    SEffectsOnMapStart();
    HitgroupsLoad();
    Anticamp_Startup();
}

public OnMapEnd()
{
    // Forward event to modules.
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
    
    // Forward event to modules.
    SEffectsLoad();
}

public OnClientPutInServer(client)
{
    bMotherInfectImmune[client] = false;
    
    // Forward event to modules.
    RoundEndGetClientDXLevel(client);
    ClassClientInit(client);
    SEffectsClientInit(client);
    WeaponsClientInit(client);
    SpawnProtectClientInit(client);
    RespawnClientInit(client);
    ZHPClientInit(client);
    
    ClientHookAttack(client);
    
    for (new x = 0; x < MAXTIMERS; x++)
    {
        tHandles[client][x] = INVALID_HANDLE;
    }
}

public OnClientDisconnect(client)
{
    ClientUnHookAttack(client);
    
    PlayerLeft(client);
    
    // Forward event to modules.
    ClassOnClientDisconnect(client);
    WeaponsOnClientDisconnect(client);
    ZTeleResetClient(client);
    
    for (new x = 0; x < MAXTIMERS; x++)
    {
        if (tHandles[client][x] != INVALID_HANDLE)
        {
            KillTimer(tHandles[client][x]);
            tHandles[client][x] = INVALID_HANDLE;
        }
    }
}

MapChangeCleanup()
{
    tInfect = INVALID_HANDLE;
    AntiStickReset();
    
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        for (new y = 0; y < MAXTIMERS; y++)
        {
            if (tHandles[x][y] != INVALID_HANDLE)
            {
                tHandles[x][y] = INVALID_HANDLE;
            }
        }
    }
}

/*ZREnd()
{
    TerminateRound(3.0, Game_Commencing);
        
    UnhookCvars();
    UnhookEvents();
    
    // TODO: Disable all modules! Teleport, ambience, overlays, antistick, etc.
    
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
                KillTimer(tHandles[x][y]);
                tHandles[x][y] = INVALID_HANDLE;
            }
        }
    }
}*/