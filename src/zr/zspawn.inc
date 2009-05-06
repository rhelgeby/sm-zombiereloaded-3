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
    
    // If client is alive, then stop.
    if (IsPlayerAlive(client))
    {
        // Tell client the command may only be used when joining late.
        ZR_PrintToChat(client, "Must be alive");
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
    
    // Tell respawn module to respawn client.
    RespawnSpawnClient(client);
    
    return true;
}
