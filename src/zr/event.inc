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
}

public Action:RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{   
    ChangeLightStyle();
    AmbienceRestartAll();
    AntiStickRestart();
    RefreshList();
    
    if (tRound != INVALID_HANDLE)
    {
        KillTimer(tRound);
        tRound = INVALID_HANDLE;
    }
    
    if (tInfect != INVALID_HANDLE)
    {
        KillTimer(tInfect);
        tInfect = INVALID_HANDLE;
    }
    
    ZR_PrintToChat(0, "Round objective");
}

public Action:RoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    RemoveObjectives();
    
    if (tRound != INVALID_HANDLE)
    {
        KillTimer(tRound);
    }
    
    new Float:roundlen = GetConVarFloat(FindConVar("mp_roundtime")) * 60.0;
    tRound = CreateTimer(roundlen, RoundOver, _, TIMER_FLAG_NO_MAPCHANGE);
    
    if (tInfect != INVALID_HANDLE)
    {
        KillTimer(tInfect);
    }
    
    new Float:min = GetConVarFloat(gCvars[CVAR_SPAWN_MIN]);
    new Float:max = GetConVarFloat(gCvars[CVAR_SPAWN_MAX]);
    new Float:randlen = GetRandomFloat(min, max);
    tInfect = CreateTimer(randlen, MotherZombie, _, TIMER_FLAG_NO_MAPCHANGE);
    
    ZTeleEnable();

}

public Action:RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    if (tRound != INVALID_HANDLE)
    {
        KillTimer(tRound);
        tRound = INVALID_HANDLE;
    }
    
    if (tInfect != INVALID_HANDLE)
    {
        KillTimer(tInfect);
        tInfect = INVALID_HANDLE;
    }

    zombieSpawned = false;
    
    for (new x = 1; x<= MaxClients; x++)
    {
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        gZombie[x] = false;
        
        new bool:consecutive_infect = GetConVarBool(gCvars[CVAR_CONSECUTIVE_INFECT]);
        gBlockMotherInfect[x] = consecutive_infect ? false : motherZombie[x];
    }
    
    BalanceTeams();
    
    new reason = GetEventInt(event, "reason");
    
    if (reason == CTs_PreventEscape)
    {
        ShowOverlays(5.0, Human);
    }
    else if (reason == Terrorists_Escaped)
    {
        ShowOverlays(5.0, Zombie);
    }

    ZTeleReset();
}

public Action:PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new team = GetEventInt(event, "team");
    
    if (team == CS_TEAM_SPECTATOR)
    {
        gZombie[index] = false;
        motherZombie[index] = false;
    }
    
    new bool:allow_player_team = GetConVarBool(gCvars[CVAR_ALLOW_PLAYER_TEAM]);
    if (allow_player_team && !IsPlayerInList(index))
    {
        return Plugin_Continue;
    }
    
    return Plugin_Handled;
}

public Action:PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    
    for (new x = 0; x < MAXTIMERS; x++)
    {
        if (tHandles[index][x] != INVALID_HANDLE)
        {
            KillTimer(tHandles[index][x]);
            tHandles[index][x] = INVALID_HANDLE;
        }
    }
    
    gZombie[index] = false;
    motherZombie[index] = false;
    
    // Reset FOV and overlay.
    SetPlayerFOV(index, 90);
    ClientCommand(index, "r_screenoverlay \"\"");
    
    // Forward event to modules.
    SpawnProtectOnClientSpawn(index);
    RespawnOnClientSpawn(index);
    ZHPOnClientSpawn(index);
    
    // Stop here if client isn't on a team.
    new team = GetClientTeam(index);
    if (team != CS_TEAM_T && team != CS_TEAM_CT)
    {
        return;
    }
    
    new bool:cashfill = GetConVarBool(gCvars[CVAR_CASHFILL]);
    if (cashfill)
    {
        new cash = GetConVarInt(gCvars[CVAR_CASHAMOUNT]);
        SetPlayerMoney(index, cash);
    }
    
    // Remove night vision.
    NightVisionOn(index, false);
    NightVision(index, false);
    
    if (zombieSpawned)
    {
        if (team == CS_TEAM_T)
        {
            CS_SwitchTeam(index, CS_TEAM_CT);
            CS_RespawnPlayer(index);
        }
    }
    else
    {
        SetPlayerAlpha(index, 255);
    }
    
    // Forward event to modules.
    ClassOnClientSpawn(index);
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
    
    // Check if the player is a human.
    if (IsPlayerHuman(index))
    {
        // We're done now. Nothing more to do on humans.
        return;
    }
    
    // Play a random zombie hurt sound.
    if (GetRandomInt(1, 5) == 1)
    {
        decl String:sound[64];
        new randsound = GetRandomInt(1, 6);
        
        Format(sound, sizeof(sound), "npc/zombie/zombie_pain%d.wav", randsound);
        
        PrecacheSound(sound);
        EmitSoundToAll(sound, index);
    }
    
    // Napalm effect.
    new Float:napalm_time = ClassGetNapalmTime(index);
    if (StrEqual(weapon, "hegrenade", false) && napalm_time > 0.0)
    {
        IgniteEntity(index, napalm_time);
    }
    
    // Forward event to modules.
    ClassAlphaUpdate(index);
    KnockbackPlayerHurt(index, attacker, weapon, hitgroup, dmg_health);
    ZHPOnPlayerHurt(index);
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
            decl String:sound[64];
    
            new randsound = GetRandomInt(1, 3);
            Format(sound, sizeof(sound), "npc/zombie/zombie_die%d.wav", randsound);
            
            PrecacheSound(sound);
            EmitSoundToAll(sound, index);
            
            // Give kill bonus.
            if (ZRIsValidClient(attacker))
            {
                new bonus = ClassGetKillBonus(attacker);
                AddPlayerScore(attacker, bonus);
            }
            
            // Set gKilledByWorld to true if attacker is not a valid client.
            gKilledByWorld[index] = !ZRIsValidClient(attacker);
        }
        
        // Kill various timers.
        for (new x = 0; x < MAXTIMERS; x++)
        {
            if (tHandles[index][x] != INVALID_HANDLE)
            {
                KillTimer(tHandles[index][x]);
                tHandles[index][x] = INVALID_HANDLE;
            }
        }
    }
    
    // Forward event to modules.
    ClassOnClientDeath(index);
    SpawnProtectOnClientDeath(index);
    RespawnOnClientDeath(index, weapon);
    ZHPOnClientDeath(index);
    
    new ZTeam:team = IsRoundOver();    
    RoundWin(team);
}

public Action:PlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));    
    new Float:distance = ClassGetJumpDistance(client);
    new Float:height = ClassGetJumpHeight(client);
        
    JumpBoost(client, distance, height);
}
