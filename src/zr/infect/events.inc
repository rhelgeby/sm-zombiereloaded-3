/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          events.inc
 *  Type:          Module component
 *  Description:   Event handlers for the infection module.
 *
 *  Copyright (C) 2009-2015  Greyscale, Richard Helgeby
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
    if (!InfectHasZombieSpawned())
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
    if (ZRIsClientOnTeam(client, CS_TEAM_T) && InfectHasZombieSpawned())
    {
        CS_SwitchTeam(client, CS_TEAM_CT);
        CS_RespawnPlayer(client);
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

/**
 * Client has been hurt.
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
    
    // Check if the immunity module is handling the infection.
    if (ImmunityOnClientInfect(client, attacker))
    {
        //PrintToChatAll("InfectOnClientHurt - Infect blocked.");
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
    if (InfectHasZombieSpawned())
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
        // Disable zombie flag on client.
        bZombie[x] = false;
    }
}
