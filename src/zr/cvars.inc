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
    Handle:CVAR_CONFIG_PATH_MODELS,
    Handle:CVAR_CONFIG_PATH_DOWNLOADS,
    Handle:CVAR_CONFIG_PATH_PLAYERCLASSES,
    Handle:CVAR_CONFIG_PATH_WEAPONS,
    Handle:CVAR_CONFIG_PATH_WEAPONGROUPS,
    Handle:CVAR_CONFIG_PATH_HITGROUPS,
    Handle:CVAR_CLASSES_SPAWN,
    Handle:CVAR_CLASSES_RANDOM,
    Handle:CVAR_CLASSES_DEFAULT_ZOMBIE,
    Handle:CVAR_CLASSES_DEFAULT_HUMAN,
    Handle:CVAR_CLASSES_DEFAULT_ADMIN,
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
    Handle:CVAR_RESPAWN_ZOMBIE,
    Handle:CVAR_RESPAWN_ZOMBIE_WORLD,
    Handle:CVAR_ZSPAWN,
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
    
    // To be modulized/recoded.
    Handle:CVAR_OVERLAY_REDISPLAY,
    Handle:CVAR_ZVISION_ALLOW_DISABLE,
    Handle:CVAR_MENU_AUTOCLOSE,
    Handle:CVAR_ANTICAMP,
    Handle:CVAR_ANTICAMP_UPDATE_INTERVAL,
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
    
    // Forward event to modules.
    VEffectsOnCvarInit();
}

/**
 * Create all cvars for plugin.
 */
