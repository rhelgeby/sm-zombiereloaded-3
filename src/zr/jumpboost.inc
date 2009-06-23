/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          jumpboost.inc
 *  Type:          Module
 *  Description:   Modified jump vector magnitudes.
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

/**
 * Client is jumping.
 * 
 * @param client    The client index.
 */
JumpBoostOnClientJumpPost(client)
{
    // Get class jump multipliers.
    new Float:distancemultiplier = ClassGetJumpDistance(client);
    new Float:heightmultiplier = ClassGetJumpHeight(client);
    
    new Float:vecVelocity[3];
    
    // Get client's current velocity.
    ToolsClientVelocity(client, vecVelocity, false);
    
    // Apply multipliers to jump vector.
    vecVelocity[0] *= distancemultiplier;
    vecVelocity[1] *= distancemultiplier;
    vecVelocity[2] *= heightmultiplier;
    
    // Protect against bunnyhop.
    JumpBoostBHopProtect(vecVelocity);
    
    // Set new velocity.
    ToolsClientVelocity(client, vecVelocity, true, false);
}

/**
 * This function protects against excessive bunnyhopping.
 * Note: This ONLY stops bunnyhopping from being worse than CS:S already allows.
 * 
 * @param vecVelocity           The velocity of the client jumping.
 * @param distancemultiplier    The distance multiplier used on this jump.
 */
stock JumpBoostBHopProtect(Float:vecVelocity[])
{
    // If bunnyhop protection is disabled, then stop.
    new bool:bunnyhopprotect = GetConVarBool(g_hCvarsList[CVAR_JUMPBOOST_BHOP_PROTECT]);
    if (!bunnyhopprotect)
    {
        return;
    }
    
    // Calculate the magnitude of jump on the xy plane.
    new Float:magnitude = SquareRoot(Pow(vecVelocity[0], 2.0) + Pow(vecVelocity[1], 2.0));
    
    // If the magnitude doesn't exceed the limit, then stop.
    new Float:bunnyhopmax = GetConVarFloat(g_hCvarsList[CVAR_JUMPBOOST_BHOP_MAX]);
    if (magnitude <= bunnyhopmax)
    {
        return;
    }
    
    // Now we must scale the jump.
    new Float:bunnyhopscale = GetConVarFloat(g_hCvarsList[CVAR_JUMPBOOST_BHOP_SCALE]);
    
    // Scale with cvar value.
    vecVelocity[0] *= (magnitude * bunnyhopscale) / magnitude;
    vecVelocity[1] *= (magnitude * bunnyhopscale) / magnitude;
}
