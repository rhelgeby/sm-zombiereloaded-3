/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          cvars.inc
 *   Type:          Core
 *   Description:   Config creation and cvar control.
 *
 * ============================================================================
 */

/**
 * @section Locked cvar value defines.
 */
#define CVARS_AUTOTEAMBALANCE_LOCKED 0
#define CVARS_LIMITTEAMS_LOCKED 0
/**
 * @endsection
 */

/**
 * List of cvars used by the plugin.
 */
enum CvarsList
{
    Handle:CVAR_ENABLE,
    Handle:CVAR_LOG,
    Handle:CVAR_LOG_LOAD,
    Handle:CVAR_LOG_CORE,
    Handle:CVAR_LOG_FILTER_MODULES,
    Handle:CVAR_LOG_FILTER_DESCRIPTION,
    Handle:CVAR_LOG_FILTER_DEBUG,
    Handle:CVAR_LOG_PRINT_ADMINS,
    Handle:CVAR_LOG_PRINT_CHAT,
    Handle:CVAR_LOG_PRINT_CONSOLE,
    Handle:CVAR_CONFIG_PATH_MODELS,
    Handle:CVAR_CONFIG_PATH_DOWNLOADS,
    Handle:CVAR_CONFIG_PATH_CLASSES,
    Handle:CVAR_CONFIG_PATH_WEAPONS,
    Handle:CVAR_CONFIG_PATH_WEAPONGROUPS,
    Handle:CVAR_CONFIG_PATH_HITGROUPS,
    Handle:CVAR_CLASSES_SPAWN,
    Handle:CVAR_CLASSES_RANDOM,
    Handle:CVAR_CLASSES_DEFAULT_ZOMBIE,
    Handle:CVAR_CLASSES_DEFAULT_HUMAN,
    Handle:CVAR_CLASSES_DEFAULT_ADMIN,
    Handle:CVAR_CLASSES_MENU_AUTOCLOSE,
    Handle:CVAR_CLASSES_OVERLAY_TOGGLE,
    Handle:CVAR_CLASSES_OVERLAY_TOGGLECMDS,
    Handle:CVAR_CLASSES_OVERLAY_DEFAULT,
    Handle:CVAR_WEAPONS,
    Handle:CVAR_WEAPONS_RESTRICT,
    Handle:CVAR_WEAPONS_ZMARKET_BUYZONE,
    Handle:CVAR_HITGROUPS,
    Handle:CVAR_DAMAGE_HITGROUPS,
    Handle:CVAR_DAMAGE_BLOCK_FF,
    Handle:CVAR_DAMAGE_BLOCK_BLAST,
    Handle:CVAR_DAMAGE_SUICIDE_ZOMBIE,
    Handle:CVAR_DAMAGE_SUICIDE_HUMAN,
    Handle:CVAR_DAMAGE_SUICIDE_CMDS,
    Handle:CVAR_SAYHOOKS_QUIET,
    Handle:CVAR_SAYHOOKS_QUIET_FILTER,
    Handle:CVAR_SAYHOOKS_QUIET_FLAGS,
    Handle:CVAR_OVERLAYS_UPDATE_TIME,
    Handle:CVAR_ROUNDEND_OVERLAY,
    Handle:CVAR_ROUNDEND_OVERLAY_ZOMBIE,
    Handle:CVAR_ROUNDEND_OVERLAY_HUMAN,
    Handle:CVAR_INFECT_SPAWNTIME_MIN,
    Handle:CVAR_INFECT_SPAWNTIME_MAX,
    Handle:CVAR_INFECT_CONSECUTIVE_BLOCK,
    Handle:CVAR_INFECT_WEAPONS_DROP,
    Handle:CVAR_INFECT_MZOMBIE_RATIO,
    Handle:CVAR_INFECT_MZOMBIE_RESPAWN,
    Handle:CVAR_INFECT_SOUND,
    Handle:CVAR_INFECT_ESPLASH,
    Handle:CVAR_INFECT_FIREBALL,
    Handle:CVAR_INFECT_SMOKE,
    Handle:CVAR_INFECT_SPARKS,
    Handle:CVAR_INFECT_SHAKE,
    Handle:CVAR_INFECT_SHAKE_AMP,
    Handle:CVAR_INFECT_SHAKE_FREQUENCY,
    Handle:CVAR_INFECT_SHAKE_DURATION,
    Handle:CVAR_ACCOUNT_CASHFILL,
    Handle:CVAR_ACCOUNT_CASHFILL_VALUE,
    Handle:CVAR_VEFFECTS_LIGHTSTYLE,
    Handle:CVAR_VEFFECTS_LIGHTSTYLE_VALUE,
    Handle:CVAR_VEFFECTS_SKY,
    Handle:CVAR_VEFFECTS_SKY_PATH,
    Handle:CVAR_VEFFECTS_SUN_DISABLE,
    Handle:CVAR_VEFFECTS_FOG,
    Handle:CVAR_VEFFECTS_FOG_OVERRIDE,
    Handle:CVAR_VEFFECTS_FOG_PCOLOR,
    Handle:CVAR_VEFFECTS_FOG_SCOLOR,
    Handle:CVAR_VEFFECTS_FOG_DENSITY,
    Handle:CVAR_VEFFECTS_FOG_STARTDIST,
    Handle:CVAR_VEFFECTS_FOG_ENDDIST,
    Handle:CVAR_VEFFECTS_FOG_FARZ,
    Handle:CVAR_SEFFECTS_MOAN,
    Handle:CVAR_SEFFECTS_GROAN,
    Handle:CVAR_SEFFECTS_DEATH,
    Handle:CVAR_AMBIENTSOUNDS,
    Handle:CVAR_AMBIENTSOUNDS_FILE,
    Handle:CVAR_AMBIENTSOUNDS_LENGTH,
    Handle:CVAR_AMBIENTSOUNDS_VOLUME,
    Handle:CVAR_ANTISTICK,
    Handle:CVAR_ANTISTICK_INTERVAL,
    Handle:CVAR_SPAWNPROTECT,
    Handle:CVAR_SPAWNPROTECT_TIME,
    Handle:CVAR_SPAWNPROTECT_SPEED,
    Handle:CVAR_SPAWNPROTECT_ALPHA,
    Handle:CVAR_RESPAWN,
    Handle:CVAR_RESPAWN_DELAY,
    Handle:CVAR_RESPAWN_TEAM_ZOMBIE,
    Handle:CVAR_RESPAWN_TEAM_ZOMBIE_WORLD,
    Handle:CVAR_JUMPBOOST_BUNNYHOP_PROTECT,
    Handle:CVAR_VOL,
    Handle:CVAR_VOL_UPDATE_INTERVAL,
    Handle:CVAR_VOL_TRIGGER_INTERVAL,
    Handle:CVAR_ZSPAWN,
    Handle:CVAR_ZSPAWN_TEAM_OVERRIDE,
    Handle:CVAR_ZSPAWN_TEAM_ZOMBIE,
    Handle:CVAR_ZSPAWN_TIMELIMIT,
    Handle:CVAR_ZSPAWN_TIMELIMIT_TIME,
    Handle:CVAR_ZTELE_ZOMBIE,
    Handle:CVAR_ZTELE_HUMAN_BEFORE,
    Handle:CVAR_ZTELE_HUMAN_AFTER,
    Handle:CVAR_ZTELE_DELAY_ZOMBIE,
    Handle:CVAR_ZTELE_DELAY_HUMAN,
    Handle:CVAR_ZTELE_MAX_ZOMBIE,
    Handle:CVAR_ZTELE_MAX_HUMAN,
    Handle:CVAR_ZTELE_AUTOCANCEL,
    Handle:CVAR_ZTELE_AUTOCANCEL_DISTANCE,
    Handle:CVAR_ZHP,
    Handle:CVAR_ZHP_DEFAULT,
}

