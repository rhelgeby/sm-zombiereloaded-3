/**
 * ====================
 *   Zombie:Reloaded
 *   File: damagecontrol.inc
 *   Author: Greyscale
 * ==================== 
 */

#define DMG_GENERIC			0			// generic damage was done
#define DMG_BULLET			(1 << 1)	// shot
#define DMG_SLASH			(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN			(1 << 3)	// heat burned
#define DMG_FALL			(1 << 5)	// fell too far
#define DMG_BLAST			(1 << 6)	// explosive blast damage
#define DMG_DROWN			(1 << 14)	// Drowning

enum ZRHooks
{
    Hook_TraceAttack,
    Hook_OnTakeDamage
}

new gHooks[MAXPLAYERS+1][ZRHooks];

InitDmgControl()
{
    /* It's case sensitive! */
    RegConsoleCmd("kill", Attempt_Suicide);
    RegConsoleCmd("KILL", Attempt_Suicide);        
    RegConsoleCmd("jointeam", Attempt_Suicide);
    RegConsoleCmd("JOINTEAM", Attempt_Suicide);
    RegConsoleCmd("spectate", Attempt_Suicide);
}

ClientHookAttack(client)
{
    gHooks[client][Hook_TraceAttack] = Hacks_Hook(client, HACKS_HTYPE_TRACEATTACK, TraceAttack, false);
    gHooks[client][Hook_OnTakeDamage] = Hacks_Hook(client, HACKS_HTYPE_ONTAKEDAMAGE, OnTakeDamage, false);
}

ClientUnHookAttack(client)
{
    Hacks_Unhook(gHooks[client][Hook_TraceAttack]);
    Hacks_Unhook(gHooks[client][Hook_OnTakeDamage]);
}

public TraceAttack(client, inflictor, attacker, damage, hitbox, hitgroup)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);

    if (!attacker || !IsClientPlayer(attacker) || !IsClientInGame(attacker) || !enabled)
    {
        return Hacks_Continue;
    }
    
    if (IsPlayerZombie(client) && IsPlayerZombie(attacker))
    {
        return 0;
    }
    
    if (IsPlayerHuman(client) && IsPlayerHuman(attacker))
    {
        return 0;
    }
    
    return Hacks_Continue;
}

public OnTakeDamage(client, inflictor, attacker, damage, damagetype, ammotype)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (!enabled)
    {
        return Hacks_Continue;
    }
    
    decl String:classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));
    if (StrContains(classname, "trigger") > -1)
    {
        return Hacks_Continue;
    }
    
    if (damagetype & DMG_FALL)
    {
        if (!IsPlayerZombie(client))
        {
            return Hacks_Continue;
        }
        
        new bool:blockfalldamage = GetClassNoFallDamage(pClass[client]);
        if (!blockfalldamage)
        {
            return Hacks_Continue;
        }
        
        return 0;
    }
    
    if (damagetype & DMG_BLAST)
    {
        if (!IsPlayerHuman(client) || !IsClientPlayer(attacker) || !IsClientInGame(attacker))
        {
            return Hacks_Continue;
        }
        
        return 0;
    }
    
    if (damagetype & DMG_BULLET)
    {
        if (!client || !IsClientPlayer(client) || !IsClientInGame(client))
        {
            return Hacks_Continue;
        }
        
        if (!attacker || !IsClientPlayer(attacker) || !IsClientInGame(attacker))
        {
            return Hacks_Continue;
        }
        
        if (IsPlayerZombie(client) && IsPlayerHuman(attacker))
        {
            return Hacks_Continue;
        }
        
        if (IsPlayerHuman(client) && IsPlayerZombie(attacker))
        {
            new health = GetClientHealth(client);
            SetEntityHealth(client, health + damage);
            
            return Hacks_Continue;
        }
        
        return 0;
    }
    
    return Hacks_Continue;
}

public Action:Attempt_Suicide(client, argc)
{
    if (!client)
    {
        return Plugin_Continue;
    }
    
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (!enabled)
    {
        return Plugin_Continue;
    }
    
    
    
    new bool:suicide = IsPlayerZombie(client) ? GetConVarBool(gCvars[CVAR_SUICIDE_ZOMBIE]) : GetConVarBool(gCvars[CVAR_SUICIDE_HUMAN]);
    if (!suicide)
    {
        return Plugin_Continue;
    }
    
    decl String:cmd[16];
    GetCmdArg(0, cmd, sizeof(cmd));
    
    if (!IsPlayerAlive(client))
    {
        return Plugin_Continue;
    }
    
    ZR_ReplyToCommand(client, "Suicide text");
    ZR_PrintToChat(client, "Suicide text");
    
    decl String:clientname[64];
    decl String:buffer[192];

    GetClientName(client, clientname, sizeof(clientname));
    if (LogFlagCheck(LOG_GAME_EVENTS, LOG_MODULE_DAMAGECONTROL))
    {
        ZR_LogMessageFormatted(client, "damage control", "suicide", "Player \"%s\" attempted suicide.", true, clientname);
    }
    if (GetConVarBool(gCvars[CVAR_SUICIDE_ECHO]))
    {
        Format(buffer, sizeof(buffer), "Player '%s' attempted suicide.", clientname);
        ZR_PrintToAdminChat(buffer);
    }
    
    return Plugin_Handled;
}