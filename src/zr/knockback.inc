/**
 * ====================
 *   Zombie:Reloaded
 *   File: knockback.inc
 *   Author: Greyscale
 * ==================== 
 */
  
/** Client has been hurt.
 *
 * @param client        The client index. (zombie)
 * @param attacker      The attacker index. (human)
 * @param weapon        The weapon used.
 * @param hitgroup      Hitgroup attacker has damaged. 
 * @param dmg_health    Damage done.
 */
KnockbackOnClientHurt(client, attacker, const String:weapon[], hitgroup, dmg_health)
{
    // If attacker is invalid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return;
    }
    
    // Client is a human, then stop.
    if (InfectIsClientHuman(client))
    {
        return;
    }
    
    // If attacker is a zombie, then stop.
    if (InfectIsClientInfected(attacker))
    {
        return;
    }
    
    // Get zombie knockback value.
    new Float:knockback = ClassGetKnockback(client);
    
    new Float:clientloc[3];
    new Float:attackerloc[3];
    
    GetClientAbsOrigin(client, clientloc);
    
    // Check if a grenade was thrown.
    if (StrEqual(weapon, "hegrenade"))
    {
        // Get the location of the grenade.
        if (KnockbackFindExplodingGrenade(attackerloc) == -1)
        {
            // If the grenade wasn't found, then stop.
            return;
        }
    }
    else
    {
        // Get attackers eye position.
        GetClientEyePosition(attacker, attackerloc);
        
        // Get attackers eye angles.
        new Float:attackerang[3];
        KnockbackGetClientEyeAngles(attacker, attackerang);
        
        // Calculate knockback end-vector.
        TR_TraceRayFilter(attackerloc, attackerang, MASK_ALL, RayType_Infinite, KnockbackTRFilter);
        TR_GetEndPosition(clientloc);
    }
    
    // Retrieve weapon knockback boost.
    new Float:boostWeapon = WeaponGetWeaponKnockback(weapon);
    
    // Retrieve hitgroup knockback boost.
    new Float:boostHitgroup = HitgroupsGetHitgroupKnockback(hitgroup);
    
    // Apply all knockback multipliers.
    knockback *= float(dmg_health) * boostWeapon * boostHitgroup;
    
    // Apply knockback.
    KnockbackSetVelocity(client, attackerloc, clientloc, knockback);
}

/**
 * Sets velocity on a player.
 *  
 * @param client        The client index.
 * @param startpoint    The starting coordinate to push from.
 * @param endpoint      The ending coordinate to push towards.
 * @param magnitude     Magnitude of the push.
 */  
KnockbackSetVelocity(client, const Float:startpoint[3], const Float:endpoint[3], Float:magnitude)
{
    // Create vector from the given starting and ending points.
    new Float:vector[3];
    MakeVectorFromPoints(startpoint, endpoint, vector);
    
    // Normalize the vector (equal magnitude at varying distances).
    NormalizeVector(vector, vector);
    
    // Apply the magnitude by scaling the vector (multiplying each of its components).
    ScaleVector(vector, magnitude);
    
    // ADD the given vector to the client's current velocity.
    ToolsClientVelocity(client, vector);
}

/**
 * Trace Ray forward, used as a filter to continue tracing if told so. (See sdktools_trace.inc)
 *  
 * @param entity        The entity index.
 * @param contentsMask  The contents mask.
 * @return              True to allow hit, false to continue tracing. 
 */ 
public bool:KnockbackTRFilter(entity, contentsMask)
{
    // If entity is a player, continue tracing.
    if (entity > 0 && entity < MAXPLAYERS)
    {
        return false;
    }
    
    // Allow hit.
    return true;
}

/**
 * Find the location of an exploding grenade (currently inflicting damage in player_hurt).
 *  
 * @param heLoc     The location of the exploding grenade.
 * @return          The entity index of the grenade. 
 */  
KnockbackFindExplodingGrenade(Float:heLoc[3])
{
    decl String:classname[64];
    
    // Find max entities and loop through all of them.
    new maxentities = GetMaxEntities();
    for (new x = MaxClients; x <= maxentities; x++)
    {
        // If entity is invalid, then stop.
        if (!IsValidEdict(x))
        {
            continue;
        }
        
        // If entity isn't a grenade, then stop.
        GetEdictClassname(x, classname, sizeof(classname));
        if (!StrEqual(classname, "hegrenade_projectile", false))
        {
            continue;
        }
        
        // If m_takedamage is set to 0, we found our grenade.
        new takedamage = GetEntProp(x, Prop_Data, "m_takedamage");
        if (takedamage == 0)
        {
            // Return its location.
            GetEntPropVector(x, Prop_Send, "m_vecOrigin", heLoc);
            
            // Return its entity index.
            return x;
        }
    }
    
    // Didn't find the grenade.
    return -1;
}

/**
 * Get client's eye angles.
 * 
 * @param client    The client index.
 * @param vecAngles The angle vector of the client's eyes.
 */
KnockbackGetClientEyeAngles(client, Float:vecAngles[3])
{
    SDKCall(g_hToolsEyeAngles, client, vecAngles);
}