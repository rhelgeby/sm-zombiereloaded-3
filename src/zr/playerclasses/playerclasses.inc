/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          playerclasses.inc
 *  Type:          Core 
 *  Description:   Provides functions for managing classes.
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

/*
    Ideas for immunity modes for humans:
    - Zombies have to stab x times to infect.
    - Zombies have to hurt humans so they loose hp. When the hp reach zero (or
      below) they turn into zombies.
    - Fully imune to all damage. Can't take or give damage. Sould only be used
      on admin classes.

    TODO: Admin command for overriding class values;
          zr_class_ovverride <classname> <key> <value> [value_is_multiplier]
          Either set absolute values or use it as a multiplier, if possible.
          "classname" should support "all" to set a spesific value on all
          classes.
          
          How overrides should work:
          When a player select a class, its attributes are cached for each
          player. On infection or spawn, the plugin will use the cached
          values.
          
          When we override, the real class attributes are changed and then
          copied to the player caches (with matching team IDs).
          
          There is one problem, the in-game knockback multiplier menu. It
          depends on the original class knockback values. A solution might be
          to have another array of class data, which is never canged.
    
    TODO: Make class attributes for for changing model render mode and colors.
    
    TODO: Make class attributes for fancy effects, like a glow (if possible).
    
    TODO: Make alpha settings also apply to weapons (we could use code from
          zombie riot in this case).
    
    TODO: Make immunity settings suitable for both teams, or certain zombie-
          only/human-only settings.
*/

/**
 * Total number of classes that can be stored in each cache. A total of 32
 * classes should be enough. Too many classes will confuse players.
 */
#define ZR_CLASS_MAX 32

/**
 * @section Class cache types. Specifies what data array to use.
 */
#define ZR_CLASS_CACHE_ORIGINAL 0   /** Points ClassData array. A cache that is never changed after loading. */
#define ZR_CLASS_CACHE_MODIFIED 1   /** Points to ClassDataCache array. Modified by admins or overrides. */
#define ZR_CLASS_CACHE_PLAYER   2   /** Points to ClassPlayerCache array. Current player attributes. */
/**
 * @endsection
 */

/**
 * Number of available class teams.
 */
#define ZR_CLASS_TEAMCOUNT      3

/**
 * @section Available class teams. The admin team is optional and not required
 * in class configs.
 */
#define ZR_CLASS_TEAM_ZOMBIES   0
#define ZR_CLASS_TEAM_HUMANS    1
#define ZR_CLASS_TEAM_ADMINS    2   /** Note: Will set you in a special mode where you don't really participates in the game, but just walk around. */
/**
 * @endsection
 */

/**
 * @section Damage immunity modes. The mode effects will vary depending on the
 * class team.
 */
#define ZR_CLASS_IMMUNITY_DISABLED  0   /** No immunity. */
#define ZR_CLASS_IMMUNITY_CONSTANT  1   /** Always imune. Should only be used in special cases like on admin classes. */
#define ZR_CLASS_IMMUNITY_TIMED     2   /** Imune to damage for n seconds. The time is specified in a class as immunity amount. */
/**
 * @endsection
 */

/**
 * @section Overall default class settings. Since this is a zombie plugin the
 * default values represent a zombie.
 */
#define ZR_CLASS_DEFAULT_ENABLED                true
#define ZR_CLASS_DEFAULT_TEAM                   ZR_CLASS_TEAM_ZOMBIES
#define ZR_CLASS_DEFAULT_TEAM_DEFAULT           true
#define ZR_CLASS_DEFAULT_NAME                   "classic"
#define ZR_CLASS_DEFAULT_DESCRIPTION            "Need brains!!! Arrrrggghh!"
#define ZR_CLASS_DEFAULT_MODEL_PATH             "models/player/zh/zh_zombie003.mdl"
#define ZR_CLASS_DEFAULT_ALPHA_INITIAL          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGED          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGE           0
#define ZR_CLASS_DEFAULT_OVERLAY_PATH           "overlays/zr/zvision"
#define ZR_CLASS_DEFAULT_NVGS                   true
#define ZR_CLASS_DEFAULT_FOV                    90
#define ZR_CLASS_DEFAULT_HAS_NAPALM             1
#define ZR_CLASS_DEFAULT_NAPALM_TIME            10.0
#define ZR_CLASS_DEFAULT_IMMUNITY_MODE          ZR_CLASS_IMMUNITY_DISABLED
#define ZR_CLASS_DEFAULT_IMMUNITY_AMOUNT        0.0
#define ZR_CLASS_DEFAULT_NO_FALL_DAMAGE         true
#define ZR_CLASS_DEFAULT_HEALTH                 6000
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_INTERVAL  0.0
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_AMOUNT    2
#define ZR_CLASS_DEFAULT_HEALTH_INFECT_GAIN     800
#define ZR_CLASS_DEFAULT_KILL_BONUS             2
#define ZR_CLASS_DEFAULT_SPEED                  360.0
#define ZR_CLASS_DEFAULT_KNOCKBACK              2.0
#define ZR_CLASS_DEFAULT_JUMP_HEIGHT            10.0
#define ZR_CLASS_DEFAULT_JUMP_DISTANCE          0.2
/**
 * @endsection
 */

