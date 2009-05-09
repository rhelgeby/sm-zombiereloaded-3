/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          config.inc
 *   Type:          Core
 *   Description:   Config API and executing.
 *
 * ============================================================================
 */

/**
 * @section List of config files under this modules control.
 */
#define CONFIG_FILE_MODELS         0
#define CONFIG_FILE_DOWNLOADS      1
#define CONFIG_FILE_PLAYERCLASSES  2
#define CONFIG_FILE_WEAPONS        3
#define CONFIG_FILE_WEAPONGROUPS   4
#define CONFIG_FILE_HITGROUPS      5
/**
 * @endsection
 */

/**
 * @section Config file flags.
 */
#define CONFIG_FILE_FLAG_MODELS         1
#define CONFIG_FILE_FLAG_DOWNLOADS      2
#define CONFIG_FILE_FLAG_PLAYERCLASSES  4
#define CONFIG_FILE_FLAG_WEAPONS        8
#define CONFIG_FILE_FLAG_WEAPONGROUPS   16
#define CONFIG_FILE_FLAG_HITGROUPS      32
/**
 * @endsection
 */

/**
 * The max length of a config/value string.
 */
#define CONFIG_OPTION_MAX_LENGTH 32

/**
 * Actions to use when working on key/values.
 */
enum ConfigKeyvalueAction
{
    Create, /** Create a key. */
    Delete, /** Delete a key. */
    Set, /** Modify setting of a key. */
    Get, /** Get setting of a key. */
}
/**
 * @endsection
 */

/**
 * @section Global data handle initializations.
 */
new Handle:arrayModelsList = INVALID_HANDLE;
new Handle:kvClassData = INVALID_HANDLE;
new Handle:kvWeapons = INVALID_HANDLE;
new Handle:kvWeaponGroups = INVALID_HANDLE;
new Handle:kvHitgroups = INVALID_HANDLE;

/**
 * Load plugin configs.
 */
ConfigLoad()
{
    decl String:mapconfig[PLATFORM_MAX_PATH];
    
    // Get map name and format into config path.
    GetCurrentMap(mapconfig, sizeof(mapconfig));
    Format(mapconfig, sizeof(mapconfig), "sourcemod/zombiereloaded/%s.cfg", mapconfig);
    
    // Prepend cfg to path.
    decl String:path[PLATFORM_MAX_PATH];
    Format(path, sizeof(path), "cfg/%s", mapconfig);
    
    // File doesn't exist, then stop.
    if (!FileExists(path))
    {
        return;
    }
    
    // Execute config file.
    ServerCommand("exec %s", mapconfig);
    
    // Log action.
    if (LogCheckFlag(LOG_CORE_EVENTS))
    {
        LogMessageFormatted(-1, "", "", "Executed map config file: %s.", LOG_FORMAT_TYPE_SIMPLE, mapconfig);
    }
}

/**
 * Executed when modules are done loading. After all init calls in
 * OnConfigsExecuted.
 *
 * Executes post map configs if they exist.
 */
ConfigOnModulesLoaded()
{
    decl String:mapname[256];
    decl String:mapconfig[PLATFORM_MAX_PATH];
    decl String:path[PLATFORM_MAX_PATH];
    new bool:cfgexists;
    
    // Get map name and format into config path.
    GetCurrentMap(mapname, sizeof(mapname));
    Format(mapconfig, sizeof(mapconfig), "sourcemod/zombiereloaded/%s.post.cfg", mapname);
    
    // Prepend cfg to path.
    Format(path, sizeof(path), "cfg/%s", mapconfig);
    
    // Workaround for bug 3083 in SourceMod compiler. Having FileExist directly
    // in the if in this function makes it crash. Storing the result in a
    // boolean first works.
    
    // Check if the file exist.
    cfgexists = FileExists(path);
    if (!cfgexists)
    {
        // File doesn't exist, then stop.
        return;
    }
    
    // Execute config file.
    ServerCommand("exec %s", mapconfig);
    
    // Log action.
    if (LogCheckFlag(LOG_CORE_EVENTS))
    {
        LogMessageFormatted(-1, "", "", "Executed post map config file: %s.", LOG_FORMAT_TYPE_SIMPLE, mapconfig);
    }
}

/**
 * Load config file.
 * 
 * @param file  The cvar define of the path to the file.
 * @return      True if the file exists, false if not.
 */
bool:ConfigGetFilePath(CvarsList:cvar, String:path[])
{
    // Get cvar's path.
    decl String:filepath[PLATFORM_MAX_PATH];
    GetConVarString(Handle:g_hCvarsList[cvar], filepath, sizeof(filepath));
    
    // Build full path in return string.
    BuildPath(Path_SM, path, PLATFORM_MAX_PATH, filepath);
    
    return FileExists(path);
}


/**
 * Creates, deletes, sets, or gets any key/setting of any ZR config keyvalue file in memory.
 * Only use when interacting with a command or manipulating single keys/values,
 * using this function everywhere would be EXTREMELY inefficient.
 * 
 * @param config        Config index of config to modify. (see CONFIG_FILE_* defines)
 * @param action        Action to perform on keyvalue tree. (see enum ConfigKeyvalueAction)
 * @param keys          Array containing keys to traverse into.
 * @param keysMax       The size of the 'keys' array.
 * @param setting       (Optional) The name of the setting to modify.
 * @param value         (Optional) The new value to set.
 * @param maxlen        (Optional) The maxlength of the gotten value.
 * @return              True if the change was made successfully, false otherwise. 
 */
