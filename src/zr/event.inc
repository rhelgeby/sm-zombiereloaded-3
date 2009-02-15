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
    RefreshList();
    
    if (tRound != INVALID_HANDLE)
    {
        CloseHandle(tRound);
        tRound = INVALID_HANDLE;
    }
    
    if (tInfect != INVALID_HANDLE)
    {
        CloseHandle(tInfect);
        tInfect = INVALID_HANDLE;
    }
    
    ZR_PrintToChat(0, "Round objective");
}

public Action:RoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    RemoveObjectives();
    
    if (tRound != INVALID_HANDLE)
    {
        CloseHandle(tRound);
    }
    
    new Float:roundlen = GetConVarFloat(FindConVar("mp_roundtime")) * 60.0;
    tRound = CreateTimer(roundlen, RoundOver, _, TIMER_FLAG_NO_MAPCHANGE);
    
    if (tInfect != INVALID_HANDLE)
    {
        CloseHandle(tInfect);
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
        CloseHandle(tRound);
        tRound = INVALID_HANDLE;
    }
    
    if (tInfect != INVALID_HANDLE)
    {
        CloseHandle(tInfect);
        tInfect = INVALID_HANDLE;
    }

    zombieSpawned = false;
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x<= maxplayers; x++)
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
    
    if (team == 1)
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
        if (x == TRESPAWN)
        {
            continue;
        }
        
        if (tHandles[index][x] != INVALID_HANDLE)
        {
            CloseHandle(tHandles[index][x]);
            tHandles[index][x] = INVALID_HANDLE;
        }
    }
    
    gZombie[index] = false;
    motherZombie[index] = false;
    
    SetPlayerFOV(index, 90);
    
    ClientCommand(index, "r_screenoverlay \"\"");
    
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
    
    ZTeleClientSpawned(index);
    
    NightVisionOn(index, false);
    NightVision(index, false);
  
    if (pNextClass[index] != -1)
    {
        Call_StartForward(hOnZClassChanged);
        Call_PushCell(index);
        Call_PushCell(pClass[index]);
        Call_PushCell(pNextClass[index]);
        Call_Finish();
        
        pClass[index] = pNextClass[index];
        pNextClass[index] = -1;
    }
    
    pProtect[index] = false;
    if (zombieSpawned)
    {
        if (team == CS_TEAM_T)
        {
            CS_SwitchTeam(index, CS_TEAM_CT);
            CS_RespawnPlayer(index);
        }
        
        new protect = GetConVarInt(gCvars[CVAR_PROTECT]);
        if (protect > 0)
        {
            decl String:respawnteam[32];
            GetConVarString(gCvars[CVAR_RESPAWN_TEAM], respawnteam, sizeof(respawnteam));
        
            if (!StrEqual(respawnteam, "zombie", false))
            {
                SetPlayerAlpha(index, 0);
                SetPlayerSpeed(index, 600.0);
                pProtect[index] = true;
                
                ZR_PrintToChat(index, "Spawn protection begin", protect);
                ZR_PrintCenterText(index, "Spawn protection begin", protect);
                
                if (tHandles[index][TPROTECT] != INVALID_HANDLE)
                {
                    CloseHandle(tHandles[index][TPROTECT]);
                }
                
                protCount[index] = protect;
                PrintHintText(index, "%d", protCount[index]);
                protCount[index]--;

                tHandles[index][TPROTECT] = CreateTimer(1.0, EndProtect, index, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
            }
        }
    }
    else
    {
        SetPlayerAlpha(index, 255);
    }
    
    new bool:randomclass = GetConVarBool(gCvars[CVAR_CLASSES_RANDOM]);
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        new bool:showmenu = GetConVarBool(gCvars[CVAR_CLASSES_SPAWN]);
        if (showmenu && !randomclass)
        {
            ClassMenu(index);
        }
    }
    
    ZR_PrintToChat(index, "!zmenu reminder");
    
    decl String:steamid[16];
    GetClientAuthString(index, steamid, sizeof(steamid));
    if (StrEqual(steamid, "BOT") || randomclass)
    {
        new class = GetRandomInt(0, classCount - 1);
    
        Call_StartForward(hOnZClassChanged);
        Call_PushCell(index);
        Call_PushCell(pClass[index]);
        Call_PushCell(class);
        Call_Finish();
    
        pClass[index] = class;
        
        decl String:classname[32];
        GetClassName(class, classname, sizeof(classname));
        
        ZR_PrintToChat(index, "Auto-assign", classname);
    }
}

