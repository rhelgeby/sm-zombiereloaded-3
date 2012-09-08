/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          knockback.sp
 *  Type:          Test plugin
 *  Description:   Tests basic knock back.
 *
 *  Copyright (C) 2009-2012  Greyscale, Richard Helgeby
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

#pragma semicolon 1
#include <sourcemod>
#include <sdktools>
#include <sdkhooks-2.2>

public Plugin:myinfo =
{
    name = "Knock back",
    author = "Greyscale | Richard Helgeby",
    description = "Tests basic knock back.",
    version = "1.0.0",
    url = "http://code.google.com/p/zombiereloaded/"
};

new Handle:hKnockBackMultiplier;
new g_iToolsVelocity;

public OnPluginStart()
{
    hKnockBackMultiplier = CreateConVar("zrtest_knockback", "4.0", "Knock back multiplier.");
    
    /*if (!HookEventEx("player_hurt", Event_PlayerHurt))
    {
        LogError("Failed to hook event player_hurt.");
    }*/
    
    // If offset "m_vecVelocity[0]" can't be found, then stop the plugin.
    g_iToolsVelocity = FindSendPropInfo("CBasePlayer", "m_vecVelocity[0]");
    if (g_iToolsVelocity == -1)
    {
        LogError("Offset \"CBasePlayer::m_vecVelocity[0]\" was not found.");
    }
}

public OnClientPutInServer(client)
{
    SDKHook(client, SDKHook_OnTakeDamagePost, OnTakeDamagePost);
}

public OnClientDisconnect(client)
{
    SDKUnhook(client, SDKHook_OnTakeDamagePost, OnTakeDamagePost);
}

/*public Action:Event_PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    new victim = GetClientOfUserId(GetEventInt(event, "userid"));
    new damage = GetEventInt(event, "dmg_health");
}*/

public Action:OnTakeDamage(victim, &attacker, &inflictor, &Float:damage, &damagetype, &weapon, Float:damageForce[3], Float:damagePosition[3])
{
    KnockbackOnClientHurt(victim, attacker, Float:damage);
    
    // Allow damage.
    return Plugin_Continue;
}

public OnTakeDamagePost(victim, attacker, inflictor, Float:damage, damagetype, weapon, const Float:damageForce[3], const Float:damagePosition[3])
{
    if (attacker > 0 && attacker < MaxClients)
    {
        KnockbackOnClientHurt(victim, attacker, Float:damage);
    }
}

/** Client has been hurt.
 *
 * @param client        The client index. (zombie)
 * @param attacker      The attacker index. (human)
 * @param weapon        The weapon used.
 * @param hitgroup      Hitgroup attacker has damaged. 
 * @param dmg_health    Damage done.
 */
KnockbackOnClientHurt(client, attacker, Float:dmg_health)
{
    // If attacker is invalid, then stop.
    if (!(attacker > 0 || attacker < MaxClients))
    {
        return;
    }
    
    // Get zombie knockback value.
    new Float:knockback = GetConVarFloat(hKnockBackMultiplier);
    
    new Float:clientloc[3];
    new Float:attackerloc[3];
    
    GetClientAbsOrigin(client, clientloc);
    
    // Get attackers eye position.
    GetClientEyePosition(attacker, attackerloc);
    
    // Get attackers eye angles.
    new Float:attackerang[3];
    GetClientEyeAngles(attacker, attackerang);
    
    // Calculate knockback end-vector.
    TR_TraceRayFilter(attackerloc, attackerang, MASK_ALL, RayType_Infinite, KnockbackTRFilter);
    TR_GetEndPosition(clientloc);
    
    // Apply damage knockback multiplier.
    knockback *= dmg_health;
    
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
 * Get or set a client's velocity.
 * @param client        The client index.
 * @param vecVelocity   Array to store vector in, or velocity to set on client.
 * @param retrieve      True to get client's velocity, false to set it.
 * @param stack         If modifying velocity, then true will stack new velocity onto the client's
 *                      current velocity, false will reset it.
 */
stock ToolsClientVelocity(client, Float:vecVelocity[3], bool:apply = true, bool:stack = true)
{
    // If retrieve if true, then get client's velocity.
    if (!apply)
    {
        // x = vector component.
        for (new x = 0; x < 3; x++)
        {
            vecVelocity[x] = GetEntDataFloat(client, g_iToolsVelocity + (x*4));
        }
        
        // Stop here.
        return;
    }
    
    // If stack is true, then add client's velocity.
    if (stack)
    {
        // Get client's velocity.
        new Float:vecClientVelocity[3];
        
        // x = vector component.
        for (new x = 0; x < 3; x++)
        {
            vecClientVelocity[x] = GetEntDataFloat(client, g_iToolsVelocity + (x*4));
        }
        
        AddVectors(vecClientVelocity, vecVelocity, vecVelocity);
    }
    
    // Apply velocity on client.
    TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vecVelocity);
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