CvarsCreate()
{
    // Cvar naming guidelines:
    // 1. Prefix: "zr_"
    // 2. Module name next. "zr_modulename_"
    // 3. (If applys) Submodule name next. "zr_modulename_submodulename_" Ex "zr_weapons_restrict_"
    // 4. Name last part to be defaulted to "1" (if boolean cvar) Ex "zr_damage_suicide_zombie",
    //    because it will be defaulted to "1".  "zr_damage_suicide_zombie_disable" = bad.  Because we want
    //    to try to default everything to "1" and let hosts disable what they don't want.
    //
    // Cvar description guidelines
    // Assume cvar is enabled.
    // Ex. "zr_roundend_overlay" "1" "Display round win overlays to all clients."
    // Explain (additional) options in brackets.
    // Ex. "zr_classes_default_zombie" "" "Default zombie class. ['""' = Class config defaults.  'random' = Give client random class on connect.]"
    // Any thing else?
    
    // ===========================
    // General (Core)
    // ===========================
    
    g_hCvarsList[CVAR_ENABLE]                    =    CreateConVar("zr_enable", "1", "");
    
    // ===========================
    // Log (core)
    // ===========================
    
    // Cvars created in log.inc, because of compilation problems when creating them here.
    
    // ===========================
    // Translations (core)
    // ===========================
    
    // (None)
    
    // ===========================
    // Config (core)
    // ===========================
    
    g_hCvarsList[CVAR_CONFIG_PATH_MODELS]        =    CreateConVar("zr_config_path_models", "configs/zr/models.txt", "");
    g_hCvarsList[CVAR_CONFIG_PATH_DOWNLOADS]     =    CreateConVar("zr_config_path_downloads", "configs/zr/downloads.txt");
    g_hCvarsList[CVAR_CONFIG_PATH_PLAYERCLASSES] =    CreateConVar("zr_config_path_playerclasses", "configs/zr/playerclasses.txt");
    g_hCvarsList[CVAR_CONFIG_PATH_WEAPONS]       =    CreateConVar("zr_config_path_weapons", "configs/zr/weapons/weapons.txt");
    g_hCvarsList[CVAR_CONFIG_PATH_WEAPONGROUPS]  =    CreateConVar("zr_config_path_weapongroups", "configs/zr/weapons/weapongroups.txt");
    g_hCvarsList[CVAR_CONFIG_PATH_HITGROUPS]     =    CreateConVar("zr_config_path_hitgroups", "configs/zr/hitgroups.txt");
    
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
    
    g_hCvarsList[CVAR_CLASSES_SPAWN]                    =    CreateConVar("zr_classes_spawn", "0", "");
    // Old Desc: Classmenu is re-displayed every spawn (0: Disable)
    g_hCvarsList[CVAR_CLASSES_RANDOM]                   =    CreateConVar("zr_classes_random", "0", "");
    // Old Desc: A random class is assigned to each player every round. Overrides zr_classes_spawn and default classes. (0: Disable)
    g_hCvarsList[CVAR_CLASSES_DEFAULT_ZOMBIE]           =    CreateConVar("zr_classes_default_zombie", "random", "");
    // Old Desc: Default zombie class selected for all players when they connect. Use \"random\" to select a random class, or blank to use class config defaults.
    g_hCvarsList[CVAR_CLASSES_DEFAULT_HUMAN]            =    CreateConVar("zr_classes_default_human", "random", "");
    // Old Desc: 
    g_hCvarsList[CVAR_CLASSES_DEFAULT_ADMIN]            =    CreateConVar("zr_classes_default_admin", "random", "");
    // Old Desc: Default admin-only class selected for admins when they connect. Use \"random\" to select a random class, or blank to use class config defaults.
    
    // ===========================
    // Weapons (core)
    // ===========================
    
    // General
    
    g_hCvarsList[CVAR_WEAPONS]                          =    CreateConVar("zr_weapons", "1", "");
    // Note make config file cvar.
    // Note this disables restrict submodule.
    
    // Restrict
    
    g_hCvarsList[CVAR_WEAPONS_RESTRICT]                 =    CreateConVar("zr_weapons_restrict", "1", "");
    // Note make config file cvar.
    
    // Market Handler
    
    g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]          =    CreateConVar("zr_weapons_zmarket_buyzone", "1", "");
    // Old Desc: Must be in buyzone to access !zmarket, if Market is installed (0: Can be used anywhere)
    
    // ===========================
    // Hitgroups (core)
    // ===========================
    
    g_hCvarsList[CVAR_HITGROUPS]                        =    CreateConVar("zr_hitgroups", "1", "");
    // Note make config file cvar.
    
    // ===========================
    // Round End (core)
    // ===========================
    
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY]                 =    CreateConVar("zr_roundend_overlay", "1", "");
    // Old Desc: Shows an overlay to all clients when a team wins. (0: Disable)
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY_HUMAN]           =    CreateConVar("zr_roundend_overlays_human", "overlays/zr/humans_win");
    // Old Desc: overlays/zr/humans_win", "Path to \"humans win\" overlay
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY_ZOMBIE]          =    CreateConVar("zr_roundend_overlays_zombie", "overlays/zr/zombies_win", "");
    // Old Desc: Path to \"zombies win\" overlay
    
    // ===========================
    // Infect (core)
    // ===========================
    
    g_hCvarsList[CVAR_INFECT_MZOMBIE_RATIO]             =    CreateConVar("zr_infect_mzombie_ratio", "5", "");
    g_hCvarsList[CVAR_INFECT_MZOMBIE_RESPAWN]           =    CreateConVar("zr_infect_mzombie_respawn", "0", "");
    g_hCvarsList[CVAR_INFECT_SPAWNTIME_MIN]             =    CreateConVar("zr_infect_spawntime_min", "30.0", "");
    g_hCvarsList[CVAR_INFECT_SPAWNTIME_MAX]             =    CreateConVar("zr_infect_spawntime_max", "50.0", "");
    g_hCvarsList[CVAR_INFECT_CONSECUTIVE_BLOCK]         =    CreateConVar("zr_infect_consecutive_block", "1", "");
    g_hCvarsList[CVAR_INFECT_WEAPONS_DROP]              =    CreateConVar("zr_infect_weapons_drop", "1", "");
    
    // Effects
    
    g_hCvarsList[CVAR_INFECT_FIREBALL]                  =    CreateConVar("zr_infect_fireball", "1", "");
    g_hCvarsList[CVAR_INFECT_SMOKE]                     =    CreateConVar("zr_infect_smoke", "1", "");
    g_hCvarsList[CVAR_INFECT_SPARKS]                    =    CreateConVar("zr_infect_sparks", "1", "");
    g_hCvarsList[CVAR_INFECT_SOUND]                     =    CreateConVar("zr_infect_sound", "npc/fast_zombie/fz_scream1.wav", "");
    g_hCvarsList[CVAR_INFECT_ESPLASH]                   =    CreateConVar("zr_infect_esplash", "1", "");
    g_hCvarsList[CVAR_INFECT_SHAKE]                     =    CreateConVar("zr_infect_shake", "1", "");
    g_hCvarsList[CVAR_INFECT_SHAKE_AMP]                 =    CreateConVar("zr_infect_shake_amp", "15.0", "");
    g_hCvarsList[CVAR_INFECT_SHAKE_FREQUENCY]           =    CreateConVar("zr_infect_shake_frequency", "1.0", "");
    g_hCvarsList[CVAR_INFECT_SHAKE_DURATION]            =    CreateConVar("zr_infect_shake_duration", "5.0", ""); 
    
    // ===========================
    // Damage (core)
    // ===========================
    
    // Hitgroup Damage
    
    g_hCvarsList[CVAR_DAMAGE_HITGROUPS]                 =    CreateConVar("zr_damage_hitgroups", "1", "");
    
    // Block Damage Types
    
    g_hCvarsList[CVAR_DAMAGE_BLOCK_FF]                  =    CreateConVar("zr_damage_block_ff", "1", "");
    // note overrides mp_friendlyfire
    g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]               =    CreateConVar("zr_damage_block_blast", "1", "");
    
    // Suicide Intercept
    
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_ZOMBIE]            =    CreateConVar("zr_damage_suicide_zombie", "1", "");
    // Old Desc: Intercept human suicide attempts.
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_HUMAN]             =    CreateConVar("zr_damage_suicide_human", "1", "");
    // Old Desc: Intercept zombie suicide attempts.
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_CMDS]              =    CreateConVar("zr_damage_suicide_cmds", "kill, spectate, jointeam", "");
    // Old Desc: List of suicide commands to intercept. (Delimited by \", \"
    
    // ===========================
    // Say Hooks (core)
    // ===========================
    
    g_hCvarsList[CVAR_SAYHOOKS_QUIET]                   =    CreateConVar("zr_sayhooks_quiet", "1", "");
    g_hCvarsList[CVAR_SAYHOOKS_QUIET_FILTER]            =    CreateConVar("zr_sayhooks_quiet_filter", "1", "");
    // when enabled it filters out failed sayhooks
    g_hCvarsList[CVAR_SAYHOOKS_QUIET_FLAGS]             =    CreateConVar("zr_sayhooks_quiet_flags", "58", "");
    // Flags (default: 2 + 8 + 16 + 32)
    // 0    Allow all.
    // 1    Quiet "!zmenu" say hook.
    // 2    Quiet "!zadmin" say hook.
    // 4    Quiet "!zclass" say hook.
    // 8    Quiet "!zspawn" say hook.
    // 16   Quiet "!ztele" say hook.
    // 32   Quiet "!zhp" say hook.
    // 64   Quiet "!zmarket" say hook.
    
    // ===========================
    // Account (module)
    // ===========================
    
    g_hCvarsList[CVAR_ACCOUNT_CASHFILL]                 =    CreateConVar("zr_account_cashfill", "1", "");
    // Old Desc: Enable the mod to set the players cash to zr_cashamount (0: Disabled)
    g_hCvarsList[CVAR_ACCOUNT_CASHFILL_VALUE]           =    CreateConVar("zr_account_cashfill_value", "12000", "");
    // Old Desc: How much money players will have when they spawn when zr_cashfill is 1
    
    // ===========================
    // Visual Effects (module)
    // ===========================
    
    // Lightstyle
    
    g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE]              =    CreateConVar("zr_veffects_lightstyle", "1", "");
    g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE]        =    CreateConVar("zr_veffects_lightstyle_value", "b", "");
    
    // Sky
    
    g_hCvarsList[CVAR_VEFFECTS_SKY]                     =    CreateConVar("zr_veffects_sky", "1", "");
    g_hCvarsList[CVAR_VEFFECTS_SKY_PATH]                =    CreateConVar("zr_veffects_sky_path", "sky_borealis01up.vmt", "");
    
    // Sun
    
    g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE]             =    CreateConVar("zr_veffects_sun_disable", "1", "");
    
    // Fog
    
    g_hCvarsList[CVAR_VEFFECTS_FOG]                     =    CreateConVar("zr_veffects_fog", "0", "");
    // NOTE DISABLE. SOURCEMOD DOESNT SUPPORT THIS YET.
    g_hCvarsList[CVAR_VEFFECTS_FOG_OVERRIDE]            =    CreateConVar("zr_veffects_fog_override", "0", "");
    // NOTE DISABLE. SOURCEMOD DOESNT SUPPORT THIS YET.
    g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR]              =    CreateConVar("zr_veffects_fog_pcolor", "255 255 255", "");
    g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR]              =    CreateConVar("zr_veffects_fog_scolor", "255 255 255", "");
    g_hCvarsList[CVAR_VEFFECTS_FOG_DENSITY]             =    CreateConVar("zr_veffects_fog_density", "0.8", "");
    g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST]           =    CreateConVar("zr_veffects_fog_startdist", "0", "");
    g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST]             =    CreateConVar("zr_veffects_fog_enddist", "400", "");
    g_hCvarsList[CVAR_VEFFECTS_FOG_FARZ]                =    CreateConVar("zr_veffects_fog_farz", "2000", "");
    
    // ===========================
    // Sound Effects (module)
    // ===========================
    
    // Zombie Sounds
    
    g_hCvarsList[CVAR_SEFFECTS_MOAN]                    =    CreateConVar("zr_seffects_moan", "30", "");
    // Old Desc: How often, in seconds, a zombie moans (0: Disable)
    g_hCvarsList[CVAR_SEFFECTS_GROAN]                   =    CreateConVar("zr_seffects_groan", "5", "");
    // Old Desc: Chance factor a zombie will groan when shot (Lower: More often, 0: Disable)
    g_hCvarsList[CVAR_SEFFECTS_DEATH]                   =    CreateConVar("zr_seffects_death", "1", "");
    // Old Desc: Zombie will emit a death sound when killed (0: Disable)
    
    // Ambient Sounds
    
    g_hCvarsList[CVAR_AMBIENTSOUNDS]                    =    CreateConVar("zr_ambientsounds", "1", "");
    // Old Desc: Enable creepy ambience to be played throughout the game (0: Disable)
    g_hCvarsList[CVAR_AMBIENTSOUNDS_FILE]               =    CreateConVar("zr_ambientsounds_file", "ambient/zr/zr_ambience.mp3", "");
    // Old Desc: Path to ambient sound file that will be played throughout the game, when zr_ambience is 1
    g_hCvarsList[CVAR_AMBIENTSOUNDS_LENGTH]             =    CreateConVar("zr_ambientsounds_length", "60.0", "");
    // Old Desc: The length, in seconds, of the ambient sound file
    g_hCvarsList[CVAR_AMBIENTSOUNDS_VOLUME]             =    CreateConVar("zr_ambientsounds_volume", "1.0", "");
    // Old Desc: Volume of ambient sounds when zr_ambience is 1 (0.0: Unhearable,  1.0: Max volume)
    
    // ===========================
    // Antistick (module)
    // ===========================
    
    g_hCvarsList[CVAR_ANTISTICK]                        =    CreateConVar("zr_antistick", "1", "");
    // Old Desc: Enable the anti-stick module, which will automatically unstick players stuck within each other. (0:Disable)
    g_hCvarsList[CVAR_ANTISTICK_INTERVAL]               =    CreateConVar("zr_antistick_interval", "0.5", "");
    // Old Desc: How often, in seconds, the anti-stick module checks each player for being stuck. (1.0: Default)
    
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
    g_hCvarsList[CVAR_SPAWNPROTECT_ALPHA]               =    CreateConVar("zr_spawnprotect_speed", "0", "");
    
    // ===========================
    // Respawn (module)
    // ===========================
    
    g_hCvarsList[CVAR_RESPAWN]                          =    CreateConVar("zr_respawn", "0", "");
    // Old Desc: When player is killed,  player will respawn
    g_hCvarsList[CVAR_RESPAWN_DELAY]                    =    CreateConVar("zr_respawn_delay", "1", "");
    // Old Desc: How long to wait after death to respawn,  in seconds
    g_hCvarsList[CVAR_RESPAWN_ZOMBIE]                   =    CreateConVar("zr_respawn_zombie", "1", "");
    // Old Desc: Respawn player as zombie (0: Respawn as human)
    g_hCvarsList[CVAR_RESPAWN_ZOMBIE_WORLD]             =    CreateConVar("zr_respawn_zombie_world", "1", "");
    // Old Desc: Respawn zombies as zombies if they were killed by the world, like elevators, doors and lasers. (0: Disable)
    
    // ===========================
    // Napalm (module)
    // ===========================
    
    // (None)
    
    // ===========================
    // ZSpawn (module)
    // ===========================
    
    g_hCvarsList[CVAR_ZSPAWN]                           =    CreateConVar("zr_zspawn", "1", "");
    // Old Desc: Allow players to spawn if they just joined the game (0: Disable)
    
    g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT]                 =    CreateConVar("zr_zspawn_timelimit", "1", "");
    g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT_TIME]            =    CreateConVar("zr_zspawn_timelimit_time", "120", "");
    // ===========================
    // ZTele (module)
    // ===========================
    
    g_hCvarsList[CVAR_ZTELE_ZOMBIE]                     =    CreateConVar("zr_ztele_zombie", "1", "");
    g_hCvarsList[CVAR_ZTELE_HUMAN_BEFORE]               =    CreateConVar("zr_ztele_human_before", "1", "");
    g_hCvarsList[CVAR_ZTELE_HUMAN_AFTER]                =    CreateConVar("zr_ztele_human_after", "0", "");
    g_hCvarsList[CVAR_ZTELE_DELAY_ZOMBIE]               =    CreateConVar("zr_ztele_delay_zombie", "3", "");
    g_hCvarsList[CVAR_ZTELE_DELAY_HUMAN]                =    CreateConVar("zr_ztele_delay_human", "3", "");
    g_hCvarsList[CVAR_ZTELE_MAX_ZOMBIE]                 =    CreateConVar("zr_ztele_max_zombie", "3", "");
    g_hCvarsList[CVAR_ZTELE_MAX_HUMAN]                  =    CreateConVar("zr_ztele_max_human", "1", "");
    g_hCvarsList[CVAR_ZTELE_AUTOCANCEL]                 =    CreateConVar("zr_ztele_autocancel", "1", "");
    g_hCvarsList[CVAR_ZTELE_AUTOCANCEL_DISTANCE]        =    CreateConVar("zr_ztele_autocancel_distance", "20", "");
    
    // ===========================
    // ZHP (module)
    // ===========================
    
    g_hCvarsList[CVAR_ZHP]                              =    CreateConVar("zr_zhp", "1", "");
    // Old Desc: Allows clients to enable/disable zombie health display (1: On,  0: Off)
    g_hCvarsList[CVAR_ZHP_DEFAULT]                       =    CreateConVar("zr_zhp_default", "1", "");
    // Old Desc: The default value of zombie health display to new clients (1: On,  0: Off)
    
    
    // TO BE MODULIZED/RECODED.
    g_hCvarsList[CVAR_OVERLAY_REDISPLAY]                =    CreateConVar("zr_overlay_redisplay", "3", "");
    // Old Desc: Frequency, in seconds, to display overlay on the client's screen (Never go below 0.1, 0.2 seems safe)
    g_hCvarsList[CVAR_ZVISION_ALLOW_DISABLE]            =    CreateConVar("zr_zvision_allow_disable", "1", "");
    // Old Desc: Allow users to disable ZVision with their nightvision key (0: Disable)
    g_hCvarsList[CVAR_MENU_AUTOCLOSE]                   =    CreateConVar("zr_menu_autoclose", "0", "");
    // Old Desc: Automatically close menus on selection. If disabled the menu will remain open.
    g_hCvarsList[CVAR_ANTICAMP]                         =    CreateConVar("zr_anticamp", "1", "");
    // Old Desc:  Enables or disables hurt volumes for preventing unfair camping. (0: Disable)
    g_hCvarsList[CVAR_ANTICAMP_UPDATE_INTERVAL]         =    CreateConVar("zr_anticamp_update_interval", "1", "");
    // Old Desc: How often to update player locations (in seconds).
    
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
    
    // Anticamp shtuff. (needs to be moved to anticamp if these hooks are necessary)
    HookConVarChange(g_hCvarsList[CVAR_ANTICAMP], AnticampHook);
    HookConVarChange(g_hCvarsList[CVAR_ANTICAMP_UPDATE_INTERVAL], UpdateIntervalHook);
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
        if (LogCheckFlag(LOG_CORE_EVENTS, LOG_MODULE_CORE))
        {
            LogMessageFormatted(-1, "Cvars", "Cvar Locked", "Cvar \"mp_autoteambalance\" was reverted back to \"CVARS_AUTOTEAMBALANCE_LOCKED\".", LOG_FORMAT_TYPE_FULL);
        }
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
        if (LogCheckFlag(LOG_CORE_EVENTS, LOG_MODULE_CORE))
        {
            LogMessageFormatted(-1, "Cvars", "Cvar Locked", "Cvar \"mp_limitteams\" was reverted back to \"CVARS_LIMITTEAMS_LOCKED\".", LOG_FORMAT_TYPE_FULL);
        }
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
    if (LogCheckFlag(LOG_CORE_EVENTS, LOG_MODULE_CORE))
    {
        LogMessageFormatted(-1, "Cvars", "Restart Game", "\"mp_restartgame\" was caught and blocked, commencing round.", LOG_FORMAT_TYPE_FULL);
    }
}
