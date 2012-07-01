/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          infect.inc
 *  Type:          Core
 *  Description:   Client infection functions.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
 * @section Global variables to store infect timer handles.
 */
new Handle:tInfect = INVALID_HANDLE;
new Handle:tInfectCountdown = INVALID_HANDLE;
/**
 * @endsection
 */

/**
 * Infection countdown data pack.
 */
new Handle:hInfectCountdownData = INVALID_HANDLE;

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
 * Map is ending.
 */
InfectOnMapEnd()
{
    // Reset timers. Infect timers are invalidated on a map change if they are
    // still running.
    ZREndTimer(tInfect);
    ZREndTimer(tInfectCountdown);
    InfectStopCountdown();
}

/**
 * Loads downloadable content data for infect module.
 */
InfectLoad()
{
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
 * Create commands specific to infect here.
 */
InfectOnCommandsCreate()
{
    RegConsoleCmd("zr_infect", InfectInfectCommand, "Infect a client. Usage: zr_infect <filter> [respawn - 1/0]");
    RegConsoleCmd("zr_human", InfectHumanCommand, "Turn a client into a human. Usage: zr_human <filter> [respawn - 1/0]");
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
    // If client is still connecting, then stop.
    if (!IsClientInGame(client))
    {
        return;
    }
    
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
    if (InfectIsClientHuman(client))
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
        RoundEndTerminateRound(ROUNDEND_DELAY, ZombiesWin);
        
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
        // Destroy handle.
        CloseHandle(arrayEligibleClients);
        return;
    }
    
    // Get a random valid array index.
    new randindex = Math_GetRandomInt(0, eligibleclients - 1);
    
    // Get the client stored in the random array index.
    new randclient = GetArrayCell(arrayEligibleClients, randindex);
    
    // Infect player.
    InfectHumanToZombie(randclient);
    
    // Tell client they have been randomly been chosen to replace disconnecting zombie.
    TranslationPrintToChat(randclient, "Infect disconnect");
    
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
    
    // Check if client is spawning on the terrorist team.
    if (ZRIsClientOnTeam(client, CS_TEAM_T))
    {
        if (g_bZombieSpawned)
        {
            CS_SwitchTeam(client, CS_TEAM_CT);
            CS_RespawnPlayer(client);
        }
    }
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 * @param attacker  The attacker index.
 */
InfectOnClientDeath(client, attacker)
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return;
    }
    
    // If attacker isn't a human, then stop.
    if (!InfectIsClientHuman(attacker))
    {
        return;
    }
    
    // If client isn't a zombie, then stop.
    if (!InfectIsClientInfected(client))
    {
        return;
    }
    
    // Add kill bonus to attacker's score.
    new bonus = ClassGetKillBonus(client);
    new score = ToolsClientScore(attacker, true, false);
    ToolsClientScore(attacker, true, true, score + bonus);
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
    if (!ZRIsClientValid(attacker))
    {
        return;
    }
    
    // If client isn't a human, then stop.
    if (!InfectIsClientHuman(client))
    {
        return;
    }
    
    // Attacker isn't a zombie, then stop.
    if (!InfectIsClientInfected(attacker))
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
    InfectHumanToZombie(client, attacker);
}

/**
 * The round is starting.
 */
InfectOnRoundStart()
{
    // Stop infect timers if running.
    ZREndTimer(tInfect);
    ZREndTimer(tInfectCountdown);
    
    // Tell plugin there are no zombies.
    g_bZombieSpawned = false;
}

/**
 * The freeze time is ending.
 */