/**
 * Array to store cvar data in.
 */
new g_hCvarsList[CvarsList];

/**
 * @section Global cvar handles.
 */
new Handle:g_hAutoTeamBalance = INVALID_HANDLE;
new Handle:g_hLimitTeams = INVALID_HANDLE;
new Handle:g_hRestartGame = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Cvars module init function.
 */
CvarsInit()
{
    // Retrieve handles of CS:S cvars.
    g_hAutoTeamBalance = FindConVar("mp_autoteambalance");
    g_hLimitTeams = FindConVar("mp_limitteams");
    g_hRestartGame = FindConVar("mp_restartgame");
    
    // Create zombiereloaded cvars.
    CvarsCreate();
    
    // Hook cvars.
    CvarsHook();
    
    // Create public cvar for tracking.
    decl String:description[64];
    Format(description, sizeof(description), "%s Current version of this plugin", TRANSLATION_PHRASE_PREFIX);
    CreateConVar("gs_zombiereloaded_version", VERSION, description, FCVAR_PLUGIN|FCVAR_SPONLY|FCVAR_UNLOGGED|FCVAR_DONTRECORD|FCVAR_REPLICATED|FCVAR_NOTIFY);
    
    // Forward event to modules.
    VEffectsOnCvarInit();
}

/**
 * Create all cvars for plugin.
 */