/**
 * @section Attribute limit values. Used when validating.
 */
#define ZR_CLASS_TEAM_MIN                   0
#define ZR_CLASS_TEAM_MAX                   2
#define ZR_CLASS_NAME_MIN                   1
#define ZR_CLASS_DESCRIPTION_MIN            1
/** Model path is checked for existance. */
#define ZR_CLASS_ALPHA_INITIAL_MIN          0
#define ZR_CLASS_ALPHA_INITIAL_MAX          255
#define ZR_CLASS_ALPHA_DAMAGED_MIN          0
#define ZR_CLASS_ALPHA_DAMAGED_MAX          255
#define ZR_CLASS_ALPHA_DAMAGE_MIN           0
#define ZR_CLASS_ALPHA_DAMAGE_MAX           20000
/** Overlay path is optional, and file is checked for existance if specified. */
#define ZR_CLASS_FOV_MIN                    15
#define ZR_CLASS_FOV_MAX                    165
#define ZR_CLASS_NAPALM_TIME_MIN            0.0
#define ZR_CLASS_NAPALM_TIME_MAX            600.0
#define ZR_CLASS_HEALTH_MIN                 1
#define ZR_CLASS_HEALTH_MAX                 20000
#define ZR_CLASS_HEALTH_REGEN_INTERVAL_MIN  0.0
#define ZR_CLASS_HEALTH_REGEN_INTERVAL_MAX  900.0
#define ZR_CLASS_HEALTH_REGEN_AMOUNT_MIN    0
#define ZR_CLASS_HEALTH_REGEN_AMOUNT_MAX    10000
#define ZR_CLASS_HEALTH_INFECT_GAIN_MIN     0
#define ZR_CLASS_HEALTH_INFECT_GAIN_MAX     20000
#define ZR_CLASS_KILL_BONUS_MIN             0
#define ZR_CLASS_KILL_BONUS_MAX             16
#define ZR_CLASS_SPEED_MIN                  10.0
#define ZR_CLASS_SPEED_MAX                  2000.0
#define ZR_CLASS_KNOCKBACK_MIN              -30.0
#define ZR_CLASS_KNOCKBACK_MAX              30.0
#define ZR_CLASS_JUMP_HEIGHT_MIN            -500.0
#define ZR_CLASS_JUMP_HEIGHT_MAX            500.0
#define ZR_CLASS_JUMP_DISTANCE_MIN          -5.0
#define ZR_CLASS_JUMP_DISTANCE_MAX          5.0
/**
 * @endsection
 */

/**
 * @section Flags used for specifying one or more attributes.
 */
#define ZR_CLASS_FLAG_ENABLED               (1<<0)
#define ZR_CLASS_FLAG_TEAM                  (1<<1)
#define ZR_CLASS_FLAG_TEAM_DEFAULT          (1<<2)
#define ZR_CLASS_FLAG_NAME                  (1<<3)
#define ZR_CLASS_FLAG_DESCRIPTION           (1<<4)
#define ZR_CLASS_FLAG_MODEL_PATH            (1<<5)
#define ZR_CLASS_FLAG_ALPHA_INITIAL         (1<<6)
#define ZR_CLASS_FLAG_ALPHA_DAMAGED         (1<<7)
#define ZR_CLASS_FLAG_ALPHA_DAMAGE          (1<<8)
#define ZR_CLASS_FLAG_OVERLAY_PATH          (1<<9)
#define ZR_CLASS_FLAG_NVGS                  (1<<10)
#define ZR_CLASS_FLAG_FOV                   (1<<11)
#define ZR_CLASS_FLAG_HAS_NAPALM            (1<<12)
#define ZR_CLASS_FLAG_NAPALM_TIME           (1<<13)
#define ZR_CLASS_FLAG_IMMUNITY_MODE         (1<<14)
#define ZR_CLASS_FLAG_IMMUNITY_AMOUNT       (1<<15)
#define ZR_CLASS_FLAG_NO_FALL_DAMAGE        (1<<16)
#define ZR_CLASS_FLAG_HEALTH                (1<<17)
#define ZR_CLASS_FLAG_HEALTH_REGEN_INTERVAL (1<<18)
#define ZR_CLASS_FLAG_HEALTH_REGEN_AMOUNT   (1<<19)
#define ZR_CLASS_FLAG_HEALTH_INFECT_GAIN    (1<<20)
#define ZR_CLASS_FLAG_KILL_BONUS            (1<<21)
#define ZR_CLASS_FLAG_SPEED                 (1<<22)
#define ZR_CLASS_FLAG_KNOCKBACK             (1<<23)
#define ZR_CLASS_FLAG_JUMP_HEIGHT           (1<<24)
#define ZR_CLASS_FLAG_JUMP_DISTANCE         (1<<25)
/**
 * @endsection
 */

/**
 * Generic player attributes.
 */
enum ClassAttributes
{
    /* General */
    bool:class_enabled,
    class_team,
    bool:class_team_default,
    
    String:class_name[64],
    String:class_description[256],
    
    /* Model */
    String:class_model_path[PLATFORM_MAX_PATH],
    class_alpha_initial,
    class_alpha_damaged,
    class_alpha_damage,
    
