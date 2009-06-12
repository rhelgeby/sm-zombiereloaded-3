/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          healthregen.inc
 *  Type:          Core 
 *  Description:   Functions for managing health regeneration on a client.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
