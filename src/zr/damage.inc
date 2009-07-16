/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          damage.inc
 *  Type:          Core
 *  Description:   Modify damage stuff here.
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
 * @endsection
 */

/**
 * @section Suicide intercept defines.
 */
#define DAMAGE_SUICIDE_MAX_CMDS 5
#define DAMAGE_SUICIDE_MAX_LENGTH 16
/**
 * @endsection
 */

/**
 * Array to store TraceAttack HookIDs.
 */
new g_iDamageTraceAttackHookID[MAXPLAYERS + 1] = {-1, ...};

/**
 * Array to store OnTakeDamage HookIDs.
 */
new g_iDamageOnTakeDamageHookID[MAXPLAYERS + 1] = {-1, ...};

/**
 * Array to keep track of normal/mother zombies.
 */
new bool:g_bDamageMotherZombie[MAXPLAYERS + 1];

/**
 * Hook commands related to damage here.
 */
DamageOnCommandsHook()
{
    // Create command callbacks (intercepts) for listed suicide commands.
    decl String:suicidecmds[DAMAGE_SUICIDE_MAX_CMDS * DAMAGE_SUICIDE_MAX_LENGTH];
    GetConVarString(g_hCvarsList[CVAR_DAMAGE_SUICIDE_CMDS], suicidecmds, sizeof(suicidecmds));
    
    // Create array to store cmds
    new String:arrayCmds[DAMAGE_SUICIDE_MAX_CMDS][DAMAGE_SUICIDE_MAX_LENGTH];
    
    // Explode string into array indexes.
    new cmdcount = ExplodeString(suicidecmds, ", ", arrayCmds, DAMAGE_SUICIDE_MAX_CMDS, DAMAGE_SUICIDE_MAX_LENGTH);
    
    // x = Array index.
    // arrayCmds[x] = suicide command.
    for (new x = 0; x <= cmdcount - 1; x++)
    {
        // Prepare intercept for this command.
        RegConsoleCmd(arrayCmds[x], DamageSuicideIntercept);
    }
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
DamageClientInit(client)
{
    // Hook damage callbacks.
    g_iDamageTraceAttackHookID[client] = ZRTools_HookTraceAttack(client, DamageTraceAttack);
    g_iDamageOnTakeDamageHookID[client] = ZRTools_HookOnTakeDamage(client, DamageOnTakeDamage);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
DamageOnClientDisconnect(client)
{
    // Unhook damage callbacks, and reset variables.
    
    if (g_iDamageTraceAttackHookID[client] != -1)
    {
        ZRTools_UnhookTraceAttack(g_iDamageTraceAttackHookID[client]);
        g_iDamageTraceAttackHookID[client] = -1;
    }
    
    if (g_iDamageOnTakeDamageHookID[client] != -1)
    {
        ZRTools_UnhookOnTakeDamage(g_iDamageOnTakeDamageHookID[client]);
        g_iDamageOnTakeDamageHookID[client] = -1;
    }
}

/**
 * A client was infected.
 * 
 * @param client        The client index.
 * @param motherinfect  True if the zombie is mother, false if not.
 */
DamageOnClientInfected(client, bool:motherinfect)
{
    // Update if client is a mother zombie or not.
    g_bDamageMotherZombie[client] = motherinfect;
}

/**
 * Hook: TraceAttack
 * Called right before the bullet enters a client.
 * 
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param hitbox        The hitbox index.
 * @param hitgroup      The hitgroup index.  
 * @return              Return ZRTools_Handled to stop bullet from hitting client.
 *                      ZRTools_Continue to allow bullet to hit client.
 */
public ZRTools_Action:DamageTraceAttack(client, inflictor, attacker, Float:damage, hitbox, hitgroup)
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return ZRTools_Continue;
    }
    
    // If client is attacking himself, then stop.
    if(attacker == client)
    {
        return ZRTools_Continue;
    }
    
    // Get zombie flag for each client.
    new bool:clientzombie = InfectIsClientInfected(client);
    new bool:attackerzombie = InfectIsClientInfected(attacker);
    
    // If the flags are the same on both clients, then stop.
    if (clientzombie == attackerzombie)
    {
        // If friendly fire is blocked, then allow damage.
        new bool:damageblockff = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_FF]);
        if (!damageblockff)
        {
            return ZRTools_Continue;
        }
        
        // Stop bullet from hurting client.
        return ZRTools_Handled;
    }
    
    // Here we know that attacker and client are different teams.
    
    // If client is a human, then allow damage.
    if (InfectIsClientHuman(client))
    {
        // Allow damage.
        return ZRTools_Continue;
    }
    
    // If damage hitgroups cvar is disabled, then allow damage.
    new bool:damagehitgroups = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_HITGROUPS]);
    if (!damagehitgroups)
    {
        // Allow damage.
        return ZRTools_Continue;
    }
    
    // If damage is disabled for this hitgroup, then stop.
    new index = HitgroupToIndex(hitgroup);
    
    // If index can't be found, then allow damage.
    if (index == -1)
    {
        // Allow damage.
        return ZRTools_Continue;
    }
    
    new bool:candamage = HitgroupsCanDamage(index);
    if (!candamage)
    {
        // Stop bullet from hurting client.
        return ZRTools_Handled;
    }
    
    // Allow damage.
    return ZRTools_Continue;
}