bool:ConfigKeyvalueTreeSetting(config, ConfigKeyvalueAction:action = Create, const String:keys[][], keysMax, const String:setting[] = "", String:value[] = "", maxlen = 0)
{
    // Retrieve handle of the keyvalue tree.
    new Handle:hConfig = ConfigGetFileHandle(config);
    
    // If handle is invalid, then stop.
    if (hConfig == INVALID_HANDLE)
    {
        return false;
    }
    
    // Rewind keyvalue tree.
    KvRewind(hConfig);
    
    // x = keys index.
    // Traverse into the keygroup, stop if it fails.
    for (new x = 0; x < keysMax; x++)
    {
        // If key is empty, then break the loop.
        if (!keys[x][0])
        {
            break;
        }
        
        // Try to jump to next level in the transversal stack, create key if specified.
        new bool:exists = KvJumpToKey(hConfig, keys[x], (action == Create));
        
        // If exists is false, then stop.
        if (!exists)
        {
            // Key doesn't exist.
            return false;
        }
    }
    
    switch(action)
    {
        case Create:
        {
            if (!setting[0] || !value[0])
            {
                // We created the key already, so return true.
                return true;
            }
            
            // Set new value.
            KvSetString(hConfig, setting, value);
        }
        case Delete:
        {
            // Return deletion result.
            return KvDeleteKey(hConfig, setting);
        }
        case Set:
        {
            // Set new value.
            KvSetString(hConfig, setting, value);
        }
        case Get:
        {
            // Get current value.
            KvGetString(hConfig, setting, value, maxlen);
        }
    }
    
    // We successfully set or got the value.
    return true;
}

/**
 * Return handle to array or keygroup for globally stored data.
 * 
 * @param configindex   Index of the config. (see CONFIG_FILE_* defines)
 */
Handle:ConfigGetFileHandle(config)
{
    switch(config)
    {
        case CONFIG_FILE_MODELS:
        {
            // Return model list array handle.
            return arrayModelsList;
        }
        case CONFIG_FILE_DOWNLOADS:
        {
            // We don't store download data.
            return INVALID_HANDLE;
        }
        case CONFIG_FILE_PLAYERCLASSES:
        {
            // Return class config keyvalue file handle.
            return kvClassData;
        }
        case CONFIG_FILE_WEAPONS:
        {
            // Return weapon config keyvalue file handle.
            return kvWeapons;
        }
        case CONFIG_FILE_WEAPONGROUPS:
        {
            // Return weapon groups config keyvalue file handle.
            return kvWeaponGroups;
        }
        case CONFIG_FILE_HITGROUPS:
        {
            // Return hitgroups config keyvalue file handle.
            return kvHitgroups;
        }
    }
    
    // Invalid config index.
    return INVALID_HANDLE;
}

/**
 * Iterate through a file and store each line in an array.
 * 
 * @param path  Path to the file to iterate through.
 * @return      The handle of the array, don't forget to call CloseHandle
 *              on it when finished!
 */
Handle:ConfigLinesToArray(const String:path[])
{
    new Handle:arrayLines = CreateArray(PLATFORM_MAX_PATH);
    decl String:line[PLATFORM_MAX_PATH];
    
    // Open file.
    new Handle:hFile = OpenFile(path, "r");
    
    // If file couldn't be opened, then stop.
    if (hFile == INVALID_HANDLE)
    {
        return INVALID_HANDLE;
    }
    
    while(!IsEndOfFile(hFile))
    {
        // Get current line text.
        ReadFileLine(hFile, line, sizeof(line));
        
        // If line contains a ";", then stop.
        if (StrContains(line, ";") > -1)
        {
            continue;
        }
        
        // Cut out comments at the end of a line.
        if (StrContains(line, "//") > -1)
        {
            SplitString(line, "//", line, sizeof(line));
        }
        
        // Trim off whitespace.
        TrimString(line);
        
        // If line is empty, then stop.
        if (!line[0])
        {
            continue;
        }
        
        // Push line into array.
        PushArrayString(arrayLines, line);
    }
    
    // Close file handle.
    CloseHandle(hFile);
    
    // Return array handle.
    return arrayLines;
}

/**
 * Converts string of "yes" or "no" to a boolean value.
 * 
 * @param option    "yes" or "no" string to be converted.
 * @return          True if string is "yes", false otherwise.
 */
bool:ConfigSettingToBool(const String:option[])
{
    // If option is equal to "yes," then return true.
    if (StrEqual(option, "yes", false))
    {
        return true;
    }
    
    // Option isn't "yes."
    return false;
}

/**
 * Converts boolean value to "yes" or "no".
 * 
 * @param bOption   True/false value to be converted to "yes"/"no", respectively.
 * @param option    Destination string buffer to store "yes" or "no" in.
 * @param maxlen    Length of destination string buffer (can't be more than 4).
 */
ConfigBoolToSetting(bool:bOption, String:option[], maxlen)
{
    // If option is true, then copy "yes" to return string.
    if (bOption)
    {
        strcopy(option, maxlen, "yes");
    }
    // If option is false, then copy "no" to return string.
    else
    {
        strcopy(option, maxlen, "no");
    }
}
