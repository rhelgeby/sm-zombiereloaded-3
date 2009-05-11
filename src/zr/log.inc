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
#define LOG_FORMAT_TYPE_SIMPLE      0      /** Simple log message. */
#define LOG_FORMAT_TYPE_FULL        1      /** Full log message, printed in normal log. */
#define LOG_FORMAT_TYPE_ERROR       2      /** Full log message, printed in error log. */
#define LOG_FORMAT_TYPE_FATALERROR  3      /** Full log message, stops the plugin and printed in error logs. */
/**
 * @endsection
 */

/**
 * @section Logging flags.
 */
#define LOG_CORE_EVENTS             1           /** Config validation, other core events. */
#define LOG_GAME_EVENTS             2           /** Admin commands, suicide prevention, anticamp kills. */
#define LOG_PLAYER_COMMANDS         4           /** Commands executed by non-admins: zspawn, teleport, class change. */
#define LOG_DEBUG                   8           /** Debug messages. */
#define LOG_DEBUG_DETAIL            16          /** Debug messages with more detail. May cause spam. */
#define LOG_DEBUG_MAX_DETAIL        32          /** Low level debug messages. Causes spam! Only enable for a limited period right before and after testing. */
#define LOG_TO_ADMINS               64          /** Copy kinds of log events to admin chat. */
#define LOG_TO_CLIENT               128         /** Copy all log events related to a player, to the players console. */
#define LOG_IGNORE_CONSOLE          256         /** Don't log messages from the console (client 0). */
#define LOG_MODULES_ENABLED         512         /** Enable module based log control. Module logs overrides previous flags, including debug flags. */
#define LOG_MODULE_CORE             1024        /** The core of the plugin (startup, loading configs, etc.). Not really a module. */
#define LOG_MODULE_COMMANDS         2048        /** commands.inc */
#define LOG_MODULE_CLASSES          4096        /** Class system - playerclasses/ *.inc */
#define LOG_MODULE_ZOMBIE           8192        /** zombie.inc */
#define LOG_MODULE_SAYTRIGGERS      16384       /** sayhooks.inc */
#define LOG_MODULE_AMBIENTSOUNDS    32768       /** ambientsounds.inc */
#define LOG_MODULE_OVERLAYS         65536       /** overlays.inc */
#define LOG_MODULE_TELEPORT         131072      /** teleport.inc */
#define LOG_MODULE_WEAPONS          262144      /** Weapons module - weapons/ *.inc */
#define LOG_MODULE_HITGROUPS        524288      /** hitgroups.inc */
#define LOG_MODULE_ANTICAMP         1048576     /** anticamp.inc */
#define LOG_MODULE_DAMAGE           2097152     /** damage.inc */
#define LOG_MODULE_OFFSETS          4194304     /** offsets.inc */
/*
 * @endsection
 */

/**
 * @section Global handles for modules cvars.
 */
new Handle:g_hLog = INVALID_HANDLE;
new Handle:g_hLogFlags = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Log module init function.
 */
LogInit()
{
    // Create modules cvars.
    g_hLog      =   CreateConVar("zr_log", "1", "");
    g_hLogFlags =   CreateConVar("zr_logflags", "331", "");
    // Old Desc: Logging flags. Log messages to sourcemod logs, server console or client console. Use zr_log_flags to see a list of flags. (0: Disable)
}

/**
 * Logs a formatted message with module and block info depending, on the type.
 *
 * @param client    Specifies the client who triggered the event/command. Use
 *                  -1 for core events like validation, etc.
 * @param module    what module the log event belongs to.
 * @param block     What function or code block the log is triggered from.
 * @param message   Log message. Formatted string.
 * @param type      Optional. What logging type or style to use. Options:
 *                  LOG_FORMAT_TYPE_SIMPLE - Simple, no module or block info.
 *                  LOG_FORMAT_TYPE_FULL - Full, with module and block info, printed in normal log.
 *                  LOG_FORMAT_TYPE_ERROR - Full, printed in error log.
 *                  LOG_FORMAT_TYPE_FATALERROR - Full, stops the plugin.
 * @param any...    Formatting parameters.
 */