InfectOnRoundFreezeEnd()
{
    // Stop infect timers if running.
    ZREndTimer(tInfect);
    ZREndTimer(tInfectCountdown);
    
    // If the zombie has spawned already (had to be through admin) then stop.
    if (g_bZombieSpawned)
    {
        return;
    }
    
    // Get min and max times.
    new Float:infectspawntimemin = GetConVarFloat(g_hCvarsList[CVAR_INFECT_SPAWNTIME_MIN]);
    new Float:infectspawntimemax = GetConVarFloat(g_hCvarsList[CVAR_INFECT_SPAWNTIME_MAX]);
    
    // Pick random time between min and max.
    new Float:randomtime = GetRandomFloat(infectspawntimemin, infectspawntimemax);
    
    // Round to the nearest whole number (and convert back to a float) so the countdown is synched with it.
    float(RoundToNearest(randomtime));
    
    tInfect = CreateTimer(randomtime, InfectMotherZombie, _, TIMER_FLAG_NO_MAPCHANGE);
    
    // Check cvar and start a countdown timer if enabled.
    new bool:countdown = GetConVarBool(g_hCvarsList[CVAR_INFECT_MZOMBIE_COUNTDOWN]);
    if (countdown && randomtime > 1.0)
    {
        // Stop old countdown timer, if it exists.
        InfectStopCountdown();
        
        // Store the time until infection, and initialize the counter.
        hInfectCountdownData = CreateDataPack();
        WritePackFloat(hInfectCountdownData, randomtime);
        WritePackFloat(hInfectCountdownData, 0.0);
        tInfectCountdown = CreateTimer(1.0, InfectCountdown, _, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
        
        // Display initial tick.
        InfectCountdown(tInfectCountdown);
    }
}

/**
 * The round is ending.
 */
InfectOnRoundEnd()
{
    // Stop infect timers if running.
    ZREndTimer(tInfect);
    ZREndTimer(tInfectCountdown);
    
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
        // Destroy handle.
        CloseHandle(arrayEligibleClients);
        return;
    }
    
    // Variable to store client stored in random array index.
    new client;
    
    // Prune list of immune clients.
    // x = Array index.
    // client = client index.
    for (new x = 0; x < eligibleclients; x++)
    {
        // Stop pruning if there is only 1 player left.
        if (eligibleclients <= 1)
        {
            break;
        }
        
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
        InfectHumanToZombie(client, _, true);
    }
    else
    {
        // Initialize count variables
        new zombiecount;
        new humancount;
        
        // Count valid human clients
        ZRCountValidClients(zombiecount, humancount, _, true);
        
        // Calculate mother zombie count.
        new mothercount = RoundToNearest(float(humancount) / ratio);
        
        // If mothercount is 0, then set to 1.
        if (!mothercount)
        {
            mothercount = 1;
        }
            
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
            InfectHumanToZombie(client, _, true);
            
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
 * Timer callback, displays countdown to clients.
 * 
 * @param timer     The timer handle.  
 */ 
public Action:InfectCountdown(Handle:timer)
{
    new bool:countdown = GetConVarBool(g_hCvarsList[CVAR_INFECT_MZOMBIE_COUNTDOWN]);
    if (!countdown)
    {
        InfectStopCountdown();
        return Plugin_Stop;
    }
    
    // Read the info from the datapack.
    ResetPack(hInfectCountdownData);
    new Float:length = ReadPackFloat(hInfectCountdownData);
    new Float:counter = ReadPackFloat(hInfectCountdownData);
    
    // Check if the countdown has finished.
    if (counter >= length)
    {
        InfectStopCountdown();
        return Plugin_Stop;
    }
    
    // Print the countdown text to the clients.
    TranslationPrintCenterTextAll(false, "Infect countdown", RoundToNearest(length - counter));
    
    counter++;
    
    // Write the new counter value to the datapack.
    ResetPack(hInfectCountdownData);
    WritePackFloat(hInfectCountdownData, length);
    WritePackFloat(hInfectCountdownData, counter);
    
    return Plugin_Continue;
}

/**
 * Stops the infection countdown timer.
 */
InfectStopCountdown()
{
    // Kill the timer.
    ZREndTimer(tInfectCountdown);
    
    // Destroy data pack.
    if (hInfectCountdownData != INVALID_HANDLE)
    {
        CloseHandle(hInfectCountdownData);
        hInfectCountdownData = INVALID_HANDLE;
    }
}

/**
 * Infects a client. Execute events, sets attributes and flags that indicate
 * that the client is a zombie.
 *
 * @param client            The client to infect.
 * @param attacker          (Optional) The attacker who did the infect.
 * @param motherinfect      (Optional) Indicates a mother zombie infect.
 * @param respawnoverride   (Optional) Set to true to override respawn cvar.
 * @param respawn           (Optional) Value to override with.
 */
InfectHumanToZombie(client, attacker = -1, bool:motherinfect = false, bool:respawnoverride = false, bool:respawn = false)
{
    // Forward pre-event to modules.
    new Action:result = APIOnClientInfect(client, attacker, motherinfect, respawnoverride, respawn);
    
    // Check if infection should be blocked.
    if (result == Plugin_Handled)
    {
        return;
    }
    
    // Mark player as zombie.
    bZombie[client] = true;
    
    // Check if consecutive infection protection is enabled.
    new bool:infectconsecutiveblock = GetConVarBool(g_hCvarsList[CVAR_INFECT_CONSECUTIVE_BLOCK]);
    if (infectconsecutiveblock)
    {
        // If this is a mother infect, flag the player as immune for next mother
        // infection. Otherwise do nothing and keep the current flag.
        if (motherinfect)
        {
            bInfectImmune[client][INFECT_TYPE_MOTHER] = true;
        }
    }
    else
    {
        // Consecutive infection protection is disabled. No immunity.
        bInfectImmune[client][INFECT_TYPE_MOTHER] = false;
    }
    
    // Apply effects.
    InfectFireEffects(client);
    
    // If attacker is valid, then continue.
    if (ZRIsClientValid(attacker))
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
        new score = ToolsClientScore(attacker, true, false);
        ToolsClientScore(attacker, true, true, ++score);
        
        // Add a death to the zombie's score.
        new deaths = ToolsClientScore(client, false, false);
        ToolsClientScore(client, false, true, ++deaths);
        
        // Apply infect HP gain.
        new healthgain = ClassGetHealthInfectGain(attacker);
        new health = GetClientHealth(attacker);
        
        // Set attacker's new health.
        SetEntityHealth(attacker, health + healthgain);
        
        // Forward event to modules.
        ZHPOnHealthInfectGain(attacker);
    }
    
    // Get a list of all client's weapon indexes.
    new weapons[WeaponsSlot];
    WeaponsGetClientWeapons(client, weapons);
    
    // Check if weapons drop is enabled.
    new bool:weaponsdrop = GetConVarBool(g_hCvarsList[CVAR_INFECT_WEAPONS_DROP]);
    
    // This must be after the event forwarding because it fixes a problem caused by changing models in ClassOnClientInfected.
    // Remove all weapons but knife.
    WeaponsRemoveAllClientWeapons(client, weaponsdrop);
    
    // Switch the player to terrorists.
    // TODO: A solution to stop confusing bots? Respawn and teleport?
    CS_SwitchTeam(client, CS_TEAM_T);
    
    // If respawn is enabled, then teleport mother zombie back to spawnpoint.
    if (motherinfect)
    {
        new bool:zombierespawn = GetConVarBool(g_hCvarsList[CVAR_INFECT_MZOMBIE_RESPAWN]);
        if(zombierespawn)
        {
            ZTeleTeleportClient(client);
        }
    }
    // Check override.
    else
    {
        if (respawnoverride && respawn)
        {
            ZTeleTeleportClient(client);
        }
    }
    
    // Print message to client.
    TranslationPrintToChat(client, "Infect infected");
    
    // Forward event to modules.
    ClassOnClientInfected(client, motherinfect);
    RoundEndOnClientInfected();
    DamageOnClientInfected(client, motherinfect);
    SEffectsOnClientInfected(client);
    ZTeleOnClientInfected(client);
    ZHPOnClientInfected(client);
    APIOnClientInfected(client, attacker, motherinfect, respawnoverride, respawn);
}

/**
 * Turns a zombie back into a human. Execute events, sets attributes and flags that indicate
 * that the client is a human.
 *
 * @param client        The client to make human.
 * @param respawn       Teleport client back to spawn if true.
 * @param protect       Start spawn protection on new human.
 */
InfectZombieToHuman(client, bool:respawn = false, bool:protect = false)
{
    // Forward pre-event to modules.
    new Action:result = APIOnClientHuman(client, respawn, protect);
    
    // Check if action should be blocked.
    if (result == Plugin_Handled)
    {
        return;
    }
    
    // Mark player as human.
    bZombie[client] = false;
    
    // Switch the player to counter-terrorists.
    CS_SwitchTeam(client, CS_TEAM_CT);
    
    // Set client as translation target.
    SetGlobalTransTarget(client);
    
    // Print message to client.
    TranslationPrintToChat(client, "Infect human");
    
    // Forward event to modules.
    ClassReloadPlayer(client);
    RoundEndOnClientInfected();
    ZTeleOnClientInfected(client);
    
    // Give human a new knife. (If you leave the old one there will be glitches with the knife positioning)
    new knife = GetPlayerWeaponSlot(client, _:Slot_Melee);
    if (knife != -1)
    {
        RemovePlayerItem(client, knife);
        AcceptEntityInput(knife, "Kill");
        GivePlayerItem(client, "weapon_knife");
    }
    
    // Check if we should respawn the client.
    if (respawn)
    {
        ZTeleTeleportClient(client);
    }
    
    // Check if we should spawn protect the client.
    if (protect)
    {
        SpawnProtectStart(client);
    }
    
    // Forward event to modules.
    SEffectsOnClientHuman(client);
    APIOnClientHumanPost(client, respawn, protect);
}

/**
 * Creates effects on a newly infected client.
 * 
 * @param client    The client index.
 */
InfectFireEffects(client)
{
    // Initialize vector variables.
    new Float:clientloc[3];
    new Float:direction[3] = {0.0, 0.0, 0.0};
    
    // Get client's position.
    GetClientAbsOrigin(client, clientloc);
    clientloc[2] += 30;
    
    new bool:explosion = GetConVarBool(g_hCvarsList[CVAR_INFECT_EXPLOSION]);
    if (explosion)
    {
        // Initialize explosion flags variable.
        new flags;
        
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
        
        // Create explosion at client's origin.
        VEffectsCreateExplosion(clientloc, flags);
    }
    
    // Emit scream sound if enabled.
    ZombieSoundsScream(client);
    
    // If energy splash effect is enabled, then continue.
    new bool:esplash = GetConVarBool(g_hCvarsList[CVAR_INFECT_ESPLASH]);
    if (esplash)
    {
        // Create energy splash effect.
        VEffectsCreateEnergySplash(clientloc, direction, true);
    }
    
    // If shake effect is enabled, then continue.
    new bool:shake = GetConVarBool(g_hCvarsList[CVAR_INFECT_SHAKE]);
    if (shake)
    {
        // Get shake info.
        new Float:shakeamp = GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_AMP]);
        new Float:shakefrequency = GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_FREQUENCY]);
        new Float:shakeduration = GetConVarFloat(g_hCvarsList[CVAR_INFECT_SHAKE_DURATION]);
        
        // Shake client's screen.
        VEffectsShakeClientScreen(client, shakeamp, shakefrequency, shakeduration);
    }
}

