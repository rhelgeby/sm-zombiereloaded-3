/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        cvars.inc
 *   Description: Config creation and cvar control.
 *
 * ============================================================================
 */

/**
 * List of cvars used by the plugin.
 */
enum CvarsList
{
    Handle:CVAR_ENABLE,
    Handle:CVAR_ENABLE_ZM_VERSION_VAR,
    Handle:CVAR_LOG,
    Handle:CVAR_LOGFLAGS,
    Handle:CVAR_CLASSES_SPAWN,
    Handle:CVAR_CLASSES_RANDOM,
    Handle:CVAR_CLASSES_DEFAULT_ZOMBIE,
    Handle:CVAR_CLASSES_DEFAULT_HUMAN,
    Handle:CVAR_CLASSES_DEFAULT_ADMIN,
    Handle:CVAR_CLASSES_FILE,
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
    Handle:CVAR_ROUNDEND_OVERLAY,
    Handle:CVAR_ROUNDEND_OVERLAY_ZOMBIE,
    Handle:CVAR_ROUNDEND_OVERLAY_HUMAN,
    Handle:CVAR_INFECT_SPAWNTIME_MIN,
    Handle:CVAR_INFECT_SPAWNTIME_MAX,
    Handle:CVAR_INFECT_CONSECUTIVE_BLOCK,
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
    Handle:CVAR_SOUNDEFFECTS_MOAN,
    Handle:CVAR_SOUNDEFFECTS_GROAN,
    Handle:CVAR_SOUNDEFFECTS_DEATH,
    Handle:CVAR_AMBIENTSOUNDS,
    Handle:CVAR_AMBIENTSOUNDS_FILE,
    Handle:CVAR_AMBIENTSOUNDS_LENGTH,
    Handle:CVAR_AMBIENTSOUNDS_VOLUME,
    Handle:CVAR_ANTISTICK,
    Handle:CVAR_ANTISTICK_INTERVAL,
    Handle:CVAR_SPAWNPROTECT,
    Handle:CVAR_SPAWNPROTECT_TIME,
    Handle:CVAR_RESPAWN,
    Handle:CVAR_RESPAWN_DELAY,
    Handle:CVAR_RESPAWN_ZOMBIE,
    Handle:CVAR_RESPAWN_ZOMBIE_WORLD,
    Handle:CVAR_ZHP,
    Handle:CVAR_ZHP_DEFAULT,
    
    // To be modulized/recoded.
    Handle:CVAR_OVERLAY_REDISPLAY,
    Handle:CVAR_ZVISION_ALLOW_DISABLE,
    Handle:CVAR_MENU_AUTOCLOSE,
    Handle:CVAR_DARK,
    Handle:CVAR_DARK_LEVEL,
    Handle:CVAR_DARK_SKY,
    Handle:CVAR_ZSPAWN,
    Handle:CVAR_ZTELE,
    Handle:CVAR_ZTELE_STARTUP_DELAY,
    Handle:CVAR_ZTELE_COOLDOWN,
    Handle:CVAR_ZTELE_HUMAN_DELAY,
    Handle:CVAR_ZTELE_HUMAN_LIMIT,
    Handle:CVAR_ZTELE_ZOMBIE_DELAY,
    Handle:CVAR_ZTELE_ZOMBIE_LIMIT,
    Handle:CVAR_ZTELE_RESET_BUFFERS,
    Handle:CVAR_ANTICAMP,
    Handle:CVAR_ANTICAMP_UPDATE_INTERVAL,
}

/**
 * Array to store cvar data in.
 */
new g_hCvarsList[CvarsList];

/**
 * Cvars module init function.
 */ 