    /* Hud */
    String:class_overlay_path[PLATFORM_MAX_PATH],
    bool:class_nvgs,
    class_fov,
    
    /* Effects */
    bool:class_has_napalm,
    Float:class_napalm_time,
    
    /* Player behaviour */
    class_immunity_mode,
    Float:class_immunity_amount,
    bool:class_no_fall_damage,
    
    class_health,
    Float:class_health_regen_interval,
    class_health_regen_amount,
    class_health_infect_gain,
    class_kill_bonus,
    
    Float:class_speed,
    Float:class_knockback,
    Float:class_jump_height,
    Float:class_jump_distance
}

/**
 * Class attributes that support multipliers.
 */
enum ClassMultipliers
{
    ClassM_Invalid = 0,
    Float:ClassM_NapalmTime,
    Float:ClassM_Health,
    Float:ClassM_HealthRegenInterval,
    Float:ClassM_HealthRegenAmount,
    Float:ClassM_HealthInfectGain,
    Float:ClassM_Speed,
    Float:ClassM_Knockback,
    Float:ClassM_JumpHeight,
    Float:ClassM_JumpDistance
}

/**
 * Available class teams, used to specify targets.
 */
enum ClassTeams
{
    ClassTeam_Zombies = 0,
    ClassTeam_Humans,
    ClassTeam_Admins,
    ClassTeam_All
}

/**
 * Data types used in class attributes.
 */
enum ClassDataTypes
{
    ClassDataType_InvalidType,  /** Invalid type */
    ClassDataType_Boolean,      /** Boolean value */
    ClassDataType_Integer,      /** Integer value */
    ClassDataType_Float,        /** Floating point value */
    ClassDataType_String        /** String value */
}

/**
 * Keyvalue handle to store class data.
 */
new Handle:kvClassData = INVALID_HANDLE;

/**
 * The original class data. This array only changed when class data is loaded.
 * ZR_CLASS_CACHE_ORIGINAL is the cache type to this array.
 */
new ClassData[ZR_CLASS_MAX][ClassAttributes];

/**
 * The class thata cache that can be modified. ZR_CLASS_CACHE_MODIFIED is the
 * cache type to this array.
 */
new ClassDataCache[ZR_CLASS_MAX][ClassAttributes];

/**
 * Cache for player attributes. Makes it possible for one or more players to
 * have custom attributes. ZR_CLASS_CACHE_PLAYER is the cache type to this
 * array.
 */
new ClassPlayerCache[MAXPLAYERS + 1][ClassAttributes];

/**
 * Cache for storing global multipliers, per team and per attribute when
 * possible. Only attributes that support multipliers will be used, others are
 * ignored.
 */
new Float:ClassMultiplierCache[ZR_CLASS_TEAMCOUNT][ClassMultipliers];

/**
 * Number of classes loaded.
 */
new ClassCount;

/**
 * Specifies wether the class team requirement and attributes are valid or not.
 * Used to block events that happend before the module is done loading.
 */
new bool:ClassValidated;

/**
 * Stores what class that the player have selected, for each team.
 */
new ClassSelected[MAXPLAYERS + 1][ZR_CLASS_TEAMCOUNT];

/**
 * Cache for the currently selected team (admin menus).
 */
new ClassAdminTeamSelected[MAXPLAYERS + 1];

/**
 * Cache for the currently selected attribute multiplier (admin menus).
 */
new ClassMultipliers:ClassAdminAttributeSelected[MAXPLAYERS + 1];

/**
 * Specifies whether a player is currently in admin mode.
 */
new bool:ClassPlayerInAdminMode[MAXPLAYERS + 1];

/**
 * Specifies whether a player is set to be in admin mode next spawn.
 */
new bool:ClassPlayerAdminMode[MAXPLAYERS + 1];

/**
 * Specifies the admin class to use on next admin mode spawn.
 */
new ClassPlayerNextAdminClass[MAXPLAYERS + 1];

#include "zr/playerclasses/filtertools"
#include "zr/playerclasses/attributes"
#include "zr/playerclasses/apply"
#include "zr/playerclasses/clientoverlays"
#include "zr/playerclasses/clientalpha"
#include "zr/playerclasses/healthregen"
#include "zr/playerclasses/classevents"
#include "zr/playerclasses/classmenus"
#include "zr/playerclasses/classcommands"

/**
 * Loads class attributes from the class file into ClassData array. If any
 * error occur the plugin load will fail, and errors will be logged.
 *
 * @param keepMultipliers   Optional. Don't reset multipliers. Default is
 *                          false.
 */
