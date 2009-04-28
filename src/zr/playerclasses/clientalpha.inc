/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        clientalpha.inc
 *   Description: Handles transparency on clients.
 *
 * ============================================================================
 */

/**
 * Updates the alpha value on a player.
 */
ClassAlphaUpdate(client)
{
    new current_health = GetClientHealth(client);
    new max_health     = ClassGetHealth(client);
    new max_damage     = ClassGetAlphaDamage(client);
    
    // Calculate if enough damage is done to change alpha.
    if ((max_health - current_health) > max_damage)
    {
        new alpha_damaged = ClassGetAlphaDamaged(client);
        ToolsSetClientAlpha(client, alpha_damaged);
    }
}