CvarsInit()
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
    g_hCvarsList[CVAR_ENABLE_ZM_VERSION_VAR]     =    CreateConVar("zr_enable_zm_version_var", "1", "Enable zombie_version variable for compatibility with ZombieMod server filters.");
    
    // ===========================
	// Log (core)
	// ===========================
	
    g_hCvarsList[CVAR_LOG]                   =    CreateConVar("zr_log", "1", "");
    g_hCvarsList[CVAR_LOGFLAGS]              =    CreateConVar("zr_logflags", "331", "");
    // Old Desc: Logging flags. Log messages to sourcemod logs, server console or client console. Use zr_log_flags to see a list of flags. (0: Disable)
    
    // ===========================
	// Translations (core)
	// ===========================
	
    // (None)
    
    // ===========================
	// Offsets (core)
	// ===========================
	
	// (None)
	
	// ===========================
	// Models (core)
	// ===========================
	
	// TODO: config file path.
	
    // ===========================
    // Classes (core)
    // ===========================
    
    g_hCvarsList[CVAR_CLASSES_SPAWN]             =    CreateConVar("zr_classes_spawn", "0", "");
    // Old Desc: Classmenu is re-displayed every spawn (0: Disable)
    g_hCvarsList[CVAR_CLASSES_RANDOM]            =    CreateConVar("zr_classes_random", "0", "");
    // Old Desc: A random class is assigned to each player every round. Overrides zr_classes_spawn and default classes. (0: Disable)
    g_hCvarsList[CVAR_CLASSES_DEFAULT_ZOMBIE]    =    CreateConVar("zr_classes_default_zombie", "random", "");
    // Old Desc: Default zombie class selected for all players when they connect. Use \"random\" to select a random class, or blank to use class config defaults.
    g_hCvarsList[CVAR_CLASSES_DEFAULT_HUMAN]     =    CreateConVar("zr_classes_default_human", "random", "");
    // Old Desc: 
    g_hCvarsList[CVAR_CLASSES_DEFAULT_ADMIN]     =    CreateConVar("zr_classes_default_admin", "random", "");
    // Old Desc: Default admin-only class selected for admins when they connect. Use \"random\" to select a random class, or blank to use class config defaults.
    g_hCvarsList[CVAR_CLASSES_FILE]              =    CreateConVar("zr_classes_file", "configs/zr/playerclasses.txt", "");
    // Old Desc: Class data file to read from, in Valves key/values format. The path is relative to the \"sourcemod\" folder.
    
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
    
    g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]           =    CreateConVar("zr_weapons_zmarket_buyzone", "1", "");
    // Old Desc: Must be in buyzone to access !zmarket, if Market is installed (0: Can be used anywhere)
    
    // ===========================
    // Hitgroups (core)
    // ===========================
    
    g_hCvarsList[CVAR_HITGROUPS]                        =    CreateConVar("zr_hitgroups", "1", "");
    // Note make config file cvar.
    
    // ===========================
	// Round End (core)
	// ===========================
	
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY]          =    CreateConVar("zr_roundend_overlay", "1", "");
    // Old Desc: Shows an overlay to all clients when a team wins. (0: Disable)
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY_HUMAN]    =    CreateConVar("zr_roundend_overlays_human", "");
    // Old Desc: overlays/zr/humans_win", "Path to \"humans win\" overlay
    g_hCvarsList[CVAR_ROUNDEND_OVERLAY_ZOMBIE]   =    CreateConVar("zr_roundend_overlays_zombie", "overlays/zr/zombies_win", "");
    // Old Desc: Path to \"zombies win\" overlay
    
    // ===========================
    // Infect (core)
    // ===========================
    
    g_hCvarsList[CVAR_INFECT_MZOMBIE_RATIO]       =    CreateConVar("zr_infect_motherzombie_ratio", "5", "");
    g_hCvarsList[CVAR_INFECT_MZOMBIE_RESPAWN]     =    CreateConVar("zr_infect_motherzombie_respawn", "0", "");
    g_hCvarsList[CVAR_INFECT_SPAWNTIME_MIN]            =    CreateConVar("zr_infect_spawntime_min", "30.0", "");
    g_hCvarsList[CVAR_INFECT_SPAWNTIME_MAX]            =    CreateConVar("zr_infect_spawntime_max", "50.0", "");
    g_hCvarsList[CVAR_INFECT_CONSECUTIVE_BLOCK]         =    CreateConVar("zr_infect_consecutive_block", "1", "");
    
    // Effects
    
    g_hCvarsList[CVAR_INFECT_FIREBALL]           =    CreateConVar("zr_infect_fireball", "1", "");
    g_hCvarsList[CVAR_INFECT_SMOKE]              =    CreateConVar("zr_infect_smoke", "1", "");
    g_hCvarsList[CVAR_INFECT_SPARKS]             =    CreateConVar("zr_infect_sparks", "1", "");
    g_hCvarsList[CVAR_INFECT_SOUND]              =    CreateConVar("zr_infect_sound", "npc/fast_zombie/fz_scream1.wav", "");
    g_hCvarsList[CVAR_INFECT_ESPLASH]            =    CreateConVar("zr_infect_esplash", "1", "");
    g_hCvarsList[CVAR_INFECT_SHAKE]              =    CreateConVar("zr_infect_shake", "1", "");
    g_hCvarsList[CVAR_INFECT_SHAKE_AMP]          =    CreateConVar("zr_infect_shake_amp", "15.0", "");
    g_hCvarsList[CVAR_INFECT_SHAKE_FREQUENCY]    =    CreateConVar("zr_infect_shake_frequency", "1.0", "");
    g_hCvarsList[CVAR_INFECT_SHAKE_DURATION]     =    CreateConVar("zr_infect_shake_duration", "5.0", ""); 
    
    // ===========================
    // Damage (core)
    // ===========================
    
    // Hitgroup Damage
    
    g_hCvarsList[CVAR_DAMAGE_HITGROUPS]              =    CreateConVar("zr_damage_hitgroups", "1", "");
    
    // Block Damage Types
    
    g_hCvarsList[CVAR_DAMAGE_BLOCK_FF]              =    CreateConVar("zr_damage_block_ff", "1", "");
    // note overrides mp_friendlyfire
    g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]           =    CreateConVar("zr_damage_block_blast", "1", "");
    
    // Suicide Intercept
    
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_ZOMBIE]            =    CreateConVar("zr_damage_suicide_zombie", "1", "");
    // Old Desc: Intercept human suicide attempts.
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_HUMAN]             =    CreateConVar("zr_damage_suicide_human", "1", "");
    // Old Desc: Intercept zombie suicide attempts.
    g_hCvarsList[CVAR_DAMAGE_SUICIDE_CMDS]              =    CreateConVar("zr_damage_suicide_cmds", "kill, spectate, jointeam", "");
    // Old Desc: List of suicide commands to intercept. (Delimited by \", \"
    
    // ===========================
    // Account (module)
    // ===========================
    
    g_hCvarsList[CVAR_ACCOUNT_CASHFILL]                 =    CreateConVar("zr_account_cashfill", "1", "");
    // Old Desc: Enable the mod to set the players cash to zr_cashamount (0: Disabled)
    g_hCvarsList[CVAR_ACCOUNT_CASHFILL_VALUE]           =    CreateConVar("zr_account_cashfill_value", "12000", "");
    // Old Desc: How much money players will have when they spawn when zr_cashfill is 1
    
    // ===========================
	// Sound Effects (module)
	// ===========================
	
	// Zombie Sounds
	
    g_hCvarsList[CVAR_SOUNDEFFECTS_MOAN]         =    CreateConVar("zr_soundeffects_moan", "30", "");
    // Old Desc: How often, in seconds, a zombie moans (0: Disable)
    g_hCvarsList[CVAR_SOUNDEFFECTS_GROAN]        =    CreateConVar("zr_soundeffects_groan", "5", "");
    // Old Desc: Chance factor a zombie will groan when shot (Lower: More often, 0: Disable)
    g_hCvarsList[CVAR_SOUNDEFFECTS_DEATH]        =    CreateConVar("zr_soundeffects_death", "1", "");
    // Old Desc: Zombie will emit a death sound when killed (0: Disable)
    
    // Ambient Sounds
    
    g_hCvarsList[CVAR_AMBIENTSOUNDS]             =    CreateConVar("zr_ambientsounds", "1", "");
    // Old Desc: Enable creepy ambience to be played throughout the game (0: Disable)
    g_hCvarsList[CVAR_AMBIENTSOUNDS_FILE]        =    CreateConVar("zr_ambientsounds_file", "ambient/zr/zr_ambience.mp3", "");
    // Old Desc: Path to ambient sound file that will be played throughout the game, when zr_ambience is 1
    g_hCvarsList[CVAR_AMBIENTSOUNDS_LENGTH]      =    CreateConVar("zr_ambientsounds_length", "60.0", "");
    // Old Desc: The length, in seconds, of the ambient sound file
    g_hCvarsList[CVAR_AMBIENTSOUNDS_VOLUME]      =    CreateConVar("zr_ambientsounds_volume", "1.0", "");
    // Old Desc: Volume of ambient sounds when zr_ambience is 1 (0.0: Unhearable,  1.0: Max volume)
    
    // ===========================
	// Antistick (module)
	// ===========================
	
    g_hCvarsList[CVAR_ANTISTICK]                 =    CreateConVar("zr_antistick", "1", "");
    // Old Desc: Enable the anti-stick module, which will automatically unstick players stuck within each other. (0:Disable)
    g_hCvarsList[CVAR_ANTISTICK_INTERVAL]        =    CreateConVar("zr_antistick_interval", "0.5", "");
    // Old Desc: How often, in seconds, the anti-stick module checks each player for being stuck. (1.0: Default)
    
    // ===========================
	// Knockback (module)
	// ===========================
	
	// (None)
	
	// ===========================
	// Spawn Protect (module)
	// ===========================
	
    g_hCvarsList[CVAR_SPAWNPROTECT]                   =    CreateConVar("zr_spawnprotect", "1", "");
    g_hCvarsList[CVAR_SPAWNPROTECT_TIME]              =    CreateConVar("zr_spawnprotect_time", "10", "");
    
    // ===========================
	// Respawn (module)
	// ===========================
	
    g_hCvarsList[CVAR_RESPAWN]                   =    CreateConVar("zr_respawn", "0", "");
    // Old Desc: When player is killed,  player will respawn
    g_hCvarsList[CVAR_RESPAWN_DELAY]             =    CreateConVar("zr_respawn_delay", "1", "");
    // Old Desc: How long to wait after death to respawn,  in seconds
    g_hCvarsList[CVAR_RESPAWN_ZOMBIE]            =    CreateConVar("zr_respawn_zombie", "1", "");
    // Old Desc: Respawn player as zombie (0: Respawn as human)
    g_hCvarsList[CVAR_RESPAWN_ZOMBIE_WORLD]      =    CreateConVar("zr_respawn_zombie_world", "1", "");
    // Old Desc: Respawn zombies as zombies if they were killed by the world, like elevators, doors and lasers. (0: Disable)
    
    // ===========================
	// Napalm (module)
	// ===========================
	
	// (None)
	
	// ===========================
	// ZHP (module)
	// ===========================
	
    g_hCvarsList[CVAR_ZHP]                       =    CreateConVar("zr_zhp", "1", "");
    // Old Desc: Allows clients to enable/disable zombie health display (1: On,  0: Off)
    g_hCvarsList[CVAR_ZHP_DEFAULT]               =    CreateConVar("zr_zhp_default", "1", "");
    // Old Desc: The default value of zombie health display to new clients (1: On,  0: Off)
    
    
    // TO BE MODULIZED/RECODED.
    g_hCvarsList[CVAR_OVERLAY_REDISPLAY]         =    CreateConVar("zr_overlay_redisplay", "3", "");
    // Old Desc: Frequency, in seconds, to display overlay on the client's screen (Never go below 0.1, 0.2 seems safe)
    g_hCvarsList[CVAR_ZVISION_ALLOW_DISABLE]     =    CreateConVar("zr_zvision_allow_disable", "1", "");
    // Old Desc: Allow users to disable ZVision with their nightvision key (0: Disable)
    g_hCvarsList[CVAR_MENU_AUTOCLOSE]            =    CreateConVar("zr_menu_autoclose", "0", "");
    // Old Desc: Automatically close menus on selection. If disabled the menu will remain open.
    g_hCvarsList[CVAR_DARK]                      =    CreateConVar("zr_dark", "0", "");
    // Old Desc: Default value for darkening maps,  most dislike this feature (0: Disable)
    g_hCvarsList[CVAR_DARK_LEVEL]                =    CreateConVar("zr_dark_level", "a", "");
    // Old Desc: The darkness of the map,  a being the darkest,  z being extremely bright when zr_dark is 1 (n: Default)
    g_hCvarsList[CVAR_DARK_SKY]                  =    CreateConVar("zr_dark_sky", "sky_borealis01", "");
    // Old Desc: The sky the map will have when zr_dark is 1
    g_hCvarsList[CVAR_ZSPAWN]                    =    CreateConVar("zr_zspawn", "1", "");
    // Old Desc: Allow players to spawn if they just joined the game (0: Disable)
    g_hCvarsList[CVAR_ZTELE]                     =    CreateConVar("zr_tele", "1", "");
    // Old Desc: Allow players to use the teleporter to get to spawn. (0: Disable)
    g_hCvarsList[CVAR_ZTELE_STARTUP_DELAY]       =    CreateConVar("zr_tele_startup_delay", "30", "");
    // Old Desc: Number of seconds to wait before the teleporter is enabled (0: Always enabled)
    g_hCvarsList[CVAR_ZTELE_COOLDOWN]            =    CreateConVar("zr_tele_cooldown", "20", "");
    // Old Desc: Number of seconds to wait before the teleporter can be used again, after a teleport. (0: Disable)
    g_hCvarsList[CVAR_ZTELE_HUMAN_DELAY]         =    CreateConVar("zr_tele_human_delay", "10", "");
    // Old Desc: Teleport delay for humans. (0: No delay)
    g_hCvarsList[CVAR_ZTELE_HUMAN_LIMIT]         =    CreateConVar("zr_tele_human_limit", "3", "");
    // Old Desc: Maximum number of teleports humans can do. (0: Humans can't use the teleporter. -1: Unlimited)
    g_hCvarsList[CVAR_ZTELE_ZOMBIE_DELAY]        =    CreateConVar("zr_tele_zombie_delay", "0", "");
    // Old Desc: Teleport delay for zombies. (0: No delay)
    g_hCvarsList[CVAR_ZTELE_ZOMBIE_LIMIT]        =    CreateConVar("zr_tele_zombie_limit", "8", "");
    // Old Desc: Maximum number of teleports zombies can do. (0: Zombies can't use the teleporter. -1: Unlimited)
    g_hCvarsList[CVAR_ZTELE_RESET_BUFFERS]       =    CreateConVar("zr_tele_reset_buffers", "1", "");
    // Old Desc: Reset custom saved teleport locations on each round start. (0: Disable)
    g_hCvarsList[CVAR_ANTICAMP]                  =    CreateConVar("zr_anticamp", "1", "");
    // Old Desc:  Enables or disables hurt volumes for preventing unfair camping. (0: Disable)
    g_hCvarsList[CVAR_ANTICAMP_UPDATE_INTERVAL]  =    CreateConVar("zr_anticamp_update_interval", "1", "");
    // Old Desc: How often to update player locations (in seconds).
    
    // TODO: Recode.
    //HookConVarChange(g_hCvarsList[CVAR_ENABLE], EnableHook);
    
    AutoExecConfig(true, "zombiereloaded", "sourcemod/zombiereloaded");
}

