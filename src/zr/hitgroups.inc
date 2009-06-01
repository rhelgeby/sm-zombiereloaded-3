/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          hitgroup.inc
 *   Type:          Core
 *   Description:   API for loading hitgroup specific settings.
 *
 * ============================================================================
 */

/**
 * Maximum length for a hitgroup name
 */
#define HITGROUPS_MAX_LENGTH 32

/**
 * @section Player hitgroup values.
 */
#define HITGROUP_GENERIC    0
#define HITGROUP_HEAD       1
#define HITGROUP_CHEST      2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4
#define HITGROUP_RIGHTARM   5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG   7
#define HITGROUP_GEAR       10
/**
 * @endsection
 */

/**
 * Hitgroup config data indexes.
 */
enum HitgroupsData
{
    HITGROUPS_DATA_NAME = 0,
    HITGROUPS_DATA_INDEX,
    HITGROUPS_DATA_DAMAGE,
    HITGROUPS_DATA_KNOCKBACK,
}

/**
 * Array handle to store hitgroups data.
 */
new Handle:arrayHitgroups = INVALID_HANDLE;

/**
 * Loads hitgroup data from file.
 */ 
HitgroupsLoad()
{
    // Register config file.
    ConfigRegisterConfig(File_Hitgroups, Structure_Keyvalue, CONFIG_FILE_ALIAS_HITGROUPS);
    
    // If module is disabled, then stop.
    new bool:hitgroups = GetConVarBool(g_hCvarsList[CVAR_HITGROUPS]);
    if (!hitgroups)
    {
        return;
    }
    
    // Get hitgroups config path.
    decl String:pathhitgroups[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_HITGROUPS, pathhitgroups);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure.
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Hitgroups, "Config Validation", "Missing hitgroups config file: %s", pathhitgroups);
        
        return;
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_Hitgroups, pathhitgroups);
    
    // Load config from file and create array structure.
    new bool:success = ConfigLoadConfig(File_Hitgroups, arrayHitgroups);
    
    // Unexpected error, stop plugin.
    if (!success)
    {
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Hitgroups, "Config Validation", "Unexpected error encountered loading: %s", pathhitgroups);
        
        return;
    }
    
    // Validate hitgroups config.
    new size = GetArraySize(arrayHitgroups);
    if (!size)
    {
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Hitgroups, "Config Validation", "No usable data found in hitgroups config file: %s", pathhitgroups);
    }
    
    // Now copy data to array structure.
    HitgroupsCacheData();
    
    // Set config data.
    ConfigSetConfigLoaded(File_Hitgroups, true);
    ConfigSetConfigReloadFunc(File_Hitgroups, GetFunctionByName(GetMyHandle(), "HitgroupsOnConfigReload"));
    ConfigSetConfigHandle(File_Hitgroups, arrayHitgroups);
}

/**
 * Caches hitgroup data from file into arrays.
 * Make sure the file is loaded before (ConfigLoadConfig) to prep array structure.
 */
HitgroupsCacheData()
{
    // Get config's file path.
    decl String:pathhitgroups[PLATFORM_MAX_PATH];
    ConfigGetConfigPath(File_Hitgroups, pathhitgroups, sizeof(pathhitgroups));
    
    new Handle:kvHitgroups;
    new bool:success = ConfigOpenConfigFile(File_Hitgroups, kvHitgroups);
    
    if (!success)
    {
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Hitgroups, "Config Validation", "Unexpected error caching data from hitgroups config file: %s", pathhitgroups);
    }
    
    decl String:hitgroupname[HITGROUPS_MAX_LENGTH];
    
    // x = array index
    new size = GetArraySize(arrayHitgroups);
    for (new x = 0; x < size; x++)
    {
        HitgroupsGetName(x, hitgroupname, sizeof(hitgroupname));
        
        KvRewind(kvHitgroups);
        if (!KvJumpToKey(kvHitgroups, hitgroupname))
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Hitgroups, "Config Validation", "Couldn't cache hitgroup data for: %s (check hitgroup config)", hitgroupname);
            continue;
        }
        
        // General
        new index = KvGetNum(kvHitgroups, "index", -1);
        
        // Damage
        new bool:damage = ConfigKvGetStringBool(kvHitgroups, "damage", "yes");
        
        // Knockback (module)
        new Float:knockback = KvGetFloat(kvHitgroups, "knockback", 1.0);
        
        new Handle:arrayHitgroup = GetArrayCell(arrayHitgroups, x);
        
        // Push data into array.
        PushArrayCell(arrayHitgroup, index);        // Index: 1
        PushArrayCell(arrayHitgroup, damage);       // Index: 2
        PushArrayCell(arrayHitgroup, knockback);    // Index: 3
    }
    
    // We're done with this file now, so we can close it.
    CloseHandle(kvHitgroups);
}