CvarsCreate()
{
    // ===========================
    // General (Core)
    // ===========================
    g_hCvarsList[CVAR_ENABLE]                    =    CreateConVar("zr_enable", "1", "");
    
    
    // ===========================
    // Log (core)
    // ===========================
    g_hCvarsList[CVAR_LOG]                       =    CreateConVar("zr_log",                    "1",    "Logs key actions performed by the plugin, including errors and debug.");
    g_hCvarsList[CVAR_LOG_FILTER_MODULES]        =    CreateConVar("zr_log_filter_modules",     "",     "If any log module name is within this string, log will be suppressed. [Dependency: zr_log]");
    g_hCvarsList[CVAR_LOG_FILTER_DESCRIPTION]    =    CreateConVar("zr_log_filter_description", "",     "If any log description is within this string, log will be suppressed. [Dependency: zr_log]");
    g_hCvarsList[CVAR_LOG_FILTER_DEBUG]          =    CreateConVar("zr_log_filter_debug",       "1",    "Supresses debug messages. [Dependency: zr_log]");
    g_hCvarsList[CVAR_LOG_PRINT_ADMINS]          =    CreateConVar("zr_log_print_admins",       "0",    "Print all logs to currently connected admins. [Dependency: zr_log]");
    g_hCvarsList[CVAR_LOG_PRINT_CHAT]            =    CreateConVar("zr_log_print_chat",         "1",    "Print log to admin's chat area. [Dependency: zr_log & zr_log_print_admins]");
    g_hCvarsList[CVAR_LOG_PRINT_CONSOLE]         =    CreateConVar("zr_log_print_console",      "1",    "Print log to admin's console. [Dependency: zr_log & zr_log_print_admins]");
    
    
    // ===========================
    // Translations (core)
    // ===========================
    
    // (None)
    
    
    // ===========================
    // Config (core)
    // ===========================
    g_hCvarsList[CVAR_CONFIG_PATH_MODELS]        =    CreateConVar("zr_config_path_models",         "configs/zr/models.txt",                "Path, relative to root sourcemod directory, to models config file.");
    g_hCvarsList[CVAR_CONFIG_PATH_DOWNLOADS]     =    CreateConVar("zr_config_path_downloads",      "configs/zr/downloads.txt",             "Path, relative to root sourcemod directory, to downloads file.");
    g_hCvarsList[CVAR_CONFIG_PATH_CLASSES] =    CreateConVar("zr_config_path_playerclasses",  "configs/zr/playerclasses.txt",         "Path, relative to root sourcemod directory, to playerclasses config file.");
    g_hCvarsList[CVAR_CONFIG_PATH_WEAPONS]       =    CreateConVar("zr_config_path_weapons",        "configs/zr/weapons/weapons.txt",       "Path, relative to root sourcemod directory, to weapons config file.");
    g_hCvarsList[CVAR_CONFIG_PATH_WEAPONGROUPS]  =    CreateConVar("zr_config_path_weapongroups",   "configs/zr/weapons/weapongroups.txt",  "Path, relative to root sourcemod directory, to weapongroups config file.");
    g_hCvarsList[CVAR_CONFIG_PATH_HITGROUPS]     =    CreateConVar("zr_config_path_hitgroups",      "configs/zr/hitgroups.txt",             "Path, relative to root sourcemod directory, to hitgroups config file.");
    
    
    // ===========================
    // Tools (core)
    // ===========================
    
    // (None)
    
    
    // ===========================
    // Models (core)
    // ===========================
    
    // TODO: config file path.
    
    
    // ===========================
    // Classes (core)
    // ===========================
    
    // General
    g_hCvarsList[CVAR_CLASSES_SPAWN]                    =    CreateConVar("zr_classes_spawn",               "0",            "Re-display class selection menu every spawn.");
    g_hCvarsList[CVAR_CLASSES_RANDOM]                   =    CreateConVar("zr_classes_random",              "0",            "Player is assigned a random class every spawn. [Override: zr_classes_spawn & zr_classes_default_*]");
    g_hCvarsList[CVAR_CLASSES_DEFAULT_ZOMBIE]           =    CreateConVar("zr_classes_default_zombie",      "random",       "Zombie class assigned to players on connect. ['random' = Random class | '\"\"' = Class config default]");
    g_hCvarsList[CVAR_CLASSES_DEFAULT_HUMAN]            =    CreateConVar("zr_classes_default_human",       "random",       "Human class assigned to players on connect. ['random' = Random class | '\"\"' = Class config default]");
    g_hCvarsList[CVAR_CLASSES_DEFAULT_ADMIN]            =    CreateConVar("zr_classes_default_admin",       "random",       "Admin class assigned to admins on connect. ['random' = Random class | '\"\"' = Class config default]");
    
    // Menu
    g_hCvarsList[CVAR_CLASSES_MENU_AUTOCLOSE]           =    CreateConVar("zr_classes_menu_autoclose",      "0",            "Automatically close class selection menu.");
    
    // Overlays
    g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLE]           =    CreateConVar("zr_classes_overlay_toggle",      "1",            "Allow players to toggle class overlay.");
    g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLECMDS]       =    CreateConVar("zr_classes_overlay_togglecmds",  "nightvision",  "List of commands to hook that players can use to toggle class overlay. [Dependency: zr_classes_overlay_toggle | Delimiter: \", \"]");
    g_hCvarsList[CVAR_CLASSES_OVERLAY_DEFAULT]          =    CreateConVar("zr_classes_overlay_default",     "0",            "Default class overlay toggle state set on connecting player. [Dependency: zr_classes_overlay_toggle]");
    
    
    // ===========================
    // Weapons (core)
    // ===========================
    
    // General
    g_hCvarsList[CVAR_WEAPONS]                          =    CreateConVar("zr_weapons",                 "1",    "Enable weapons module, disabling this will disable any weapons-related features. (weapon restrictions, weapon knockback multipliers, etc)");
    
    // Restrict
    g_hCvarsList[CVAR_WEAPONS_RESTRICT]                 =    CreateConVar("zr_weapons_restrict",        "1",    "Enable weapon restriction module, disabling this will disable weapon restriction commands.");
    
    // Market Handler
    g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]          =    CreateConVar("zr_weapons_zmarket_buyzone", "1",    "Requires player to be inside a buyzone to use ZMarket.");
    
    
    // ===========================
    // Hitgroups (core)
    // ===========================
    g_hCvarsList[CVAR_HITGROUPS]                        =    CreateConVar("zr_hitgroups",   "1",    "Enable hitgroups module, disabling this will disable hitgroup-related features. (hitgroup knockback multipliers, hitgroup damage control)");
    
    
    // ===========================
    // Infect (core)
    // ===========================
    g_hCvarsList[CVAR_INFECT_MZOMBIE_RATIO]             =    CreateConVar("zr_infect_mzombie_ratio",        "5",                                "Number of mother zombies to infect (when infect timer is up) in proportion to number of humans on the server.");
    g_hCvarsList[CVAR_INFECT_MZOMBIE_RESPAWN]           =    CreateConVar("zr_infect_mzombie_respawn",      "0",                                "Teleport mother zombies back to spawn on infect.");
    g_hCvarsList[CVAR_INFECT_SPAWNTIME_MIN]             =    CreateConVar("zr_infect_spawntime_min",        "30.0",                             "Minimum time from the start of the round until picking the mother zombie(s).");
    g_hCvarsList[CVAR_INFECT_SPAWNTIME_MAX]             =    CreateConVar("zr_infect_spawntime_max",        "50.0",                             "Maximum time from the start of the round until picking the mother zombie(s).");
    g_hCvarsList[CVAR_INFECT_CONSECUTIVE_BLOCK]         =    CreateConVar("zr_infect_consecutive_block",    "1",                                "Prevent a player from being chosen as mother zombie two rounds in a row.");
    g_hCvarsList[CVAR_INFECT_WEAPONS_DROP]              =    CreateConVar("zr_infect_weapons_drop",         "1",                                "Force player to drop all weapons on infect, disabling this will strip weapons instead.");
    
    // Effects
    g_hCvarsList[CVAR_INFECT_FIREBALL]                  =    CreateConVar("zr_infect_fireball",             "1",                                "Spawn a fireball effect around player on infection.");
    g_hCvarsList[CVAR_INFECT_SMOKE]                     =    CreateConVar("zr_infect_smoke",                "1",                                "Spawn a smoke cloud effect around player on infection.");
    g_hCvarsList[CVAR_INFECT_SPARKS]                    =    CreateConVar("zr_infect_sparks",               "1",                                "Emit sparks from player on infection.");
    g_hCvarsList[CVAR_INFECT_SOUND]                     =    CreateConVar("zr_infect_sound",                "npc/fast_zombie/fz_scream1.wav",   "Sound, relative to \"sounds\" folder, to play from player on infection. ['\"\"' = No sound]");
    g_hCvarsList[CVAR_INFECT_ESPLASH]                   =    CreateConVar("zr_infect_esplash",              "1",                                "Emit an energy splash from player on infection.");
    g_hCvarsList[CVAR_INFECT_SHAKE]                     =    CreateConVar("zr_infect_shake",                "1",                                "Shake player's view on infect.");
    g_hCvarsList[CVAR_INFECT_SHAKE_AMP]                 =    CreateConVar("zr_infect_shake_amp",            "15.0",                             "Amplitude of shaking effect. [Dependency: zr_infect_shake]");
    g_hCvarsList[CVAR_INFECT_SHAKE_FREQUENCY]           =    CreateConVar("zr_infect_shake_frequency",      "1.0",                              "Frequency of shaking effect. [Dependency: zr_infect_shake]");
    g_hCvarsList[CVAR_INFECT_SHAKE_DURATION]            =    CreateConVar("zr_infect_shake_duration",       "5.0",                              "Duration of shaking effect. [Dependency: zr_infect_shake]"); 
    
    
    // ===========================
    // Damage (core)
    // ===========================
    
    // Hitgroup Damage
    g_hCvarsList[CVAR_DAMAGE_HITGROUPS]                 =    CreateConVar("zr_damage_hitgroups",        "1",                            "Read hitgroup damage control from hitgroup config file, disabling this allows all zombie hitgroups to be shot.");
    
    // Block Damage Types
    g_hCvarsList[CVAR_DAMAGE_BLOCK_FF]                  =    CreateConVar("zr_damage_block_ff",         "1",                            "Block friendly fire.");
    g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]               =    CreateConVar("zr_damage_block_blast",      "1",                            "Block blast damage inflicted on self or teammates.");
    
    // Suicide Intercept
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_ZOMBIE]            =    CreateConVar("zr_damage_suicide_zombie",   "1",                            "Intercept suicide commands attempted by zombies.");
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_HUMAN]             =    CreateConVar("zr_damage_suicide_human",    "1",                            "Intercept suicide commands attempted by humans.");
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_CMDS]              =    CreateConVar("zr_damage_suicide_cmds",     "kill, spectate, jointeam",     "List of client commands to intercept as suicide attempts. [Delimiter: \", \"]");
    
    
    // ===========================
    // Say Hooks (core)
    // ===========================
    g_hCvarsList[CVAR_SAYHOOKS_QUIET]                   =    CreateConVar("zr_sayhooks_quiet",          "1",    "Stop certain in-game commands from appearing in chat. [See zr_sayhooks_quiet_filter & zr_sayhooks_quiet_flags]");
    g_hCvarsList[CVAR_SAYHOOKS_QUIET_FILTER]            =    CreateConVar("zr_sayhooks_quiet_filter",   "1",    "Stops any failed in-game command from showing in chat.  Ex: Typing !zspawn while alive. [Dependency: zr_sayhooks_quiet]");
    g_hCvarsList[CVAR_SAYHOOKS_QUIET_FLAGS]             =    CreateConVar("zr_sayhooks_quiet_flags",    "58",   "Flag separate in-game commands to always hide from chat. [Dependency: zr_sayhooks_quiet]\n In-Game Command Flags:\n * Add numbers of desired flags together to produce final flag value.\n --------------------------------------------------------------------\n 1:  !zmenu\n 2:  !zadmin\n 4:  !zclass\n 8:  !zspawn\n 16: !ztele\n 32: !zhp\n 64: !zmarket");
    // Flags (default: 2 + 8 + 16 + 32)
    
    
    // ===========================
    // Overlays (core)
    // ===========================
    g_hCvarsList[CVAR_OVERLAYS_UPDATE_TIME]             =    CreateConVar("zr_overlays_update_time",    "1.0",      "How often to update overlays on players. [0.0 = Disabled]");
    
    
    // ===========================
    // Round End (core)
    // ===========================
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY]                 =    CreateConVar("zr_roundend_overlay",            "1",                            "Show specified overlay to players depending on winner when the round ends.");
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY_HUMAN]           =    CreateConVar("zr_roundend_overlays_human",     "overlays/zr/humans_win",       "Overlay, relative to \"materials\" folder, to display when humans win the round. [Dependency: zr_roundend_overlay]");
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY_ZOMBIE]          =    CreateConVar("zr_roundend_overlays_zombie",    "overlays/zr/zombies_win",      "Overlay, relative to \"materials\" folder, to display when zombies win the round. [Dependency: zr_roundend_overlay]");
    
    
    // ===========================
    // Account (module)
    // ===========================
    g_hCvarsList[CVAR_ACCOUNT_CASHFILL]                 =    CreateConVar("zr_account_cashfill",            "1",        "Reset player's cash each spawn.");
    g_hCvarsList[CVAR_ACCOUNT_CASHFILL_VALUE]           =    CreateConVar("zr_account_cashfill_value",      "12000",    "Amount of cash to set player's account to. [Dependency: zr_account_cashfill]");
    
    
    // ===========================
    // Visual Effects (module)
    // ===========================
    
    // Lightstyle
    g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE]              =    CreateConVar("zr_veffects_lightstyle",         "1",                    "Change lightstyle (brightness) of the map.");
    g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE]        =    CreateConVar("zr_veffects_lightstyle_value",   "b",                    "Lightstyle value. ['a' = Darkest | 'z' = Brightest | Dependency: zr_veffects_lightstyle]");
    
    // Sky
    g_hCvarsList[CVAR_VEFFECTS_SKY]                     =    CreateConVar("zr_veffects_sky",                "1",                    "Change map skybox.");
    g_hCvarsList[CVAR_VEFFECTS_SKY_PATH]                =    CreateConVar("zr_veffects_sky_path",           "sky_borealis01up.vmt", "Skybox file, relative to \"materials/skybox\" folder, to change map skybox to. [Dependency: zr_veffects_sky]");
    
    // Sun
    g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE]             =    CreateConVar("zr_veffects_sun_disable",        "1",                    "Disable sun rendering on map.");
    
    // Fog
    g_hCvarsList[CVAR_VEFFECTS_FOG]                     =    CreateConVar("zr_veffects_fog",                "0",                    "(UNSUPPORTED) Enable fog rendering on the map.");
    g_hCvarsList[CVAR_VEFFECTS_FOG_OVERRIDE]            =    CreateConVar("zr_veffects_fog_override",       "0",                    "(UNSUPPORTED) If fog exists already on the map, then replace with new modified fog. [Dependency: zr_veffects_fog]");
    g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR]              =    CreateConVar("zr_veffects_fog_pcolor",         "255 255 255",          "(UNSUPPORTED) Primary color of the fog. [Dependency: zr_veffects_fog]");
    g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR]              =    CreateConVar("zr_veffects_fog_scolor",         "255 255 255",          "(UNSUPPORTED) Secondary color of the fog. [Dependency: zr_veffects_fog]");
    g_hCvarsList[CVAR_VEFFECTS_FOG_DENSITY]             =    CreateConVar("zr_veffects_fog_density",        "0.8",                  "(UNSUPPORTED) Density (thickness) of the fog. [Dependency: zr_veffects_fog]");
    g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST]           =    CreateConVar("zr_veffects_fog_startdist",      "0",                    "(UNSUPPORTED) Distance from player to start rendering foremost fog. [Dependency: zr_veffects_fog]");
    g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST]             =    CreateConVar("zr_veffects_fog_enddist",        "400",                  "(UNSUPPORTED) Distance from player to stop rendering fog. [Dependency: zr_veffects_fog]");
    g_hCvarsList[CVAR_VEFFECTS_FOG_FARZ]                =    CreateConVar("zr_veffects_fog_farz",           "2000",                 "(UNSUPPORTED) Vertical clipping plane.");
    
    
    // ===========================
    // Sound Effects (module)
    // ===========================
    
    // Zombie Sounds
    g_hCvarsList[CVAR_SEFFECTS_MOAN]                    =    CreateConVar("zr_seffects_moan",           "30.0",                         "Time between emission of a moan sound from a zombie.");
    g_hCvarsList[CVAR_SEFFECTS_GROAN]                   =    CreateConVar("zr_seffects_groan",          "5",                            "The probability that a groan sound will be emitted from a zombie when shot. ['100' = 1% chance | '50' = 2% chance | '1' = 100% chance]");
    g_hCvarsList[CVAR_SEFFECTS_DEATH]                   =    CreateConVar("zr_seffects_death",          "1",                            "Emit a death sound when a zombie dies.");
    
    // Ambient Sounds
    g_hCvarsList[CVAR_AMBIENTSOUNDS]                    =    CreateConVar("zr_ambientsounds",           "1",                            "Play an ambient sound to all players during gameplay.");
    g_hCvarsList[CVAR_AMBIENTSOUNDS_FILE]               =    CreateConVar("zr_ambientsounds_file",      "ambient/zr/zr_ambience.mp3",   "Sound file, relative to \"sounds\" folder, to play as ambience. [Dependency: zr_ambientsounds]");
    g_hCvarsList[CVAR_AMBIENTSOUNDS_LENGTH]             =    CreateConVar("zr_ambientsounds_length",    "60.0",                         "Length of the ambient sound. [Dependency: zr_ambientsounds]");
    g_hCvarsList[CVAR_AMBIENTSOUNDS_VOLUME]             =    CreateConVar("zr_ambientsounds_volume",    "0.8",                          "Volume of the ambient sound. [1.0 = Max volume | 0.0001 = Not audible | Dependency: zr_ambientsounds]");
    
    
    // ===========================
    // Antistick (module)
    // ===========================
    g_hCvarsList[CVAR_ANTISTICK]                        =    CreateConVar("zr_antistick",               "1",        "Automatically unstick players when stuck within each others' collision hull.");
    g_hCvarsList[CVAR_ANTISTICK_INTERVAL]               =    CreateConVar("zr_antistick_interval",      "0.5",      "Time between each check for stuck players. [Dependency: zr_antistick]");
    
    
    // ===========================
    // Knockback (module)
    // ===========================
    
    // (None)
    
    
    // ===========================
    // Spawn Protect (module)
    // ===========================
    g_hCvarsList[CVAR_SPAWNPROTECT]                     =    CreateConVar("zr_spawnprotect", "1", "");
    g_hCvarsList[CVAR_SPAWNPROTECT_TIME]                =    CreateConVar("zr_spawnprotect_time", "10", "");
    g_hCvarsList[CVAR_SPAWNPROTECT_SPEED]               =    CreateConVar("zr_spawnprotect_speed", "600.0", "");
    g_hCvarsList[CVAR_SPAWNPROTECT_ALPHA]               =    CreateConVar("zr_spawnprotect_alpha", "0", "");
    
    
    // ===========================
    // Respawn (module)
    // ===========================
    g_hCvarsList[CVAR_RESPAWN]                          =    CreateConVar("zr_respawn",                     "0",    "Respawn players after death. [Recommended: (Enable) zr_zspawn*]");
    g_hCvarsList[CVAR_RESPAWN_DELAY]                    =    CreateConVar("zr_respawn_delay",               "1",    "Time after death to delay player respawn. [Dependency: zr_respawn]");
    g_hCvarsList[CVAR_RESPAWN_TEAM_ZOMBIE]              =    CreateConVar("zr_respawn_team_zombie",         "1",    "Respawn player as a zombie. [Dependency: zr_respawn]");
    g_hCvarsList[CVAR_RESPAWN_TEAM_ZOMBIE_WORLD]        =    CreateConVar("zr_respawn_team_zombie_world",   "1",    "Respawn player as a zombie if player was killed by the world. [Override: zr_respawn_team_zombie]");
    
    
    // ===========================
    // Napalm (module)
    // ===========================
    
    // (None)
    
    
    // ===========================
    // Jump Boost (module)
    // ===========================
    g_hCvarsList[CVAR_JUMPBOOST_BUNNYHOP_PROTECT]       =    CreateConVar("zr_jumpboost_bunnyhop_protect",  "1",    "Prevent players from using forward jump boost multipliers to bunny hop.");
    
    
    // ===========================
    // Volumetric features (module)
    // ===========================
    g_hCvarsList[CVAR_VOL]                              =    CreateConVar("zr_vol",                     "1",    "Enables volumetric features.");
    g_hCvarsList[CVAR_VOL_UPDATE_INTERVAL]              =    CreateConVar("zr_vol_update_interval",     "1.0",  "How often to update player positions and trigger events, in seconds.");
    g_hCvarsList[CVAR_VOL_TRIGGER_INTERVAL]             =    CreateConVar("zr_vol_trigger_interval",    "1.0",  "How often to check for delayed events, in seconds. Use lower values for more precise delays.");
    
    
    // ===========================
    // ZSpawn (module)
    // ===========================
    g_hCvarsList[CVAR_ZSPAWN]                           =    CreateConVar("zr_zspawn",                  "1", "Allow players to spawn into the game late.");
    
    g_hCvarsList[CVAR_ZSPAWN_TEAM_OVERRIDE]             =    CreateConVar("zr_zspawn_team_override",    "1", "Override spawn team when spawning by means of ZSpawn.");
    g_hCvarsList[CVAR_ZSPAWN_TEAM_ZOMBIE]               =    CreateConVar("zr_zspawn_team_zombie",      "0", "Spawn player on zombie team when spawning by means of ZSpawn. [Dependency: zr_zspawn_team_override | Override: zr_respawn_zombie]");
    
    g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT]                 =    CreateConVar("zr_zspawn_timelimit",        "1", "Put a time limit on the use of ZSpawn.");
    g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT_TIME]            =    CreateConVar("zr_zspawn_timelimit_time",   "120.0", "Time from the start of the round to allow ZSpawn. [Dependency: zr_zspawn_timelimit]");
    
    
    // ===========================
    // ZTele (module)
    // ===========================
    g_hCvarsList[CVAR_ZTELE_ZOMBIE]                     =    CreateConVar("zr_ztele_zombie",                "1",    "Allow zombies to use ZTele.");
    g_hCvarsList[CVAR_ZTELE_HUMAN_BEFORE]               =    CreateConVar("zr_ztele_human_before",          "1",    "Allow humans to use ZTele before the mother zombie has spawned.");
    g_hCvarsList[CVAR_ZTELE_HUMAN_AFTER]                =    CreateConVar("zr_ztele_human_after",           "0",    "Allow humans to use ZTele after the mother zombie has spawned.");
    g_hCvarsList[CVAR_ZTELE_DELAY_ZOMBIE]               =    CreateConVar("zr_ztele_delay_zombie",          "3.0",  "Time between using ZTele command and teleportation for zombies. [Dependency: zr_ztele_zombie]");
    g_hCvarsList[CVAR_ZTELE_DELAY_HUMAN]                =    CreateConVar("zr_ztele_delay_human",           "3.0",  "Time between using ZTele command and teleportation for humans. [Dependency: zr_ztele_human_(before)/(after)]");
    g_hCvarsList[CVAR_ZTELE_MAX_ZOMBIE]                 =    CreateConVar("zr_ztele_max_zombie",            "3",    "Max number of times a zombie is allowed to use ZTele per round. [Dependency: zr_ztele_zombie]");
    g_hCvarsList[CVAR_ZTELE_MAX_HUMAN]                  =    CreateConVar("zr_ztele_max_human",             "1",    "Max number of times a human is allowed to use ZTele per round. [Dependency: zr_ztele_human_(before)/(after)]");
    g_hCvarsList[CVAR_ZTELE_AUTOCANCEL]                 =    CreateConVar("zr_ztele_autocancel",            "1",    "Automatically cancel ZTele if player moves out of a set boundary. [Dependency: zr_ztele_(zombie)/(human)[_(before)/(after)]]");
    g_hCvarsList[CVAR_ZTELE_AUTOCANCEL_DISTANCE]        =    CreateConVar("zr_ztele_autocancel_distance",   "20",   "Maximum distance, in feet, player is allowed to travel before teleport is cancelled. [Dependency: zr_ztele_autocancel]");
    
    
    // ===========================
    // ZHP (module)
    // ===========================
    g_hCvarsList[CVAR_ZHP]                              =    CreateConVar("zr_zhp",             "1",    "Allow player to toggle real HP display as a zombie.");
    g_hCvarsList[CVAR_ZHP_DEFAULT]                      =    CreateConVar("zr_zhp_default",     "1",    "Default ZHP toggle state set on connecting player. [Dependency: zr_zhp]");
    
    
    // Auto-generate config file if it doesn't exist, then execute.
    AutoExecConfig(true, "zombiereloaded", "sourcemod/zombiereloaded");
}

