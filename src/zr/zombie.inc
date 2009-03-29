/**
 * ====================
 *   Zombie:Reloaded
 *   File: zombie.inc
 *   Author: Greyscale
 * ==================== 
 */

#define EXP_NODAMAGE               1
#define EXP_REPEATABLE             2
#define EXP_NOFIREBALL             4
#define EXP_NOSMOKE                8
#define EXP_NODECAL               16
#define EXP_NOSPARKS              32
#define EXP_NOSOUND               64
#define EXP_RANDOMORIENTATION    128
#define EXP_NOFIREBALLSMOKE      256
#define EXP_NOPARTICLES          512
#define EXP_NODLIGHTS           1024
#define EXP_NOCLAMPMIN          2048
#define EXP_NOCLAMPMAX          4096
        
new String:skyname[32];

HookCommands()
{
    RegConsoleCmd("nightvision", Command_NightVision);
}

public Action:Command_NightVision(client, argc)
{
    new bool:allow_disable = GetConVarBool(gCvars[CVAR_ZVISION_ALLOW_DISABLE]);
    if (!allow_disable)
    {
        return;
    }
    
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (!enabled)
    {
        return;
    }
    
    if (!IsPlayerZombie(client))
    {
        return;
    }
    
    bZVisionOn[client] = !bZVisionOn[client];
    
    if (bZVisionOn[client])
    {
        ZVisionPreCheck(client);
    }
    else
    {
        ZVisionStop(client);
    }
}

FindMapSky()
{
    GetConVarString(FindConVar("sv_skyname"), skyname, sizeof(skyname));
}

ChangeLightStyle()
{
    new bool:dark = GetConVarBool(gCvars[CVAR_DARK]);
    if (dark)
    {
        decl String:darkness[2];
        decl String:sky[32];
        
        GetConVarString(gCvars[CVAR_DARK_LEVEL], darkness, sizeof(darkness));
        GetConVarString(gCvars[CVAR_DARK_SKY], sky, sizeof(sky));
        
        SetLightStyle(0, darkness);
        SetConVarString(FindConVar("sv_skyname"), sky, true, false);
    }
    else
    {
        SetLightStyle(0, "n");
        SetConVarString(FindConVar("sv_skyname"), skyname, true, false);
    }
}

public RestartGameHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    SetConVarInt(FindConVar("mp_restartgame"), 0);
    TerminateRound(StringToFloat(newValue), Round_Draw);
}

public Action:MotherZombie(Handle:timer)
{
    RefreshList();
    
    new size = GetArraySize(pList);
    new immune = 0;
    
    for (new x = 0; x < size; x++)
    {
        new index = GetArrayCell(pList, x);
        
        if (gBlockMotherInfect[index])
        {
            immune++;
        }
        
        if (!IsPlayerAlive(index) || IsPlayerZombie(index))
        {
            continue;
        }
        
        CS_SwitchTeam(index, CS_TEAM_CT);
    }
    
    if (!(size - immune))
    {
        tInfect = INVALID_HANDLE;
        
        return;
    }
    
    new randclient;
    new ratio = GetConVarInt(gCvars[CVAR_MOTHER_ZOMBIE_RATIO]);
    
    if (ratio <= 0)
    {
        do
        {
            randclient = RandomPlayerFromList();
        } while (!IsPlayerAlive(randclient) || gBlockMotherInfect[randclient]);
        
        Zombify_Mother(randclient);
    }
    else
    {
        new mothercount = RoundToCeil(float(size) / ratio);
        
        for (new x = 0; x < mothercount; x++)
        {
            do
            {
                randclient = RandomPlayerFromList();
            } while (IsPlayerZombie(randclient) || !IsPlayerAlive(randclient) || gBlockMotherInfect[randclient]);
            
            Zombify_Mother(randclient);
        }
    }
    
    tInfect = INVALID_HANDLE;
    
    zombieSpawned = true;
}

