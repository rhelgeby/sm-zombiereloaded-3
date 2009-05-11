/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          damage.inc
 *   Type:          Core
 *   Description:   Modify damage stuff here.
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
 * List of damage-related hooks.
 */
enum DamageHooks
{
    Hook_TraceAttack, /** TraceAttack HookID */
    Hook_OnTakeDamage, /** OnTakeDamage HookID */
}

new g_iDamageHookID[MAXPLAYERS + 1][DamageHooks];

/**
 * Damage module init function.
 */
DamageInit()
{
    // Create command callbacks (intercepts) for listed suicide commands.
    decl String:suicidecmds[64];
    GetConVarString(g_hCvarsList[CVAR_DAMAGE_SUICIDE_CMDS], suicidecmds, sizeof(suicidecmds));
    
    // Create array to store cmds
    new String:arrayCmds[DAMAGE_SUICIDE_MAX_CMDS][DAMAGE_SUICIDE_MAX_LENGTH];
    
    // Explode string into array indexes.
    new cmdcount = ExplodeString(suicidecmds, ", ", arrayCmds, DAMAGE_SUICIDE_MAX_CMDS, DAMAGE_SUICIDE_MAX_LENGTH);
    
    // x = array index.
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
    g_iDamageHookID[client][Hook_TraceAttack] = ZRTools_HookTraceAttack(client, DamageTraceAttack);
    g_iDamageHookID[client][Hook_OnTakeDamage] = ZRTools_HookOnTakeDamage(client, DamageOnTakeDamage);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
DamageOnClientDisconnect(client)
{
    // Unhook damage callbacks.
    ZRTools_UnhookTraceAttack(g_iDamageHookID[client][Hook_TraceAttack]);
    ZRTools_UnhookOnTakeDamage(g_iDamageHookID[client][Hook_OnTakeDamage]);
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
    
    // If damage hitgroups cvar is disabled, then allow damage.
    new bool:damagehitgroups = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_HITGROUPS]);
    if (!damagehitgroups)
    {
        // Allow damage.
        return ZRTools_Continue;
    }
    
    // If damage is disabled for this hitgroup, then stop.
    new bool:candamage = HitgroupsCanDamageHitgroup(hitgroup);
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
    // Disabled.
    /**
    new bool:enabled = GetConVarBool(g_hCvarsList[CVAR_ENABLE]);
    if (!enabled)
    {
        return Plugin_Continue;
    }*/
    
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
    
    // Get zombie flag on client.
    new bool:clientzombie = InfectIsClientInfected(client);
    
    // Get cvar values for suicide interception.
    new bool:suicidezombie = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_ZOMBIE]);
    new bool:suicidehuman = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_HUMAN]);
    
    // Determine whether to block suicide based off of the client's zombie flag and cvar values.
    new bool:blocksuicide = clientzombie ? suicidezombie : suicidehuman;
    
    // If cvar for this team is disabled, then stop.
    if (!blocksuicide)
    {
        return Plugin_Continue;
    }
    
    // Tell client their command has been intercepted.
    ZR_ReplyToCommand(client, "Damage suicide intercept");
    
    // Log attempt.
    if (LogCheckFlag(LOG_GAME_EVENTS, LOG_MODULE_DAMAGE))
    {
        LogMessageFormatted(client, "Damage", "Suicide Intercept", "Player %N attempted suicide.", LOG_FORMAT_TYPE_FULL, client);
    }
    
    // Block command.
    return Plugin_Handled;
}
