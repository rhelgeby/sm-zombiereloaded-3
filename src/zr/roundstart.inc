/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          roundstart.inc
 *   Type:          Core
 *   Description:   Handles round start actions.
 *
 * ============================================================================
 */

/**
 * List of objective entities.
 */
#define ROUNDSTART_OBJECTIVE_ENTITIES "func_bomb_target|func_hostage_rescue|c4|hostage_entity"

/**
 * The round is starting.
 */
RoundStartOnRoundStart()
{
    // Kill all objective entities.
    RoundStartKillObjectives();
}

/**
 * Kills all objective entities.
 */
RoundStartKillObjectives()
{
    decl String:classname[64];
    
    // Get max entity count.
    new maxentities = GetMaxEntities();
    
    // x = entity index.
    for (new x = 0; x <= maxentities; x++)
    {
        // If entity isn't valid, then stop.
        if(!IsValidEdict(x))
        {
            continue;
        }
        
        // Get valid edict's classname.
        GetEdictClassname(x, classname, sizeof(classname));
        
        // Check if it matches any objective entities, then stop if it doesn't.
        if(StrContains(ROUNDSTART_OBJECTIVE_ENTITIES, classname) == -1)
        {
            continue;
        }
        
        // Entity is an objective, kill it.
        RemoveEdict(x);
    }
}
