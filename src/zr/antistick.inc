/**
 * ====================
 *   Zombie:Reloaded
 *   File: antistick.inc
 *   Author: Greyscale
 * ====================
 */

/**
 * @section Offsets relating to player hull dimensions.
*/
#define PLAYER_HULL_XY_OFFSET 32
#define PLAYER_HULL_Z_OFFSET 12
#define PLAYER_HULL_STACK_OFFSET 14
/**
 * @endsection
*/

/**
 * Handle to keep track of AntiStickTimer.
*/
new Handle:tAntiStick = INVALID_HANDLE;

/**
 * Restarts the AntiStickTimer function.
*/
AntiStickRestart()
{
    if (tAntiStick != INVALID_HANDLE)
    {
        KillTimer(tAntiStick);
    }
    
    new bool:antistick = GetConVarBool(gCvars[CVAR_ANTISTICK]);
    if (antistick)
    {
        new Float:interval = GetConVarFloat(gCvars[CVAR_ANTISTICK_INTERVAL]);
        tAntiStick = CreateTimer(interval, AntiStickTimer, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
    }
}

/**
 * Resets the handle that keeps track of the AntiStickTimer function.
*/
AntiStickReset()
{
    tAntiStick = INVALID_HANDLE;
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
        if (xydistance < PLAYER_HULL_XY_OFFSET)
        {
            if (clientloc[2] <= stuckloc[2])
            {
                new Float:eyeloc[3];
                GetPlayerEyePosition(client, eyeloc);
                
                // Get the distance between the eyes and feet and subtract the stack "view crush."
                new Float:eyedistance = FloatAbs(eyeloc[2] - clientloc[2]) - PLAYER_HULL_STACK_OFFSET;
                new Float:zdistance = FloatAbs(stuckloc[2] - clientloc[2]);
                
                if (zdistance <= eyedistance + PLAYER_HULL_Z_OFFSET)
                {
                    return x;
                }
            }
        }
    }
    
    return -1;
}

/**
 * Repeated timer function.
 * Automatically unsticks players that are stuck together. 
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
        
        if (CanCollide(x))
        {
            NoCollide(x, true);
            CreateTimer(0.5, AntiStickSolidify, x, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
        }
        
        if (CanCollide(stuckindex))
        {
            NoCollide(stuckindex, true);
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
    if (!IsClientInGame(client) || !IsPlayerAlive(client) || CanCollide(client))
    {
        return Plugin_Stop;
    }
    
    // Stop if the player is still stuck.
    if (AntiStickIsStuck(client) > -1)
    {
        return Plugin_Continue;
    }
    
    NoCollide(client, false);
    
    return Plugin_Stop;
}
    
        