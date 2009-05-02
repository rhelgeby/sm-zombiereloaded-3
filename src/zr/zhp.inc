/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        zhp.inc
 *   Description: Displays HP to zombies.
 *   Author:      Greyscale, Richard Helgeby
 *
 * ============================================================================
 */

/**
 * Array for storing ZHP timer handles per client.
 */
new Handle:tZHP[MAXPLAYERS + 1];

/**
 * Array for flagging client to display HP
 */
new bool:pZHP[MAXPLAYERS + 1];

/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
ZHPClientInit(client)
{
    // Get default client setting from cvar.
    new bool:zhp = GetConVarBool(g_hCvarsList[CVAR_ZHP_DEFAULT]);
    
    // Set flag to default value.
    pZHP[client] = zhp;
    
    // Reset timer handle.
    tZHP[client] = INVALID_HANDLE;
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
ZHPOnClientSpawn(client)
{
    // If timer is running, kill it.
    if (tZHP[client] != INVALID_HANDLE)
    {
        KillTimer(tZHP[client]);
    }
    
    // Reset timer handle.
    tZHP[client] = INVALID_HANDLE;
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
ZHPOnClientDeath(client)
{
    // If timer is running, kill it.
    if (tZHP[client] != INVALID_HANDLE)
    {
        KillTimer(tZHP[client]);
    }
    
    // Reset timer handle.
    tZHP[client] = INVALID_HANDLE;
}

/**
 * Player has been infected.
 * 
 * @param client    The client index.
 */
ZHPOnClientInfected(client)
{
    // If ZHP is disabled, then stop.
    new bool:zhp = GetConVarBool(g_hCvarsList[CVAR_ZHP]);
    if (!zhp)
    {
        return;
    }
    
    // Update HP display.
    ZHPUpdateHUD(client);
    
    // If timer is currently running, kill it.
    if (tZHP[client] != INVALID_HANDLE)
    {
        KillTimer(tZHP[client]);
    }
    
    // Start repeating timer to update display.
    tZHP[client] = CreateTimer(5.0, ZHPTimer, client, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
}

/**
 * Client has been hurt.
 * 
 * @param client    The client index.
 */
ZHPOnClientHurt(client)
{
    // Update HP display.
    ZHPUpdateHUD(client);
}

/**
 * Zombie has gained health for infecting a player.
 * 
 * @param client    The client index.
 */
ZHPOnHealthInfectGain(client)
{
    // Update HP display.
    ZHPUpdateHUD(client);
}

/**
 * Toggle ZHP on a client.
 * 
 * @param client    The client index.
 */
ZHPToggle(client)
{
    // If ZHP is disabled, then stop.
    new bool:zhp = GetConVarBool(g_hCvarsList[CVAR_ZHP]);
    if (!zhp)
    {
        // Tell client feature is disabled.
        ZR_PrintToChat(client, "Feature is disabled");
        
        // Stop.
        return;
    }
    
    // If ZHP is enabled, then tell client it's being disabled.
    if (pZHP[client])
    {
        ZR_PrintToChat(client, "ZHP disable");
    }
    // If ZHP is disabled, then tell client it's being enabled.
    else
    {
        ZR_PrintToChat(client, "ZHP enable");
        
        // Update HP display.
        ZHPUpdateHUD(client);
    }
    
    // Toggle ZHP flag.
    pZHP[client] = !pZHP[client];
}

/**
 * Update HP display for a player.
 * 
 * @param client    The client index.
 */ 
ZHPUpdateHUD(client)
{
    // If ZHP is disabled, then stop.
    new bool:zhp = GetConVarBool(g_hCvarsList[CVAR_ZHP]);
    if (!zhp)
    {
        return;
    }
    
    // If player is a zombie, or has ZHP disabled, then stop.
    if (!InfectIsClientInfected(client) || !pZHP[client])
    {
        return;
    }
    
    // Get health, and if below 0 then set back to 0. (for display purposes)
    new health = GetClientHealth(client);
    if (health < 0)
    {
        health = 0;
    }
    
    // Display HP
    ZR_HudHint(client, "Display HP", health);
}

/**
 * Timer callback.  Repetitively calls ZHPUpdateHUD() until stopped.
 * 
 * @param timer     The timer handle. 
 * @param client    The client index.
 */ 
public Action:ZHPTimer(Handle:timer, any:client)
{
    // If client leaves, then stop timer.
    if (!IsClientInGame(client))
    {
        return Plugin_Stop;
    }
    
    // Update HP display.
    ZHPUpdateHUD(client);
    
    // Allow timer to continue.
    return Plugin_Continue;
}