ClassLoad(bool:keepMultipliers = false)
{
    // Register config file.
    ConfigRegisterConfig(File_Classes, Structure_Keyvalue, CONFIG_FILE_ALIAS_CLASSES);
    
    // Make sure kvClassData is ready to use.
    if (kvClassData != INVALID_HANDLE)
    {
        CloseHandle(kvClassData);
    }
    kvClassData = CreateKeyValues(CONFIG_FILE_ALIAS_CLASSES);
    
    // Get weapons config path.
    decl String:pathclasses[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_CLASSES, pathclasses);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Missing playerclasses config file \"%s\"", pathclasses);
        return;
    }
    
    // Log what class file that is loaded.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Loading classes from file \"%s\".", pathclasses);
    
    // Put file data into memory.
    FileToKeyValues(kvClassData, pathclasses);
    
    // Try to find the first class.
    KvRewind(kvClassData);
    if (!KvGotoFirstSubKey(kvClassData))
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Can't find any classes in \"%s\"", pathclasses);
    }
    
    decl String:name[64];
    decl String:description[256];
    decl String:model_path[PLATFORM_MAX_PATH];
    decl String:overlay_path[PLATFORM_MAX_PATH];
    
    ClassCount = 0;
    new failedcount;
    new ClassErrorFlags;
    
    // Loop through all classes and store attributes in the ClassData array.
    do
    {
        if (ClassCount > ZR_CLASS_MAX)
        {
            // Maximum classes reached. Write a warning and exit the loop.
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Maximum classes reached (%d). Skipping other classes.", ZR_CLASS_MAX + 1);
            
            break;
        }
        
        /* General */
        ClassData[ClassCount][class_enabled] = bool:KvGetNum(kvClassData, "enabled", ZR_CLASS_DEFAULT_ENABLED);
        ClassData[ClassCount][class_team] = KvGetNum(kvClassData, "team", ZR_CLASS_DEFAULT_TEAM);
        ClassData[ClassCount][class_team_default] = bool:KvGetNum(kvClassData, "team_default", ZR_CLASS_DEFAULT_TEAM_DEFAULT);
        
        KvGetString(kvClassData, "name", name, sizeof(name), ZR_CLASS_DEFAULT_NAME);
        strcopy(ClassData[ClassCount][class_name], 64, name);
        
        KvGetString(kvClassData, "description", description, sizeof(description), ZR_CLASS_DEFAULT_DESCRIPTION);
        strcopy(ClassData[ClassCount][class_description], 256, description);
        
        
        /* Model */
        KvGetString(kvClassData, "model_path", model_path, sizeof(model_path), ZR_CLASS_DEFAULT_MODEL_PATH);
        strcopy(ClassData[ClassCount][class_model_path], PLATFORM_MAX_PATH, model_path);
        
        ClassData[ClassCount][class_alpha_initial] = KvGetNum(kvClassData, "alpha_initial", ZR_CLASS_DEFAULT_ALPHA_INITIAL);
        ClassData[ClassCount][class_alpha_damaged] = KvGetNum(kvClassData, "alpha_damaged", ZR_CLASS_DEFAULT_ALPHA_DAMAGED);
        ClassData[ClassCount][class_alpha_damage] = KvGetNum(kvClassData, "alpha_damage", ZR_CLASS_DEFAULT_ALPHA_DAMAGE);
        
        
        /* Hud */
        KvGetString(kvClassData, "overlay_path", overlay_path, sizeof(overlay_path), ZR_CLASS_DEFAULT_OVERLAY_PATH);
        strcopy(ClassData[ClassCount][class_overlay_path], PLATFORM_MAX_PATH, overlay_path);
        
        ClassData[ClassCount][class_nvgs] = bool:KvGetNum(kvClassData, "nvgs", ZR_CLASS_DEFAULT_NVGS);
        ClassData[ClassCount][class_fov] = KvGetNum(kvClassData, "fov", ZR_CLASS_DEFAULT_FOV);
        
        
        /* Effects */
        ClassData[ClassCount][class_has_napalm] = bool:KvGetNum(kvClassData, "have_napalm", ZR_CLASS_DEFAULT_HAS_NAPALM);
        ClassData[ClassCount][class_napalm_time] = KvGetFloat(kvClassData, "napalm_time", ZR_CLASS_DEFAULT_NAPALM_TIME);
        
        
        /* Player behaviour */
        ClassData[ClassCount][class_immunity_mode] = KvGetNum(kvClassData, "immunity_mode", ZR_CLASS_DEFAULT_IMMUNITY_MODE);
        ClassData[ClassCount][class_immunity_amount] = KvGetFloat(kvClassData, "immunity_amount", ZR_CLASS_DEFAULT_IMMUNITY_AMOUNT);
        ClassData[ClassCount][class_no_fall_damage] = bool:KvGetNum(kvClassData, "no_fall_damage", ZR_CLASS_DEFAULT_NO_FALL_DAMAGE);
        
        ClassData[ClassCount][class_health] = KvGetNum(kvClassData, "health", ZR_CLASS_DEFAULT_HEALTH);
        ClassData[ClassCount][class_health_regen_interval] = KvGetFloat(kvClassData, "health_regen_interval", ZR_CLASS_DEFAULT_HEALTH_REGEN_INTERVAL);
        ClassData[ClassCount][class_health_regen_amount] = KvGetNum(kvClassData, "health_regen_amount", ZR_CLASS_DEFAULT_HEALTH_REGEN_AMOUNT);
        ClassData[ClassCount][class_health_infect_gain] = KvGetNum(kvClassData, "health_infect_bonus", ZR_CLASS_DEFAULT_HEALTH_INFECT_GAIN);
        ClassData[ClassCount][class_kill_bonus] = KvGetNum(kvClassData, "kill_bonus", ZR_CLASS_DEFAULT_KILL_BONUS);
        
        ClassData[ClassCount][class_speed] = KvGetFloat(kvClassData, "speed", ZR_CLASS_DEFAULT_SPEED);
        ClassData[ClassCount][class_knockback] = KvGetFloat(kvClassData, "knockback", ZR_CLASS_DEFAULT_KNOCKBACK);
        ClassData[ClassCount][class_jump_height] = KvGetFloat(kvClassData, "jump_height", ZR_CLASS_DEFAULT_JUMP_HEIGHT);
        ClassData[ClassCount][class_jump_distance] = KvGetFloat(kvClassData, "jump_distance", ZR_CLASS_DEFAULT_JUMP_DISTANCE);
        
        // Validate the class attributes.
        ClassErrorFlags = ClassValidateAttributes(ClassCount);
        if (ClassErrorFlags > 0)
        {
            // There's one or more invalid class attributes. Disable the class
            // and log an error message.
            ClassData[ClassCount][class_enabled] = false;
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Warning: Invalid class at index %d, disabled class. Class error flags: %d.", ClassCount, ClassErrorFlags);
            
            failedcount++;
        }
        
        // Update the counter.
        ClassCount++;
    } while (KvGotoNextKey(kvClassData));
    
    // Validate team requirements.
    if (!ClassValidateTeamRequirements())
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "The class configuration doesn't match the team requirements.");
    }
    
    // Validate team default requirements.
    if (!ClassValidateTeamDefaults())
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Couldn't find a default class for one or more teams. At least one class per team must be marked as default.");
    }
    
    // Cache class data.
    ClassReloadDataCache();
    
    // Reset attribute multipliers, if not keeping.
    if (!keepMultipliers)
    {
        ClassResetMultiplierCache();
    }
    
    // Mark classes as valid.
    ClassValidated = true;
    
    // Log summary.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Playerclasses, "Config Validation", "Total: %d | Successful: %d | Unsuccessful: %d", ClassCount, ClassCount - failedcount, failedcount);
    
    // Set config data.
    ConfigSetConfigLoaded(File_Classes, true);
    ConfigSetConfigReloadFunc(File_Classes, GetFunctionByName(GetMyHandle(), "ClassOnConfigReload"));
    // ConfigSetConfigHandle(File_Classes, INVALID_HANDLE);
    ConfigSetConfigPath(File_Classes, pathclasses);
}