/**
 * Called when configs are being reloaded.
 * 
 * @param config    The config being reloaded. (only if 'all' is false)
 */
public HitgroupsOnConfigReload(ConfigFile:config)
{
    // Reload hitgroups config.
    HitgroupsLoad();
}

/**
 * Find the array index at which the hitgroup index is at.
 * 
 * @param hitgroup  The hitgroup index to search for.
 * @return          The array index that contains the given hitgroup index.
 */
stock HitgroupToIndex(hitgroup)
{
    // x = Array index.
    new size = GetArraySize(arrayHitgroups);
    for (new x = 0; x < size; x++)
    {
        // Get hitgroup index at this array index.
        new index = HitgroupsGetIndex(x);
        
        // If hitgroup indexes match, then return array index.
        if (hitgroup == index)
        {
            return x;
        }
    }
    
    // Hitgroup index doesn't exist.
    return -1;
}

/**
 * Gets the name of a hitgroup at a given index.
 * @param index     The hitgroup index.
 * @param hitgroup  The string to return name in.
 * @param maxlen    The max length of the string.
 */
stock HitgroupsGetName(index, String:hitgroup[], maxlen)
{
    // Get array handle of hitgroup at given index.
    new Handle:arrayHitgroup = GetArrayCell(arrayHitgroups, index);
    
    // Get hitgroup name.
    GetArrayString(arrayHitgroup, _:HITGROUPS_DATA_NAME, hitgroup, maxlen);
}

/**
 * Retrieve hitgroup index.
 * 
 * @param index     The array index.
 * @return          The hitgroup index.
 */
stock HitgroupsGetIndex(index)
{
    // Get array handle of hitgroup at given index.
    new Handle:arrayHitgroup = GetArrayCell(arrayHitgroups, index);
    
    // Return hitgroup index of the hitgroup.
    return GetArrayCell(arrayHitgroup, _:HITGROUPS_DATA_INDEX);
}

/**
 * Retrieve hitgroup damage value.
 * 
 * @param index     The array index.
 * @return          True if hitgroup can be damaged, false if not.
 */
stock bool:HitgroupsCanDamage(index)
{
    // Get array handle of hitgroup at given index.
    new Handle:arrayHitgroup = GetArrayCell(arrayHitgroups, index);
    
    // Return true if hitgroup can be damaged, false if not.
    return bool:GetArrayCell(arrayHitgroup, _:HITGROUPS_DATA_DAMAGE);
}

/**
 * Retrieve hitgroup knockback value.
 * 
 * @param index     The array index.
 * @return          The knockback multiplier of the hitgroup.
 */
stock Float:HitgroupsGetKnockback(index)
{
    // Get array handle of hitgroup at given index.
    new Handle:arrayHitgroup = GetArrayCell(arrayHitgroups, index);
    
    // Return the knockback multiplier for the hitgroup.
    return Float:GetArrayCell(arrayHitgroup, _:HITGROUPS_DATA_KNOCKBACK);
}
