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
 * The max length of any config string value.
 */
#define CONFIG_MAX_LENGTH 32

/**
 * @section Config file reference aliases.
 */
#define CONFIG_FILE_ALIAS_MODELS        "models"
#define CONFIG_FILE_ALIAS_DOWNLOADS     "downloads"
#define CONFIG_FILE_ALIAS_CLASSES       "classes"
#define CONFIG_FILE_ALIAS_WEAPONS       "weapons"
#define CONFIG_FILE_ALIAS_WEAPONGROUPS  "weapongroups"
#define CONFIG_FILE_ALIAS_HITGROUPS     "hitgroups"
/**
 * @endsection
 */

/**
 * List of config files used by the plugin.
 */
enum ConfigFile
{
    ConfigInvalid = -1, /** Invalid config file. */
    ConfigModels,       /** <sourcemod root>/configs/zr/models.txt (default) */
    ConfigDownloads,    /** <sourcemod root>/configs/zr/downloads.txt (default) */
    ConfigClasses,      /** <sourcemod root>/configs/zr/playerclasses.txt (default) */
    ConfigWeapons,      /** <sourcemod root>/configs/zr/weapons/weapons.txt/weapongroups.txt (default) */
    ConfigHitgroups,    /** <sourcemod root>/configs/zr/hitgroups.txt (default) */
}

/**
 * Data container for each config file.
 */
enum ConfigData
{
    bool:ConfigLoaded,                      /** True if config is loaded, false if not. */
    Function:ConfigReloadFunc,              /** Function to call to reload config. */
    Handle:ConfigHandle,                    /** Handle of the config file. */
    String:ConfigPath[PLATFORM_MAX_PATH],   /** Full path to config file. */
    String:ConfigAlias[CONFIG_MAX_LENGTH],  /** Config file alias, used for client interaction. */
}

/**
 * Stores all config data.
 */
new g_ConfigData[ConfigFile][ConfigData];

/**
 * Actions to use when working on key/values.
 */
enum ConfigKeyvalueAction
{
    ConfigKVCreate, /** Create a key. */
    ConfigKVDelete, /** Delete a key. */
    ConfigKVSet,    /** Modify setting of a key. */
    ConfigKVGet,    /** Get setting of a key. */
}

/**
 * @section Global data handle initializations.
 */
new Handle:arrayModelsList = INVALID_HANDLE;
new Handle:arrayDownloadsList = INVALID_HANDLE;
new Handle:kvClassData = INVALID_HANDLE;
new Handle:kvWeapons = INVALID_HANDLE;
new Handle:kvWeaponGroups = INVALID_HANDLE;
new Handle:kvHitgroups = INVALID_HANDLE;

/**
 * Create commands related to config here.
 */
ConfigOnCommandsCreate()
{
    // Create config admin commands.
    RegAdminCmd("zr_config_reload", ConfigReloadCommand, ADMFLAG_GENERIC, "Reloads a config file. Usage: zr_config_reload <file alias>");
    RegAdminCmd("zr_config_reloadall", ConfigReloadAllCommand, ADMFLAG_GENERIC, "Reloads all config files. Usage: zr_config_reloadall");
}

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
    LogPrintToLog(LOG_FORMAT_TYPE_NORMAL, "Config", "Map Configs", "Executed map config file: %s", path);
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
    
    // Get map name and format into config path.
    GetCurrentMap(mapname, sizeof(mapname));
    Format(mapconfig, sizeof(mapconfig), "sourcemod/zombiereloaded/%s.post.cfg", mapname);
    
    // Prepend cfg to path.
    Format(path, sizeof(path), "cfg/%s", mapconfig);
    
    // Workaround for bug 3083 in SourceMod compiler. Having FileExist directly
    // in the if in this function makes it crash. Storing the result in a
    // boolean first works.
    
    // Check if the file exist.
    new bool:cfgexists = FileExists(path);
    if (!cfgexists)
    {
        // File doesn't exist, then stop.
        return;
    }
    
    // Execute config file.
    ServerCommand("exec %s", mapconfig);
    
    // Log action.
    LogPrintToLog(LOG_FORMAT_TYPE_NORMAL, "Config", "Map Configs", "Executed post map config file: %s", path);
}

/**
 * Used by modules that rely on configs to register their config file info.
 *
 * @param file      Config file entry to register.
 * @param loaded    True if the config should be loaded, false if not.
 * @param path      (Optional) Full path to config file.
 * @param alias     (Optional) Config file alias, used for client interaction.
 */