/**
 * Called when configs are being reloaded.
 * 
 * @param config    The config being reloaded. (only if 'all' is false)
 */
public ClassOnConfigReload(ConfigFile:config)
{
    // Reload class config.
    ClassLoad(true);
}

/**
 * Updates the class data cache. Original values are retrieved from ClassData.
 *
 * @return      True on success, false otherwise.
 */
bool:ClassReloadDataCache()
{
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        /* General */
        ClassDataCache[classindex][class_enabled] = ClassData[classindex][class_enabled];
        ClassDataCache[classindex][class_team] = ClassData[classindex][class_team];
        ClassDataCache[classindex][class_team_default] = ClassData[classindex][class_team_default];
        strcopy(ClassDataCache[classindex][class_name], 64, ClassData[classindex][class_name]);
        strcopy(ClassDataCache[classindex][class_description], 256, ClassData[classindex][class_description]);
        
        /* Model */
        strcopy(ClassDataCache[classindex][class_model_path], PLATFORM_MAX_PATH, ClassData[classindex][class_model_path]);
        ClassDataCache[classindex][class_alpha_initial] = ClassData[classindex][class_alpha_initial];
        ClassDataCache[classindex][class_alpha_damaged] = ClassData[classindex][class_alpha_damaged];
        ClassDataCache[classindex][class_alpha_damage] = ClassData[classindex][class_alpha_damage];
        
        /* Hud */
        strcopy(ClassDataCache[classindex][class_overlay_path], PLATFORM_MAX_PATH, ClassData[classindex][class_overlay_path]);
        ClassDataCache[classindex][class_nvgs] = ClassData[classindex][class_nvgs];
        ClassDataCache[classindex][class_fov] = ClassData[classindex][class_fov];
        
        /* Effects */
        ClassDataCache[classindex][class_has_napalm] = ClassData[classindex][class_has_napalm];
        ClassDataCache[classindex][class_napalm_time] = ClassData[classindex][class_napalm_time];
        
        /* Player behaviour */
        ClassDataCache[classindex][class_immunity_mode] = ClassData[classindex][class_immunity_mode];
        ClassDataCache[classindex][class_immunity_amount] = ClassData[classindex][class_immunity_amount];
        ClassDataCache[classindex][class_no_fall_damage] = ClassData[classindex][class_no_fall_damage];
        ClassDataCache[classindex][class_health] = ClassData[classindex][class_health];
        ClassDataCache[classindex][class_health_regen_interval] = ClassData[classindex][class_health_regen_interval];
        ClassDataCache[classindex][class_health_regen_amount] = ClassData[classindex][class_health_regen_amount];
        ClassDataCache[classindex][class_health_infect_gain] = ClassData[classindex][class_health_infect_gain];
        ClassDataCache[classindex][class_kill_bonus] = ClassData[classindex][class_kill_bonus];
        ClassDataCache[classindex][class_speed] = ClassData[classindex][class_speed];
        ClassDataCache[classindex][class_knockback] = ClassData[classindex][class_knockback];
        ClassDataCache[classindex][class_jump_height] = ClassData[classindex][class_jump_height];
        ClassDataCache[classindex][class_jump_distance] = ClassData[classindex][class_jump_distance];
    }
    
    return true;
}

