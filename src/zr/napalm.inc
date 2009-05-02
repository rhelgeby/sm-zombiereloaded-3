/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        napalm.inc
 *   Description: Grenades burn zombies when damaged by them.
 *
 * ============================================================================
 */

/**
 * The fuse length of an hegrenade.
 */ 
#define GRENADE_FUSE_TIME 3.0

/**
 *  Client has been hurt.
 * 
 * @param client    The client index.
 * @param weapon    The weapon name. 
 */
NapalmOnClientHurt(client, const String:weapon[])
{
    // If player isn't a zombie, then stop.
    if (!InfectIsClientInfected(client))
    {
        return;
    }
    
    // If napalm time is invalid or 0, then stop.
    new Float:napalm_time = ClassGetNapalmTime(client);
    if (napalm_time <= 0.0)
    {
        return;
    }
    
    // If weapon is a grenade, then ignite player.
    if (StrEqual(weapon, "hegrenade", false))
    {
        // Put any existing fire out.
        ExtinguishEntity(client);
        
        // Re-ignite to start burn-time over.
        IgniteEntity(client, napalm_time);
    }
}

/**
 *  Weapon has been fired.
 *
 * @param weapon       The weapon name. 
 */
NapalmOnWeaponFire(const String:weapon[])
{
    // If human class can't throw napalm grenades, then stop. (TODO)
    /*if ()
    {
        return;
    }*/
    
    // If weapon isn't a grenade, then stop.
    if (!StrEqual(weapon, "hegrenade", false))
    {
        return;
    }
    
    // Wait .1 seconds.
    CreateTimer(0.1, NapalmIgniteGrenade);
}

/**
 * Timer callback, ignite's all hegrenade projectiles.
 * 
 * @param timer     The timer handle.
 */   
public Action:NapalmIgniteGrenade(Handle:timer)
{
    decl String:classname[64];
    
    // Get max entities.
    new maxentities = GetMaxEntities();
    
    // x = entity index.
    for (new x = 0; x <= maxentities; x++)
    {
        // If entity is invalid, then stop.
        if(!IsValidEdict(x))
        {
            continue;
        }
        
        GetEdictClassname(x, classname, sizeof(classname));
        if(StrEqual(classname, "hegrenade_projectile"))
        {
            IgniteEntity(x, GRENADE_FUSE_TIME);
        }
    }
}
