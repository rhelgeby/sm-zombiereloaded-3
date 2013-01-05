/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          immunityhandler.inc
 *  Type:          Core module
 *  Description:   Manages infection immunity modes for every player.
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
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

/**
 * Current immunity modes.
 */
new ImmunityMode:PlayerImmunityMode[MAXPLAYERS + 1] = {Immunity_None, ...};

/**
 * Timers for handling timed immunity actions.
 */
new Handle:PlayerImmunityTimer[MAXPLAYERS + 1] = {INVALID_HANDLE, ...};

/**
 * Remaining time of timed immunity actions.
 */
new PlayerImmunityDuration[MAXPLAYERS + 1] = {-1, ...};

/**
 * Cached attacker index for delayed infections, if available.
 */
new PlayerImmunityAttacker[MAXPLAYERS + 1] = {0, ...};

/**
 * Timestamp of last action. Usage depends on mode (cooldown, etc).
 */
new PlayerImmunityLastUse[MAXPLAYERS + 1] = {0, ...};

/**
 * Whether the player has passed a threshold (infect mode).
 */
new bool:PlayerImmunityThresholdPassed[MAXPLAYERS + 1] = {false, ...};

/**
 * Handles immunity when a client is about to be infected. This function may
 * delay or block infection according to the immunity mode class settings.
 *
 * @param client    Client that's being infected.
 * @param attacker  Attacker client (zombie).
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityOnClientInfect(client, attacker)
{
    PrintToChatAll("ImmunityOnClientInfect(client=%d, attacker=%d)", client, attacker);
    
    // Get immunity mode from client class.
    new ImmunityMode:mode = ClassGetImmunityMode(client);
    
    // Check mode.
    switch(mode)
    {
        case Immunity_None:
        {
            // Instant infection.
            return false;
        }
        case Immunity_Full:
        {
            // Full immunity, do nothing.
            return true;
        }
        case Immunity_Infect:
        {
            return ImmunityInfectModeHandler(client, attacker);
        }
        case Immunity_Delay:
        {
            return ImmunityDelayModeHandler(client, attacker);
        }
        case Immunity_Shield:
        {
            return ImmunityShieldModeHandler(client);
        }
    }
    
    // Current mode doesn't apply to infection.
    return false;
}

/**
 * TraceAttack hook.
 *
 * Returns whether attacker damage should be blocked. If damage is blocked this
 * module will handle it.
 *
 * @param client    Client index.
 * @param attacker  Attacker client, if any.
 * @param damage    Damage received by client.
 * @param hitgroup  Hitgroup receiving damage.
 *
 * @return          True if damage should be blocked, false otherwise.
 */
bool:ImmunityOnClientTraceAttack(client, attacker, Float:damage, hitgroup, damageType)
{
    PrintToChatAll("ImmunityOnClientTraceAttack(client=%d, attacker=%d, damage=%f, hitgroup=%d, damageType=%d)", client, attacker, damage, hitgroup, damageType);
    
    // Check if there is no attacker (world damage).
    if (!ZRIsClientValid(attacker))
    {
        // Allow damage.
        return false;
    }
    
    // Get immunity mode from client class.
    new ImmunityMode:mode = ClassGetImmunityMode(client);
    
    // Check mode.
    switch(mode)
    {
        case Immunity_Full:
        {
            // Block damage.
            return true;
        }
        case Immunity_Infect:
        {
            // Client must be human.
            if (InfectIsClientInfected(client))
            {
                // Allow damage.
                return false;
            }
            
            // Check if damage give HP below the infection threshold.
            if (ImmunityBelowInfectThreshold(client, damage))
            {
                PrintToChatAll("ImmunityOnClientTraceAttack - Threshold passed");
                PlayerImmunityThresholdPassed[client] = true;
            }
            else
            {
                PlayerImmunityThresholdPassed[client] = false;
            }
            
            /*new threshold = ClassGetImmunityAmount(client);
            new clientHP = GetClientHealth(client);
            new dmg = RoundToNearest(damage);
            PrintToChatAll("threshold=%d, clientHp=%d", threshold, clientHP);
            
            // Prevent humans with low HP from dying when a zombie is
            // attacking, and stab to death is disabled (threshold above zero).
            if (clientHP - dmg <= 0.0 && threshold > 0)
            {
                // Client is about to be infected. Remove damage, but don't
                // block the damage event. The infect module need it to detect
                // zombie attack.
                damage = 0.0;
                
                // Client is about to be infected, re-add HP so they aren't
                // killed by knife. Don't block the damage, the infect
                // module need the damage event.
                //PrintToChatAll("Re-add HP.");
                //SetEntityHealth(client, clientHP + dmg);
                return false;
            }*/
        }
        case Immunity_Damage:
        {
            // Client must be zombie.
            if (!InfectIsClientInfected(client))
            {
                return false;
            }
            
            // Get attacker weapon.
            decl String:weapon[32];
            weapon[0] = 0;
            if (damageType == DMG_BLAST)
            {
                // Most likely a HE grenade. GetClientWeapon can't be used if
                // the attacker throw grenades. The attacker may switch weapon
                // before the grenade explodes.
                strcopy(weapon, sizeof(weapon), "hegrenade");
            }
            else
            {
                GetClientWeapon(attacker, weapon, sizeof(weapon));
            }
            
            // Since damage is blocked, trigger knock back hurt event manually.
            KnockbackOnClientHurt(client, attacker, weapon, hitgroup, RoundToNearest(damage));
            
            // Block damage from attacker.
            return true;
        }
        case Immunity_Delay:
        {
            // Client must be human.
            if (InfectIsClientInfected(client))
            {
                return false;
            }
            
            // Block damage if there's an infection in progress.
            
        }
    }
    
    PrintToChatAll("Allow damage.");
    return false;
}