LogMessageFormatted(client, const String:module[], const String:block[], const String:message[], type = LOG_FORMAT_TYPE_FULL, any:...)
{
    // If logging is disabled, then stop.
    new bool:log = GetConVarBool(g_hLog);
    if (!log)
    {
        return;
    }
    
    decl String:logtext[LOG_MAX_LENGTH_FILE];
    
    // If client is invalid (console), and console log events are ignored, then stop.
    if (client == 0 && LogCheckFlag(LOG_IGNORE_CONSOLE))
    {
        return;
    }
    
    // Format log text.
    VFormat(logtext, sizeof(logtext), message, 6);
    
    // Format other parameters onto the log text.
    switch (type)
    {
        // Log type is simple.
        case LOG_FORMAT_TYPE_SIMPLE:
        {
            LogMessage(logtext);
        }
        // Log type is full.
        case LOG_FORMAT_TYPE_FULL:
        {
            Format(logtext, sizeof(logtext), "\"%s\" : \"%s\" -- %s", module, block, logtext);
            LogMessage(logtext);
        }
        // Log type is error.
        case LOG_FORMAT_TYPE_ERROR:
        {
            Format(logtext, sizeof(logtext), "\"%s\" : \"%s\" -- %s", module, block, logtext);
            LogError(logtext);
        }
        case LOG_FORMAT_TYPE_FATALERROR:
        {
            Format(logtext, sizeof(logtext), "\"%s\" : \"%s\" -- %s", module, block, logtext);
            SetFailState(logtext);
        }
    }
    
    // If log to admin flag is enabled, then print to admins.
    if (LogCheckFlag(LOG_TO_ADMINS))
    {
        // Print text to admins.
        LogToAdmins(logtext);
    }
    
    // If client is a valid client (but not console), and we log to client's then continue.
    if (ZRIsClientValid(client) && LogCheckFlag(LOG_TO_CLIENT))
    {
        // Set client as translation target.
        SetGlobalTransTarget(client);
        
        // Print to client.
        PrintToConsole(client, "%t %s", "ZR", logtext);
    }
}

LogToAdmins(String:message[])
{
    decl String:buffer[LOG_MAX_LENGTH_CHAT];
    
    // x = client index.
    for (new x = 1; x < MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client isn't an admin, then stop.
        if (!ZRIsClientAdmin(x))
        {
            continue;
        }
        
        // Set client as translation target.
        SetGlobalTransTarget(x);
        
        // Format message to admin, then print.
        Format(buffer, sizeof(buffer), "%t %s", "ZR", message);
        PrintToChat(x, buffer);
    }
}

/**
 * Checks if the zr_logflags cvar has a certain flag.
 * 
 * @param flag  The flag.
 */
bool:LogHasFlag(flag)
{
    // Get log flags.
    new logflags = GetConVarInt(g_hLogFlags);
    
    // Return true if flag is found, false if not.
    return bool:(logflags & flag);
}

/**
 * Check if a log message should be written depending on log flags. If module
 * overrides are enabled only logs with it's module flag set will be logged.
 *
 * @param logtype           Log type flag.
 * @param modulefilter      Specifies what module the log event belongs to.
 * @return                  True if the event should be logged, false otherwise.
 */
bool:LogCheckFlag(logtype, modulefilter = 0)
{
    if (modulefilter && (logtype & LOG_MODULES_ENABLED))
    {
        return bool:(logtype & modulefilter);
    }
    
    return LogHasFlag(logtype);
}

/**
 * Toggles a log flag.
 * 
 * @param flag  The flag to toggle.
 */
LogToggleFlag(flag)
{
    // Get current flags
    new logflags = GetConVarInt(g_hLogFlags);
    
    // If cvar contains flag, then remove it.
    if (logflags & flag)
    {
        logflags = logflags - flag;
    }
    // If cvar doesn't have the flag, then add it.
    else
    {
        logflags = logflags + flag;
    }
    
    // Set new value to logflags cvar.
    SetConVarInt(g_hLogFlags, logflags);
}
