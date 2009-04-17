/**
 * ====================
 *   Zombie:Reloaded
 *   File: zombiereloaded.inc
 *   Author: Greyscale
 * ==================== 
 */
 
#define DEFAULT_FOV 90

/**
 * @section Logging flags.
 */
#define LOG_CORE_EVENTS             1           /** Executing config files, error messages, etc. */
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
#define LOG_MODULE_DAMAGECONTROL    2097152     /** damagecontrol.inc */
#define LOG_MODULE_OFFSETS          4194304     /** offsets.inc */
/*
 * @endsection
 */

/**
 * Global variable set to true if market plugin is installed
 */ 
new bool:g_bMarket;

/**
 * The DirectX level of a client.
 */
new dxLevel[MAXPLAYERS + 1];

/**
 * Global variable set to true when the first zombie(s) is/are spawned.
 */
new bool:g_bZombieSpawned;

/**
 * Array for flagging client as zombie.
 */
new bool:bZombie[MAXPLAYERS + 1];

/**
 * Array for flagging player has immune to mother infect.
 */
new bool:bMotherInfectImmune[MAXPLAYERS + 1];

/**
 * Global variable to store the infect timer handle.
 */
new Handle:tInfect = INVALID_HANDLE;

// TODO: MOVE TO TELEPORT MODULE.
new Float:spawnLoc[MAXPLAYERS + 1][3];
new Float:bufferLoc[MAXPLAYERS + 1][3];
new bool:ztele_spawned[MAXPLAYERS + 1] = {false, ...};
new bool:bufferLocSaved[MAXPLAYERS + 1] = {false, ...};
new ztele_countdown[MAXPLAYERS + 1] = {-1, ...};
new ztele_count[MAXPLAYERS + 1];
new bool:ztele_online = false;
new Handle:ztele_startup_timer = INVALID_HANDLE;
new Handle:ztele_countdown_timer[MAXPLAYERS + 1] = {INVALID_HANDLE, ...};
new Handle:ztele_cooldown_timer[MAXPLAYERS + 1] = {INVALID_HANDLE, ...};


// TODO: USE SEPARATE VARIABLE TO STORE TELEPORT TIMER HANDLE
// THEN WE CAN REMOVE tHandles ARRAY FOR GOOD.
#define MAXTIMERS 1

#define TTELE 1

new Handle:tHandles[MAXPLAYERS + 1][MAXTIMERS];

/**
 * Converts string of "yes" or "no" to a boolean value.
 * 
 * @param option    "yes" or "no" string to be converted.
 * @return          True if string is "yes", false otherwise.
 */
bool:ZRConfigSettingToBool(const String:option[])
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
 * @param option    Variable to store "yes" or "no" in.
 * @param maxlen    Max length of return string, (can't be more than 4)
 */
ZRBoolToConfigSetting(bool:bOption, String:option[], maxlen)
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

/**
 * Check if a client index is a valid player.
 * 
 * @param client    The client index.
 * @param console   True to include console (index 0), false if not.
 * @return          True if client is valid, false otherwise. 
 */    
bool:ZRIsValidClient(client, bool:console = false)
{
    // If index is greater than max number of clients, then return false.
    if (client > MaxClients)
    {
        return false;
    }
    
    // If console is true, return if client is >= 0, if not, then return client > 0.
    return console ? (client >= 0) : (client > 0);
}

/**
 * Count clients on each team.
 * 
 * @param zombies   This is set to the number of clients that are zombies.
 * @param humans    This is set to the number of clients that are humans.
 * @param alive     If true it will only count live players, false will count alive and dead.
 * @return          True if successful (zombie has spawned), false otherwise.
 */
bool:ZRCountValidClients(&zombiecount = 0, &humancount = 0, bool:alive = true)
{
    // If zombie hasn't spawned, then stop.
    if (!g_bZombieSpawned)
    {
        return false;
    }
    
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client isn't on a team, then stop.
        if (!ZRIsClientOnTeam(x))
        {
            continue;
        }
        
        // If player must be alive, and player is dead, then stop.
        if (alive && !IsPlayerAlive(x))
        {
            continue;
        }
        
        // If player is a zombie, then increment zombie variable.
        if (IsPlayerZombie(x))
        {
            zombiecount++;
        }
        // If player is a human, then increment human variable.
        else if (IsPlayerHuman(x))
        {
            humancount++;
        }
    }
    
    return true;
}

/**
 * Check if a client index is on a team.
 * 
 * @param client    The client index.
 * @param team      Team to check if player is on, -1 to check both. 
 * @return          True if client is on a team, false otherwise. 
 */    
bool:ZRIsClientOnTeam(client, team = -1)
{
    // If index is invalid, then stop.
    if (!ZRIsValidClient(client))
    {
        return false;
    }
    
    // Get client team.
    new clientteam = GetClientTeam(client);
    
    if (team == -1)
    {
        return (clientteam == CS_TEAM_T || clientteam == CS_TEAM_CT);
    }
    
    return (clientteam == team);
}

/**
 * Check if there are clients on a team.
 * 
 * @param team      (Optional) Team to check if there are clients on.
 */
bool:ZRTeamHasClients(team = -1)
{
    // If team is
    if (team == -1)
    {
        // Return true if both teams have at least 1 client.
        return (GetTeamClientCount(CS_TEAM_T) && GetTeamClientCount(CS_TEAM_CT));
    }
    
    // Return true if given team has at least 1 client.
    return (GetTeamClientCount(team));
}

/**
 * Returns whether a player is a generic admin or not.
 *
 * @param client    The client index.
 * @return          True if generic admin, false otherwise.
 */
bool:ZRIsClientAdmin(client)
{
    // If index is invalid, then stop.
    if (!ZRIsValidClient(client))
    {
        return false;
    }
    // If client doesn't have the Admin_Generic flag, then stop.
    if (!GetAdminFlag(GetUserAdmin(client), Admin_Generic))
    {
        return false;
    }
    
    // Client is an admin.
    return true;
}