/**
 * TakeDamage hook.
 *
 * Blocks or modifies damage in certain situations.
 *
 * @param client    Client index.
 * @param attacker  Attacker client, if any.
 * @param damage    Damage received by client.
 *
 * @return          True if damage was blocked, false otherwise.
 */
bool:ImmunityOnClientDamage(client, attacker, &Float:damage)
{
    PrintToChatAll("ImmunityOnClientDamage(client=%d, attacker=%d, damage=%f)", client, attacker, damage);
    
    // Check if there is no attacker (world damage).
    if (!ZRIsClientValid(attacker))
    {
        // Allow damage.
        return false;
    }
    
    // Get immunity mode from client class.
    new ImmunityMode:mode = ClassGetImmunityMode(client);
    
    switch(mode)
    {
        case Immunity_Infect:
        {
            // Prevent humans with low HP from dying when a zombie is
            // attacking, and stab to death is disabled (threshold above zero).
            if (ImmunityBelowInfectThreshold(client, damage))
            {
                PrintToChatAll("ImmunityOnClientDamage - Below threshold, removing damage.");
                // Client is about to be infected. Remove damage so the client
                // won't die.
                //damage = 0.0;
                
                // Fake hurt event because it's not triggered when the damage
                // was removed (because no one is actually hurt).
                InfectOnClientHurt(client, attacker, "knife");
                
                // Block damage to prevent player from dying.
                return true;
            }
        }
    }
    
    // Allow damage.
    return false;
}

/**
 * Client is about to receive knock back (zombie).
 *
 * @param       Client that's receiving knock back.
 *
 * @return      True if knock back should be blocked, false otherwise.
 */
bool:ImmunityOnClientKnockBack(client)
{
}

/**
 * Handles infect mode immunity.
 *
 * Allow humans to receive damage from zombies until HP is below a certain
 * threshold. If the threshold is zero, never infect.
 *
 * @param client    Client that's being infected.
 * @param attacker  Attacker client (zombie).
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityInfectModeHandler(client, attacker)
{
    PrintToChatAll("ImmunityInfectModeHandler(client=%d, attacker=%d)", client, attacker);
    
    // Note: ImmunityOnClientDamage and ImmunityOnClientTraceAttack hook into
    //       the damage module to prevent humans with low HP from dying when
    //       they're not supposed to.
    
    new threshold = ClassGetImmunityAmount(client);
    PrintToChatAll("threshold=%d", threshold);
    
    // Check if infection is disabled.
    if (threshold == 0)
    {
        // Infection is handled here: blocked.
        PrintToChatAll("Infection disabled, block infection.");
        return true;
    }
    
    if (PlayerImmunityThresholdPassed[client])
    {
        // Client HP below threshold, allow instant infection.
        return false;
    }
    
    /*new clientHP = GetClientHealth(client);
    PrintToChatAll("clientHP=%d", clientHP);
    
    if (clientHP < threshold)
    {
        // Client HP below threshold, allow instant infection.
        return false;
    }*/
    
    PrintToChatAll("Above threshold, block infection.");
    return true;
}