Zombify_Mother(client)
{
    Call_StartForward(hZombify);
    Call_PushCell(client);
    Call_PushCell(true);
    Call_Finish();
    
    gZombie[client] = true;
    motherZombie[client] = true;
    
    ApplyZombieHealth(client,true);
    
    ApplyZombieSpeed(client);
    
    CS_SwitchTeam(client, CS_TEAM_T);
    
    RemoveAllPlayersWeapons(client);
    GivePlayerItem(client, "weapon_knife");
    
    ApplyZombieNightVision(client);

    ZVisionPreCheck(client);
    
    ApplyZombieFOV(client);
    
    ApplyZombieModel(client);
    
    ApplyZombieAlpha(client);

    InfectionEffects(client);
    
    ztele_count[client] = 0;
    AbortTeleport(client);
    
    ZR_PrintToChat(client, "You are a zombie");
    
    new bool:mother_zombie_respawn = GetConVarBool(gCvars[CVAR_MOTHER_ZOMBIE_RESPAWN]);
    if (mother_zombie_respawn)
    {
        TeleportEntity(client, spawnLoc[client], NULL_VECTOR, NULL_VECTOR);
    }
    
    new Float:interval = GetConVarFloat(gCvars[CVAR_EMITSOUNDS]);
    if (interval > 0.0)
    {
        if (tHandles[client][TMOAN] != INVALID_HANDLE)
        {
            KillTimer(tHandles[client][TMOAN]);
        }
        tHandles[client][TMOAN] = CreateTimer(interval, ZombieMoanTimer, client, TIMER_REPEAT);
    }
    
    if (tHandles[client][TPROTECT] != INVALID_HANDLE)
    {
        pProtect[client] = false;
        
        KillTimer(tHandles[client][TPROTECT]);
        tHandles[client][TPROTECT] = INVALID_HANDLE;
    }
    
    if (tHandles[client][TZHP] != INVALID_HANDLE)
    {
        KillTimer(tHandles[client][TZHP]);
        tHandles[client][TZHP] = INVALID_HANDLE;
    }
    
    new bool:zhp = GetConVarBool(gCvars[CVAR_ZHP]);
    if (zhp)
    {
        UpdateHPDisplay(client);
        
        tHandles[client][TZHP] = CreateTimer(5.0, ZHPTimer, client, TIMER_REPEAT);
    }
}

Zombify(client, attacker)
{
    if (attacker != 0)
    {
        new Handle:event = CreateEvent("player_death");
        if (event != INVALID_HANDLE)
        {
            SetEventInt(event, "userid", GetClientUserId(client));
            SetEventInt(event, "attacker", GetClientUserId(attacker));
            SetEventString(event, "weapon", "zombie_claws_of_death");
            FireEvent(event, false);
        }
    }
    
    Call_StartForward(hZombify);
    Call_PushCell(client);
    Call_PushCell(false);
    Call_Finish();
    
    gZombie[client] = true;
    
    ApplyZombieHealth(client, false);
    
    ApplyZombieSpeed(client);
    
    RemoveAllPlayersWeapons(client);
    GivePlayerItem(client, "weapon_knife");
    
    ApplyZombieNightVision(client);
    
    ZVisionPreCheck(client);
    
    ApplyZombieFOV(client);
    
    ApplyZombieModel(client);
    
    ApplyZombieAlpha(client);
    
    InfectionEffects(client);
    
    ztele_count[client] = 0;
    AbortTeleport(client);
    
    new ZTeam:team = IsRoundOver();
    RoundWin(team);
    
    CS_SwitchTeam(client, CS_TEAM_T);
    
    new Float:interval = GetConVarFloat(gCvars[CVAR_EMITSOUNDS]);
    if (interval > 0.0)
    {
        if (tHandles[client][TMOAN] != INVALID_HANDLE)
        {
            KillTimer(tHandles[client][TMOAN]);
        }
        tHandles[client][TMOAN] = CreateTimer(interval, ZombieMoanTimer, client, TIMER_REPEAT);
    }
    
    if (tHandles[client][TPROTECT] != INVALID_HANDLE)
    {
        pProtect[client] = false;
        
        KillTimer(tHandles[client][TPROTECT]);
        tHandles[client][TPROTECT] = INVALID_HANDLE;
    }
    
    if (tHandles[client][TZHP] != INVALID_HANDLE)
    {
        KillTimer(tHandles[client][TZHP]);
        tHandles[client][TZHP] = INVALID_HANDLE;
    }
    
    new bool:zhp = GetConVarBool(gCvars[CVAR_ZHP]);
    if (zhp)
    {
        UpdateHPDisplay(client);
        
        tHandles[client][TZHP] = CreateTimer(5.0, ZHPTimer, client, TIMER_REPEAT);
    }
    
    AntiStick(attacker, client);
}