CvarsHook()
{   
    SetConVarBool(FindConVar("mp_autoteambalance"), false);
    SetConVarInt(FindConVar("mp_limitteams"), 0);
    
    HookConVarChange(FindConVar("mp_autoteambalance"), AutoTeamBalanceHook);
    HookConVarChange(FindConVar("mp_limitteams"), LimitTeamsHook);
    
    HookConVarChange(g_hCvarsList[CVAR_ANTICAMP], AnticampHook);
    HookConVarChange(g_hCvarsList[CVAR_ANTICAMP_UPDATE_INTERVAL], UpdateIntervalHook);
}

CvarsUnhook()
{
    UnhookConVarChange(FindConVar("mp_autoteambalance"), AutoTeamBalanceHook);
    UnhookConVarChange(FindConVar("mp_limitteams"), LimitTeamsHook);
}

/*public EnableHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    new bool:enable = bool:StringToInt(newValue);
    
    if (enable)
    {
        HookEvents();
        HookCvars();
        
        TerminateRound(3.0, Game_Commencing);
    }
    else
    {
        ZREnd();
    }
}*/

public AutoTeamBalanceHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    SetConVarBool(FindConVar("mp_autoteambalance"), false);
}

public LimitTeamsHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    SetConVarInt(FindConVar("mp_limitteams"), 0);
}