stock ConfigRegisterConfig(ConfigFile:file, bool:loaded, Function:reloadfunc, Handle:filehandle = INVALID_HANDLE, const String:path[] = "", const String:alias[] = "")
{
    // Copy file info to data container.
    g_ConfigData[file][ConfigLoaded] = loaded;
    g_ConfigData[file][ConfigHandle] = filehandle;
    g_ConfigData[file][ConfigReloadFunc] = reloadfunc;
    strcopy(g_ConfigData[file][ConfigPath], PLATFORM_MAX_PATH, path);
    strcopy(g_ConfigData[file][ConfigAlias], CONFIG_MAX_LENGTH, alias);
}

/**
 * Set the loaded state of a config file entry.
 * 
 * @param config    Config file to set load state of.
 * @param loaded    True to set as loaded, false to set as unloaded.
 */
stock ConfigSetConfigLoaded(ConfigFile:config, bool:loaded)
{
    // Set load state.
    g_ConfigData[config][ConfigLoaded] = loaded;
}

/**
 * Set the reload function of a config file entry.
 * 
 * @param config        Config file to set reload function of.
 * @param reloadfunc    Reload function.
 */
stock ConfigSetConfigReloadFunc(ConfigFile:config, Function:reloadfunc)
{
    // Set reload function.
    g_ConfigData[config][ConfigReloadFunc] = reloadfunc;
}

/**
 * Set the file handle of a config file entry.
 * 
 * @param config    Config file to set handle of.
 * @param loaded    Config file handle.
 */
stock ConfigSetConfigHandle(ConfigFile:config, Handle:file)
{
    // Set file handle.
    g_ConfigData[config][ConfigHandle] = file;
}

/**
 * Set the config file path of a config file entry.
 * 
 * @param config    Config file to set file path of.
 * @param loaded    File path.
 */
stock ConfigSetConfigPath(ConfigFile:config, const String:path[])
{
    // Set config file path.
    strcopy(g_ConfigData[config][ConfigPath], PLATFORM_MAX_PATH, path);
}

/**
 * Set the alias of a config file entry.
 * 
 * @param config    Config file to set alias of.
 * @param loaded    Alias of the config file entry.
 */
stock ConfigSetConfigAlias(ConfigFile:config, const String:alias[])
{
    // Set config alias.
    strcopy(g_ConfigData[config][ConfigAlias], CONFIG_MAX_LENGTH, alias);
}

/**
 * Returns if a config was successfully loaded.
 * 
 * @param config    Config file to check load status of.
 * @return          True if config is loaded, false otherwise.
 */
stock bool:ConfigIsConfigLoaded(ConfigFile:config)
{
    // Return load status.
    return g_ConfigData[config][ConfigLoaded];
}

/**
 * Returns config's reload function.
 * 
 * @param config    Config file to get reload function of.
 * @return          Config reload function.
 */
stock Function:ConfigGetConfigReloadFunc(ConfigFile:config)
{
    // Return load status.
    return g_ConfigData[config][ConfigReloadFunc];
}

/**
 * Returns config's file handle.
 * 
 * @param config    Config file to get file handle of.
 * @return          Config file handle.
 */
stock Handle:ConfigGetConfigHandle(ConfigFile:config)
{
    // Return load status.
    return g_ConfigData[config][ConfigHandle];
}

/**
 * Returns the path for a given config file entry.
 * 
 * @param config    Config file to get path of. (see ConfigFile enum)
 */
stock ConfigGetConfigPath(ConfigFile:config, String:path[], maxlen)
{
    // Copy path to return string.
    strcopy(path, maxlen, g_ConfigData[config][ConfigPath]);
}

/**
 * Returns the alias for a given config file entry.
 * 
 * @param config    Config file to get alias of. (see ConfigFile enum)
 */
stock ConfigGetConfigAlias(ConfigFile:config, String:alias[], maxlen)
{
    // Copy alias to return string.
    strcopy(alias, maxlen, g_ConfigData[config][ConfigAlias]);
}

/**
 * Reload a config file.
 * 
 * @param config    The config file entry to reload.
 * @return          True if the config is loaded, false if not.
 */
stock bool:ConfigReloadFile(ConfigFile:config)
{
    // If file isn't loaded, then stop.
    new bool:loaded = ConfigIsConfigLoaded(config);
    if (!loaded)
    {
        return false;
    }
    
    // Call reload function
    new Function:reloadfunc = ConfigGetConfigReloadFunc(config);
    
    if (reloadfunc == INVALID_FUNCTION)
    {
        // Get config alias.
        decl String:configalias[CONFIG_MAX_LENGTH];
        ConfigGetConfigAlias(config, configalias, sizeof(configalias));
        
        // Print reload failure to logs.
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Config", "Reload Function", "Invalid reload function for config: \"%s\"", configalias);
        
        return true;
    }
    
    // Call reload function.
    Call_StartFunction(GetMyHandle(), reloadfunc);
    Call_PushCell(config);
    Call_Finish();
    
    return true;
}

