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
JumpBoostOnClientJump(client)
{
    new Float:vecVelocity[3];
    new Float:vecBuffer[3];
    new Float:magnitude;
    
    // Get class jump multipliers.
    new Float:distance = ClassGetJumpDistance(client);
    new Float:height = ClassGetJumpHeight(client);
    
    // Do not apply jump boost if settings indicate no boost.
    if (height == 0.0 && distance == 0.2)
    {
        return;
    }
    
    // Get client's current velocity.
    ToolsClientVelocity(client, vecVelocity, false);
    
    // Get bunny hop setting.
    new bool:bunnyhopprotect = GetConVarBool(g_hCvarsList[CVAR_JUMPBOOST_BUNNYHOP_PROTECT]);
    
    // Check if bunny hop protection is enabled.
    if (bunnyhopprotect)
    {
        // Get jump boost settings.
        new bool:bunnyhopreset = GetConVarBool(g_hCvarsList[CVAR_JUMPBOOST_BUNNYHOP_RESET]);
        new Float:bunnyhopmax = GetConVarFloat(g_hCvarsList[CVAR_JUMPBOOST_BUNNYHOP_MAX]);
        
        // Get movement distance.
        magnitude = SquareRoot(Pow(vecVelocity[0], 2.0) + Pow(vecVelocity[1], 2.0));
        
        // Check if maximum speed is reached and reset setting is set.
        if ((magnitude > bunnyhopmax) && bunnyhopreset)
        {
            // Reset horizontal velocity.
            distance = 0.0;
        }
        
        // Check if maximum speed is reached and reset setting is disabled.
        else if ((magnitude > bunnyhopmax) && !bunnyhopreset)
        {
            // Set horizontal velocity to maximum allowed.
            
            // Copy to buffer.
            vecBuffer[0] = vecVelocity[0];
            vecBuffer[1] = vecVelocity[1];
            vecBuffer[2] = 1.0;                 // Dummy value.
            
            // Normalize buffer vector to make it a unit vector.
            NormalizeVector(vecBuffer, vecBuffer);
            
            // Restore horizontal axis in buffer to velocity vector.
            vecVelocity[0] = vecBuffer[0];
            vecVelocity[1] = vecBuffer[1];
            
            // Set distance to maximum speed.
            distance = bunnyhopmax;
        }
    }
    
    // Apply forward jump boost.
    vecVelocity[0] *= distance;
    vecVelocity[1] *= distance;
    
    // Apply vertical jump boost.
    vecVelocity[2] += height;
    
    // Apply velocity.
    JumpBoostSetClientVelocity(client, vecVelocity);
}

/**
 * Set new velocity on client. (Special method separate from ToolsClientVelocity)
 * 
 * @param client        The client index.
 * @param vecVelocity   Velocity to set on client.
 */
JumpBoostSetClientVelocity(client, const Float:vecVelocity[3])
{
    SetEntDataVector(client, g_iToolsBaseVelocity, vecVelocity, true);
}
