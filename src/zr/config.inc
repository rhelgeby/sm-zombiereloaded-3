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
#define CONFIG_FILE_ALIAS_HITGROUPS     "hitgroups"
/**
 * @endsection
 */

/**
 * List of config formats used by the plugin.
 */
enum ConfigStructure
{
    Structure_List,     /** Config is structured as a simple list of strings. */
    Structure_Keyvalue, /** Config is a keyvalue structure */
}

/**
 * List of config files used by the plugin.
 */
enum ConfigFile
{
    File_Invalid = -1,  /** Invalid config file. */
    File_Models,        /** <sourcemod root>/configs/zr/models.txt (default) */
    File_Downloads,     /** <sourcemod root>/configs/zr/downloads.txt (default) */
    File_Classes,       /** <sourcemod root>/configs/zr/playerclasses.txt (default) */
    File_Weapons,       /** <sourcemod root>/configs/zr/weapons.txt (default) */
    File_Hitgroups,     /** <sourcemod root>/configs/zr/hitgroups.txt (default) */
}

/**
 * Data container for each config file.
 */
enum ConfigData
{
    bool:               Data_Loaded,                    /** True if config is loaded, false if not. */
    ConfigStructure:    Data_Structure,                 /** Format of the config */
    Function:           Data_ReloadFunc,                /** Function to call to reload config. */
    Handle:             Data_Handle,                    /** Handle of the config file. */
    String:             Data_Path[PLATFORM_MAX_PATH],   /** Full path to config file. */
    String:             Data_Alias[CONFIG_MAX_LENGTH],  /** Config file alias, used for client interaction. */
}

/**
 * Stores all config data.
 */
new g_ConfigData[ConfigFile][ConfigData];

/**
 * Actions to use when working on key/values.
 */