/**
 * Load config file.
 * 
 * @param file  The cvar define of the path to the file.
 * @return      True if the file exists, false if not.
 */
stock bool:ConfigGetCvarFilePath(CvarsList:cvar, String:path[])
{
    // Get cvar's path.
    decl String:filepath[PLATFORM_MAX_PATH];
    GetConVarString(Handle:g_hCvarsList[cvar], filepath, sizeof(filepath));
    
    // Build full path in return string.
    BuildPath(Path_SM, path, PLATFORM_MAX_PATH, filepath);
    
    return FileExists(path);
}

/**
 * Finds a config file entry, (see ConfigFile enum) for a given alias.
 * 
 * @param alias     The alias to find config file entry of.
 * @return          Config file entry, ConfigInvalid is returned if alias was not found.
 */
stock ConfigFile:ConfigAliasToConfigFile(const String:alias[])
{
    decl String:checkalias[CONFIG_MAX_LENGTH];
    
    // x = config file entry index.
    for (new x = 0; x < sizeof(g_ConfigData); x++)
    {
        // Get config alias.
        ConfigGetConfigAlias(ConfigFile:x, checkalias, sizeof(checkalias));
        
        // If alias doesn't match, then stop.
        if (!StrEqual(alias, checkalias, false))
        {
            continue;
        }
        
        // Return config file entry.
        return ConfigFile:x;
    }
    
    // Invalid config file.
    return ConfigInvalid;
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
stock bool:ConfigKeyvalueTreeSetting(config, ConfigKeyvalueAction:action = ConfigKVCreate, const String:keys[][], keysMax, const String:setting[] = "", String:value[] = "", maxlen = 0)
{
    // Retrieve handle of the keyvalue tree.
    new Handle:hConfig = ConfigGetConfigHandle(config);
    
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
        case ConfigKVCreate:
        {
            if (!setting[0] || !value[0])
            {
                // We created the key already, so return true.
                return true;
            }
            
            // Set new value.
            KvSetString(hConfig, setting, value);
        }
        case ConfigKVDelete:
        {
            // Return deletion result.
            return KvDeleteKey(hConfig, setting);
        }
        case ConfigKVSet:
        {
            // Set new value.
            KvSetString(hConfig, setting, value);
        }
        case ConfigKVGet:
        {
            // Get current value.
            KvGetString(hConfig, setting, value, maxlen);
        }
    }
    
    // We successfully set or got the value.
    return true;
}

/**
 * Command callback (zr_reloadconfig)
 * Reloads a config file and forwards event to modules.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ConfigReloadCommand(client, argc)
{
    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "Config command reload syntax", CONFIG_FILE_ALIAS_MODELS, CONFIG_FILE_ALIAS_DOWNLOADS, CONFIG_FILE_ALIAS_CLASSES, CONFIG_FILE_ALIAS_WEAPONS, CONFIG_FILE_ALIAS_WEAPONGROUPS, CONFIG_FILE_ALIAS_HITGROUPS);
        return Plugin_Handled;
    }
    
    // arg1 = file alias being reloaded.
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    // If alias is invalid, then stop.
    new ConfigFile:config = ConfigAliasToConfigFile(arg1);
    if (config == ConfigInvalid)
    {
        TranslationReplyToCommand(client, "Config command reload invalid", arg1);
        return Plugin_Handled;
    }
    
    // Reload config file.
    new bool:loaded = ConfigReloadFile(config);
    
    // Get config file path.
    decl String:path[PLATFORM_MAX_PATH];
    ConfigGetConfigPath(config, path, sizeof(path));
    
    // If file isn't loaded then tell client, then stop.
    if (!loaded)
    {
        TranslationReplyToCommand(client, "Config command reload not loaded", path);
        return Plugin_Handled;
    }
    
    return Plugin_Handled;
}

/**
 * Command callback (zr_reloadconfigall)
 * Reloads all config files and forwards event to all modules.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ConfigReloadAllCommand(client, argc)
{
    // Begin statistics.
    TranslationReplyToCommand(client, "Config command reload all stats begin");
    
    decl String:configalias[CONFIG_MAX_LENGTH];
    
    // x = config file entry index.
    for (new x = 0; x < sizeof(g_ConfigData); x++)
    {
        // Reload config file.
        new bool:successful = ConfigReloadFile(ConfigFile:x);
        
        // Get config's alias.
        ConfigGetConfigAlias(ConfigFile:x, configalias, sizeof(configalias));
        
        if (successful)
        {
            TranslationReplyToCommand(client, "Config command reload all stats successful", configalias);
        }
        else
        {
            TranslationReplyToCommand(client, "Config command reload all stats failed", configalias);
        }
    }
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
