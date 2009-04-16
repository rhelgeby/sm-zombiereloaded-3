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
 * Client is joining the server.
 */ 
RespawnClientInit(client)
{
    // Reset timer handle.
    tRespawn[client] = INVALID_HANDLE;
    
    // Init gKilledByWorld for client.
    gKilledByWorld[client] = false;
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
RespawnOnClientDeath(client)
{
    // If timer is running, kill it.
    if (tRespawn[client] != INVALID_HANDLE)
    {
        KillTimer(tRespawn[client]);
    }
    
    // If respawn is disabled, stop here.
    new bool:respawn = GetConVarBool(gCvars[CVAR_RESPAWN]);
    if (!respawn)
    {
        return;
    }
    
    // Start respawn timer.
    new Float:delay = GetConVarFloat(gCvars[CVAR_RESPAWN_DELAY]);
    tRespawn[client] = CreateTimer(delay, RespawnTimer, client, TIMER_FLAG_NO_MAPCHANGE);
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
    if (!zombieSpawned)
    {
        return;
    }
    
    // Get respawn team.
    new bool:respawn_zombie = GetConVarBool(gCvars[CVAR_RESPAWN_ZOMBIE]);
    
    // Get suicide respawn cvar
    if (respawn_zombie)
    {
        InfectPlayer(client);
        return;
    }
    
    if (GetConVarBool(gCvars[CVAR_SUICIDE_WORLD_DAMAGE]) && gKilledByWorld[client])
    {
        InfectPlayer(client);
        gKilledByWorld[client] = false;
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
    if (!IsPlayerAlive(client))
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