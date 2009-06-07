/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          log.inc
 *   Type:          Core
 *   Description:   Logging API.
 *
 * ============================================================================
 */

/*
 * Note: See log.h.inc for header types and defines.
 */


/**
 * Gets a module type as a human readable string.
 *
 * @param buffer    Destination string buffer.
 * @param maxlen    Size of destination buffer.
 * @param module    Module type to convert.
 *
 * @return      Number of cells written.
 */
LogGetModuleNameString(String:buffer[], maxlen, LogModules:module)
{
    switch (module)
    {
        case LogModule_Account:
        {
            return strcopy(buffer, maxlen, "Account");
        }
        case LogModule_Antistick:
        {
            return strcopy(buffer, maxlen, "Anti-Stick");
        }
        case LogModule_Config:
        {
            return strcopy(buffer, maxlen, "Config");
        }
        case LogModule_Cvars:
        {
            return strcopy(buffer, maxlen, "CVARs");
        }
        case LogModule_Damage:
        {
            return strcopy(buffer, maxlen, "Damage");
        }
        case LogModule_Downloads:
        {
            return strcopy(buffer, maxlen, "Downloads");
        }
        case LogModule_Hitgroups:
        {
            return strcopy(buffer, maxlen, "Hit Groups");
        }
        case LogModule_Infect:
        {
            return strcopy(buffer, maxlen, "Infect");
        }
        case LogModule_Models:
        {
            return strcopy(buffer, maxlen, "Models");
        }
        case LogModule_Playerclasses:
        {
            return strcopy(buffer, maxlen, "Player Classes");
        }
        case LogModule_Soundeffects:
        {
            return strcopy(buffer, maxlen, "Sound Effects");
        }
        case LogModule_Tools:
        {
            return strcopy(buffer, maxlen, "Tools");
        }
        case LogModule_Volfetures:
        {
            return strcopy(buffer, maxlen, "Volumetric Features");
        }
        case LogModule_Weapons:
        {
            return strcopy(buffer, maxlen, "Weapons");
        }
        case LogModule_Weaponrestrict:
        {
            return strcopy(buffer, maxlen, "Weapon Restrictions");
        }
    }
    
    // Module mismatch.
    return 0;
}

/**
 * Check if the specified log flag is set.
 *
 * @param eventType     The log flag to check.
 * @return              True if set, false otherwise.
 */
