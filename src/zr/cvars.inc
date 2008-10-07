/**
 * ====================
 *   Zombie:Reloaded
 *   File: cvars.inc
 *   Author: Greyscale
 * ====================
 */

enum ZRSettings
{
    Handle:CVAR_ENABLE,
    Handle:CVAR_ALLOW_PLAYER_TEAM,
    Handle:CVAR_AMBIENCE,
    Handle:CVAR_AMBIENCE_FILE,
    Handle:CVAR_AMBIENCE_LENGTH,
    Handle:CVAR_AMBIENCE_VOLUME,
    Handle:CVAR_EMITSOUNDS,
    Handle:CVAR_CLASSES,
    Handle:CVAR_CLASSES_SPAWN,
    Handle:CVAR_CLASSES_RANDOM,
    Handle:CVAR_ZOMBIE_HEALTH,
    Handle:CVAR_ZOMBIE_SPEED,
    Handle:CVAR_ZOMBIE_JUMP_DISTANCE,
    Handle:CVAR_ZOMBIE_JUMP_HEIGHT,
    Handle:CVAR_ZOMBIE_KNOCKBACK,
    Handle:CVAR_ZOMBIE_NVGS,
    Handle:CVAR_ZOMBIE_FOV,
    Handle:CVAR_ZOMBIE_REGEN,
    Handle:CVAR_ZOMBIE_REGEN_HEALTH,
    Handle:CVAR_ZOMBIE_REGEN_INTERVAL,
    Handle:CVAR_ZOMBIE_NAPALM,
    Handle:CVAR_ZOMBIE_NAPALM_TIME,
    Handle:CVAR_ZOMBIE_NOFALLDAMAGE,
    Handle:CVAR_ZOMBIE_KILL_BONUS,
    Handle:CVAR_ZOMBIE_INFECT_HEALTH,
    Handle:CVAR_ZOMBIE_ZVISION,
    Handle:CVAR_ZVISION_REDISPLAY,
    Handle:CVAR_ZVISION_ALLOW_DISABLE,
    Handle:CVAR_DARK,
    Handle:CVAR_DARK_LEVEL,
    Handle:CVAR_DARK_SKY,
    Handle:CVAR_MOTHER_ZOMBIE_RATIO,
    Handle:CVAR_MOTHER_ZOMBIE_RESPAWN,
    Handle:CVAR_RESPAWN,
    Handle:CVAR_RESPAWN_TEAM,
    Handle:CVAR_RESPAWN_DELAY,
    Handle:CVAR_SUICIDE,
    Handle:CVAR_SPAWN_MIN,
    Handle:CVAR_SPAWN_MAX,
    Handle:CVAR_PROTECT,
    Handle:CVAR_CONSECUTIVE_INFECT,
    Handle:CVAR_OVERLAYS,
    Handle:CVAR_OVERLAYS_HUMAN,
    Handle:CVAR_OVERLAYS_ZOMBIE,
    Handle:CVAR_ZMARKET_BUYZONE,
    Handle:CVAR_ZSPAWN,
    Handle:CVAR_ZTELE,
    Handle:CVAR_ZTELE_LIMIT,
    Handle:CVAR_ZSTUCK,
    Handle:CVAR_ZHP,
    Handle:CVAR_ZHP_DEFAULT,
    Handle:CVAR_CASHFILL,
    Handle:CVAR_CASHAMOUNT,
    Handle:CVAR_INFECT_FIREBALL,
    Handle:CVAR_INFECT_SMOKE,
    Handle:CVAR_INFECT_SPARKS,
    Handle:CVAR_INFECT_SOUND,
    Handle:CVAR_INFECT_ESPLASH,
    Handle:CVAR_INFECT_SHAKE,
    Handle:CVAR_INFECT_SHAKE_AMP,
    Handle:CVAR_INFECT_SHAKE_FREQUENCY,
    Handle:CVAR_INFECT_SHAKE_DURATION
}

new gCvars[ZRSettings];