/**
 * Refresh the specified player's cache from the specified class data cache.
 *
 * @param client        The client index.
 * @param classindex    The index of the class to read from.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options: ZR_CLASS_CACHE_ORIGINAL (unchanged class
 *                      data), ZR_CLASS_CACHE_MODIFIED (default, modified class
 *                      data).
 * @return      True if successful, false otherwise.
 */
bool:ClassReloadPlayerCache(client, classindex, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Validate indexes.
    if (!ClassValidateIndex(classindex) || !ZRIsClientValid(client))
    {
        return false;
    }
    
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            /* General */
            ClassPlayerCache[client][class_enabled] = ClassData[classindex][class_enabled];
            ClassPlayerCache[client][class_team] = ClassData[classindex][class_team];
            ClassPlayerCache[client][class_team_default] = ClassData[classindex][class_team_default];
            strcopy(ClassPlayerCache[client][class_name], 64, ClassData[classindex][class_name]);
            strcopy(ClassPlayerCache[client][class_description], 256, ClassData[classindex][class_description]);
            
            /* Model */
            strcopy(ClassPlayerCache[client][class_model_path], PLATFORM_MAX_PATH, ClassData[classindex][class_model_path]);
            ClassPlayerCache[client][class_alpha_initial] = ClassData[classindex][class_alpha_initial];
            ClassPlayerCache[client][class_alpha_damaged] = ClassData[classindex][class_alpha_damaged];
            ClassPlayerCache[client][class_alpha_damage] = ClassData[classindex][class_alpha_damage];
            
            /* Hud */
            strcopy(ClassPlayerCache[client][class_overlay_path], PLATFORM_MAX_PATH, ClassData[classindex][class_overlay_path]);
            ClassPlayerCache[client][class_nvgs] = ClassData[classindex][class_nvgs];
            ClassPlayerCache[client][class_fov] = ClassData[classindex][class_fov];
            
            /* Effects */
            ClassPlayerCache[client][class_has_napalm] = ClassData[classindex][class_has_napalm];
            ClassPlayerCache[client][class_napalm_time] = ClassData[classindex][class_napalm_time];
            
            /* Player behaviour */
            ClassPlayerCache[client][class_immunity_mode] = ClassData[classindex][class_immunity_mode];
            ClassPlayerCache[client][class_immunity_amount] = ClassData[classindex][class_immunity_amount];
            ClassPlayerCache[client][class_no_fall_damage] = ClassData[classindex][class_no_fall_damage];
            ClassPlayerCache[client][class_health] = ClassData[classindex][class_health];
            ClassPlayerCache[client][class_health_regen_interval] = ClassData[classindex][class_health_regen_interval];
            ClassPlayerCache[client][class_health_regen_amount] = ClassData[classindex][class_health_regen_amount];
            ClassPlayerCache[client][class_health_infect_gain] = ClassData[classindex][class_health_infect_gain];
            ClassPlayerCache[client][class_kill_bonus] = ClassData[classindex][class_kill_bonus];
            ClassPlayerCache[client][class_speed] = ClassData[classindex][class_speed];
            ClassPlayerCache[client][class_knockback] = ClassData[classindex][class_knockback];
            ClassPlayerCache[client][class_jump_height] = ClassData[classindex][class_jump_height];
            ClassPlayerCache[client][class_jump_distance] = ClassData[classindex][class_jump_distance];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            /* General */
            ClassPlayerCache[client][class_enabled] = ClassDataCache[classindex][class_enabled];
            ClassPlayerCache[client][class_team] = ClassDataCache[classindex][class_team];
            ClassPlayerCache[client][class_team_default] = ClassDataCache[classindex][class_team_default];
            strcopy(ClassPlayerCache[client][class_name], 64, ClassDataCache[classindex][class_name]);
            strcopy(ClassPlayerCache[client][class_description], 256, ClassDataCache[classindex][class_description]);
            
            /* Model */
            strcopy(ClassPlayerCache[client][class_model_path], PLATFORM_MAX_PATH, ClassDataCache[classindex][class_model_path]);
            ClassPlayerCache[client][class_alpha_initial] = ClassDataCache[classindex][class_alpha_initial];
            ClassPlayerCache[client][class_alpha_damaged] = ClassDataCache[classindex][class_alpha_damaged];
            ClassPlayerCache[client][class_alpha_damage] = ClassDataCache[classindex][class_alpha_damage];
            
            /* Hud */
            strcopy(ClassPlayerCache[client][class_overlay_path], PLATFORM_MAX_PATH, ClassDataCache[classindex][class_overlay_path]);
            ClassPlayerCache[client][class_nvgs] = ClassDataCache[classindex][class_nvgs];
            ClassPlayerCache[client][class_fov] = ClassDataCache[classindex][class_fov];
            
            /* Effects */
            ClassPlayerCache[client][class_has_napalm] = ClassDataCache[classindex][class_has_napalm];
            ClassPlayerCache[client][class_napalm_time] = ClassDataCache[classindex][class_napalm_time];
            
            /* Player behaviour */
            ClassPlayerCache[client][class_immunity_mode] = ClassDataCache[classindex][class_immunity_mode];
            ClassPlayerCache[client][class_immunity_amount] = ClassDataCache[classindex][class_immunity_amount];
            ClassPlayerCache[client][class_no_fall_damage] = ClassDataCache[classindex][class_no_fall_damage];
            ClassPlayerCache[client][class_health] = ClassDataCache[classindex][class_health];
            ClassPlayerCache[client][class_health_regen_interval] = ClassDataCache[classindex][class_health_regen_interval];
            ClassPlayerCache[client][class_health_regen_amount] = ClassDataCache[classindex][class_health_regen_amount];
            ClassPlayerCache[client][class_health_infect_gain] = ClassDataCache[classindex][class_health_infect_gain];
            ClassPlayerCache[client][class_kill_bonus] = ClassDataCache[classindex][class_kill_bonus];
            ClassPlayerCache[client][class_speed] = ClassDataCache[classindex][class_speed];
            ClassPlayerCache[client][class_knockback] = ClassDataCache[classindex][class_knockback];
            ClassPlayerCache[client][class_jump_height] = ClassDataCache[classindex][class_jump_height];
            ClassPlayerCache[client][class_jump_distance] = ClassDataCache[classindex][class_jump_distance];
        }
        default:
        {
            // Invalid cache specified.
            return false;
        }
    }
    
    return true;
}