enum ConfigKvAction
{
    KvAction_Create, /** Create a key. */
    KvAction_KVDelete, /** Delete a key. */
    KvAction_KVSet,    /** Modify setting of a key. */
    KvAction_KVGet,    /** Get setting of a key. */
}

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
 * (Don't forget to set 'loaded' to 'true' (ConfigSetConfigLoaded) in config load function)
 *
 * @param file      Config file entry to register.
 * @param alias     Config file alias, used for client interaction.
 */
stock ConfigRegisterConfig(ConfigFile:file, ConfigStructure:structure, const String:alias[] = "")
{
    // Copy file info to data container.
    g_ConfigData[file][Data_Loaded] = false;
    g_ConfigData[file][Data_Structure] = structure;
    g_ConfigData[file][Data_Handle] = INVALID_HANDLE;
    g_ConfigData[file][Data_ReloadFunc] = INVALID_FUNCTION;
    strcopy(g_ConfigData[file][Data_Path], PLATFORM_MAX_PATH, "");
    strcopy(g_ConfigData[file][Data_Alias], CONFIG_MAX_LENGTH, alias);
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
    g_ConfigData[config][Data_Loaded] = loaded;
}

/**
 * Set the structure type of a config file entry.
 * 
 * @param config    Config file to set structure type of.
 * @param structure Structure to set as.
 */
stock ConfigSetConfigStructure(ConfigFile:config, ConfigStructure:structure)
{
    // Set load state.
    g_ConfigData[config][Data_Structure] = structure;
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
    g_ConfigData[config][Data_ReloadFunc] = reloadfunc;
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
    g_ConfigData[config][Data_Handle] = file;
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
    strcopy(g_ConfigData[config][Data_Path], PLATFORM_MAX_PATH, path);
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
    strcopy(g_ConfigData[config][Data_Alias], CONFIG_MAX_LENGTH, alias);
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
    return g_ConfigData[config][Data_Loaded];
}

/**
 * Returns config's structure type.
 * 
 * @param config    Config file to get structure type of.
 * @return          Config structure type.
 */
stock ConfigStructure:ConfigGetConfigStructure(ConfigFile:config)
{
    // Return load status.
    return g_ConfigData[config][Data_Structure];
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
    return g_ConfigData[config][Data_ReloadFunc];
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
    return g_ConfigData[config][Data_Handle];
}

/**
 * Returns the path for a given config file entry.
 * 
 * @param config    Config file to get path of. (see ConfigFile enum)
 */
stock ConfigGetConfigPath(ConfigFile:config, String:path[], maxlen)
{
    // Copy path to return string.
    strcopy(path, maxlen, g_ConfigData[config][Data_Path]);
}

/**
 * Returns the alias for a given config file entry.
 * 
 * @param config    Config file to get alias of. (see ConfigFile enum)
 */
stock ConfigGetConfigAlias(ConfigFile:config, String:alias[], maxlen)
{
    // Copy alias to return string.
    strcopy(alias, maxlen, g_ConfigData[config][Data_Alias]);
}

/**
 * Loads a config file and sets up a nested array type data storage.
 * 
 * @param config        The config file to load.
 * @param arrayConfig   Handle of the main array containing file data.
 * @return              True if file was loaded successfuly, false otherwise.
 */
stock bool:ConfigLoadConfig(ConfigFile:config, &Handle:arrayConfig)
{
    // Get config's structure.
    new ConfigStructure:structure = ConfigGetConfigStructure(config);
    
    // Get config's alias
    decl String:configalias[CONFIG_MAX_LENGTH];
    ConfigGetConfigAlias(config, configalias, sizeof(configalias));
    
    // Get config's file path.
    decl String:configpath[PLATFORM_MAX_PATH];
    ConfigGetConfigPath(config, configpath, sizeof(configpath));
    
    // If handle is still open, then close it before creating a new one.
    if (arrayConfig != INVALID_HANDLE)
    {
        CloseHandle(arrayConfig);
    }
    
    // Create array in handle.
    arrayConfig = CreateArray(CONFIG_MAX_LENGTH);
    
    switch(structure)
    {
        case Structure_List:
        {
            // Open file.
            new Handle:hFile;
            new success = ConfigOpenConfigFile(config, hFile);
            
            // If config file failed to open, then stop.
            if (!success)
            {
                return false;
            }
            
            // Clear out array.
            ClearArray(arrayConfig);
            
            decl String:line[PLATFORM_MAX_PATH];
            
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
                PushArrayString(arrayConfig, line);
            }
            
            // We're done this file, so now we can destory it from memory. 
            CloseHandle(hFile);
            
            return true;
        }
        case Structure_Keyvalue:
        {
            // Open file.
            new Handle:hKeyvalue;
            new success = ConfigOpenConfigFile(config, hKeyvalue);
            
            // If config file failed to open, then stop.
            if (!success)
            {
                return false;
            }
            
            // Destroy all old data.
            ConfigClearKvArray(arrayConfig);
            
            if (KvGotoFirstSubKey(hKeyvalue))
            {
                do
                {
                    // Create new array to store information for config entry.
                    new Handle:arrayConfigEntry = CreateArray(CONFIG_MAX_LENGTH);
                    
                    // Push the key name into the config entry's array.
                    decl String:keyname[CONFIG_MAX_LENGTH];
                    KvGetSectionName(hKeyvalue, keyname, sizeof(keyname));
                    
                    PushArrayString(arrayConfigEntry, keyname); // Index: 0
                    
                    // Store this handle in the main array.
                    PushArrayCell(arrayConfig, arrayConfigEntry);
                } while(KvGotoNextKey(hKeyvalue));
            }
            
            // We're done this file for now, so now we can destory it from memory. 
            CloseHandle(hKeyvalue);
            
            return true;
        }
    }
    
    return false;
}

/**
 * Reload a config file.
 * 
 * @param config    The config file entry to reload.
 * @return          True if the config is loaded, false if not.
 */
stock bool:ConfigReloadConfig(ConfigFile:config)
{
    // If file isn't loaded, then stop.
    new bool:loaded = ConfigIsConfigLoaded(config);
    if (!loaded)
    {
        return false;
    }
    
    // Call reload function
    new Function:reloadfunc = ConfigGetConfigReloadFunc(config);
    
    // This should never be true unless someone has tampered with the code.
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
    Call_Finish();
    
    return true;
}

/**
 * Opens a config file with appropriate method.
 * 
 * @param config    The config file.
 * @param structure The structure of the config file.
 * @param hConfig   The handle of the opened file.
 */
stock bool:ConfigOpenConfigFile(ConfigFile:config, &Handle:hConfig)
{
    // Get config's structure
    new ConfigStructure:structure = ConfigGetConfigStructure(config);
    
    // Get config's file path.
    decl String:configpath[PLATFORM_MAX_PATH];
    ConfigGetConfigPath(config, configpath, sizeof(configpath));
    
    // Get config's alias
    decl String:configalias[CONFIG_MAX_LENGTH];
    ConfigGetConfigAlias(config, configalias, sizeof(configalias));
    
    switch(structure)
    {
        case Structure_List:
        {
            // Open file.
            hConfig = OpenFile(configpath, "r");
            
            // If file couldn't be opened, then stop.
            if (hConfig == INVALID_HANDLE)
            {
                return false;
            }
            
            return true;
        }
        case Structure_Keyvalue:
        {
            hConfig = CreateKeyValues(configalias);
            return FileToKeyValues(hConfig, configpath);
        }
    }
    
    return false;
}