/**
 * Handles delayed infections.
 *
 * @param client    Client that's being infected.
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityDelayModeHandler(client, attacker)
{
    // Check if an infection is in progress
    if (PlayerImmunityTimer[client] != INVALID_HANDLE)
    {
        // Additional attacks while a delayed infection is in progress will
        // speedup the infection.
        
        // Trigger timer event to reduce duration.
        ImmunityDelayTimerHandler(PlayerImmunityTimer[client], client);
        
        // Block infection.
        return false;
    }
    
    // Start a delayed infection. Initialize duration and attacker.
    PlayerImmunityDuration[client] = ClassGetImmunityAmount(client);
    PlayerImmunityAttacker[client] = attacker;
    
    // Create repated 1-second timer.
    PlayerImmunityTimer[client] = CreateTimer(1.0, ImmunityDelayTimerHandler, client, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT);
    
    // Block infection.
    return false;
}

public Action:ImmunityDelayTimerHandler(Handle:timer, any:client)
{
    // Verify that client is connected and alive.
    if (!IsClientInGame(client) || !IsPlayerAlive(client))
    {
        // Client disconnected or died. Abort immunity action.
        PlayerImmunityTimer[client] = INVALID_HANDLE;
        ImmunityAbortHandler(client, false);
        return Plugin_Stop;
    }
    
    // TODO: Read cvar for reduction amount.
    new reduction = 2;
    
    // Reduce duration.
    PlayerImmunityDuration[client] -= reduction;
    
    // Check if time is up.
    if (PlayerImmunityDuration[client] <= 0)
    {
        // Time is up. Reset data.
        PlayerImmunityDuration[client] = 0;
        PlayerImmunityTimer[client] = INVALID_HANDLE;
        ZREndTimer(PlayerImmunityTimer[client]);
        
        // Infect client.
        InfectHumanToZombie(client, PlayerImmunityAttacker[client]);
        
        return Plugin_Stop;
    }
    
    return Plugin_Continue;
}

/**
 * Handles shield mode immunity.
 *
 * Zombies will get a shield against knock back, while humans become immune of
 * infections.
 *
 * @param client    Client deploying shield.
 * @param asZombie  Client is a zombie.
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityShieldModeHandler(client, bool:asZombie = true)
{
    return false;
}

/**
 * Aborts any immunity mode in action (shields, delays, etc.). Resets values.
 *
 * @param client        Client that's aborting immunity mode actions.
 * @param resetLastUse  Reset timestamp of last use. This will reset cooldown.
 */
ImmunityAbortHandler(client, bool:resetLastUse = true)
{
    // Stop timer, if running.
    ZREndTimer(PlayerImmunityTimer[client]);
    
    // Reset data.
    PlayerImmunityDuration[client] = -1;
    PlayerImmunityAttacker[client] = 0;
    PlayerImmunityThresholdPassed[client] = false;
    
    if (resetLastUse)
    {
        PlayerImmunityLastUse[client] = 0;
    }
}

/**
 * Aborts all immunity modes in action.
 *
 * @param resetLastUse  Reset timestamp of last use. This will reset cooldown.
 */
ImmunityAbortAll(bool:resetLastUse = true)
{
    for (new client = 0; client < MAXPLAYERS + 1; client++)
    {
        ImmunityAbortHandler(resetLastUse);
    }
}

ImmunityOnClientHuman(client)
{
    ImmunityAbortHandler(client);
}

ImmunityOnClientDeath(client)
{
    ImmunityAbortHandler(client, false);
}

ImmunityClientInit(client)
{
    ImmunityAbortHandler(client);
}

ImmunityClientSpawn(client)
{
    ImmunityAbortHandler(client, false);
}

ImmunityOnClientDisconnect(client)
{
    // Loop through attacker cache and remove client (set to 0).
}

ImmunityOnClientTeam(client)
{
    ImmunityAbortHandler(client);
}

ImmunityOnRoundEnd()
{
    ImmunityAbortAll();
}

ImmunityOnMapEnd()
{
    ImmunityAbortAll();
}

bool:ImmunityBelowInfectThreshold(client, Float:damage)
{
    new threshold = ClassGetImmunityAmount(client);
    new clientHP = GetClientHealth(client);
    new dmg = RoundToNearest(damage);
    
    PrintToChatAll("threshold=%d, clientHp=%d", threshold, clientHP);
    
    // Check if the damage go below the HP threshold.
    if (clientHP - dmg <= 0.0 && threshold > 0)
    {
        return true;
    }
    
    return false;
}

/**
 * Converts a string to an immunity mode.
 *
 * @param mode      String to convert.
 *
 * @return          Immunity mode or Immunity_Invalid on error.
 */
ImmunityMode:ImmunityStringToMode(const String:mode[])
{
    if (strlen(mode) == 0)
    {
        return Immunity_Invalid;
    }
    
    if (StrEqual(mode, "none", false))
    {
        return Immunity_None;
    }
    else if (StrEqual(mode, "full", false))
    {
        return Immunity_Full;
    }
    else if (StrEqual(mode, "infect", false))
    {
        return Immunity_Infect;
    }
    else if (StrEqual(mode, "damage", false))
    {
        return Immunity_Damage;
    }
    else if (StrEqual(mode, "delay", false))
    {
        return Immunity_Delay;
    }
    else if (StrEqual(mode, "shield", false))
    {
        return Immunity_Shield;
    }
    
    return Immunity_Invalid;
}
