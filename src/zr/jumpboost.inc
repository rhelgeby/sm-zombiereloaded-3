/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          jumpboost.inc
 *   Type:          Module
 *   Description:   Modified jump vector magnitudes.
 *
 * ============================================================================
 */

/**
 * The maximum forward velocity client can have for any additional push to be applied.
 */
#define JUMPBOOST_FORWARD_VEL_MAX 275.0

/**
 * Client is jumping.
 * 
 * @param client    The client index.
 */
JumpBoostOnClientJump(client)
{
    // Get class jump multipliers.
    new Float:distance = ClassGetJumpDistance(client);
    new Float:height = ClassGetJumpHeight(client);
    
    // Do not apply jump boost if class jump boost multiplier is 1.0.
    if (height == 1.0)
    {
        return;
    }
    
    // Get client's current velocity.
    new Float:vecVelocity[3];
    ToolsClientVelocity(client, vecVelocity, false);
    
    // Protect against bunnyhopping, if cvar is enabled.
    new bool:bunnyhopprotect = GetConVarBool(g_hCvarsList[CVAR_JUMPBOOST_BUNNYHOP_PROTECT]);
    if (bunnyhopprotect)
    {
        new Float:magnitude = SquareRoot(Pow(vecVelocity[0], 2.0) + Pow(vecVelocity[1], 2.0));
        if (magnitude >= JUMPBOOST_FORWARD_VEL_MAX)
        {
            // Set distance multiplier to 0.
            distance = 0.0;
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
