/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          zombiereloaded.sp
 *   Type:          Base
 *   Description:   Plugin's base file.
 *
 * ============================================================================
 */

#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <cstrike>
#include <zrtools>

#undef REQUIRE_PLUGIN
#include <market>

#define VERSION "3.0-dev"

// Core includes.
#include "zr/zombiereloaded"
#include "zr/translation"
#include "zr/cvars"
#include "zr/log"
#include "zr/config"
#include "zr/serial"
#include "zr/sayhooks"
#include "zr/tools"
#include "zr/models"
#include "zr/downloads"
#include "zr/overlays"
#include "zr/playerclasses/playerclasses"
#include "zr/weapons/weapons"
#include "zr/hitgroups"
#include "zr/roundstart"
#include "zr/roundend"
#include "zr/infect"
#include "zr/damage"
#include "zr/menu"
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
#include "zr/jumpboost"
#include "zr/zspawn"
#include "zr/ztele"
#include "zr/zhp"
#include "zr/jumpboost"
#include "zr/volfeatures/volfeatures"

/**
 * Record plugin info.
 */
public Plugin:myinfo =
{
    name = "Zombie:Reloaded",
    author = "Greyscale | Richard Helgeby",
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
    // Forward event to modules.
    TranslationInit();
    CvarsInit();
    ToolsInit();
    CommandsInit();
    WeaponsInit();
    SayHooksInit();
    EventInit();
    MarketInit();
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
    SerialOnMapStart();
    OverlaysOnMapStart();
    RoundEndOnMapStart();
    InfectOnMapStart();
    SEffectsOnMapStart();
    AntiStickOnMapStart();
    ZSpawnOnMapStart();
}

/**
 * The map is ending.
 */
public OnMapEnd()
{
    // Forward event to modules.
}

/**
 * Configs just finished getting executed.
 */
public OnConfigsExecuted()
{
    // Forward event to modules.
    ConfigLoad();
    ModelsLoad();
    DownloadsLoad();
    WeaponsLoad();
    HitgroupsLoad();
    InfectLoad();
    VEffectsLoad();
    SEffectsLoad();
    ClassLoad();
    
    ConfigOnModulesLoaded();
    ClassOnModulesLoaded();
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
    OverlaysClientInit(client);
    WeaponsClientInit(client);
    InfectClientInit(client);
    DamageClientInit(client);
    SEffectsClientInit(client);
    SpawnProtectClientInit(client);
    RespawnClientInit(client);
    ZTeleClientInit(client);
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
}
