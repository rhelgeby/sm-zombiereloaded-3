/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          zspawn.inc
 *   Type:          Module
 *   Description:   Handles zspawn command, spawns late-joining clients into the game.
 *
 * ============================================================================
 */

/**
 * Global variable to store infect timer handle.
 */
new Handle:tZSpawn = INVALID_HANDLE;

/**
 * Array to block zspawn for a unique client serial number.
 */
new bool:g_bZSpawnBlock[MAXPLAYERS + 1];

/**
 * Map is starting.
 */
ZSpawnOnMapStart()
{
    // Reset timer handle.
    tZSpawn = INVALID_HANDLE;
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
ZSpawnOnClientDisconnect(client)
{
    // Get client's unique serial number.
    new serial = GetClientSerial(client);
    
    // Block zspawn.
    g_bZSpawnBlock[serial] = true;
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
ZSpawnOnClientDeath(client)
{
    // Get client's unique serial number.
    new serial = GetClientSerial(client);
    
    // Block zspawn.
    g_bZSpawnBlock[serial] = true;
}

/**
 * The round is starting.
 */
ZSpawnOnRoundStart()
{
    // Disable flag that blocks zspawn for all clients.
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // Unblock zspawn.
        g_bZSpawnBlock[x] = false;
    }
    
    // If zspawn timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);
        
        // Reset timer handle.
        tZSpawn = INVALID_HANDLE;
    }
}

/**
 * The freeze time is ending.
 */
ZSpawnOnRoundFreezeEnd()
{
    // If infect timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);
    }
    
    // If zspawn is disabled, then stop.
    new bool:zspawn = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN]);
    if (!zspawn)
    {
        return;
    }
    
    // If timelimit is disabled, then stop.
    new bool:zspawntimelimit = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT]);
    if (!zspawntimelimit)
    {
        return;
    }
    
    // Get timelimit
    new Float:zspawntime = GetConVarFloat(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT_TIME]);
    
    // Start timer.
    tZSpawn = CreateTimer(zspawntime, ZSpawnTimer, _, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 */
ZSpawnOnRoundEnd()
{
    // If zspawn timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);
        
        // Reset timer handle.
        tZSpawn = INVALID_HANDLE;
    }
}

/**
 * Spawns a late-joining client into the game.
 * 
 * @param client    The client index.
 * @return          True if successful, false otherwise.
 */
bool:ZSpawnClient(client)
{
    // If zspawn is disabled, then stop.
    new bool:zspawn = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN]);
    if (!zspawn)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        // Tell client the command may only be used when on a team.
        ZR_PrintToChat(client, "Must be on team");
        return false;
    }
    
    // If client is alive, then stop.
    if (IsPlayerAlive(client))
    {
        // Tell client the command may only be used when dead.
        ZR_PrintToChat(client, "Must be dead");
        return false;
    }
    
    // Block if client has already played during this round.
    new serial = GetClientSerial(client);
    if (g_bZSpawnBlock[serial])
    {
        // Tell client the command may only be used when joining late.
        ZR_PrintToChat(client, "ZSpawn double spawn");
        return false;
    }
    
    // Block is the time limit is up.
    new bool:zspawntimelimit = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT]);
    if (zspawntimelimit)
    {
        if (tZSpawn == INVALID_HANDLE)
        {
            // Get timelimit
            new Float:zspawntime = GetConVarFloat(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT_TIME]);
            
            // Tell client the timelimit for this command has expired.
            ZR_PrintToChat(client, "ZSpawn timelimit", RoundToNearest(zspawntime));
            return false;
        }
    }
    
    // Tell respawn module to respawn client.
    RespawnSpawnClient(client);
    
    return true;
}

/**
 * Timer callback, resets handle.
 * 
 * @param timer     The timer handle.
 */
public Action:ZSpawnTimer(Handle:timer)
{
    // Reset timer handle.
    tZSpawn = INVALID_HANDLE;
}