/**
 * Refresh the specified player's cache and re-apply attributes.
 *
 * @param client        The client index.
 * @return              True if successful, false otherwise.
 */
bool:ClassReloadPlayer(client)
{
    new activeclass;
    
    // Get active class index.
    activeclass = ClassGetActiveIndex(client);
    
    // Validate index.
    if (activeclass < 0)
    {
        return false;
    }
    
    // Refresh cache and re-apply attributes.
    ClassOnClientDeath(client);         // Dummy event to clean up stuff.
    ClassReloadPlayerCache(client, activeclass);
    ClassApplyAttributes(client);
    
    return true;
}

/**
 * Reset all class attribute multipliers to 1.0.
 */
ClassResetMultiplierCache()
{
    // Loop through all teams.
    for (new teamid = 0; teamid < ZR_CLASS_TEAMCOUNT; teamid++)
    {
        ClassMultiplierCache[teamid][ClassM_NapalmTime] = 1.0;
        ClassMultiplierCache[teamid][ClassM_Health] = 1.0;
        ClassMultiplierCache[teamid][ClassM_HealthRegenInterval] = 1.0;
        ClassMultiplierCache[teamid][ClassM_HealthRegenAmount] = 1.0;
        ClassMultiplierCache[teamid][ClassM_HealthInfectGain] = 1.0;
        ClassMultiplierCache[teamid][ClassM_Speed] = 1.0;
        ClassMultiplierCache[teamid][ClassM_Knockback] = 1.0;
        ClassMultiplierCache[teamid][ClassM_JumpHeight] = 1.0;
        ClassMultiplierCache[teamid][ClassM_JumpDistance] = 1.0;
    }
}

/**
 * Sets default class indexes for each team on all players, or a single player
 * if specified.
 *
 * @param client    Optional. The client index.
 */
ClassClientSetDefaultIndexes(client = -1)
{
    // Get indexes.
    new zombieindex = ClassGetDefaultSpawnClass(ZR_CLASS_TEAM_ZOMBIES);
    new humanindex = ClassGetDefaultSpawnClass(ZR_CLASS_TEAM_HUMANS);
    new adminindex = ClassGetDefaultSpawnClass(ZR_CLASS_TEAM_ADMINS);
    
    // Validate zombie class index.
    if (!ClassValidateIndex(zombieindex))
    {
        // Invalid class index. Fall back to default class in class config and
        // log a warning.
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Set Default Indexes", "Warning: Failed to get default zombie class, falling back to default class in class config. Check spelling in \"zr_classes_default_zombie\".");
        
        // Use default class.
        zombieindex = ClassGetDefaultClass(ZR_CLASS_TEAM_ZOMBIES);
    }
    
    // Validate human class index.
    if (!ClassValidateIndex(humanindex))
    {
        // Invalid class index. Fall back to default class in class config and
        // log a warning.
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Playerclasses, "Set Default Indexes", "Warning: Failed to get default human class, falling back to default class in class config. Check spelling in \"zr_classes_default_human\".");
        
        // Use default class.
        humanindex = ClassGetDefaultClass(ZR_CLASS_TEAM_HUMANS);
    }
    
    // Validate admin class index.
    if (!ClassValidateIndex(adminindex))
    {
        // Invalid class index. Fall back to default class in class config if
        // possible. A invalid class index (-1) can also be stored if there are
        // no admin classes at all.
        adminindex = ClassGetDefaultClass(ZR_CLASS_TEAM_ADMINS);
    }
    
    // Check if a client isn't specified.
    if (client < 1)
    {
        // No client specified. Loop through all players.
        for (new clientindex = 1; clientindex <= MAXPLAYERS; clientindex++)
        {
            ClassSelected[clientindex][ZR_CLASS_TEAM_ZOMBIES] = zombieindex;
            ClassSelected[clientindex][ZR_CLASS_TEAM_HUMANS] = humanindex;
            ClassSelected[clientindex][ZR_CLASS_TEAM_ADMINS] = adminindex;
            ClassPlayerNextAdminClass[clientindex] = adminindex;
            
            // Copy human class data to player cache.
            ClassReloadPlayerCache(client, humanindex);
        }
    }
    else
    {
        ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES] = zombieindex;
        ClassSelected[client][ZR_CLASS_TEAM_HUMANS] = humanindex;
        ClassSelected[client][ZR_CLASS_TEAM_ADMINS] = adminindex;
        ClassPlayerNextAdminClass[client] = adminindex;
        
        // Copy human class data to player cache.
        ClassReloadPlayerCache(client, humanindex);
    }
}

