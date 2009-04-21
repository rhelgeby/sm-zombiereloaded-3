/**
 * ====================
 *   Zombie:Reloaded
 *   File: events.inc
 *   Author: Greyscale
 * ==================== 
 */

HookEvents()
{
    HookEvent("round_start", RoundStart);
    HookEvent("round_freeze_end", RoundFreezeEnd);
    HookEvent("round_end", RoundEnd);
    HookEvent("player_team", PlayerTeam, EventHookMode_Pre);
    HookEvent("player_spawn", PlayerSpawn);
    HookEvent("player_hurt", PlayerHurt);
    HookEvent("player_death", PlayerDeath);
    HookEvent("player_jump", PlayerJump);
    HookEvent("weapon_fire", WeaponFire);
}

UnhookEvents()
{
    UnhookEvent("round_start", RoundStart);
    UnhookEvent("round_freeze_end", RoundFreezeEnd);
    UnhookEvent("round_end", RoundEnd);
    UnhookEvent("player_team", PlayerTeam, EventHookMode_Pre);
    UnhookEvent("player_spawn", PlayerSpawn);
    UnhookEvent("player_hurt", PlayerHurt);
    UnhookEvent("player_death", PlayerDeath);
    UnhookEvent("player_jump", PlayerJump);
    UnhookEvent("weapon_fire", WeaponFire);
}

public Action:RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{   
    ChangeLightStyle();
    
    if (tInfect != INVALID_HANDLE)
    {
        KillTimer(tInfect);
        tInfect = INVALID_HANDLE;
    }

    g_bZombieSpawned = false;
    
    for (new x = 1; x<= MaxClients; x++)
    {
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        bZombie[x] = false;
    }
    
    ZR_PrintToChat(0, "Round objective");
    
    // Forward event to sub-modules.
    RoundEndOnRoundStart();
    SEffectsOnRoundStart();
    AntiStickOnRoundStart();
}

public Action:RoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    RemoveObjectives();
    
    if (tInfect != INVALID_HANDLE)
    {
        KillTimer(tInfect);
    }
    
    new Float:min = GetConVarFloat(g_hCvarsList[CVAR_SPAWN_MIN]);
    new Float:max = GetConVarFloat(g_hCvarsList[CVAR_SPAWN_MAX]);
    new Float:randlen = GetRandomFloat(min, max);
    tInfect = CreateTimer(randlen, MotherZombie, _, TIMER_FLAG_NO_MAPCHANGE);
    
    // Forward events to modules.
    RoundEndOnRoundFreezeEnd();
    ZTeleEnable();

}

public Action:RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    new reason = GetEventInt(event, "reason");
    
    if (tInfect != INVALID_HANDLE)
    {
        KillTimer(tInfect);
        tInfect = INVALID_HANDLE;
    }

    g_bZombieSpawned = false;
    
    for (new x = 1; x<= MaxClients; x++)
    {
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        bZombie[x] = false;
    }
    
    // Forward event to modules.
    RoundEndOnRoundEnd(reason);
    ZTeleReset();
}

public Action:PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new team = GetEventInt(event, "team");
    
    if (team == CS_TEAM_SPECTATOR)
    {
        bZombie[index] = false;
    }
    
    return Plugin_Handled;
}

public Action:PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    
    bZombie[index] = false;
    
    // Reset FOV and overlay.
    SetPlayerFOV(index, 90);
    ClientCommand(index, "r_screenoverlay \"\"");
    
    // Check if client is on a team.
    if (ZRIsClientOnTeam(index))
    {
        new bool:cashfill = GetConVarBool(g_hCvarsList[CVAR_CASHFILL]);
        if (cashfill)
        {
            new cash = GetConVarInt(g_hCvarsList[CVAR_CASHAMOUNT]);
            SetPlayerMoney(index, cash);
        }
        
        // Remove night vision.
        NightVisionOn(index, false);
        NightVision(index, false);
        
        if (g_bZombieSpawned)
        {
            if (ZRIsClientOnTeam(index, CS_TEAM_T))
            {
                CS_SwitchTeam(index, CS_TEAM_CT);
                CS_RespawnPlayer(index);
            }
        }
        else
        {
            SetPlayerAlpha(index, 255);
        }
    }
    
    // Forward event to modules.
    ClassOnClientSpawn(index);
    SEffectsOnClientSpawn(index);
    SpawnProtectOnClientSpawn(index);
    RespawnOnClientSpawn(index);
    ZHPOnClientSpawn(index);
    ZTeleClientSpawned(index);
    
    ZR_PrintToChat(index, "!zmenu reminder");
}

public Action:PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));
    
    new hitgroup = GetEventInt(event, "hitgroup");
    
    new dmg_health = GetEventInt(event, "dmg_health");
    
    decl String:weapon[32];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // Check if the attacker is a player.
    if (ZRIsValidClient(attacker))
    {
        // Check if a zombie attacks a human.
        if (IsPlayerHuman(index) && IsPlayerZombie(attacker))
        {
            // Check if spawn protection is disabled and the weapon is a knife.
            if (!pSpawnProtect[index] && StrEqual(weapon, "knife"))
            {
                InfectPlayer(index, attacker);
            }
        }
    }
    
    // Forward event to modules.
    ClassAlphaUpdate(index);
    SEffectsOnClientHurt(index);
    KnockbackOnClientHurt(index, attacker, weapon, hitgroup, dmg_health);
    NapalmOnClientHurt(index, weapon);
    ZHPOnClientHurt(index);
}

public Action:PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));

    // Reset field of view and extinguish fire.
    SetPlayerFOV(index, DEFAULT_FOV);
    ExtinguishEntity(index);
    
    // Get the weapon name.
    decl String:weapon[32];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // Check if the player was infected.
    if (StrEqual(weapon, "zombie_claws_of_death", false))
    {
        // Add a death count to the players score.
        if (ZRIsValidClient(index))
        {
            AddPlayerDeath(index, 1);
        }
        
        // Give a point to the attacker.
        if (ZRIsValidClient(attacker))
        {
            AddPlayerScore(attacker, 1);
            
            new healthgain = ClassGetHealthInfectGain(attacker);
            new health = GetClientHealth(attacker);
            
            SetEntityHealth(attacker, health + healthgain);
            
            // Forward event to modules.
            ZHPOnHealthInfectGain(attacker);
        }
    }
    else
    {
        if (IsPlayerZombie(index))
        {
            
            // Give kill bonus.
            if (ZRIsValidClient(attacker))
            {
                new bonus = ClassGetKillBonus(attacker);
                AddPlayerScore(attacker, bonus);
            }
        }
    }
    
    // Forward event to modules.
    ClassOnClientDeath(index);
    SEffectsOnClientDeath(index);
    SpawnProtectOnClientDeath(index);
    RespawnOnClientDeath(index, attacker, weapon);
    ZHPOnClientDeath(index);
    
    new RoundEndOutcome:outcome;
    if (RoundEndGetRoundStatus(outcome))
    {
        RoundEndTerminateRound(outcome);
    }
}

public Action:PlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));    
    new Float:distance = ClassGetJumpDistance(client);
    new Float:height = ClassGetJumpHeight(client);
        
    JumpBoost(client, distance, height);
}

public Action:WeaponFire(Handle:event, const String:name[], bool:dontBroadcast)
{
    decl String:weapon[32];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // Forward event to modules.
    NapalmOnWeaponFire(weapon);
}
