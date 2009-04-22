/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        zombiereloaded.inc
 *   Description: General plugin functions and defines.
 *
 * ============================================================================
 */

/**
 * Minimum dx level required to see overlays.
 */
#define GENERAL_DXLEVEL_MIN 90

/**
 * The DirectX level of a client.
 */
new dxLevel[MAXPLAYERS + 1];

/**
 * Global variable set to true when the first zombie(s) is/are spawned.
 */
new bool:g_bZombieSpawned;

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
 * Create an array populated with eligible clients to be zombie.
 * 
 * @param arrayEligibleClients  The handle of the array, don't forget to call CloseHandle
 *                              on it when finished!
 * @param immunity              True to ignore clients immune from mother infect, false to count them.
 */  
ZRCreateEligibleClientList(&Handle:arrayEligibleClients, bool:team = false, bool:alive = false, bool:human = false)
{
    // Create array.
    arrayEligibleClients = CreateArray();
    
    // Populate list with eligible clients.
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client isn't on a team, then stop.
        if (team && !ZRIsClientOnTeam(x))
        {
            continue;
        }
        
        // If client is dead, then stop.
        if (alive && !IsPlayerAlive(x))
        {
            continue;
        }
        
        // If client is already zombie (via admin), then stop.
        if (human && !IsPlayerHuman(x))
        {
            continue;
        }
        
        // Add eligible client to array.
        PushArrayCell(arrayEligibleClients, x);
    }
    
    return GetArraySize(arrayEligibleClients);
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
bool:ZRCountValidClients(&zombiecount = 0, &humancount = 0, bool:alive = true, bool:ignorezombiespawned = false)
{
    // If zombie hasn't spawned and were not only counting humans, then stop.
    if (!g_bZombieSpawned && !ignorezombiespawned)
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
    return bool:GetTeamClientCount(team);
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
