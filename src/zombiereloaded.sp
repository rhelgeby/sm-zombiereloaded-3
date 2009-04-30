/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        (base) zombiereloaded.sp
 *   Description: Plugins base file.
 *
 * ============================================================================
 */

#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <hacks>

#undef REQUIRE_PLUGIN
#include <market>

#define VERSION "3.0-dev"

// Core includes.
#include "zr/zombiereloaded"
#include "zr/log"
#include "zr/cvars"
#include "zr/config"
#include "zr/translation"
#include "zr/tools"
#include "zr/models"
#include "zr/playerclasses/playerclasses"
#include "zr/weapons/weapons"
#include "zr/hitgroups"
#include "zr/roundend"
#include "zr/infect"
#include "zr/damage"
#include "zr/menu"
#include "zr/sayhooks"
#include "zr/event"
#include "zr/zadmin"
#include "zr/commands"
//#include "zr/global"

// Modules
#include "zr/account"
#include "zr/visualeffects"
#include "zr/soundeffects/soundeffects"
#include "zr/antistick"
#include "zr/knockback"
#include "zr/spawnprotect"
#include "zr/respawn"
#include "zr/napalm"
#include "zr/zspawn"
#include "zr/zhp"
#include "zr/jumpboost"
#include "zr/anticamp"
#include "zr/teleport"

// Almost replaced! :)
#include "zr/zombie"

/**
 * Tell SM ZR's info.
 */
public Plugin:myinfo =
{
    name = "Zombie:Reloaded",
    author = "Greyscale, Rhelgeby (Richard)",
    description = "Infection/survival style gameplay",
    version = VERSION,
    url = ""
};

/**
 * Called before plugin is loaded.
 * 
 * @param myself    The plugin handle.
 * @param late      True if the plugin was loaded after map change, false on map start.
 * @param error     Error message if load failed.
 * @param err_max   Max length of the error message.
 */
public bool:AskPluginLoad(Handle:myself, bool:late, String:error[], err_max)
{
    // TODO: EXTERNAL API
    
    // Let plugin load.
    return true;
}

/**
 * Plugin is loading.
 */
public OnPluginStart()
{
    // Load translations phrases used by plugin.
    LoadTranslations("common.phrases.txt");
    LoadTranslations("zombiereloaded.phrases.txt");
    
    // Start loading ZR init functions.
    ZR_PrintToServer("Plugin loading");
    
    // Log
    LogInit();
    
    // Cvars
    CvarsInit();
    
    // Tools
    ToolsInit();
    
    // TODO: Be modulized/recoded.
    CreateCommands();
    HookCommands();
    
    // Weapons
    WeaponsInit();
    
    // Damage
    DamageInit();
    
    // Say Hooks
    SayHooksInit();
    
    // Event
    EventInit();
    
    // Set market variable to true if market is installed.
    g_bMarket = LibraryExists("market");
    
    // Create public cvar for tracking.
    CreateConVar("gs_zombiereloaded_version", VERSION, "[ZR] Current version of this plugin", FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY);
    
    // Finish loading ZR init functions.
    ZR_PrintToServer("Plugin loaded");
}

/**
 * Library is being removed.
 * 
 * @param name  The name of the library.
 */
public OnLibraryRemoved(const String:name[])
{
    // If market is being removed, then set variable to false.
    if (StrEqual(name, "market", false))
    {
        g_bMarket = false;
    }
}

/**
 * Library is being added.
 * 
 * @param name  The name of the library.
 */
public OnLibraryAdded(const String:name[])
{
    // If market is being added, then set variable to true.
    if (StrEqual(name, "market", false))
    {
        g_bMarket = true;
    }
}

/**
 * The map is starting.
 */
public OnMapStart()
{
    // Forward event to modules.
    RoundEndOnMapStart();
    InfectOnMapStart();
    SEffectsOnMapStart();
    AntiStickOnMapStart();
    Anticamp_Startup();
}

/**
 * The map is ending.
 */
public OnMapEnd()
{
    // Forward event to modules.
    Anticamp_Disable();
}

/**
 * Configs just finished getting executed.
 */
public OnConfigsExecuted()
{
    // Forward event to modules.
    ConfigLoad();
    ModelsLoad();
    WeaponsLoad();
    HitgroupsLoad();
    InfectLoad();
    VEffectsLoad();
    SEffectsLoad();
    ClassLoad();
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
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

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
public OnClientDisconnect(client)
{
    // Forward event to modules.
    ClassOnClientDisconnect(client);
    WeaponsOnClientDisconnect(client);
    InfectOnClientDisconnect(client);
    DamageOnClientDisconnect(client);
    ZSpawnOnClientDisconnect(client);
    ZTeleResetClient(client);
}