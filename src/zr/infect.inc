/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        infect.inc
 *   Description: Client infection functions.
 *
 * ============================================================================
 */

/**
 * @section Explosion flags.
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
/**
 * @endsection
 */

/**
 * Global variable to store infect timer handle.
 */
new Handle:tInfect = INVALID_HANDLE;

/**
 * Array for flagging client as zombie.
 */
new bool:bZombie[MAXPLAYERS + 1];

/**
 * @section bInfectImmune indexes
 */
#define INFECT_TYPE_MOTHER 0
#define INFECT_TYPE_NORMAL 1
/**
 * @endsection
 */

/**
 * Array for flagging client to be protected. (See defines above)
 */
new bool:bInfectImmune[MAXPLAYERS + 1][2];

/**
 * Map is starting.
 */
InfectOnMapStart()
{
    // Reset timer handle.
    tInfect = INVALID_HANDLE;
    
    // Get infection sound.
    decl String:sound[PLATFORM_MAX_PATH];
    GetConVarString(g_hCvarsList[CVAR_INFECT_SOUND], sound, sizeof(sound));
    
    // If infect sound cvar is empty, then stop.
    if (!sound[0])
    {
        return;
    }
    
    // Prepend sound/ to the path.
    Format(sound, sizeof(sound), "sound/%s", sound);
    
    // Add sound file to downloads table.
    AddFileToDownloadsTable(sound);
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
InfectClientInit(client)
{
    // Reset infect immunity flags.
    bInfectImmune[client][INFECT_TYPE_MOTHER] = false;
    bInfectImmune[client][INFECT_TYPE_NORMAL] = false;
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
InfectOnClientDisconnect(client)
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
    
    // Initialize count variables
    new zombiecount;
    new humancount;
    
    // Count valid clients.
    ZRCountValidClients(zombiecount, humancount);
    
    // If client is a human.
    if (IsPlayerHuman(client))
    {
        // If there are other humans (ignoring this human), then stop.
        if (humancount > 1)
        {
            return;
        }
        
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
    
    // If there is 1 or less humans, then stop.
    if (humancount <= 1)
    {
        return;
    }
    
    // If there are other zombies (ignoring this zombie), then stop.
    if (zombiecount - 1)
    {
        return;
    }
    
    // Create eligible player list.
    new Handle:arrayEligibleClients = INVALID_HANDLE;
    
    // Create eligible client list, with no mother infect immunities
    new eligibleclients = ZRCreateEligibleClientList(arrayEligibleClients, true, true, true);
    
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

/**
 * Client is joining a team.
 * 
 * @param client    The client index.
 * @param team      The team index. 
 */
InfectOnClientTeam(client, team)
{
    // If client isn't joining spec, then stop.
    if (team != CS_TEAM_SPECTATOR)
    {
        return;
    }
    
    // Disable zombie flag on client.
    bZombie[client] = false;
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
InfectOnClientSpawn(client)
{
    // Disable zombie flag on client.
    bZombie[client] = false;
}

/** Client has been hurt.
 *
 * @param client        The client index.
 * @param attacker      The attacker index.
 * @param weapon        The weapon used.
 */
InfectOnClientHurt(client, attacker, const String:weapon[])
{
    // If attacker isn't valid, then stop.
    if (!ZRIsValidClient(attacker))
    {
        return;
    }
    
    // If client isn't a human, then stop.
    if (!IsPlayerHuman(client))
    {
        return;
    }
    
    // Attacker isn't a zombie, then stop.
    if (!IsPlayerZombie(attacker))
    {
        return;
    }
    
    // If client has infect immunity, then stop.
    if (bInfectImmune[client][INFECT_TYPE_NORMAL])
    {
        return;
    }
    
    // If weapon isn't a knife, then stop.
    if (!StrEqual(weapon, "knife"))
    {
        return;
    }
    
    // Infect client.
    InfectPlayer(client, attacker);
}

/**
 * The round is starting.
 */
InfectOnRoundStart()
{
    // If infect timer is running, then kill it.
    if (tInfect != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tInfect);
        
        // Reset timer handle.
        tInfect = INVALID_HANDLE;
    }
    
    // x = client index.
    for (new x = 1; x <=  MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // Disable zombie flag on client.
        bZombie[x] = false;
    }
}

/**
 * The freeze time is ending.
 */
InfectOnRoundFreezeEnd()
{
    // If infect timer is running, then kill it.
    if (tInfect != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tInfect);
    }
    
    // Get min and max times.
    new Float:infectspawntimemin = GetConVarFloat(g_hCvarsList[CVAR_INFECT_SPAWNTIME_MIN]);
    new Float:infectspawntimemax = GetConVarFloat(g_hCvarsList[CVAR_INFECT_SPAWNTIME_MAX]);
    
    // Pick random time between min and max.
    new Float:randomtime = GetRandomFloat(infectspawntimemin, infectspawntimemax);
    
    tInfect = CreateTimer(randomtime, InfectMotherZombie, _, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 */
InfectOnRoundEnd()
{
    // If infect timer is running, then kill it.
    if (tInfect != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tInfect);
        
        // Reset timer handle.
        tInfect = INVALID_HANDLE;
    }
    
    // x = client index.
    for (new x = 1; x <=  MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // Disable zombie flag on client.
        bZombie[x] = false;
    }
}

/**
 * Timer callback, chooses mother zombies.
 * 
 * @param timer     The timer handle.  
 */ 
public Action:InfectMotherZombie(Handle:timer)
{
    // Reset timer handle.
    tInfect = INVALID_HANDLE;
    
    // Create eligible player list.
    new Handle:arrayEligibleClients = INVALID_HANDLE;
    new eligibleclients = ZRCreateEligibleClientList(arrayEligibleClients, true, true, true);
    
    // If there are no eligible client's then stop.
    if (!eligibleclients)
    {
        return;
    }
    
    // Variable to store client stored in random array index.
    new client;
    
    // Prune list of immune clients.
    // x = array index.
    // client = client index.
    for (new x = 0; x < eligibleclients; x++)
    {
        // Get client stored in array index.
        client = GetArrayCell(arrayEligibleClients, x);
        
        // If client is immune from being a mother zombie, then stop.
        if (bInfectImmune[client][INFECT_TYPE_MOTHER])
        {
            // Take away immunity.
            bInfectImmune[client][INFECT_TYPE_MOTHER] = false;
            
            // Remove client from array.
            RemoveFromArray(arrayEligibleClients, x);
            
            // Subtract one from count.
            eligibleclients--;
            
            // If there are no eligible client's then stop.
            if (!eligibleclients)
            {
                return;
            }
            
            // Backtrack one index, because we deleted it out from under the loop.
            x--;
        }
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
    
    // Ratio of mother zombies to humans.
    new ratio = GetConVarInt(g_hCvarsList[CVAR_INFECT_MZOMBIE_RATIO]);
    
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
        // Initialize count variables
        new zombiecount;
        new humancount;
        
        // Count valid human clients
        ZRCountValidClients(zombiecount, humancount, _, true);
        
        // Calculate mother zombie count.
        new mothercount = RoundToCeil(float(humancount) / ratio);
        
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
 * Infects a player. Execute events, sets attributes and flags that indicate
 * that the player is a zombie.
 *
 * @param client        The player to infect.
 * @param attacker      (Optional) The attacker who did the infect.
 * @param motherinfect  (Optional) Indicates a mother zombie infect.
 */
InfectPlayer(client, attacker = -1, bool:motherinfect = false)
{
    // Mark player as zombie.
    bZombie[client] = true;
    
    // Get a list of all client's weapon indexes.
    new weapons[WeaponsType];
    WeaponsGetClientWeapons(client, weapons);
    
    // Loop through array slots and force drop.
    // x = weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        // If this is the knife slot, then stop.
        if (WeaponsType:x == Type_Melee)
        {
            continue;
        }
        
        // If weapon is invalid, then stop.
        if (weapons[x] == -1)
        {
            continue;
        }
        
        // Force client to drop weapon.
        WeaponForceClientDrop(client, weapons[x]);
    }
    
    // If client has no knife, give them one.
    if (weapons[Type_Melee] == -1)
    {
        GivePlayerItem(client, "weapon_knife");
    }
    
    // Check if consecutive infection protection is enabled.
    new bool:infectconsecutiveblock = GetConVarBool(g_hCvarsList[CVAR_INFECT_CONSECUTIVE_BLOCK]);
    
    // Flag player to be immune from being mother zombie twice, if consecutive infect protection is enabled.
    bInfectImmune[client][INFECT_TYPE_MOTHER] = infectconsecutiveblock ? motherinfect : false;
    
    // Apply effects.
    InfectEffects(client);
    
    // Add a death to the zombie's score.
    AddPlayerDeath(client, 1);
    
    // Fire death event and set weapon info if the attacker is specified.
    if (ZRIsValidClient(attacker))
    {
        // Create and send custom player_death event.
        new Handle:event = CreateEvent("player_death");
        if (event != INVALID_HANDLE)
        {
            SetEventInt(event, "userid", GetClientUserId(client));
            SetEventInt(event, "attacker", GetClientUserId(attacker));
            SetEventString(event, "weapon", "zombie_claws_of_death");
            FireEvent(event, false);
        }
        
        // Give client's infector a point.
        AddPlayerScore(attacker, 1);
        
        // Apply infect HP gain.
        new healthgain = ClassGetHealthInfectGain(attacker);
        new health = GetClientHealth(attacker);
        
        // Set attacker's new health.
        SetEntityHealth(attacker, health + healthgain);
        
        // Forward event to modules.
        ZHPOnHealthInfectGain(attacker);
    }
    
    // Switch the player to terrorists.
    // TODO: A solution to stop confusing bots? Respawn and teleport?
    CS_SwitchTeam(client, CS_TEAM_T);
    
    // Forward event to modules.
    ClassOnClientInfected(client, motherinfect);
    SEffectsOnClientInfected(client);
    ZHPOnClientInfected(client);
    TeleportOnClientInfected(client);
    
    // Terminate the round if the last player was infected.
    new RoundEndOutcome:outcome;
    if (RoundEndGetRoundStatus(outcome))
    {
        RoundEndTerminateRound(outcome);
    }
}

/**
 * Creates effects on a newly infected client.
 * 
 * @param client    The client index.
 */
InfectEffects(client)
{
    // Create location and direction arrays.
    new Float:clientloc[3];
    new Float:direction[3] = {0.0, 0.0, 0.0};
    
    // Get client's position.
    GetClientAbsOrigin(client, clientloc);
    clientloc[2] += 30;
    
    // Get infection sound.
    decl String:sound[PLATFORM_MAX_PATH];
    GetConVarString(g_hCvarsList[CVAR_INFECT_SOUND], sound, sizeof(sound));
    if (sound[0])
    {
        SEffectsEmitSoundFromClient(client, sound, SNDLEVEL_SCREAMING);
    }
    
    // Create an energy splash effect.
    new bool:esplash = GetConVarBool(g_hCvarsList[CVAR_INFECT_ESPLASH]);
    if (esplash)
    {
        TE_SetupEnergySplash(clientloc, direction, true);
        TE_SendToAll();
    }
    
    // Create an explosion entity.
    new explosion = CreateEntityByName("env_explosion");
    
    // If explosion entity is valid, then continue.
    if (explosion != -1)
    {
        // Get and set flags on explosion.
        new flags = GetEntProp(explosion, Prop_Data, "m_spawnflags");
        flags = flags | EXP_NODAMAGE | EXP_NODECAL;
        
        // Set "nofireball" flag if fireball is disabled.
        new bool:fireball = GetConVarBool(g_hCvarsList[CVAR_INFECT_FIREBALL]);
        if (!fireball)
        {
            flags = flags | EXP_NOFIREBALL;
        }
        
        // Set "nosmoke" flag if smoke is disabled.
        new bool:smoke = GetConVarBool(g_hCvarsList[CVAR_INFECT_SMOKE]);
        if (!smoke)
        {
            flags = flags | EXP_NOSMOKE;
        }
        
        // Set "nosparks" flag if sparks are disabled.
        new bool:sparks = GetConVarBool(g_hCvarsList[CVAR_INFECT_SPARKS]);
        if (!sparks)
        {
            flags = flags | EXP_NOSPARKS;
        }
        
        // Set new flags on entity.
        SetEntProp(explosion, Prop_Data, "m_spawnflags", flags);
        
        // Spawn the entity into the world.
        DispatchSpawn(explosion);
        
        // Precache fireball model.
        PrecacheModel("materials/sprites/xfireball3.vmt");
        
        // Set origin and explosion key values on entity.
        DispatchKeyValueVector(explosion, "origin", clientloc);
        DispatchKeyValue(explosion, "fireballsprite", "materials/sprites/xfireball3.vmt");
        
        // Tell the entity to explode.
        AcceptEntityInput(explosion, "Explode");
    }
    
    // If shake effect is enabled, then continue.
    new bool:shake = GetConVarBool(g_hCvarsList[CVAR_INFECT_SHAKE]);
    if (shake)
    {
        // If shake usermsg isn't invalid, then continue.
        new Handle:hShake = StartMessageOne("Shake", client);
        if (hShake != INVALID_HANDLE)
        {
            // Write shake information to usermsg handle.
            BfWriteByte(hShake, 0);
            BfWriteFloat(hShake, GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_AMP]));
            BfWriteFloat(hShake, GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_FREQUENCY]));
            BfWriteFloat(hShake, GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_DURATION]));
            
            // End usermsg and sent to client.
            EndMessage();
        }
    }
}