/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        healthregen.inc
 *   Description: Functions for managing health regeneration on a client.
 *   Author:      Richard Helgeby
 *
 * ============================================================================
 */

new ClientHealthRegenAmount[MAXPLAYERS + 1];
new ClientHealthRegenMax[MAXPLAYERS + 1];
new Handle:tHealthRegen[MAXPLAYERS + 1] = {INVALID_HANDLE, ...};

ClassHealthRegenInitialize(client, Float:interval, amount, max)
{
    ClientHealthRegenAmount[client] = amount;
    ClientHealthRegenMax[client] = max;
    ClassHealthRegenStart(client, interval);
}

ClassHealthRegenStart(client, Float:interval)
{
    // Kill the timer if it exist.
    if (tHealthRegen[client] != INVALID_HANDLE)
    {
        KillTimer(tHealthRegen[client]);
        tHealthRegen[client] = INVALID_HANDLE;
    }
    
    tHealthRegen[client] = CreateTimer(interval, ClassHealthRegenTimer, client, TIMER_REPEAT);
}

ClassHealthRegenStop(client)
{
    // Kill the timer if it exist.
    if (tHealthRegen[client] != INVALID_HANDLE)
    {
        KillTimer(tHealthRegen[client]);
        tHealthRegen[client] = INVALID_HANDLE;
    }
}

public Action:ClassHealthRegenTimer(Handle:timer, any:client)
{
    // Kill the timer if the player is dead.
    if (!IsPlayerAlive(client))
    {
        tHealthRegen[client] = INVALID_HANDLE;
        return Plugin_Stop;
    }
    
    new health = GetClientHealth(client);    
    health += ClientHealthRegenAmount[client];
    
    // Check if the health points is below the limit.
    if (health < ClientHealthRegenMax[client])
    {
        // Increase health.
        SetEntityHealth(client, health);
    }
    
    return Plugin_Continue;
}