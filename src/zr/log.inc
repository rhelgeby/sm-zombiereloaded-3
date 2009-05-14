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

/**
 * @section Log message max lengths.
 */
#define LOG_MAX_LENGTH_FILE 2048
#define LOG_MAX_LENGTH_CHAT 192
/**
 * @endsection
 */

/**
 * @section Log format types
 */
#define LOG_FORMAT_TYPE_NORMAL      1      /** Printed in normal log. */
#define LOG_FORMAT_TYPE_DEBUG       2      /** Printed in normal log, flagged as debug. */
#define LOG_FORMAT_TYPE_ERROR       3      /** Printed in error log. */
#define LOG_FORMAT_TYPE_FATALERROR  4      /** Stops the plugin + LOG_FORMAT_TYPE_ERROR */
/**
 * @endsection
 */

/**
 * Print a formatted message to logs or error logs.
 *
 * @param type          (Optional) Logging type. (See LOG_FORMAT_TYPE_* defines)
 * @param module        Module the log belongs to.
 * @param description   Short descriptive phrase to group together similar logs.
 * @param text          Text to print to log.
 * @param ...           Formatting parameters.
 */
LogPrintToLog(type = LOG_FORMAT_TYPE_NORMAL, const String:module[], const String:description[], const String:text[], any:...)
{
    // If logging is disabled, then stop.
    new bool:log = GetConVarBool(g_hCvarsList[CVAR_LOG]);
    if (!log)
    {
        return;
    }
    
    // If module is filtered, then stop.
    decl String:filtermodules[256];
    GetConVarString(g_hCvarsList[CVAR_LOG_FILTER_MODULES], filtermodules, sizeof(filtermodules));
    
    if (StrContains(filtermodules, module, false) > -1)
    {
        return;
    }
    
    // If description is filtered, then stop.
    decl String:filterdescription[256];
    GetConVarString(g_hCvarsList[CVAR_LOG_FILTER_DESCRIPTION], filterdescription, sizeof(filterdescription));
    
    if (StrContains(filterdescription, description, false) > -1)
    {
        return;
    }
    
    // If debug is disabled, then stop.
    new bool:filterdebug = GetConVarBool(g_hCvarsList[CVAR_LOG_FILTER_DEBUG]);
    if (filterdebug && type == LOG_FORMAT_TYPE_DEBUG)
    {
        return;
    }
    
    // Format extra parameters into the log buffer.
    decl String:logbuffer[LOG_MAX_LENGTH_FILE];
    VFormat(logbuffer, sizeof(logbuffer), text, 5);
    
    // Format 
    Format(logbuffer, sizeof(logbuffer), "[%s]|[%s]: %s", module, description, logbuffer);
    
    // Format other parameters onto the log text.
    switch (type)
    {
        // Log type is normal.
        case LOG_FORMAT_TYPE_NORMAL:
        {
            LogMessage(logbuffer);
        }
        // Log type is error.
        case LOG_FORMAT_TYPE_ERROR:
        {
            LogError(logbuffer);
        }
        // Log type is fatal error.
        case LOG_FORMAT_TYPE_FATALERROR:
        {
            SetFailState(logbuffer);
        }
    }
    
    // If print to admin cvar is enabled, then print to all connect admins.
    new bool:printadmins = GetConVarBool(g_hCvarsList[CVAR_LOG_PRINT_ADMINS]);
    if (printadmins)
    {
        // Print text to admins.
        // Note: The phrase "Literal text" is a blank phrase to pass any string we want into it.
        
        new bool:printchat = GetConVarBool(g_hCvarsList[CVAR_LOG_PRINT_CHAT]);
        if (printchat)
        {
            TranslationPrintToChatAll(false, true, "Literal text", logbuffer);
        }
        
        new bool:printconsole = GetConVarBool(g_hCvarsList[CVAR_LOG_PRINT_CONSOLE]);
        if (printconsole)
        {
            TranslationPrintToConsoleAll(false, true, "Literal text", logbuffer);
        }
    }
}
