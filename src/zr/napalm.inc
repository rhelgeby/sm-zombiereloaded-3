/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          napalm.inc
 *  Type:          Module 
 *  Description:   Grenades burn zombies when damaged by them.
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
 * The fuse length of an hegrenade.
 */ 
#define GRENADE_FUSE_TIME 3.0

/**
 *  Client has been hurt.
 * 
 * @param client    The client index.
 * @param attacker  The attacker index.
 * @param weapon    The weapon name. 
 */
NapalmOnClientHurt(client, attacker, const String:weapon[])
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
    
    // If the attacker can't throw napalm grenades, then stop.
    if (!ClassGetHasNapalm(attacker))
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
 * Client has been killed.
 * 
 * @param client    The client index.
 */
NapalmOnClientDeath(client)
{
    // Extinguish any flames to stop burning sounds.
    ExtinguishEntity(client);
}

/**
 *  Weapon has been fired.
 *
 * @param client       The client index.
 * @param weapon       The weapon name.
 */
NapalmOnWeaponFire(client, const String:weapon[])
{
    // If grenade fire is disabled, then stop.
    new bool:napalmignite = GetConVarBool(g_hCvarsList[CVAR_NAPALM_IGNITE]);
    if (!napalmignite)
    {
        return;
    }
    
    // If human class can't throw napalm grenades, then stop.
    if (!ClassGetHasNapalm(client))
    {
        return;
    }
    
    // If weapon isn't a grenade, then stop.
    if (!StrEqual(weapon, "hegrenade", false))
    {
        return;
    }
    
    // Wait .15 seconds.
    CreateTimer(0.15, NapalmIgniteGrenade);
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