/**
 * Dump class data into a string. String buffer length should be at about 2048
 * cells.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          Number of cells written.
 */
ClassDumpData(index, cachetype, String:buffer[], maxlen)
{
    new cellcount;
    decl String:attribute[320];
    decl String:format_buffer[256];
    
    if (maxlen == 0)
    {
        return 0;
    }
    
    Format(format_buffer, sizeof(format_buffer), "Class data at index %d:\n", index);
    cellcount += StrCat(buffer, maxlen, format_buffer);
    cellcount += StrCat(buffer, maxlen, "-------------------------------------------------------------------------------\n");
    
    Format(attribute, sizeof(attribute), "enabled:               \"%d\"\n", ClassIsEnabled(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "team:                  \"%d\"\n", ClassGetTeamID(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "team_default:          \"%d\"\n", ClassGetTeamDefault(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    ClassGetName(index, format_buffer, sizeof(format_buffer), cachetype);
    Format(attribute, sizeof(attribute), "name:                  \"%s\"\n", format_buffer);
    cellcount += StrCat(buffer, maxlen, attribute);
    
    ClassGetDescription(index, format_buffer, sizeof(format_buffer), cachetype);
    Format(attribute, sizeof(attribute), "description:           \"%s\"\n", format_buffer);
    cellcount += StrCat(buffer, maxlen, attribute);
    
    ClassGetModelPath(index, format_buffer, sizeof(format_buffer), cachetype);
    Format(attribute, sizeof(attribute), "model_path:            \"%s\"\n", format_buffer);
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "alpha_initial:         \"%d\"\n", ClassGetAlphaInitial(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "alpha_damaged:         \"%d\"\n", ClassGetAlphaDamaged(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "alpha_damage:          \"%d\"\n", ClassGetAlphaDamage(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    ClassGetOverlayPath(index, format_buffer, sizeof(format_buffer), cachetype);
    Format(attribute, sizeof(attribute), "overlay_path:          \"%s\"\n", format_buffer);
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "nvgs:                  \"%d\"\n", ClassGetNvgs(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "fov:                   \"%d\"\n", ClassGetFOV(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "has_napalm:            \"%d\"\n", ClassGetHasNapalm(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "napalm_time:           \"%f\"\n", ClassGetNapalmTime(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "immunity_mode:         \"%d\"\n", ClassGetImmunityMode(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "immunity_amount:       \"%f\"\n", ClassGetImmunityAmount(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "no_fall_damage:        \"%d\"\n", ClassGetNoFallDamage(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "health:                \"%d\"\n", ClassGetHealth(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "health_regen_interval: \"%f\"\n", ClassGetHealthRegenInterval(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "health_regen_amount:   \"%d\"\n", ClassGetHealthRegenAmount(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "health_infect_gain:    \"%d\"\n", ClassGetHealthInfectGain(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "kill_bonus:            \"%d\"\n", ClassGetKillBonus(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "speed:                 \"%f\"\n", ClassGetSpeed(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "knockback:             \"%f\"\n", ClassGetKnockback(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "jump_height:           \"%f\"\n", ClassGetJumpHeight(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    Format(attribute, sizeof(attribute), "jump_distance:         \"%f\"\n", ClassGetJumpDistance(index, cachetype));
    cellcount += StrCat(buffer, maxlen, attribute);
    
    return cellcount;
}

/**
 * Converts a multiplier attribute to a human readable string.
 *
 * @param attribute     Attribute to convert.
 * @param buffer        Destination string buffer.
 * @param maxlen        Size of buffer.
 * @return              Number of cells written.
 */
ClassMultiplierToString(ClassMultipliers:attribute, String:buffer[], maxlen)
{
    decl String:phrase[48];
    
    switch (attribute)
    {
        case ClassM_NapalmTime:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Napalm Time");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_Health:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Health");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_HealthRegenInterval:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Regen Interval");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_HealthRegenAmount:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Regen Amount");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_HealthInfectGain:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Infect Gain");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_Speed:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Speed");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_Knockback:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Knockback");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_JumpHeight:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Jump Height");
            return strcopy(buffer, maxlen, phrase);
        }
        case ClassM_JumpDistance:
        {
            Format(phrase, sizeof(phrase), "%t", "Classes Attrib Jump Distance");
            return strcopy(buffer, maxlen, phrase);
        }
    }
    
    return 0;
}
