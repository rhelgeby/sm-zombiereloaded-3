/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          log.h.inc
 *   Type:          Core
 *   Description:   Log header. Types and defines.
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
 * @section Log flags.
 */
#define LOG_CORE_EVENTS             (1 << 0)    /** Log events from the plugin core like config validation and other messages. */
#define LOG_GAME_EVENTS             (1 << 1)    /** Log admin commands, console commands, and game related events from modules like suicide attempts and weapon restrictions. */
#define LOG_PLAYER_COMMANDS         (1 << 2)    /** Log events that are triggered by players, like chat triggers, teleporting and class change. */
#define LOG_DEBUG                   (1 << 3)    /** Log debug messages, if any. Usually only developers enable this log flag. */
#define LOG_DEBUG_DETAIL            (1 << 4)    /** Log additional debug messages with more detail. May cause spam depending on filter settings. Usually only developers enable this log flag. */
/**
 * @endsection
 */

/**
 * Log format types.
 */
enum LogTypes
{
    LogType_Normal = 0,     // Normal log message. Printed in SourceMod logs.
    LogType_Error,          // Error message. Printed in SourceMod error logs.
    LogType_Fatal           // Fatal error. Stops the plugin with the specified message.
}

/**
 * List of modules that write log events. Add new modules if needed (in
 * alphabetical order).
 * 
 * Update following when adding modules:
 * - Admin log flag menu
 * - Command_LogList
 * - LogGetModuleNameString
 */
enum LogModules
{
    bool:LogModule_Account,
    bool:LogModule_Antistick,
    bool:LogModule_Config,
    bool:LogModule_Cvars,
    bool:LogModule_Damage,
    bool:LogModule_Downloads,
    bool:LogModule_Hitgroups,
    bool:LogModule_Infect,
    bool:LogModule_Models,
    bool:LogModule_Playerclasses,
    bool:LogModule_Soundeffects,
    bool:LogModule_Tools,
    bool:LogModule_Volfetures,
    bool:LogModule_Weapons,
    bool:LogModule_Weaponrestrict
}

/**
 * Handle for dynamic string array for module filtering.
 */
new Handle:LogModuleFilter;

/**
 * Cache of current module filter settings. For fast and easy access.
 */
new LogModuleFilterCache[LogModules];
