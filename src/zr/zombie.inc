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
    
    bClientOverlayOn[client] = !bClientOverlayOn[client];
    
    decl String:overlay[256];
    ClassGetOverlayPath(client, overlay, sizeof(overlay));
    
    if (strlen(overlay) > 0)
    {
        if (bClientOverlayOn[client])
        {
            ClassOverlayInitialize(client, overlay);
        }
        else
        {
            ClassOverlayStop(client);
        }
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
        
        InfectPlayer(randclient, _, true);
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
            
            InfectPlayer(randclient, _, true);
        }
    }
    
    tInfect = INVALID_HANDLE;
    
    zombieSpawned = true;
}

/*Zombify_Mother(client)
{
    gZombie[client] = true;
    motherZombie[client] = true;
    
    CS_SwitchTeam(client, CS_TEAM_T);
    
    RemoveAllPlayersWeapons(client);
    GivePlayerItem(client, "weapon_knife");
    
    ClassOnClientInfected(client, true);
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
}*/

/**
 * Zombifies a player. Execute events, sets attributes and flags that indicate
 * that the player is a zombie.
 *
 * @param client        The player to infect.
 * @param attacker      Optional. The attacker who did the infect.
 * @param motherinfect  Optional. Indicates a mother zombie infect.
 */
InfectPlayer(client, attacker = -1, bool:motherinfect = false)
{
    // Check if the attacker was specified.
    if (attacker > 0)
    {
        // Fire death event and set weapon info.
        new Handle:event = CreateEvent("player_death");
        if (event != INVALID_HANDLE)
        {
            SetEventInt(event, "userid", GetClientUserId(client));
            SetEventInt(event, "attacker", GetClientUserId(attacker));
            SetEventString(event, "weapon", "zombie_claws_of_death");
            FireEvent(event, false);
        }
    }
    
    // Set player status.
    gZombie[client] = true;
    motherZombie[client] = motherinfect;
    
    // Remove all weapons and give a knife.
    RemoveAllPlayersWeapons(client);
    GivePlayerItem(client, "weapon_knife");
    
    ztele_count[client] = 0;    // In use?
    
    // Terminate the round if the last player was infected.
    new ZTeam:team = IsRoundOver();
    RoundWin(team);
    
    // Switch the player to terrorists.
    CS_SwitchTeam(client, CS_TEAM_T);
    
    // Check if random zombie sounds are enabled.
    new Float:interval = GetConVarFloat(gCvars[CVAR_EMITSOUNDS]);
    if (interval > 0.0)
    {
        // Create timer.
        if (tHandles[client][TMOAN] != INVALID_HANDLE)
        {
            KillTimer(tHandles[client][TMOAN]);
        }
        tHandles[client][TMOAN] = CreateTimer(interval, ZombieMoanTimer, client, TIMER_REPEAT);
    }
    
    // Forward event to modules.
    ClassOnClientInfected(client, motherinfect);
    ZHPOnClientInfected(client);
    AbortTeleport(client);
    
    // Apply effects.
    InfectionEffects(client);
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

JumpBoost(client, Float:distance, Float:height)
{
    new Float:vel[3];
    
    GetPlayerVelocity(client, vel);
    
    vel[0] *= distance;
    vel[1] *= distance;
    vel[2] *= height;
    
    SetPlayerVelocity(client, vel, false);
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
    
    for (new x = 1; x <= MaxClients; x++)
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
    InfectPlayer(randclient, _, true);
                    
    ZR_PrintToChat(randclient, "Zombie replacement");
  
    CloseHandle(aClients);
}

GetZTeamCount(ZTeam:team)
{
    new count = 0;
    
    for (new x = 1; x <= MaxClients; x++)
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
    
    for (new x = 1; x <= MaxClients; x++)
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
    
    for (new x = 1; x <= MaxClients; x++)
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
