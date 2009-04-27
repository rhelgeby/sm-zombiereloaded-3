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

// Core include
#include "zr/zombiereloaded"

// External api (not done)
//#include "zr/global"

// Log (core)
#include "zr/log"

// Cvars (core)
#include "zr/cvars"

// Translations (core)
#include "zr/translation"

// Offsets (core)
#include "zr/offsets"

// Models (core)
#include "zr/models"

// Class System (core)
#include "zr/playerclasses/playerclasses"

// Weapons (core)
#include "zr/weapons/weapons"

// Hitgroups (core)
#include "zr/hitgroups"

// Round End (core)
#include "zr/roundend"

// Infect (core)
#include "zr/infect"

// Damage (core)
#include "zr/damage"

// Account (module)
#include "zr/account"

// Visual Effects (module)
#include "zr/visualeffects"

// Sound Effects (module)
#include "zr/soundeffects/soundeffects"

// Antistick (module)
#include "zr/antistick"

// Knockback (module)
#include "zr/knockback"

// Spawn Protect (module)
#include "zr/spawnprotect"

// Respawn (module)
#include "zr/respawn"

// Napalm (module)
#include "zr/napalm"

// ZHP (module)
#include "zr/zhp"

#include "zr/anticamp"
#include "zr/teleport"
#include "zr/zombie"
#include "zr/menu"
#include "zr/sayhooks"
#include "zr/zadmin"
#include "zr/commands"

// Event
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
    
    // Log
    LogInit();
    
    // Cvars
    CvarsInit();
    
    // TODO: Be modulized/recoded.
    HookChatCmds();
    CreateCommands();
    HookCommands();
    FindOffsets();
    SetupGameData();
    
    // Weapons
    WeaponsInit();
    
    // Damage
    DamageInit();
    
    // Event
    EventInit();
    
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
    LoadModelData();
    LoadDownloadData();
    
    // Forward event to modules.
    RoundEndOnMapStart();
    InfectOnMapStart();
    SEffectsOnMapStart();
    AntiStickOnMapStart();
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
        
        if (LogCheckFlag(LOG_CORE_EVENTS))
        {
            LogMessageFormatted(-1, "", "", "Executed map config file: %s.", LOG_FORMAT_TYPE_SIMPLE, mapconfig);
        }
    }
    
    // Forward event to modules.
    WeaponsLoad();
    HitgroupsLoad();
    InfectLoad();
    VEffectsLoad();
    SEffectsLoad();
    ClassLoad();
}

public OnClientPutInServer(client)
{
    // Forward event to modules.
    ClassClientInit(client);
    WeaponsClientInit(client);
    RoundEndClientInit(client);
    InfectClientInit(client);
    DamageClientInit(client);
    SEffectsClientInit(client);
    SpawnProtectClientInit(client);
    RespawnClientInit(client);
    ZHPClientInit(client);
}

public OnClientDisconnect(client)
{
    // Forward event to modules.
    ClassOnClientDisconnect(client);
    WeaponsOnClientDisconnect(client);
    InfectOnClientDisconnect(client);
    DamageOnClientDisconnect(client);
    ZTeleResetClient(client);
}