/**
 * Hook cvar changes.
 * 
 * @param unhook    If true, cvars will be unhooked, false to hook cvars.
 */
CvarsHook(bool:unhook = false)
{
    // If unhook is true, then continue.
    if (unhook)
    {
        // Unhook all cvars.
        UnhookConVarChange(g_hAutoTeamBalance, CvarsHookLocked);
        UnhookConVarChange(g_hLimitTeams, CvarsHookLocked);
        UnhookConVarChange(g_hRestartGame, CvarsHookRestartGame);
        
        UnhookConVarChange(g_hCvarsList[CVAR_VOL], VolEnabledChanged);
        
        // Stop after unhooking cvars.
        return;
    }
    
    // Set locked cvars to their locked value.
    SetConVarInt(g_hAutoTeamBalance, CVARS_AUTOTEAMBALANCE_LOCKED);
    SetConVarInt(g_hLimitTeams, CVARS_LIMITTEAMS_LOCKED);
    
    // Hook cvar to prevent it from changing.
    HookConVarChange(g_hAutoTeamBalance, CvarsHookLocked);
    HookConVarChange(g_hLimitTeams, CvarsHookLocked);
    HookConVarChange(g_hRestartGame, CvarsHookRestartGame);
    
    HookConVarChange(g_hCvarsList[CVAR_VOL], VolEnabledChanged);
}