InfectionEffects(client)
{
    new Float:clientloc[3];
    new Float:direction[3] = {0.0, 0.0, 0.0};
    
    GetClientAbsOrigin(client, clientloc);
    clientloc[2] += 30;
    
    decl String:sound[128];
    GetConVarString(gCvars[CVAR_INFECT_SOUND], sound, sizeof(sound));
    if (sound[0])
    {
        PrecacheSound(sound);
        EmitSoundToAll(sound, client, SNDCHAN_AUTO, SNDLEVEL_SCREAMING);
    }
    
    new bool:esplash = GetConVarBool(gCvars[CVAR_INFECT_ESPLASH]);
    if (esplash)
    {
        TE_SetupEnergySplash(clientloc, direction, true);
        TE_SendToAll();
    }
    
    new explosion = CreateEntityByName("env_explosion");
    
    if (explosion != -1)
    {
        new flags = GetEntProp(explosion, Prop_Data, "m_spawnflags");
        flags = flags | EXP_NODAMAGE | EXP_NODECAL;
        
        new bool:fireball = GetConVarBool(gCvars[CVAR_INFECT_FIREBALL]);
        if (!fireball)
        {
            flags = flags | EXP_NOFIREBALL;
        }
        
        new bool:smoke = GetConVarBool(gCvars[CVAR_INFECT_SMOKE]);
        if (!smoke)
        {
            flags = flags | EXP_NOSMOKE;
        }
        
        new bool:sparks = GetConVarBool(gCvars[CVAR_INFECT_SPARKS]);
        if (!sparks)
        {
            flags = flags | EXP_NOSPARKS;
        }
        
        SetEntProp(explosion, Prop_Data, "m_spawnflags", flags);
        
        DispatchSpawn(explosion);
        
        PrecacheModel("materials/sprites/xfireball3.vmt");
        
        DispatchKeyValueVector(explosion, "origin", clientloc);
        DispatchKeyValue(explosion, "fireballsprite", "materials/sprites/xfireball3.vmt");
        
        AcceptEntityInput(explosion, "Explode");
    }
    
    new bool:shake = GetConVarBool(gCvars[CVAR_INFECT_SHAKE]);
    if (shake)
    {
        new Handle:hShake = StartMessageOne("Shake", client);
        if (hShake != INVALID_HANDLE)
        {
            BfWriteByte(hShake, 0);
            BfWriteFloat(hShake, GetConVarFloat(gCvars[CVAR_INFECT_SHAKE_AMP]));
            BfWriteFloat(hShake, GetConVarFloat(gCvars[CVAR_INFECT_SHAKE_FREQUENCY]));
            BfWriteFloat(hShake, GetConVarFloat(gCvars[CVAR_INFECT_SHAKE_DURATION]));
            
            EndMessage();
        }
    }
}

ApplyZombieHealth(client, bool:mother)
{
    new health = GetClassHealth(pClass[client]);
    
    if (mother)
    {
        health *= 2;
    }
    
    SetEntityHealth(client, health);
}

ApplyZombieSpeed(client)
{
    new Float:speed = GetClassSpeed(pClass[client]);
    SetPlayerSpeed(client, speed);
}

ApplyZombieNightVision(client)
{
    new bool:nvgs = GetClassNVGs(pClass[client]);
    NightVision(client, nvgs);

    if (nvgs)
    {
        NightVisionOn(client, nvgs);
    }
}

ApplyZombieFOV(client)
{
    new fov = GetClassFOV(pClass[client]);
    SetPlayerFOV(client, fov);
}

