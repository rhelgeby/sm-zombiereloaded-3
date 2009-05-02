/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        respawn.inc
 *   Description: Players come back to life
 *
 * ============================================================================
 */

/**
 * Array for storing respawn timer handles per client.
 */
new Handle:tRespawn[MAXPLAYERS + 1];

/**
 * Array for flagging zombies who were killed by world.
 */ 
new bool:bKilledByWorld[MAXPLAYERS + 1];

/**
 * Client is joining the server.
 */ 
RespawnClientInit(client)
{
    // Reset timer handle.
    tRespawn[client] = INVALID_HANDLE;
    
    // Init bKilledByWorld for client.
    bKilledByWorld[client] = false;
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
RespawnOnClientSpawn(client)
{
    // If timer is running, kill it.
    if (tRespawn[client] != INVALID_HANDLE)
    {
        KillTimer(tRespawn[client]);
    }
    
    // Reset timer handle.
    tRespawn[client] = INVALID_HANDLE;
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
RespawnOnClientDeath(client, attacker, const String:weapon[])
{
    // If client is a zombie, check if they were killed by world.
    if (InfectIsClientInfected(client))
    {
        // Set bKilledByWorld to true if attacker is not a valid client.
        bKilledByWorld[client] = !ZRIsClientValid(attacker);
    }
    
    // If timer is running, kill it.
    if (tRespawn[client] != INVALID_HANDLE)
    {
        KillTimer(tRespawn[client]);
    }
    
    // If player was infected, then stop.
    if (StrEqual(weapon, "zombie_claws_of_death", false))
    {
        return;
    }
    
    // If respawn is disabled, stop here.
    new bool:respawn = GetConVarBool(g_hCvarsList[CVAR_RESPAWN]);
    if (!respawn)
    {
        return;
    }
    
    // Start respawn timer.
    new Float:delay = GetConVarFloat(g_hCvarsList[CVAR_RESPAWN_DELAY]);
    tRespawn[client] = CreateTimer(delay, RespawnTimer, client, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 */
RespawnOnRoundEnd()
{
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If timer isn't currently running, then stop.
        if (tRespawn[x] == INVALID_HANDLE)
        {
            continue;
        }
        
        // Stop timer.
        KillTimer(tRespawn[x]);
        
        // Reset timer handle.
        tRespawn[x] = INVALID_HANDLE;
    }
}

/**
 * Returns if a player is to be respawned as a zombie, because they were killed by world.
 * 
 * @param client    The client index.
 * @return          True if they were killed by world, false if not or cvar is disabled.
 */
RespawnKilledByWorld(client)
{
    // Return true if both the cvar is enabled and the player was killed by world.
    return (GetConVarBool(g_hCvarsList[CVAR_RESPAWN_ZOMBIE_WORLD]) && bKilledByWorld[client]);
}

/**
 * Spawns a player into the round.
 * 
 * @param client    The client index.
 */
RespawnSpawnClient(client)
{
    // If client isn't in-game, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }
    
    // Spawn player.
    CS_RespawnPlayer(client);
    
    // Stop here if the first zombie hasn't spawned yet.
    if (!g_bZombieSpawned)
    {
        return;
    }
    
    // Get respawn team.
    new bool:respawn_zombie = GetConVarBool(g_hCvarsList[CVAR_RESPAWN_ZOMBIE]);
    
    // Get suicide respawn cvar
    if (respawn_zombie)
    {
        InfectClient(client);
        return;
    }
    
    if (GetConVarBool(g_hCvarsList[CVAR_RESPAWN_ZOMBIE_WORLD]) && bKilledByWorld[client])
    {
        InfectClient(client);
        bKilledByWorld[client] = false;
    }
}

/**
 * Timer callback, respawns a player.
 * 
 * @param timer     The timer handle.
 * @param client    The client index.
 */    
public Action:RespawnTimer(Handle:timer, any:client)
{
    // Reset timer handle.
    tRespawn[client] = INVALID_HANDLE;
    
    // If client isn't in-game, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }
    
    // If player isn't alive, then stop.
    if (IsPlayerAlive(client))
    {
        return;
    }
    
    // Get client team.
    new team = GetClientTeam(client);
    
    // If player isn't on a team, then stop.
    if (team != CS_TEAM_T && team != CS_TEAM_CT)
    {
        return;
    }
    
    // Spawn player.
    RespawnSpawnClient(client);
}
