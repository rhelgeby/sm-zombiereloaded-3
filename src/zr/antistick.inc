/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          antistick.inc
 *   Type:          Module
 *   Description:   Antistick system.
 *
 * ============================================================================
 */

/**
 * @section Collision values.
*/
#define ANTISTICK_COLLISIONS_OFF 2
#define ANTISTICK_COLLISIONS_ON 5
/**
 * @endsection
*/

/**
 * @section Offsets relating to player hull dimensions.
*/
#define ANTISTICK_PLAYER_HULL_XY_OFFSET 32
#define ANTISTICK_PLAYER_HULL_Z_OFFSET 12
#define ANTISTICK_PLAYER_HULL_STACK_OFFSET 14
/**
 * @endsection
*/

/**
 * Handle to keep track of AntiStickTimer.
*/
new Handle:tAntiStick = INVALID_HANDLE;

/**
 * Map is starting.
*/
AntiStickOnMapStart()
{
    // Reset timer handle.
    tAntiStick = INVALID_HANDLE;
}

/**
 * The round is starting.
 */
AntiStickOnRoundStart()
{
    // If timer is running, kill it.
    if (tAntiStick != INVALID_HANDLE)
    {
        KillTimer(tAntiStick);
    }
    
    // If antistick is disabled, then stop.
    new bool:antistick = GetConVarBool(g_hCvarsList[CVAR_ANTISTICK]);
    if (!antistick)
    {
        return;
    }
    
    new Float:interval = GetConVarFloat(g_hCvarsList[CVAR_ANTISTICK_INTERVAL]);
    tAntiStick = CreateTimer(interval, AntiStickTimer, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * Checks if a player is currently stuck within another player.
 *
 * @param client    The client index.
 * @return          The client index of the other stuck player, -1 when
 *                  player is not stuck.
*/
AntiStickIsStuck(client)
{
    new Float:clientloc[3];
    new Float:stuckloc[3];
    
    GetClientAbsOrigin(client, clientloc);
    
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // Validate player is in-game, alive, and isn't the player being checked. ('client')
        if (!IsClientInGame(x) || !IsPlayerAlive(x) || x == client)
        {
            continue;
        }
        
        GetClientAbsOrigin(x, stuckloc);
        
        // x-y plane distance formula: sqrt((x2-x1)^2 + (y2-y1)^2)
        new Float:xydistance = SquareRoot(Pow(stuckloc[0] - clientloc[0], 2.0) + Pow(stuckloc[1] - clientloc[1], 2.0));
        if (xydistance < ANTISTICK_PLAYER_HULL_XY_OFFSET)
        {
            if (clientloc[2] <= stuckloc[2])
            {
                new Float:eyeloc[3];
                GetClientEyePosition(client, eyeloc);
                
                // Get the distance between the eyes and feet and subtract the stack "view crush."
                new Float:eyedistance = FloatAbs(eyeloc[2] - clientloc[2]) - ANTISTICK_PLAYER_HULL_STACK_OFFSET;
                new Float:zdistance = FloatAbs(stuckloc[2] - clientloc[2]);
                
                if (zdistance <= eyedistance + ANTISTICK_PLAYER_HULL_Z_OFFSET)
                {
                    return x;
                }
            }
        }
    }
    
    return -1;
}

/**
 * Timer callback, automatically unsticks players that are stuck together. 
*/
public Action:AntiStickTimer(Handle:timer)
{
    // x = client index
    for (new x = 1; x <= MaxClients; x++)
    {
        // Validate player is in-game and alive.
        if (!IsClientInGame(x) || !IsPlayerAlive(x))
        {
            continue;
        }
        
        // Stop if the player isn't stuck.
        new stuckindex = AntiStickIsStuck(x);
        if (stuckindex == -1)
        {
            continue;
        }
        
        if (AntiStickClientCollisionGroup(x, false))
        {
            AntiStickClientCollisionGroup(x, true, ANTISTICK_COLLISIONS_OFF);
            CreateTimer(0.5, AntiStickSolidify, x, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
        }
        
        if (AntiStickClientCollisionGroup(stuckindex, false))
        {
            AntiStickClientCollisionGroup(stuckindex, true, ANTISTICK_COLLISIONS_OFF);
            CreateTimer(0.5, AntiStickSolidify, stuckindex, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
        }
    }
}

/**
 * Repeated timer function.
 * Re-solidifies a player being unstuck.
 *
 * @param timer     The timer handle. 
 * @param client    The client index.
*/
public Action:AntiStickSolidify(Handle:timer, any:client)
{
    // Validate player is in-game, alive, and is being unstuck.
    if (!IsClientInGame(client) || !IsPlayerAlive(client) || AntiStickClientCollisionGroup(client, false))
    {
        return Plugin_Stop;
    }
    
    // Stop if the player is still stuck.
    if (AntiStickIsStuck(client) > -1)
    {
        return Plugin_Continue;
    }
    
    AntiStickClientCollisionGroup(client, true, ANTISTICK_COLLISIONS_ON);
    
    return Plugin_Stop;
}

/**
 * Set collision group flags on a client.
 * @param client            The client index.
 * @param collisiongroup    Collision group flag.
 * @return                  The collision group on the client, -1 if applying collision group. 
 */
AntiStickClientCollisionGroup(client, bool:apply = true, collisiongroup = 0)
{
    if (apply)
    {
        SetEntData(client, g_iToolsCollisionGroup, collisiongroup, 1, true);
        
        return -1;
    }
    
    return GetEntData(client, g_iToolsCollisionGroup, 1);
}