ApplyZombieAlpha(client)
{
	new alpha = GetClassAlphaSpawn(pClass[client]);
	SetPlayerAlpha(client, alpha);
}

KnockBack(client, const Float:clientloc[3], const Float:attackerloc[3], Float:power, dmg, bool:boost)
{
    if (!IsPlayerZombie(client))
    {
        return;
    }
    
    new Float:vector[3];
    
    MakeVectorFromPoints(attackerloc, clientloc, vector);
    NormalizeVector(vector, vector);
    
    vector[0] *= power * (float(dmg) * 1.5);
    vector[1] *= power * (float(dmg) * 1.5);
    vector[2] *= power * (float(dmg) * 1.5);
    
    if (boost)
    {
        ScaleVector(vector, 4.0);
    }
    
    SetPlayerVelocity(client, vector);
}

JumpBoost(client, Float:distance, Float:height)
{
    new Float:vel[3];
    
    GetPlayerVelocity(client, vel);
    
    vel[0] *= distance;
    vel[1] *= distance;
    vel[2] = height;
    
    SetPlayerVelocity(client, vel);
}

PlayerLeft(client)
{
    if (!IsClientConnected(client) || !IsClientInGame(client))
    {
        return;
    }
    
    new ZTeam:team = IsRoundOver();
    if (team == Zombie)
    {
        RoundWin(team);
        return;
    }
        
    if (!IsPlayerAlive(client) || !IsPlayerZombie(client))
    {
        return;
    }
    
    new zombiecount = GetZTeamCount(Zombie);
    if (zombiecount > 1)
    {
        return;
    }
    
    new count = GetTeamClientCount(CS_TEAM_CT);
    if (count <= 1)
    {
        return;
    }
    
    new Handle:aClients = CreateArray();
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x) || !IsPlayerAlive(x) || client == x || GetClientTeam(x) != CS_TEAM_CT || gBlockMotherInfect[x])
        {
            continue;
        }
        
        PushArrayCell(aClients, x);
    }
    
    new size = GetArraySize(aClients);
    if (!size)
    {
        return;
    }
    
    new randclient = GetArrayCell(aClients, GetRandomInt(0, size-1));
    Zombify_Mother(randclient);
                    
    ZR_PrintToChat(randclient, "Zombie replacement");
  
    CloseHandle(aClients);
}

GetZTeamCount(ZTeam:team)
{
    new count = 0;
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x) || !IsPlayerAlive(x))
        {
            continue;
        }
        
        new ZTeam:pTeam = GetPlayerZTeam(x);
        if (pTeam == team)
        {
            count++;
        }
    }
    
    return count;
}

ZTeam:IsRoundOver()
{
    new bool:zombies = false;
    new bool:humans = false;
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x) || !IsPlayerAlive(x))
        {
            continue;
        }
        
        if (IsPlayerZombie(x))
        {
            zombies = true;
        }
        else
        {
            humans = true;
        }
    }
    
    if (zombies && !humans)
    {
        return Zombie;
    }
    
    if (humans && !zombies)
    {
        if (zombieSpawned)
        {
            return Human;
        }
    }
    
    return Neither;
}

RoundWin(ZTeam:team)
{
    if (team == Human)
    {
        TerminateRound(5.0, CTs_PreventEscape);
    }
    else if (team == Zombie)
    {
        TerminateRound(5.0, Terrorists_Escaped);
    }
}

BalanceTeams()
{
    new count = 0;
    new cPlayers[MAXPLAYERS];
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x) || GetClientTeam(x) <= 1)
        {
            continue;
        }
        
        CS_SwitchTeam(x, CS_TEAM_T);
        cPlayers[count++] = x;
    }
    
    for (new x = 0; x < count; x++)
    {
        if (!IsClientInGame(cPlayers[x]) || GetClientTeam(cPlayers[x]) <= 1)
        {
            continue;
        }
        
        CS_SwitchTeam(cPlayers[x], CS_TEAM_CT);
        x++;
    }
}