/**
 * Hook: OnTakeDamage
 * Called right before damage is done.
 * 
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param damagetype    The type of damage inflicted.
 * @param ammotype      The ammo type of the attacker's weapon. 
 * @return              Return ZRTools_Handled to stop the damage to client.
 *                      ZRTools_Continue to allow damage to client.
 */
public ZRTools_Action:DamageOnTakeDamage(client, inflictor, attacker, Float:damage, damagetype, ammotype)
{
    // Get classname of the inflictor.
    decl String:classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));
    
    // If entity is a trigger, then allow damage. (Map is damaging client)
    if (StrContains(classname, "trigger") > -1)
    {
        return ZRTools_Continue;
    }
    
    // Client was shot or knifed.
    if (damagetype & DMG_CSS_BULLET)
    {
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return ZRTools_Continue;
        }
        
        // Get zombie flag for each client.
        new bool:clientzombie = InfectIsClientInfected(client);
        new bool:attackerzombie = InfectIsClientInfected(attacker);
        
        // If client and attacker are on the same team, then let CS:S handle the rest.
        if (clientzombie == attackerzombie)
        {
            return ZRTools_Continue;
        }
        
        // We know that clientzombie is the opposite of attacker zombie.
        
        // If the client is a zombie, then allow damage.
        if (clientzombie)
        {
            return ZRTools_Continue;
        }
        
        // Client is about to be infected, re-add HP so they aren't killed by knife.
        new health = GetClientHealth(client);
        SetEntityHealth(client, health + RoundToNearest(damage));
        
        // Allow damage.
        return ZRTools_Continue;
    }
    // Client was damaged by explosion.
    else if (damagetype & DMG_CSS_BLAST)
    {
        // If blast damage is blocked, then stop.
        new bool:damageblockblast = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]);
        if (!damageblockblast)
        {
            return ZRTools_Continue;
        }
        
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return ZRTools_Continue;
        }
        
        // If client is a zombie, then allow damage.
        if (InfectIsClientInfected(client))
        {
            return ZRTools_Continue;
        }
        
        // Stop damage.
        return ZRTools_Handled;
    }
    // Client was damaged by falling.
    else if (damagetype & DMG_CSS_FALL)
    {
        // If client isn't a zombie, then allow damage.
        if (!InfectIsClientInfected(client))
        {
            return ZRTools_Continue;
        }
        
        // If class has "nofalldamage" disabled, then allow damage.
        new bool:blockfalldamage = ClassGetNoFallDamage(client);
        if (!blockfalldamage)
        {
            return ZRTools_Continue;
        }
        
        // Stop damage.
        return ZRTools_Handled;
    }
    
    // Allow damage.
    return ZRTools_Continue;
}

/**
 * Command callback (kill, jointeam, spectate)
 * Block command if plugin thinks they are trying to commit suicide.
 * 
 * @param client    The client index.
 * @param argc      The number of arguments in command string.
 */     
public Action:DamageSuicideIntercept(client, argc)
{
    // If zombie hasn't spawned, then stop.
    if (!g_bZombieSpawned)
    {
        return Plugin_Continue;
    }
    
    // If client is invalid, then stop.  (Stop console.)
    if (!ZRIsClientValid(client))
    {
        return Plugin_Continue;
    }
    
    // If client is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        return Plugin_Continue;
    }
    
    // Get cvar values for suicide interception.
    new bool:suicidezombie = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_ZOMBIE]);
    new bool:suicidezombiemother = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_MZOMBIE]);
    new bool:suicidehuman = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_HUMAN]);
    
    // Check if client is a zombie.
    if (InfectIsClientInfected(client))
    {
        // If client is a normal zombie, and suicide intercept is disabled for zombies, then let command go.
        if (!g_bDamageMotherZombie[client] && !suicidezombie)
        {
            return Plugin_Continue;
        }
        
        // If client is a mother zombie, and suicide intercept is disabled for mother zombies, then let command go.
        if (g_bDamageMotherZombie[client] && !suicidezombiemother)
        {
            return Plugin_Continue;
        }
    }
    
    // If client is a human, and suicide intercept is disabled for humans, then let command go.
    if (InfectIsClientHuman(client) && !suicidehuman)
    {
        return Plugin_Continue;
    }
    
    // Tell client their command has been intercepted.
    TranslationReplyToCommand(client, "Damage suicide intercept");
    
    // Log suicide interception
    LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "Player \"%L\" attempted suicide.", client);
    
    // Block command.
    return Plugin_Handled;
}