bool:LogCheckFlag(eventType)
{
    // Check if eventType is set.
    if (GetConVarInt(g_hCvarsList[CVAR_LOG_FLAGS]) & eventType)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Check if the specified module is enabled in the log module filter cache.
 *
 * @param module        Module to check.
 * @return              True if enabled, false otherwise. 
 */
bool:LogCheckModuleFilter(LogModules:module)
{
    if (LogModuleFilterCache[module])
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Print a formatted message to logs depending on log settings.
 *
 * @param isConsole     Optional. Specifies whether the log event came from
 *                      client 0. Used in console commands, do not mix with
 *                      regular log events. Default is false.
 * @param logType       Optional. Log type and action. Default is
 *                      LogType_Normal.
 * @param eventType     Optional. A log flag describing What kind of log event
 *                      it is. Default is LOG_CORE_EVENTS.
 * @param module        Module the log event were executed in.
 * @param description   Event type or function name. A short descriptive phrase
 *                      to group together similar logs.
 * @param text          Log message. Can be formatted.
 * @param ...           Formatting parameters.
 */
LogEvent(bool:isConsole = false, LogTypes:logType = LogType_Normal, eventType = LOG_CORE_EVENTS, LogModules:module, const String:description[], const String:text[], any:...)
{
    // Only do filter and flag checks if the log type isn't a fatal error.
    if (logType != LogType_Fatal)
    {
        // Check if logging is disabled.
        if (!GetConVarBool(g_hCvarsList[CVAR_LOG]))
        {
            return;
        }
        
        // Check if console is ignored.
        if (isConsole && GetConVarBool(g_hCvarsList[CVAR_LOG_IGNORE_CONSOLE]))
        {
            return;
        }
        
        // Check event type (log flag).
        if (!LogCheckFlag(eventType))
        {
            return;
        }
        
        // Check if module filtering is enabled.
        if (GetConVarBool(g_hCvarsList[CVAR_LOG_MODULE_FILTER]))
        {
            // Check if the specified module is enabled.
            if (!LogCheckModuleFilter(module))
            {
                return;
            }
        }
    }
    
    // Format extra parameters into the log buffer.
    decl String:logbuffer[LOG_MAX_LENGTH_FILE];
    VFormat(logbuffer, sizeof(logbuffer), text, 7);
    
    // Get human readable module name.
    new String:modulename[64];
    LogGetModuleNameString(modulename, sizeof(modulename), module);
    
    // Format 
    Format(logbuffer, sizeof(logbuffer), "[%s] - [%s]: %s", modulename, description, logbuffer);
    
    // Format other parameters onto the log text.
    switch (logType)
    {
        // Log type is normal.
        case LogType_Normal:
        {
            LogMessage(logbuffer);
        }
        // Log type is error.
        case LogType_Error:
        {
            LogError(logbuffer);
        }
        // Log type is fatal error.
        case LogType_Fatal:
        {
            SetFailState(logbuffer);
        }
    }
    
    // Note: The phrase "Literal text" is a blank phrase to pass any string we want into it.
    
    // Check if printing log events to admins is enabled.
    if (GetConVarBool(g_hCvarsList[CVAR_LOG_PRINT_ADMINS]))
    {
        // Print text to admins.
        TranslationPrintToChatAll(false, true, "Literal text", logbuffer);
    }
    
    // Check if printing log events to public chat is enabled.
    if (GetConVarBool(g_hCvarsList[CVAR_LOG_PRINT_CHAT]))
    {
        TranslationPrintToChatAll(false, false, "Literal text", logbuffer);
    }
}

/**
 * Creates commands for logging module. Called when commands are created.
 */
LogOnCommandsCreate()
{
    RegConsoleCmd("zr_log_list", Command_LogList, "List available logging flags and modules with their status values.");
}

/**
 * Handles the zr_log_list command. Displays flags and module filter cache.
 *
 * @param client    The client that executed the command.
 * @param argc      Number of arguments passed.
 */
public Action:Command_LogList(client, argc)
{
    decl String:buffer[2048];
    decl String:linebuffer[96];
    decl String:module[64];
    
    buffer[0] = 0;
    
    // Set language.
    SetGlobalTransTarget(client);
    
    // Log flags:
    Format(linebuffer, sizeof(linebuffer), "%-19t %-7t %t\n", "Log Generic Flag", "Log Value", "Log Status");
    StrCat(buffer, sizeof(buffer), linebuffer);
    StrCat(buffer, sizeof(buffer), "--------------------------------------------------------------------------------\n");
    
    Format(linebuffer, sizeof(linebuffer), "LOG_CORE_EVENTS     1       %t\n", LogCheckFlag(LOG_CORE_EVENTS) ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "LOG_GAME_EVENTS     2       %t\n", LogCheckFlag(LOG_GAME_EVENTS) ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "LOG_PLAYER_COMMANDS 4       %t\n", LogCheckFlag(LOG_PLAYER_COMMANDS) ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "LOG_DEBUG           8       %t\n", LogCheckFlag(LOG_DEBUG) ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "LOG_DEBUG_DETAIL    16      %t\n\n", LogCheckFlag(LOG_DEBUG_DETAIL) ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    // Module filtering status:
    Format(linebuffer, sizeof(linebuffer), "%t %t\n\n", "Log Module Filtering", GetConVarBool(g_hCvarsList[CVAR_LOG_MODULE_FILTER]) ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%-23t %t\n", "Log Module", "Log Status");
    StrCat(buffer, sizeof(buffer), linebuffer);
    StrCat(buffer, sizeof(buffer), "--------------------------------------------------------------------------------\n");
    
    // Module status:
    LogGetModuleNameString(module, sizeof(module), LogModule_Account);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Account] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Antistick);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Antistick] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Config);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Config] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Cvars);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Cvars] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Damage);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Damage] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Downloads);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Downloads] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Hitgroups);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Hitgroups] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Infect);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Infect] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Models);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Models] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Playerclasses);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Playerclasses] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Soundeffects);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Soundeffects] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Tools);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Tools] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Volfetures);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Volfetures] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Weapons);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Weapons] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    LogGetModuleNameString(module, sizeof(module), LogModule_Weaponrestrict);
    Format(linebuffer, sizeof(linebuffer), "%-19s %t\n", module, LogModuleFilterCache[LogModule_Weaponrestrict] ? "On" : "Off");
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    ReplyToCommand(client, buffer);
    
    return Plugin_Handled;
}
