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
    new bool:allow_disable = GetConVarBool(g_hCvarsList[CVAR_ZVISION_ALLOW_DISABLE]);
    if (!allow_disable)
    {
        return;
    }
    
    new bool:enabled = GetConVarBool(g_hCvarsList[CVAR_ENABLE]);
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
    new bool:dark = GetConVarBool(g_hCvarsList[CVAR_DARK]);
    if (dark)
    {
        decl String:darkness[2];
        decl String:sky[32];
        
        GetConVarString(g_hCvarsList[CVAR_DARK_LEVEL], darkness, sizeof(darkness));
        GetConVarString(g_hCvarsList[CVAR_DARK_SKY], sky, sizeof(sky));
        
        SetLightStyle(0, darkness);
        SetConVarString(FindConVar("sv_skyname"), sky, true, false);
    }
    else
    {
        SetLightStyle(0, "n");
        SetConVarString(FindConVar("sv_skyname"), skyname, true, false);
    }
}

/**
 * Create an array populated with eligible clients to be zombie.
 * 
 * @param arrayEligibleClients  The handle of the array, don't forget to call CloseHandle
 *                              on it when finished!
 * @param immunity              True to ignore clients immune from mother infect, false to count them.
 */  
CreateEligibleClientList(&Handle:arrayEligibleClients, bool:team = false, bool:alive = false, bool:human = false, bool:immunity = false)
{
    // Create array.
    arrayEligibleClients = CreateArray();
    
    // Populate list with eligible clients.
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client isn't on a team, then stop.
        if (team && !ZRIsClientOnTeam(x))
        {
            continue;
        }
        
        // If client is dead, then stop.
        if (alive && !IsPlayerAlive(x))
        {
            continue;
        }
        
        // If client is already zombie (via admin), then stop.
        if (human && !IsPlayerHuman(x))
        {
            continue;
        }
        
        // If client is immune from being a mother zombie, then stop.
        if (immunity && bMotherInfectImmune[x])
        {
            // Take away immunity.
            bMotherInfectImmune[x] = false;
            
            continue;
        }
        
        // Add eligible client to array.
        PushArrayCell(arrayEligibleClients, x);
    }
    
    return GetArraySize(arrayEligibleClients);
}

/**
 * Timer callback, chooses mother zombies.
 * 
 * @param timer     The timer handle.  
 */ 
public Action:MotherZombie(Handle:timer)
{
    // Reset timer handle.
    tInfect = INVALID_HANDLE;
    
    // Create eligible player list.
    new Handle:arrayEligibleClients = INVALID_HANDLE;
    new eligibleclients = CreateEligibleClientList(arrayEligibleClients, true, true, true, true);
    
    // If there are no eligible client's then stop.
    if (!eligibleclients)
    {
        return;
    }
    
    // Move all clients to CT
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client is dead, then stop.
        if (!IsPlayerAlive(x))
        {
            continue;
        }
        
        // Switch client to CT team.
        CS_SwitchTeam(x, CS_TEAM_CT);
    }
    
    // Variable to store randomly chosen array index.
    new randindex;
    
    // Variable to store client stored in random array index.
    new client;
    
    // Ratio of mother zombies to humans.
    new ratio = GetConVarInt(g_hCvarsList[CVAR_MOTHER_ZOMBIE_RATIO]);
    
    // If ratio is 0 or lower, then pick 1 zombie.
    if (ratio <= 0)
    {
        // Get a random valid array index.
        randindex = GetRandomInt(0, eligibleclients - 1);
        
        // Get the client stored in the random array index.
        client = GetArrayCell(arrayEligibleClients, randindex);
        
        // Infect player.
        InfectPlayer(client, _, true);
    }
    else
    {
        // Calculate mother zombie sound.
        new mothercount = RoundToCeil(float(eligibleclients) / ratio);
        
        // x = current mother zombie count.
        for (new x = 0; x < mothercount; x++)
        {
            // Recount eligible clients.
            eligibleclients = GetArraySize(arrayEligibleClients);
            
            // If there are no more eligible clients, then break loop.
            if (!eligibleclients)
            {
                break;
            }
            // Get a random valid array index.
            randindex = GetRandomInt(0, eligibleclients - 1);
            
            // Get the client stored in the random array index.
            client = GetArrayCell(arrayEligibleClients, randindex);
            
            // Infect player.
            InfectPlayer(client, _, true);
            
            // Remove player from eligible zombie list.
            RemoveFromArray(arrayEligibleClients, randindex);
        }
    }
    
    // Mother zombies have been infected.
    g_bZombieSpawned = true;
    
    // Destroy handle.
    CloseHandle(arrayEligibleClients);
}

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
    // Mark player as zombie.
    bZombie[client] = true;
    
    // Remove all weapons and give a knife.
    RemoveAllPlayersWeapons(client);
    GivePlayerItem(client, "weapon_knife");
    
    // Check if consecutive infection protection is enabled.
    new bool:infectconsecutiveblock = GetConVarBool(g_hCvarsList[CVAR_INFECT_CONSECUTIVE_BLOCK]);
    
    // Flag player to be immune from being mother zombie twice, if consecutive infect protection is enabled.
    bMotherInfectImmune[client] = infectconsecutiveblock ? motherinfect : false;
    
    // Forward event to modules.
    ClassOnClientInfected(client, motherinfect);
    SEffectsOnClientInfected(client);
    ZHPOnClientInfected(client);
    TeleportOnClientInfected(client);
    
    // Apply effects.
    InfectionEffects(client);
    
    // Fire death event and set weapon info if the attacker is specified.
    if (ZRIsValidClient(attacker))
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
    
    // Switch the player to terrorists.
    // TODO: A solution to stop confusing bots? Respawn and teleport?
    CS_SwitchTeam(client, CS_TEAM_T);
    
    // Terminate the round if the last player was infected.
    new RoundEndOutcome:outcome;
    if (RoundEndGetRoundStatus(outcome))
    {
        RoundEndTerminateRound(outcome);
    }
}

