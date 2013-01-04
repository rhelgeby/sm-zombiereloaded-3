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
 * Last time a temporary immunity action was used (currently just shield mode).
 * Used with cooldown of actions.
 */
new PlayerImmunityLastUse[MAXPLAYERS + 1] = {-1, ...};

/**
 * Handles immunity when a client is about to be infected. This function may
 * delay or block infection according to the immunity mode class settings.
 *
 * @param client    Client that's being infected.
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityOnClientInfect(client)
{
    // Get immunity data from client class.
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
        case Immunity_Damage:
        {
            return ImmunityDamageModeHandler(client);
        }
        case Immunity_Delay:
        {
            return ImmunityDelayModeHandler(client);
        }
        case Immunity_Shield:
        {
            return ImmunityShieldModeHandler(client);
        }
        default:
        {
            ThrowError("Invalid immunity mode. This is a bug in ZR.");
            return true;
        }
    }
}

/**
 * Client is about to receive damage (zombie).
 *
 * @param       Client that's receiving damage.
 *
 * @return      True if damage should be blocked, false otherwise.
 */
bool:ImmunityOnClientHurt(client)
{
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
 * Handles damage mode immunity.
 *
 * @param client    Client that's being infected.
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityDamageModeHandler(client)
{
}

/**
 * Handles delayed infections.
 *
 * @param client    Client that's being infected.
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityDelayModeHandler(client)
{
}

/**
 * Handles shield mode immunity.
 *
 * @param client    Client that's being infected.
 *
 * @return          True if infection will be handled by this module, false if
 *                  infection can be applied instantly.
 */
bool:ImmunityShieldModeHandler(client)
{
}

/**
 * Aborts any immunity mode in action (shields, delays, etc.).
 *
 * @param client    Client that's aborting immunity mode actions.
 */
ImmunityAbortHandler(client)
{
    // TODO: Stop timers, disable shield.
}

ImmunityAbortAll()
{
}

ImmunityOnClientHuman(client)
{
}

ImmunityOnClientDeath(client, attacker)
{
}

ImmunityClientInit(client)
{
    // Abord old actions, initialize variables.
}

ImmunityOnClientDisconnect(client, attacker)
{
}

ImmunityOnClientTeam(client, team)
{
}

ImmunityOnRoundEnd()
{
    ImmunityAbortAll();
}

ImmunityOnMapEnd()
{
    ImmunityAbortAll();
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