RemoveObjectives()
{
    decl String:classname[64];
    
    new maxentities = GetMaxEntities();
    for (new x = 0; x <= maxentities; x++)
    {
        if(!IsValidEdict(x))
        {
            continue;
        }
        
        GetEdictClassname(x, classname, sizeof(classname));
        if( StrEqual(classname, "func_bomb_target") ||
            StrEqual(classname, "func_hostage_rescue") ||
            StrEqual(classname, "c4") ||
            StrEqual(classname, "hostage_entity"))
            {
                RemoveEdict(x);
            }
    }
}

AntiStick(attacker, client)
{
    if (!attacker || !IsClientInGame(attacker))
    {
        return;
    }
    
    new Float:force = GetConVarFloat(gCvars[CVAR_INFECT_ANTISTICK_FORCE]);
    if (force == 0.0)
    {
        return;
    }
    
    new Float:vector[3];
    
    new Float:attackerloc[3];
    new Float:clientloc[3];
    
    GetClientAbsOrigin(attacker, attackerloc);
    GetClientAbsOrigin(client, clientloc);
    
    MakeVectorFromPoints(attackerloc, clientloc, vector);
    
    NormalizeVector(vector, vector);
    ScaleVector(vector, force);
    
    TeleportEntity(attacker, NULL_VECTOR, NULL_VECTOR, vector);
}

ZVisionPreCheck(client)
{
    if (IsFakeClient(client))
    {
        return;
    }
    
    decl String:zvision[128];
    GetClassZVision(pClass[client], zvision, sizeof(zvision));
    
    if (strlen(zvision) == 0)
    {
        bZVision[client] = false;
    }
    else
    {
        bZVision[client] = true;
        strcopy(ZVisionOverlay[client], PLATFORM_MAX_PATH, zvision);
        
        ZVisionStart(client);
    }
}

ZVisionStart(client)
{
    /*if (tHandles[client][TZVISION] != INVALID_HANDLE)
    {
        KillTimer(tHandles[client][TZVISION]);
        tHandles[client][TZVISION] = INVALID_HANDLE;
    }*/
    
    DisplayClientOverlay(client, ZVisionOverlay[client]);
    bZVisionOn[client] = true;
    
    //new Float:redisplay = GetConVarFloat(gCvars[CVAR_ZVISION_REDISPLAY]);
    //tHandles[client][TZVISION] = CreateTimer(redisplay, ZVisionTimer, client, TIMER_REPEAT);
}

ZVisionStop(client)
{
    if (tHandles[client][TZVISION] != INVALID_HANDLE)
    {
        KillTimer(tHandles[client][TZVISION]);
        tHandles[client][TZVISION] = INVALID_HANDLE;
    }
    
    ClientCommand(client, "r_screenoverlay \"\"");
    bZVisionOn[client] = false;
}

/*public Action:ZVisionTimer(Handle:timer, any:client)
{
    if (!IsClientInGame(client) || !IsPlayerZombie(client))
    {
        tHandles[client][TZVISION] = INVALID_HANDLE;
        return Plugin_Stop;
    }
    
    DisplayClientOverlay(client, ZVisionOverlay[client]);
    return Plugin_Continue;
}*/

ZombieMoan(client)
{
    decl String:sound[64];
    
    new randsound = GetRandomInt(1, 14);
    Format(sound, sizeof(sound), "npc/zombie/zombie_voice_idle%d.wav", randsound);
    
    PrecacheSound(sound);
    EmitSoundToAll(sound, client, SNDCHAN_AUTO, SNDLEVEL_SCREAMING);
}

public Action:ZombieMoanTimer(Handle:timer, any:index)
{
    if (!IsClientInGame(index) || !IsPlayerZombie(index))
    {
        tHandles[index][TMOAN] = INVALID_HANDLE;
        
        return Plugin_Stop;
    }
    
    ZombieMoan(index);
        
    return Plugin_Continue;
}