InfectionEffects(client)
{
    new Float:clientloc[3];
    new Float:direction[3] = {0.0, 0.0, 0.0};
    
    GetClientAbsOrigin(client, clientloc);
    clientloc[2] += 30;
    
    decl String:sound[128];
    GetConVarString(g_hCvarsList[CVAR_INFECT_SOUND], sound, sizeof(sound));
    if (sound[0])
    {
        SEffectsEmitSoundFromClient(client, sound, SNDLEVEL_SCREAMING);
    }
    
    new bool:esplash = GetConVarBool(g_hCvarsList[CVAR_INFECT_ESPLASH]);
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
        
        new bool:fireball = GetConVarBool(g_hCvarsList[CVAR_INFECT_FIREBALL]);
        if (!fireball)
        {
            flags = flags | EXP_NOFIREBALL;
        }
        
        new bool:smoke = GetConVarBool(g_hCvarsList[CVAR_INFECT_SMOKE]);
        if (!smoke)
        {
            flags = flags | EXP_NOSMOKE;
        }
        
        new bool:sparks = GetConVarBool(g_hCvarsList[CVAR_INFECT_SPARKS]);
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
    
    new bool:shake = GetConVarBool(g_hCvarsList[CVAR_INFECT_SHAKE]);
    if (shake)
    {
        new Handle:hShake = StartMessageOne("Shake", client);
        if (hShake != INVALID_HANDLE)
        {
            BfWriteByte(hShake, 0);
            BfWriteFloat(hShake, GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_AMP]));
            BfWriteFloat(hShake, GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_FREQUENCY]));
            BfWriteFloat(hShake, GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_DURATION]));
            
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

/**
 * Finds a new zombie if the last one disconnects.
 * 
 * @param client    The client index.  
 */ 
PlayerLeft(client)
{
    // If zombie hasn't spawned, then stop.
    if (!g_bZombieSpawned)
    {
        return;
    }
    
    // If client is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        return;
    }
    
    // Get zombie flag on client.
    new zombieclient = IsPlayerZombie(client);
    
    // Initialize count variables
    new zombiecount;
    new humancount;
    
    // Count valid clients.
    ZRCountValidClients(zombiecount, humancount);
    
    // If client is a human, and is the last one, then terminate round.
    if (IsPlayerHuman(client) && humancount <= 1)
    {
        // If there are no more clients in the server, then stop.
        if (!ZRTeamHasClients(CS_TEAM_T))
        {
            return;
        }
        
        // Manually terminate round.
        RoundEndTerminateRound(ZombiesWin);
        
        return;
    }
    
    // We know here that player is a zombie.
    
    // If there are other zombies (ignoring this zombie), then stop.
    if (zombiecount - 1)
    {
        return;
    }
    
    // Create eligible player list.
    new Handle:arrayEligibleClients = INVALID_HANDLE;
    
    // Create eligible client list, with no mother infect immunities
    new eligibleclients = CreateEligibleClientList(arrayEligibleClients, true, true, true);
    
    // If there are no eligible client's then stop.
    if (!eligibleclients)
    {
        return;
    }
    
    // Get a random valid array index.
    new randindex = GetRandomInt(0, eligibleclients - 1);
    
    // Get the client stored in the random array index.
    new randclient = GetArrayCell(arrayEligibleClients, randindex);
    
    // Infect player.
    InfectPlayer(randclient);
    
    // Tell client they have been randomly been chosen to replace disconnecting zombie.
    ZR_PrintToChat(randclient, "Zombie replacement");
    
    // Destroy handle.
    CloseHandle(arrayEligibleClients);
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

bool:IsPlayerZombie(client)
{
    return bZombie[client];
}

bool:IsPlayerHuman(client)
{
    return !bZombie[client];
}