/**
 * Cvar hook callback (mp_autoteambalance, mp_limitteams)
 * Prevents changes to cvar.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before the attempted change.
 * @param newvalue  The new value.
 */
public CvarsHookLocked(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If cvar is mp_autoteambalance, then continue.
    if (cvar == g_hAutoTeamBalance)
    {
        // If plugin is reverting value, then stop.
        if (StringToInt(newvalue) == CVARS_AUTOTEAMBALANCE_LOCKED)
        {
            return;
        }
        
        // Revert to locked value.
        SetConVarInt(g_hAutoTeamBalance, CVARS_AUTOTEAMBALANCE_LOCKED);
        
        // If log flag check fails, then don't log.
        LogPrintToLog(_, "Cvars", "Cvar Locked", "Cvar \"mp_autoteambalance\" was reverted back to \"CVARS_AUTOTEAMBALANCE_LOCKED\".");
    }
    // If cvar is mp_limitteams, then continue.
    else if (cvar == g_hLimitTeams)
    {
        // If plugin is reverting value, then stop.
        if (StringToInt(newvalue) == CVARS_LIMITTEAMS_LOCKED)
        {
            return;
        }
        
        // Revert to locked value.
        SetConVarInt(g_hLimitTeams, CVARS_LIMITTEAMS_LOCKED);
        
        // If log flag check fails, then don't log.
        LogPrintToLog(_, "Cvars", "Cvar Locked", "Cvar \"mp_limitteams\" was reverted back to \"CVARS_LIMITTEAMS_LOCKED\".");
    }
}

/**
 * Cvar hook callback (mp_restartgame)
 * Stops restart and just ends the round.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before the attempted change.
 * @param newvalue  The new value.
 */
public CvarsHookRestartGame(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // Prevent round restart.
    SetConVarInt(cvar, 0);
    
    // If value was invalid or 0, then stop.
    new Float:delay = StringToFloat(newvalue);
    if (delay <= 0)
    {
        return;
    }
    
    // Terminate the round with restart time as delay.
    RoundEndTerminateRound(delay);
    
    // If log flag check fails, then don't log.
    LogPrintToLog(_, "Cvars", "Restart Game", "\"mp_restartgame\" was caught and blocked, commencing round.");
}