public Action:Regenerate(Handle:timer, any:index)
{
    if (!IsClientInGame(index) || !IsPlayerZombie(index))
    {
        tHandles[index][TREGEN] = INVALID_HANDLE;
        
        return Plugin_Stop;
    }
    
    new health = GetClassRegenHealth(pClass[index]);
    new maxhealth = GetClassHealth(pClass[index]);
    
    if (motherZombie[index])
    {
        maxhealth *= 2;
    }
    
    new newhealth = GetClientHealth(index) + health;
    if (newhealth > maxhealth)
    {
        newhealth = maxhealth;
        
        tHandles[index][TREGEN] = INVALID_HANDLE;
        
        return Plugin_Stop;
    }
    
    SetEntityHealth(index, newhealth);
    
    UpdateHPDisplay(index);
    
    return Plugin_Continue;
}

UpdateHPDisplay(client)
{
    new bool:zhp = GetConVarBool(gCvars[CVAR_ZHP]);
    if (!zhp)
    {
        return;
    }
    
    if (!IsPlayerZombie(client) || !dispHP[client])
    {
        return;
    }
    
    new health = GetClientHealth(client);
    if (health < 0)
    {
        health = 0;
    }
    
    ZR_HudHint(client, "Display HP", health);
}

UpdateAlphaDamaged(client)
{
    new current_health = GetClientHealth(client);
    new max_health = GetClassHealth(pClass[client]);
    new max_damage = GetClassAlphaDamage(pClass[client]);
    if (max_health - current_health > max_damage)
    {
        new alpha_damaged = GetClassAlphaDamaged(pClass[client]);
        SetPlayerAlpha(client, alpha_damaged);
    }
}

public Action:ZHPTimer(Handle:timer, any:index)
{
    if (!IsClientInGame(index))
    {
        tHandles[index][TZHP] = INVALID_HANDLE;
        
        return Plugin_Stop;
    }
    
    UpdateHPDisplay(index);
    
    return Plugin_Continue;
}

public Action:ProtectTimer(Handle:timer, any:index)
{
    if (!IsClientInGame(index))
    {
        tHandles[index][TPROTECT] = INVALID_HANDLE;
        return Plugin_Stop;
    }
    
    pTimeLeft[index]--;
    ZR_HudHint(index, "Spawn Protect", pTimeLeft[index]);
    
    if (pTimeLeft[index] <= 0)
    {
        pProtect[index] = false;
    
        if (IsPlayerHuman(index))
        {
            ZR_HudHint(index, "Spawn protection end");
            SetPlayerAlpha(index, 255);
            SetPlayerSpeed(index, 300.0);
        }

        tHandles[index][TPROTECT] = INVALID_HANDLE;

        return Plugin_Stop;
    }
    
    return Plugin_Continue;
}

RespawnPlayer(client)
{
    if (!IsClientInGame(client))
    {
        return;
    }
    
    CS_RespawnPlayer(client);
    
    if (!zombieSpawned)
    {
        return;
    }
    
    decl String:team[32];
    GetConVarString(gCvars[CVAR_RESPAWN_TEAM], team, sizeof(team));
    
    if (StrEqual(team, "zombie", false))
    {
        Zombify(client, 0);
        return;
    }
    
    if (GetConVarBool(gCvars[CVAR_SUICIDE_WORLD_DAMAGE]) && gKilledByWorld[client])
    {
        Zombify(client, 0);
        gKilledByWorld[client] = false;
    }
}

public Action:RespawnTimer(Handle:timer, any:index)
{
    new team = GetClientTeam(index);
    if (!IsClientInGame(index) || IsPlayerAlive(index) || team != CS_TEAM_T && team != CS_TEAM_CT)
    {
        tHandles[index][TRESPAWN] = INVALID_HANDLE;
        return;
    }
    
    RespawnPlayer(index);
    
    tHandles[index][TRESPAWN] = INVALID_HANDLE;
}

public Action:RoundOver(Handle:timer)
{
    tRound = INVALID_HANDLE;
    
    RoundWin(Human);
}

bool:IsPlayerZombie(client)
{
    return gZombie[client];
}

bool:IsPlayerHuman(client)
{
    return !gZombie[client];
}

ZTeam:GetPlayerZTeam(client)
{
    if (IsPlayerZombie(client))
    {
        return Zombie;
    }
    
    return Human;
}