/**
 * Sends list of clients to infect/human.
 *  
 * @param client    The client index.
 */
InfectMenuClients(client)
{
    // Create menu handle.
    new Handle:menu_infect_clients = CreateMenu(InfectMenuClientsHandle);
    
    // Set client as translation target.
    SetGlobalTransTarget(client);
    
    decl String:title[MENU_LINE_TITLE_LENGTH];
    decl String:clientoption[MENU_LINE_REG_LENGTH];
    decl String:clientuserid[8];
    
    // x = Client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client isn't alive, then stop.
        if (!IsPlayerAlive(x))
        {
            continue;
        }
        
        // Get client info.
        GetClientName(x, clientoption, sizeof(clientoption));
        IntToString(GetClientUserId(x), clientuserid, sizeof(clientuserid));
        
        // Append client's current team to the option.
        if (InfectIsClientInfected(x))
        {
            Format(clientoption, sizeof(clientoption), "%s [%t]", clientoption, "Zombie");
        }
        else
        {
            Format(clientoption, sizeof(clientoption), "%s [%t]", clientoption, "Human");
        }
        
        // Add option to menu.
        AddMenuItem(menu_infect_clients, clientuserid, clientoption);
    }
    
    Format(title, sizeof(title), "%t\n ", "Infect menu clients title");
    SetMenuTitle(menu_infect_clients, title);
    
    // Create a "Back" button to the main admin menu.
    SetMenuExitBackButton(menu_infect_clients, true);
    
    // Send menu.
    DisplayMenu(menu_infect_clients, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the infect clients menu, and handles it.
 * @param menu_infect_clients   Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public InfectMenuClientsHandle(Handle:menu_infect_clients, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get selected client index.
        new target = MenuGetClientIndex(menu_infect_clients, slot);
        
        // If target has left the server, then stop.
        if (!target)
        {
            // Re-send menu.
            InfectMenuClients(client);
            return;
        }
        
        // Create an array with a single slot and set target to it.
        new targets[1];
        targets[0] = target;
        
        // Toggle infect on the client.
        if (InfectIsClientInfected(target))
        {
            InfectManualHuman(client, targets, 1);
        }
        else
        {
            InfectManualInfect(client, targets, 1);
        }
        
        // Re-send menu.
        InfectMenuClients(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            // Re-open admin menu.
            ZAdminMenu(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_infect_clients);
    }
}

/**
 * Returns if a client is infected.
 * 
 * @param client    The client index.
 * @return          True if the client has been infected, false otherwise.
 */    
bool:InfectIsClientInfected(client)
{
    // If client is invalid, then stop.
    if (!ZRIsClientValid(client))
    {
        return false;
    }
    
    // Return client's zombie flag.
    return bZombie[client];
}

/**
 * Returns if a client is a human.
 * 
 * @param client    The client index.
 * @return          True if the client is a human, false otherwise.
 */
bool:InfectIsClientHuman(client)
{
    // If client is invalid, then stop.
    if (!ZRIsClientValid(client))
    {
        return true;
    }
    
    // Return opposite of client's zombie flag.
    return !bZombie[client];
}

/**
 * Infecting a client manually (via zr_infect or the "Zombie Management" menu)
 * 
 * @param client            The client index infecting another client.
 * @param targets           Array containing all clients to infect.
 * @param count             The number of clients in the array.
 * @param respawnoverride   (Optional) True to override respawn cvar.
 * @param respawn           (Optional) True to respawn client on infect.
 */
stock InfectManualInfect(client, targets[], count, bool:respawnoverride = false, bool:respawn = false)
{
    new bool:zombiespawned = g_bZombieSpawned;
    
    // If zombie hasn't spawned, then make targetted player(s) mother zombies.
    if (!zombiespawned)
    {
        // Stop mother infect timer.
        if (tInfect != INVALID_HANDLE)
        {
            KillTimer(tInfect);
            tInfect = INVALID_HANDLE;
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
        
        // Tell the plugin a mother zombie has spawned.
        g_bZombieSpawned = true;
    }
    
    decl String:targetname[MAX_NAME_LENGTH];
    
    // x = Client index.
    for (new x = 0; x < count; x++)
    {
        // Get client's name for later use.
        GetClientName(targets[x], targetname, sizeof(targetname));
        
        // Check if client is a human before turning into zombie.
        if (!InfectIsClientHuman(targets[x]))
        {
            // If there was only 1 player targetted, then let admin know the command was unsuccessful.
            if (count == 1)
            {
                // Tell admin command was unsuccessful.
                TranslationReplyToCommand(client, "Infect command infect unsuccessful", targetname);
            }
            
            continue;
        }
        
        // If zombie hasn't spawned, then make targetted player(s) mother zombies.
        if (!zombiespawned)
        {
            // Turn client into a mother zombie.
            InfectHumanToZombie(targets[x], _, true, respawnoverride, respawn);
            
            // If there was only 1 player targetted, then let admin know the outcome of the command.
            if (count == 1)
            {
                TranslationReplyToCommand(client, "Infect command infect mother successful", targetname);
            }
            
            continue;
        }
        
        // Turn client into a zombie.
        InfectHumanToZombie(targets[x], _, false, respawnoverride, respawn);
        
        // If there was only 1 player targetted, then let admin know the outcome of the command.
        if (count == 1)
        {
            TranslationReplyToCommand(client, "Infect command infect successful", targetname);
        }
    }
}

/**
 * Infecting a client manually (via zr_human or the "Zombie Management" menu)
 * 
 * @param client    The client index changing a zombie to human.
 * @param targets   Array containing all clients to make human.
 * @param count     The number of clients in the array.
 * @param respawn   (Optional) True to respawn client upon changing to human.
 * @param protect   (Optional) True to protect client upon changing to human.
 */
stock InfectManualHuman(client, targets[], count, bool:respawn = false, bool:protect = false)
{
    decl String:targetname[MAX_NAME_LENGTH];
    
    // x = Client index.
    for (new x = 0; x < count; x++)
    {
        // Get client's name for later use.
        GetClientName(targets[x], targetname, sizeof(targetname));
        
        // Check if client is a human before turning into zombie.
        if (InfectIsClientInfected(targets[x]))
        {
            // Turn client into a zombie.
            InfectZombieToHuman(targets[x], respawn, protect);
            
            // If there was only 1 player targetted, then let admin know the outcome of the command.
            if (count == 1)
            {
                // Tell admin command was successful.
                TranslationReplyToCommand(client, "Infect command human successful", targetname);
            }
        }
        else
        {
            // If there was only 1 player targetted, then let admin know the command was unsuccessful.
            if (count == 1)
            {
                // Tell admin command was unsuccessful.
                TranslationReplyToCommand(client, "Infect command human unsuccessful", targetname);
            }
        }
    }
}

/**
 * Command callback (zr_infect)
 * Infects a client.
 *   
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:InfectInfectCommand(client, argc)
{
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Generic))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }
    
    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "Infect command infect syntax");
        return Plugin_Handled;
    }
    
    decl String:target[MAX_NAME_LENGTH], String:targetname[MAX_NAME_LENGTH];
    new targets[MAXPLAYERS], bool:tn_is_ml, result;
    
    // Get targetname.
    GetCmdArg(1, target, sizeof(target));
    
    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE , targetname, sizeof(targetname), tn_is_ml);
        
    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        ZRReplyToTargetError(client, result);
        return Plugin_Handled;
    }
    
    // Get respawn parameter.
    decl String:strRespawn[8];
    GetCmdArg(2, strRespawn, sizeof(strRespawn));
    
    new bool:respawnoverride, bool:respawn;
        
    // If parameter exists then cast it into a bool and feed it to infect function.
    if (strRespawn[0])
    {
        respawnoverride = true;
        respawn = bool:StringToInt(strRespawn);
    }
    
    // Infect player.
    InfectManualInfect(client, targets, result, respawnoverride, respawn);
    
    return Plugin_Handled;
}

/**
 * Command callback (zr_human)
 * Turns a client into a human.
 *   
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:InfectHumanCommand(client, argc)
{
    // Check if privileged.
    if (!ZRIsClientPrivileged(client, OperationType_Generic))
    {
        TranslationReplyToCommand(client, "No access to command");
        return Plugin_Handled;
    }
    
    // If not enough arguments given, then stop.
    if (argc < 1)
    {
        TranslationReplyToCommand(client, "Infect command human syntax");
        return Plugin_Handled;
    }
    
    decl String:target[MAX_NAME_LENGTH], String:targetname[MAX_NAME_LENGTH];
    new targets[MAXPLAYERS], bool:tn_is_ml, result;
    
    // Get targetname.
    GetCmdArg(1, target, sizeof(target));
    
    // Find a target.
    result = ProcessTargetString(target, client, targets, sizeof(targets), COMMAND_FILTER_ALIVE , targetname, sizeof(targetname), tn_is_ml);
        
    // Check if there was a problem finding a client.
    if (result <= 0)
    {
        ZRReplyToTargetError(client, result);
        return Plugin_Handled;
    }
    
    // Get respawn&protect parameters
    decl String:strRespawn[8], String:strProtect[8];
    GetCmdArg(2, strRespawn, sizeof(strRespawn));
    GetCmdArg(3, strProtect, sizeof(strProtect));
    
    // If parameter exists then cast it into a bool and feed it to "humanize" function.
    new bool:respawn = (strRespawn[0]) ? (bool:StringToInt(strRespawn)) : false;
    new bool:protect = (strProtect[0]) ? (bool:StringToInt(strProtect)) : false;
    
    // Turn client into human.
    InfectManualHuman(client, targets, result, respawn, protect);
    
    return Plugin_Handled;
}