CreateCvars()
{
    gCvars[CVAR_ENABLE]                 =    CreateConVar("zr_enable", "1", "Enable zombie gameplay (0: Disable)");
    gCvars[CVAR_ALLOW_PLAYER_TEAM]      =    CreateConVar("zr_allow_player_team", "0", "This will allow the player_team event to be fired on first team join, enable when using mani model menu (0: Disable)");
    gCvars[CVAR_AMBIENCE]               =    CreateConVar("zr_ambience", "1", "Enable creepy ambience to be played throughout the game (0: Disable)");
    gCvars[CVAR_AMBIENCE_FILE]          =    CreateConVar("zr_ambience_file", "ambient/zr/zr_ambience.mp3", "Path to ambient sound file that will be played throughout the game, when zr_ambience is 1");
    gCvars[CVAR_AMBIENCE_LENGTH]        =    CreateConVar("zr_ambience_length", "60.0", "The length, in seconds, of the ambient sound file");
    gCvars[CVAR_AMBIENCE_VOLUME]        =    CreateConVar("zr_ambience_volume", "0.6", "Volume of ambient sounds when zr_ambience is 1 (0.0: Unhearable,  1.0: Max volume)");
    gCvars[CVAR_EMITSOUNDS]             =    CreateConVar("zr_emitsounds", "50", "How often a zombie emits a sound,  in seconds (0: Disable)");
    gCvars[CVAR_CLASSES]                =    CreateConVar("zr_classes", "1", "Enable zombie classes");
    gCvars[CVAR_CLASSES_SPAWN]          =    CreateConVar("zr_classes_spawn", "0", "Classmenu is re-displayed every spawn (0: Disable)");
    gCvars[CVAR_CLASSES_RANDOM]         =    CreateConVar("zr_classes_random", "0", "A random class is assigned to each player every round (0: Disable)");
    gCvars[CVAR_ZOMBIE_HEALTH]          =    CreateConVar("zr_zombie_health", "5000", "The default health of a zombie");
    gCvars[CVAR_ZOMBIE_SPEED]           =    CreateConVar("zr_zombie_speed", "350", "How fast zombies travel (300: Default speed,  600: Double speed)");
    gCvars[CVAR_ZOMBIE_JUMP_DISTANCE]   =    CreateConVar("zr_zombie_jump_distance", "0.1", "How far the zombie jumps,  (0: Regular jump distance)");
    gCvars[CVAR_ZOMBIE_JUMP_HEIGHT]     =    CreateConVar("zr_zombie_jump_height", "10.0", "How high a zombie jumps (0: Regular jump height)");
    gCvars[CVAR_ZOMBIE_KNOCKBACK]       =    CreateConVar("zr_zombie_knockback", "1", "How far zombies are pushed back when shot. If classes are enabled: This one is a multiplier for class knockback values (1: Default)");
    gCvars[CVAR_ZOMBIE_NVGS]            =    CreateConVar("zr_zombie_nvgs", "-1", "Zombies will receive nightvision. Also used to override class nvgs if non-negative. (-1: No override,  0: No night vision,  1: Night vision)");    gCvars[CVAR_ZOMBIE_FOV]             =    CreateConVar("zr_zombie_fov", "110", "The field of vision of zombies (90: Default vision)");
    gCvars[CVAR_ZOMBIE_REGEN]           =    CreateConVar("zr_zombie_regen", "0", "Zombies will regenerate health");
    gCvars[CVAR_ZOMBIE_REGEN_HEALTH]    =    CreateConVar("zr_zombie_regen_health", "1", "How much health is regenerated when zr_zombie_regen is 1");
    gCvars[CVAR_ZOMBIE_REGEN_INTERVAL]  =    CreateConVar("zr_zombie_regen_interval", "5", "How often,  in seconds,  a zombie regenerates health when zr_zombie_regen is 1");
    gCvars[CVAR_ZOMBIE_NAPALM]          =    CreateConVar("zr_zombie_napalm", "1", "Turns grenades into napalm grenades that light zombies on fire (0: Disable)");
    gCvars[CVAR_ZOMBIE_NAPALM_TIME]     =    CreateConVar("zr_zombie_napalm_time", "20", "How long the zombie burns when zr_napalm is 1");
    gCvars[CVAR_ZOMBIE_NOFALLDAMAGE]    =    CreateConVar("zr_zombie_nofalldamage", "0", "Zombies wont be hurt from falling (0: Disable)");
    gCvars[CVAR_ZOMBIE_KILL_BONUS]      =    CreateConVar("zr_zombie_kill_bonus", "2", "How many additional kills are rewarded to the killer of the zombie");
    gCvars[CVAR_ZOMBIE_INFECT_HEALTH]   =    CreateConVar("zr_zombie_infect_health", "100", "How much health a zombie gains when infecting a human (0: Disable)");
    gCvars[CVAR_ZOMBIE_ZVISION]         =    CreateConVar("zr_zombie_zvision", "overlays/zr/zvision", "Overlay to be shown on all zombies' screen on infection (Leave empty to disable)");
    gCvars[CVAR_ZVISION_REDISPLAY]      =    CreateConVar("zr_zvision_redisplay", "0.2", "Frequency, in seconds, to display zvision on the client's screen (Never go below 0.1, 0.2 seems safe)");
    gCvars[CVAR_ZVISION_ALLOW_DISABLE]  =    CreateConVar("zr_zvision_allow_disable", "1", "Allow users to disable ZVision with their nightvision key (0: Disable)");
    gCvars[CVAR_DARK]                   =    CreateConVar("zr_dark", "0", "Default value for darkening maps,  most dislike this feature (0: Disable)");
    gCvars[CVAR_DARK_LEVEL]             =    CreateConVar("zr_dark_level", "a", "The darkness of the map,  a being the darkest,  z being extremely bright when zr_dark is 1 (n: Default)");
    gCvars[CVAR_DARK_SKY]               =    CreateConVar("zr_dark_sky", "sky_borealis01", "The sky the map will have when zr_dark is 1");
    gCvars[CVAR_MOTHER_ZOMBIE_RATIO]    =    CreateConVar("zr_mother_zombie_ratio", "5", "For every 'x' number of humans,  there will be 1 zombie (0: Always only 1 mother zombie)");
    gCvars[CVAR_MOTHER_ZOMBIE_RESPAWN]  =    CreateConVar("zr_mother_zombie_respawn", "0", "First zombie(s) will be teleported back to spawn when infected (0: Disable)");
    gCvars[CVAR_RESPAWN]                =    CreateConVar("zr_respawn", "0", "When player is killed,  player will respawn");
    gCvars[CVAR_RESPAWN_TEAM]           =    CreateConVar("zr_respawn_team", "zombie", "Which team to respawn player as (Choices: zombie, human)");
    gCvars[CVAR_RESPAWN_DELAY]          =    CreateConVar("zr_respawn_delay", "1", "How long to wait after death to respawn,  in seconds");
    gCvars[CVAR_SUICIDE]                =    CreateConVar("zr_suicide", "1", "Stops players from suiciding");
    gCvars[CVAR_SPAWN_MIN]              =    CreateConVar("zr_spawn_min", "30", "Minimum time a player is picked to be zombie after the round starts,  in seconds");
    gCvars[CVAR_SPAWN_MAX]              =    CreateConVar("zr_spawn_max", "50", "Maximum time a player is picked to be zombie after the round starts,  in seconds");
    gCvars[CVAR_PROTECT]                =    CreateConVar("zr_protect", "10", "Players that join late will be protected for this long, in seconds (0: Disable)");
    gCvars[CVAR_CONSECUTIVE_INFECT]     =    CreateConVar("zr_consecutive_infect", "0", "Allow player to be randomly chosen twice in a row to be a mother zombie (0: Disable)");
    gCvars[CVAR_OVERLAYS]               =    CreateConVar("zr_overlays", "1", "Will show overlays that tell who the winner of the round was (0: Disable)");
    gCvars[CVAR_OVERLAYS_HUMAN]         =    CreateConVar("zr_overlays_human", "overlays/zr/humans_win", "The overlay shown to tell everyone that humans won when zr_overlays is 1");
    gCvars[CVAR_OVERLAYS_ZOMBIE]        =    CreateConVar("zr_overlays_zombie", "overlays/zr/zombies_win", "The overlay shown to tell everyone that zombies won when zr_overlays is 1");
    gCvars[CVAR_ZMARKET_BUYZONE]        =    CreateConVar("zr_zmarket_buyzone", "1", "Must be in buyzone to access !zmarket, if Market is installed (0: Can be used anywhere)");
    gCvars[CVAR_ZSPAWN]                 =    CreateConVar("zr_zspawn", "1", "Allow players to spawn if they just joined the game (0: Disable)");
    gCvars[CVAR_ZTELE]                  =    CreateConVar("zr_ztele", "1", "Allows zombies who get stuck to teleport back to spawn (0: Disable)");
    gCvars[CVAR_ZTELE_LIMIT]            =    CreateConVar("zr_ztele_limit", "1", "Max amount of teleports per round when zr_ztele is 1 (0: Unlimited)");
    gCvars[CVAR_ZSTUCK]                 =    CreateConVar("zr_zstuck", "1", "Allow players that are stuck together to get unstuck (0: Disable)");
    gCvars[CVAR_ZHP]                    =    CreateConVar("zr_zhp", "1", "Allows clients to enable/disable zombie health display (1: On,  0: Off)");
    gCvars[CVAR_ZHP_DEFAULT]            =    CreateConVar("zr_zhp_default", "1", "The default value of zombie health display to new clients (1: On,  0: Off)");
    gCvars[CVAR_CASHFILL]               =    CreateConVar("zr_cashfill", "1", "Enable the mod to set the players cash to zr_cashamount (0: Disabled)");
    gCvars[CVAR_CASHAMOUNT]             =    CreateConVar("zr_cashamount", "12000", "How much money players will have when they spawn when zr_cashfill is 1");
    gCvars[CVAR_INFECT_FIREBALL]        =    CreateConVar("zr_infect_fireball", "1", "A fireball is created when a player is infected ( 0: Disable)");
    gCvars[CVAR_INFECT_SMOKE]           =    CreateConVar("zr_infect_smoke", "1", "A puff of smoke is created when a player is infected (0: Disable)");
    gCvars[CVAR_INFECT_SPARKS]          =    CreateConVar("zr_infect_sparks", "1", "Sparks are emitted when a player is infected (0: Disable)");
    gCvars[CVAR_INFECT_SOUND]           =    CreateConVar("zr_infect_sound", "npc/fast_zombie/fz_scream1.wav", "Sound played from from player on infection (Leave blank to disable)");
    gCvars[CVAR_INFECT_ESPLASH]         =    CreateConVar("zr_infect_esplash", "1", "An energy splash is emitted when player is infected ( 0: Disable)");
    gCvars[CVAR_INFECT_SHAKE]           =    CreateConVar("zr_infect_shake", "1", "Player's screen is shaken on infection (0: Disable)");
    gCvars[CVAR_INFECT_SHAKE_AMP]       =    CreateConVar("zr_infect_shake_amp", "15.0", "Amplitude of the shake, when zr_infect_shake is 1");
    gCvars[CVAR_INFECT_SHAKE_FREQUENCY] =    CreateConVar("zr_infect_shake_frequency", "1.0", "Frequency of the shake, when zr_infect_shake is 1");
    gCvars[CVAR_INFECT_SHAKE_DURATION]  =    CreateConVar("zr_infect_shake_duration", "5.0", "Duration of the shake, when zr_infect_shake is 1");
    
    HookConVarChange(gCvars[CVAR_ENABLE], EnableHook);
    
    AutoExecConfig(true, "zombiereloaded", "sourcemod/zombiereloaded");
}

HookCvars()
{   
    SetConVarBool(FindConVar("mp_autoteambalance"), false);
    SetConVarInt(FindConVar("mp_limitteams"), 0);
    
    HookConVarChange(FindConVar("mp_autoteambalance"), AutoTeamBalanceHook);
    HookConVarChange(FindConVar("mp_limitteams"), LimitTeamsHook);
    
    HookConVarChange(FindConVar("mp_restartgame"), RestartGameHook);
}

UnhookCvars()
{
    UnhookConVarChange(FindConVar("mp_autoteambalance"), AutoTeamBalanceHook);
    UnhookConVarChange(FindConVar("mp_limitteams"), LimitTeamsHook);
    
    UnhookConVarChange(FindConVar("mp_restartgame"), RestartGameHook);
}

public EnableHook(Handle:convar, const String:oldValue[], const String:newValue[])
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
}

public AutoTeamBalanceHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    SetConVarBool(FindConVar("mp_autoteambalance"), false);
}

public LimitTeamsHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    SetConVarInt(FindConVar("mp_limitteams"), 0);
}