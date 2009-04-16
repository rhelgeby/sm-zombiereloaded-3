/**
 * ====================
 *   Zombie:Reloaded
 *   File: zombiereloaded.inc
 *   Author: Greyscale
 * ==================== 
 */

enum ZTeam
{
    Neither, /** Round is not over */
    Zombie, /** Round is over because zombies win */
    Human, /** Round is over because humans wins */
}

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

new bool:market;
new dxLevel[MAXPLAYERS + 1];

new bool:zombieSpawned;
new bool:motherZombie[MAXPLAYERS + 1];
new bool:gZombie[MAXPLAYERS + 1];
new bool:gBlockMotherInfect[MAXPLAYERS + 1];

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

new Handle:tRound = INVALID_HANDLE;
new Handle:tInfect = INVALID_HANDLE;

new Handle:pList = INVALID_HANDLE;

#define MAXTIMERS 1

#define TTELE 1

new Handle:tHandles[MAXPLAYERS + 1][MAXTIMERS];

new QueryCookie:mat_dxlevel;

bool:ConfigSettingToBool(const String:option[])
{
    if (StrEqual(option, "yes", false))
    {
        return true;
    }
    
    return false;
}

BoolToConfigSetting(bool:bOption, String:option[], maxlen)
{
    if (bOption)
    {
        strcopy(option, maxlen, "yes");
    }
    else
    {
        strcopy(option, maxlen, "no");
    }
}

FindClientDXLevel(client)
{
    if (IsFakeClient(client))
    {
        return;
    }
    
    mat_dxlevel = QueryClientConVar(client, "mat_dxlevel", DXLevelClientQuery);
}

public DXLevelClientQuery(QueryCookie:cookie, client, ConVarQueryResult:result, const String:cvarName[], const String:cvarValue[])
{
    if (cookie != mat_dxlevel)
    {
        return;
    }
    
    dxLevel[client] = 0;
    
    if (result != ConVarQuery_Okay)
    {   
        return;
    }
    
    dxLevel[client] = StringToInt(cvarValue);
}

DisplayClientOverlay(client, const String:overlay[])
{
    if (!dxLevel[client])
    {
        FindClientDXLevel(client);
        
        return;
    }
    
    if (dxLevel[client] >= DXLEVEL_MIN)
    {
        ClientCommand(client, "r_screenoverlay \"%s\"", overlay);
    }
    else
    {
        ZR_PrintCenterText(client, "DX90 not supported", dxLevel[client], DXLEVEL_MIN);
    }
}

RefreshList()
{
    ClearList();
    
    pList = CreateArray();
    
    for (new x = 1; x <= MaxClients; x++)
    {
        if (IsClientInGame(x) && IsPlayerAlive(x))
        {  
            new team = GetClientTeam(x);
            if (team == CS_TEAM_T || team == CS_TEAM_CT)
            {
                PushArrayCell(pList, x);
            }
        }
    }
}

AddPlayerToList(client)
{
    if (pList != INVALID_HANDLE)
    {
        PushArrayCell(pList, client);
    }
}

ClearList()
{
    if (pList != INVALID_HANDLE)
    {
        ClearArray(pList);
    }
}

RandomPlayerFromList()
{
    if (pList != INVALID_HANDLE)
    {
        new size = GetArraySize(pList);
        new index = GetRandomInt(0, size - 1);
        
        return GetArrayCell(pList, index);
    }
    
    return -1;
}

bool:IsPlayerInList(client)
{
    if (pList != INVALID_HANDLE)
    {
        return (FindValueInArray(pList, client) != -1);
    }
    
    return false;
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
 * Returns whether a player is a generic admin or not.
 *
 * @param client    The client index.
 * @return          True if generic admin, false otherwise.
 */
bool:ZRIsClientAdmin(client)
{
    // Check to make sure client is valid and has the Admin_Generic SourceMod admin flag.
    if (ZRIsValidClient(client) && GetAdminFlag(GetUserAdmin(client), Admin_Generic))
    {
        return true;
    }
    
    // Client is not an admin.
    return false;
}
