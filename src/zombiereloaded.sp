/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zombiereloaded.sp
 *  Type:          Base
 *  Description:   Plugin's base file.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <clientprefs>
#include <cstrike>
#include <zrtools>

#define VERSION "3.0.0-b2-dev"

// Comment this line to exclude version info command. Temporary solution until
// there is a windows script for updating hgversion.h.inc.
#define ADD_VERSION_INFO

// Header includes.
#include "zr/log.h"

#if defined ADD_VERSION_INFO
#include "zr/hgversion.h"
#endif

// Core includes.
#include "zr/zombiereloaded"

#if defined ADD_VERSION_INFO
#include "zr/versioninfo"
#endif

#include "zr/translation"
#include "zr/cvars"
#include "zr/admintools"
#include "zr/log"
#include "zr/config"
#include "zr/steamidcache"
#include "zr/sayhooks"
#include "zr/tools"
#include "zr/menu"
#include "zr/cookies"
#include "zr/paramtools"
#include "zr/paramparser"
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
#include "zr/event"
#include "zr/zadmin"
#include "zr/commands"
//#include "zr/global"

// Modules
#include "zr/account"
#include "zr/visualeffects/visualeffects"
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
#include "zr/zcookies"
#include "zr/jumpboost"
#include "zr/volfeatures/volfeatures"
#include "zr/debugtools"

/**
 * Record plugin info.
 */
public Plugin:myinfo =
{
    name = "Zombie:Reloaded",
    author = "Greyscale | Richard Helgeby",
    description = "Infection/survival style gameplay",
    version = VERSION,
    url = "http://www.zombiereloaded.com"
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
    LogInit();          // Doesn't depend on CVARs.
    TranslationInit();
    CvarsInit();
    ToolsInit();
    CookiesInit();
    CommandsInit();
    WeaponsInit();
    EventInit();
    VolInit();
}

/**
 * All plugins have finished loading.
 */
public OnAllPluginsLoaded()
{
    // Forward event to modules.
    WeaponsOnAllPluginsLoaded();
}

/**
 * The map is starting.
 */
public OnMapStart()
{
    // Forward event to modules.
    OverlaysOnMapStart();
    RoundEndOnMapStart();
    InfectOnMapStart();
    SEffectsOnMapStart();
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
    // Forward event to modules. (OnConfigsExecuted)
    ConfigLoad();
    ModelsLoad();
    DownloadsLoad();
    WeaponsLoad();
    HitgroupsLoad();
    InfectLoad();
    VEffectsLoad();
    SEffectsLoad();
    AntiStickLoad();
    ClassLoad();
    VolLoad();
    
    // Forward event to modules. (OnModulesLoaded)
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
    AntiStickClientInit(client);
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
    AntiStickOnClientDisconnect(client);
    ZSpawnOnClientDisconnect(client);
    VolOnPlayerDisconnect(client);
}
