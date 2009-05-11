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
 * Client is jumping.
 * 
 * @param client    The client index.
 */
JumpBoostOnClientJump(client)
{
    // Get class jump multipliers.
    new Float:distance = ClassGetJumpDistance(client);
    new Float:height = ClassGetJumpHeight(client);
    
    // Get client's current velocity.
    new Float:vecVelocity[3];
    ToolsClientVelocity(client, vecVelocity, false);
    
    // Apply jump values.
    vecVelocity[0] *= distance;
    vecVelocity[1] *= distance;
    vecVelocity[2] += height;
    
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
