/**
 * ====================
 *   Zombie:Reloaded
 *   File: zombiereloaded.inc
 *   Author: Greyscale
 * ==================== 
 */

#define Target_Bombed							1		// Target Successfully Bombed!
#define VIP_Escaped								2		// The VIP has escaped!
#define VIP_Assassinated						3		// VIP has been assassinated!
#define Terrorists_Escaped						4		// The terrorists have escaped!
#define CTs_PreventEscape						5		// The CT's have prevented most of the terrorists from escaping!
#define Escaping_Terrorists_Neutralized			6		// Escaping terrorists have all been neutralized!
#define Bomb_Defused							7		// The bomb has been defused!
#define CTs_Win									8		// Counter-Terrorists Win!
#define Terrorists_Win							9		// Terrorists Win!
#define Round_Draw								10		// Round Draw!
#define All_Hostages_Rescued					11		// All Hostages have been rescued!
#define Target_Saved							12		// Target has been saved!
#define Hostages_Not_Rescued					13		// Hostages have not been rescued!
#define Terrorists_Not_Escaped					14		// Terrorists have not escaped!
#define VIP_Not_Escaped							15		// VIP has not escaped!
#define Game_Commencing							16		// Game Commencing!

#define DXLEVEL_MIN 90
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
 * Lists possible returns of the game at any time.
 */ 
enum ZTeam
{
    Neither, /** Round is not over */
    Zombie, /** Round is over because zombies win */
    Human, /** Round is over because humans wins */
}

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
 * Global variable to store round win timer handle.
 */
new Handle:tRound = INVALID_HANDLE;

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
 * Global variable to store a convar query cookie
 */ 
new QueryCookie:mat_dxlevel;

/**
 * Finds DX level of a client.
 * 
 * @param client    The client index.  
 */
ZRFindClientDXLevel(client)
{
    // If client is fake (or bot), then stop.
    if (IsFakeClient(client))
    {
        return;
    }
    
    // Query mat_dxlevel on client.
    mat_dxlevel = QueryClientConVar(client, "mat_dxlevel", ZRDXLevelClientQuery);
}

/**
 * Query callback function.
 *
 * @param cookie    Unique cookie of the query.
 * @param client    The client index.
 * @param result    The result of the query (see console.inc enum ConVarQueryResult)
 * @param cvarName  Name of the cvar.
 * @param cvarValue Value of the cvar.
 */       
public ZRDXLevelClientQuery(QueryCookie:cookie, client, ConVarQueryResult:result, const String:cvarName[], const String:cvarValue[])
{
    // If query cookie does not match cookie given by mat_dxlevel query, then stop, this isn't our query.
    if (cookie != mat_dxlevel)
    {
        return;
    }
    
    // Reset dxLevel.
    dxLevel[client] = 0;
    
    // If result is any other than ConVarQuery_Okay, then stop.
    if (result != ConVarQuery_Okay)
    {   
        return;
    }
    
    // Copy cvar value to dxLevel array.
    dxLevel[client] = StringToInt(cvarValue);
}

/**
 * Displays overlay to client, or prints unsupported message on client's screen.
 * 
 * @param client    The client index.
 * @param overlay   The overlay path.
 */    
ZRDisplayClientOverlay(client, const String:overlay[])
{
    // If dxLevel is 0, then query on client failed, so try again, then stop.
    if (!dxLevel[client])
    {
        // Query dxlevel cvar.
        ZRFindClientDXLevel(client);
        return;
    }
    
    // If dxLevel is above or equal to minimum requirement, then display overlay.
    if (dxLevel[client] >= DXLEVEL_MIN)
    {
        ClientCommand(client, "r_screenoverlay \"%s\"", overlay);
    }
    // If client doesn't meet minimum requirement, then print unsupported text.
    else
    {
        ZR_PrintCenterText(client, "DX90 not supported", dxLevel[client], DXLEVEL_MIN);
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
ZRTeamHasClients(team = -1)
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