/**
 * Creates, deletes, sets, or gets any key/setting of any ZR config keyvalue file in memory.
 * Only use when interacting with a command or manipulating single keys/values,
 * using this function everywhere would be EXTREMELY inefficient.
 * 
 * @param config        Config file to modify.
 * @param action        Action to perform on keyvalue tree. (see enum ConfigKeyvalueAction)
 * @param keys          Array containing keys to traverse into.
 * @param keysMax       The size of the 'keys' array.
 * @param setting       (Optional) The name of the setting to modify.
 * @param value         (Optional) The new value to set.
 * @param maxlen        (Optional) The maxlength of the retrieved value.
 * @return              True if the change was made successfully, false otherwise. 
 */
stock bool:ConfigKeyvalueTreeSetting(ConfigFile:config, ConfigKvAction:action = KvAction_Create, const String:keys[][], keysMax, const String:setting[] = "", String:value[] = "", maxlen = 0)
{
    // Get config file's structure.
    new ConfigStructure:structure = ConfigGetConfigStructure(config);
    
    // If the config is any other structure beside keyvalue, then stop.
    if (structure != Structure_Keyvalue)
    {
        return false;
    }
    
    // Retrieve handle of the keyvalue tree.
    new Handle:hConfig;
    new bool:success = ConfigOpenConfigFile(config, hConfig);
    
    // If the file couldn't be opened, then stop.
    if (!success)
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
        new bool:exists = KvJumpToKey(hConfig, keys[x], (action == KvAction_Create));
        
        // If exists is false, then stop.
        if (!exists)
        {
            // Key doesn't exist.
            return false;
        }
    }
    
    switch(action)
    {
        case KvAction_Create:
        {
            if (!setting[0] || !value[0])
            {
                // We created the key already, so return true.
                return true;
            }
            
            // Set new value.
            KvSetString(hConfig, setting, value);
        }
        case KvAction_Delete:
        {
            // Return deletion result.
            return KvDeleteKey(hConfig, setting);
        }
        case KvAction_Set:
        {
            // Set new value.
            KvSetString(hConfig, setting, value);
        }
        case KvAction_Get:
        {
            // Get current value.
            KvGetString(hConfig, setting, value, maxlen);
        }
    }
    
    // We successfully set or got the value.
    return true;
}

/**
 * Destroy all array handles within an array, and clear main array.
 * 
 * @param arrayKv   The array converted from a keyvalue structure.
 */
ConfigClearKvArray(Handle:arrayKv)
{
    //  x = array index
    new size = GetArraySize(arrayKv);
    for (new x = 0; x < size; x++)
    {
        // Destroy nested arrays.
        new Handle:arrayKvKey = GetArrayCell(arrayKv, x);
        CloseHandle(arrayKvKey);
    }
    
    // Now that all data within has been destroyed, we can clear the main array.
    ClearArray(arrayKv);
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
    return File_Invalid;
}

/**
 * Command callback (zr_config_reload)
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
        TranslationReplyToCommand(client, "Config command reload syntax", CONFIG_FILE_ALIAS_MODELS, CONFIG_FILE_ALIAS_DOWNLOADS, CONFIG_FILE_ALIAS_CLASSES, CONFIG_FILE_ALIAS_WEAPONS, CONFIG_FILE_ALIAS_HITGROUPS);
        return Plugin_Handled;
    }
    
    // arg1 = file alias being reloaded.
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    // If alias is invalid, then stop.
    new ConfigFile:config = ConfigAliasToConfigFile(arg1);
    if (config == File_Invalid)
    {
        TranslationReplyToCommand(client, "Config command reload invalid", arg1);
        return Plugin_Handled;
    }
    
    // Reload config file.
    new bool:loaded = ConfigReloadConfig(config);
    
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
 * Command callback (zr_config_reloadall)
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
        new bool:successful = ConfigReloadConfig(ConfigFile:x);
        
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
 * Converts string of "yes" or "no" to a boolean value.
 * 
 * @param option    "yes" or "no" string to be converted.
 * @return          True if string is "yes", false otherwise.
 */
stock bool:ConfigSettingToBool(const String:option[])
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
 * @param maxlen    Length of destination string buffer (wont't be more than 4).
 */
stock ConfigBoolToSetting(bool:bOption, String:option[], maxlen)
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

stock bool:ConfigKvGetStringBool(Handle:kv, const String:key[], const String:defaultvalue[] = "yes")
{
    decl String:value[CONFIG_MAX_LENGTH];
    KvGetString(kv, key, value, sizeof(value), defaultvalue);
    
    return ConfigSettingToBool(value);
}