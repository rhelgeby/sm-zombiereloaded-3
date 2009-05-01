/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        damage.inc
 *   Description: (Core) Modify damage stuff here.
 *
 * ============================================================================
 */

/**
 * @section Damage type flags.
 */
#define DMG_FALL        (1 << 5)    /** Client was damaged by falling. */
#define DMG_BLAST       (1 << 6)    /** Client was damaged by explosion. */
#define DMG_BULLET      (1 << 12)   /** Client was shot or knifed. */
#define DMG_HEADSHOT    (1 << 30)   /** Client was shot in the head. */

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
    g_iDamageHookID[client][Hook_TraceAttack] = Hacks_Hook(client, HACKS_HTYPE_TRACEATTACK, DamageTraceAttack);
    g_iDamageHookID[client][Hook_OnTakeDamage] = Hacks_Hook(client, HACKS_HTYPE_ONTAKEDAMAGE, DamageOnTakeDamage);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
DamageOnClientDisconnect(client)
{
    // Unhook damage callbacks.
    Hacks_Unhook(g_iDamageHookID[client][Hook_TraceAttack]);
    Hacks_Unhook(g_iDamageHookID[client][Hook_OnTakeDamage]);
}

/**
 * Hook: TraceAttack
 * Called right before the bullet enters a client.
 * 
 * @param client        The client index.
 * @param inflictor     Entity index of damage-causing entity.
 * @param attacker      The client doing the damage.
 * @param damage        The amount of damage that will be inflicted.
 * @param hitbox        The hitbox index.
 * @param hitgroup      The hitgroup index.
 * @return              Hacks_Continue allows shot to be made.
 *                      0 stops the bullet from impacting.
 */
public DamageTraceAttack(client, inflictor, attacker, damage, hitbox, hitgroup)
{
    // Disabled
    // new bool:enabled = GetConVarBool(g_hCvarsList[CVAR_ENABLE]);
    
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return Hacks_Continue;
    }
    
    // If client is attacking himself, then stop.
    if(attacker == client)
    {
        return Hacks_Continue;
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
            return Hacks_Continue;
        }
        
        // Stop bullet from hurting client.
        return 0;
    }
    
    // Here we know that attacker and client are different teams.
    
    // If damage hitgroups cvar is disabled, then allow damage.
    new bool:damagehitgroups = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_HITGROUPS]);
    if (!damagehitgroups)
    {
        // Allow damage.
        return Hacks_Continue;
    }
    
    // If damage is disabled for this hitgroup, then stop.
    new bool:candamage = HitgroupsCanDamageHitgroup(hitgroup);
    if (!candamage)
    {
        // Stop bullet from hurting client.
        return 0;
    }
    
    // Allow damage.
    return Hacks_Continue;
}

/**
 * Hook: OnTakeDamage
 * Called right before damage is done.
 *  
 * @param client        The client index.
 * @param inflictor     Entity index of damage-causing entity.
 * @param attacker      The client doing the damage.
 * @param damage        The amount of damage that will be inflicted.
 * @param damagetype    The type of damage done (see damage flag defines)
 * @param ammotype      Type of ammo attacker shot at client.
 * @return              Hacks_Continue allows shot to be made.
 *                      0 stops the bullet from doing damage.
 */
public DamageOnTakeDamage(client, inflictor, attacker, damage, damagetype, ammotype)
{
    // Disabled.
    /**
    new bool:enabled = GetConVarBool(g_hCvarsList[CVAR_ENABLE]);
    if (!enabled)
    {
        return Hacks_Continue;
    }*/
    
    // Get classname of the inflictor.
    decl String:classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));
    
    // If entity is a trigger, then allow damage. (Map is damaging client)
    if (StrContains(classname, "trigger") > -1)
    {
        return Hacks_Continue;
    }
    
    // Client was shot or knifed.
    if (damagetype & DMG_BULLET)
    {
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Hacks_Continue;
        }
        
        // Get zombie flag for each client.
        new bool:clientzombie = InfectIsClientInfected(client);
        new bool:attackerzombie = InfectIsClientInfected(attacker);
        
        // If client and attacker are on the same team, then let CS:S handle the rest.
        if (clientzombie == attackerzombie)
        {
            return Hacks_Continue;
        }
        
        // We know that clientzombie is the opposite of attacker zombie.
        
        // If the client is a zombie, then allow damage.
        if (clientzombie)
        {
            return Hacks_Continue;
        }
        
        // Client is about to be infected, re-add HP so they aren't killed by knife.
        new health = GetClientHealth(client);
        SetEntityHealth(client, health + damage);
        
        // Allow damage.
        return Hacks_Continue;
    }
    // Client was damaged by explosion.
    else if (damagetype & DMG_BLAST)
    {
        // If blast damage is blocked, then stop.
        new bool:damageblockblast = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]);
        if (!damageblockblast)
        {
            return Hacks_Continue;
        }
        
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Hacks_Continue;
        }
        
        // If client is a zombie, then allow damage.
        if (InfectIsClientInfected(client))
        {
            return Hacks_Continue;
        }
        
        // Stop damage.
        return 0;
    }
    // Client was damaged by falling.
    else if (damagetype & DMG_FALL)
    {
        // If client isn't a zombie, then allow damage.
        if (!InfectIsClientInfected(client))
        {
            return Hacks_Continue;
        }
        
        // If class has "nofalldamage" disabled, then allow damage.
        new bool:blockfalldamage = ClassGetNoFallDamage(client);
        if (!blockfalldamage)
        {
            return Hacks_Continue;
        }
        
        // Stop damage.
        return 0;
    }
    
    // Allow damage.
    return Hacks_Continue;
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
