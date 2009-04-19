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
#define DMG_GENERIC			0			// generic damage was done
#define DMG_BULLET			(1 << 1)	// shot
#define DMG_SLASH			(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN			(1 << 3)	// heat burned
#define DMG_FALL			(1 << 5)	// fell too far
#define DMG_BLAST			(1 << 6)	// explosive blast damage
#define DMG_DROWN			(1 << 14)	// Drowning
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
    Hook_TraceAttack,
    Hook_OnTakeDamage,
}

new g_iDamageHooks[MAXPLAYERS + 1][DamageHooks];

/**
 * Damage module init function.
 */
DamageInit()
{
    // Create command callbacks (intercepts) for listed suicide commands.
    decl String:suicidecmds[64];
    GetConVarString(gCvars[CVAR_SUICIDE_CMDS], suicidecmds, sizeof(suicidecmds));
    
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
    g_iDamageHooks[client][Hook_TraceAttack] = Hacks_Hook(client, HACKS_HTYPE_TRACEATTACK, DamageTraceAttack, false);
    g_iDamageHooks[client][Hook_OnTakeDamage] = Hacks_Hook(client, HACKS_HTYPE_ONTAKEDAMAGE, DamageOnTakeDamage, false);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
DamageOnClientDisconnect(client)
{
    Hacks_Unhook(g_iDamageHooks[client][Hook_TraceAttack]);
    Hacks_Unhook(g_iDamageHooks[client][Hook_OnTakeDamage]);
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
    // new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    
    // If attacker isn't valid, then stop.
    if (!ZRIsValidClient(attacker))
    {
        return Hacks_Continue;
    }
    
    // Get zombie flag for each client.
    new bool:clientzombie = IsPlayerZombie(client);
    new bool:attackerzombie = IsPlayerZombie(attacker);
    
    // If the flags are the same on both clients, then stop.
    if (clientzombie == attackerzombie)
    {
        return 0;
    }
    
    // Players are on differen't teams.
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
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
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
    
    switch(damagetype)
    {
        // Client fell too far.
        case DMG_FALL:
        {
            // If client isn't a zombie, then allow damage.
            if (!IsPlayerZombie(client))
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
        // Client is being damaged by a blast.
        case DMG_BLAST:
        {
            // If attacker isn't valid, then allow damage.
            if (!ZRIsValidClient(attacker))
            {
                return Hacks_Continue;
            }
            
            // If client is a zombie, then allow damage.
            if (IsPlayerZombie(client))
            {
                return Hacks_Continue;
            }
            
            // Stop damage.
            return 0;
        }
        // Client is being shot.
        case DMG_BULLET:
        {
            // If attacker isn't valid, then allow damage.
            if (!ZRIsValidClient(attacker))
            {
                return Hacks_Continue;
            }
            
            // Get zombie flag for each client.
            new bool:clientzombie = IsPlayerZombie(client);
            new bool:attackerzombie = IsPlayerZombie(attacker);
            
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
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
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
    if (!ZRIsValidClient(client))
    {
        return Plugin_Continue;
    }
    
    // If client is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        return Plugin_Continue;
    }
    
    // Get zombie flag on client.
    new bool:clientzombie = IsPlayerZombie(client);
    
    // Get cvar values for suicide interception.
    new bool:suicidezombie = GetConVarBool(gCvars[CVAR_SUICIDE_ZOMBIE]);
    new bool:suicidehuman = GetConVarBool(gCvars[CVAR_SUICIDE_HUMAN]);
    
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
    if (LogFlagCheck(LOG_GAME_EVENTS, LOG_MODULE_DAMAGE))
    {
        ZR_LogMessageFormatted(client, "Damage", "Suicide Intercept", "Player %N attempted suicide.", LOG_FORMAT_TYPE_FULL, client);
    }
    
    // Block command.
    return Plugin_Handled;
}