public Action:PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));
    
    new dmg = GetEventInt(event, "dmg_health");
    
    decl String:weapon[32];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    if (attacker != 0)
    {
        if (IsPlayerHuman(index) && IsPlayerZombie(attacker))
        {
            if (!pProtect[index] && StrEqual(weapon, "knife"))
            {
                Zombify(index, attacker);
            }
        }
        else if (IsPlayerHuman(attacker))
        {
            new Float:knockback = GetClassKnockback(pClass[index]);
            
            new Float:clientloc[3];
            new Float:attackerloc[3];
            
            GetClientAbsOrigin(index, clientloc);
            
            if (!StrEqual(weapon, "hegrenade"))
            {
                GetClientAbsOrigin(attacker, attackerloc);
                
                new bool:shotgun = (StrEqual(weapon, "m3") || StrEqual(weapon, "xm1014"));
                
                KnockBack(index, clientloc, attackerloc, knockback, dmg, shotgun);
            }
            else
            {
                new Float:heLoc[3];
                FindExplodingGrenade(heLoc);
                
                KnockBack(index, clientloc, heLoc, knockback, dmg, true);
            }
        }
    }
            
    if (!IsPlayerZombie(index))
    {
        return;
    }
    
    if (GetRandomInt(1, 5) == 1)
    {
        decl String:sound[64];
        new randsound = GetRandomInt(1, 6);
        
        Format(sound, sizeof(sound), "npc/zombie/zombie_pain%d.wav", randsound);
        
        PrecacheSound(sound);
        EmitSoundToAll(sound, index);
    }
    
    new bool:napalm = GetClassNapalm(pClass[index]);
    if (napalm)
    {
        if (StrEqual(weapon, "hegrenade", false))
        {
            new Float:napalm_time = GetClassNapalmTime(pClass[index]);
            IgniteEntity(index, napalm_time);
        }
    }
    
    UpdateHPDisplay(index);
    
    if (GetClassRegen(pClass[index]))
    {
        if (tHandles[index][TREGEN] == INVALID_HANDLE)
        {
            new Float:interval = GetClassRegenInterval(pClass[index]);
            tHandles[index][TREGEN] = CreateTimer(interval, Regenerate, index, TIMER_REPEAT);
        }
    }

    UpdateAlphaDamaged(index);
}

FindExplodingGrenade(Float:heLoc[3])
{
    decl String:classname[64];
                
    new maxentities = GetMaxEntities();
    for (new x = GetMaxClients(); x <= maxentities; x++)
    {
        if (IsValidEdict(x))
        {
            GetEdictClassname(x, classname, sizeof(classname));
            if (StrEqual(classname, "hegrenade_projectile"))
            {
                new takedamage = GetEntProp(x, Prop_Data, "m_takedamage");
                if (takedamage == 0)
                {
                    GetEntPropVector(x, Prop_Send, "m_vecOrigin", heLoc);
                    return x;
                }
            }
        }
    }
    
    return -1;
}

public Action:PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));

    SetPlayerFOV(index, DEFAULT_FOV);
    
    ExtinguishEntity(index);
    
    decl String:weapon[32];
    
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    if (StrEqual(weapon, "zombie_claws_of_death", false))
    {
        if (index)
        {
            AddPlayerDeath(index, 1);
        }
        
        if (attacker)
        {
            AddPlayerScore(attacker, 1);
            
            new healthgain = GetClassInfectHealth(pClass[attacker]);
            new health = GetClientHealth(attacker);
            
            SetEntityHealth(attacker, health + healthgain);
            
            UpdateHPDisplay(attacker);
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
            
            if (attacker)
            {
                new bonus = GetClassKillBonus(pClass[index]);
                AddPlayerScore(attacker, bonus);
            }
            
            if (!IsClientPlayer(attacker))
            {
                gKilledByWorld[index] = true;
            }
            else
            {
                gKilledByWorld[index] = false;
            }
        }
        
        for (new x = 0; x < MAXTIMERS; x++)
        {
            if (tHandles[index][x] != INVALID_HANDLE)
            {
                KillTimer(tHandles[index][x]);
                tHandles[index][x] = INVALID_HANDLE;
            }
        }
        
        new bool:respawn = GetConVarBool(gCvars[CVAR_RESPAWN]);
        if (respawn)
        {   
            new Float:delay = GetConVarFloat(gCvars[CVAR_RESPAWN_DELAY]);
            tHandles[index][TRESPAWN] = CreateTimer(delay, RespawnTimer, index, TIMER_FLAG_NO_MAPCHANGE);
        }
    }
    
    new ZTeam:team = IsRoundOver();
    if (team == Neither)
    {
        ClientCommand(index, "r_screenoverlay \"\"");
        
        return;
    }
    
    RoundWin(team);
}

public Action:PlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    
    if (IsPlayerZombie(index))
    {
        new Float:distance = GetClassJumpDistance(pClass[index]);
        new Float:height = GetClassJumpHeight(pClass[index]);
        
        JumpBoost(index, distance, height);
    }
}