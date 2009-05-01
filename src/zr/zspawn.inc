/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        (Module) zspawn.inc
 *   Description: Handles zspawn command, spawns late-joining clients into the game.
 *
 * ============================================================================
 */

/**
 * Array to block zspawn for a unique client serial number.
 */
new bool:g_bZSpawnBlock[MAXPLAYERS + 1];

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
}

ZSpawnClient(client)
{
    // If zspawn is disabled, then stop.
    new bool:zspawn = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN]);
    if (!zspawn)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        return;
    }
    
    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        return;
    }
    
    // If client is alive, then stop.
    if (IsPlayerAlive(client))
    {
        return;
    }
    
    // Block if client has already played during this round.
    new serial = GetClientSerial(client);
    if (g_bZSpawnBlock[serial])
    {
        return;
    }
    
    // Tell respawn module to respawn client.
    RespawnSpawnClient(client);
}
