/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          zspawn.inc
 *  Type:          Module
 *  Description:   Handles zspawn command, spawns late-joining clients into the game.
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
 * Global variable to store infect timer handle.
 */
new Handle:tZSpawn = INVALID_HANDLE;

/**
 * Create commands specific to ZSpawn.
 */
ZSpawnOnCommandsCreate()
{
    // Register ZSpawn command.
    RegConsoleCmd(SAYHOOKS_KEYWORD_ZSPAWN, ZSpawnCommand, "Spawn into the game after joining late.");
}

/**
 * Map is starting.
 */
ZSpawnOnMapStart()
{
    // Reset timer handle.
    tZSpawn = INVALID_HANDLE;
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
ZSpawnOnClientDisconnect(client)
{
    // Check if client is a bot.
    if (IsFakeClient(client))
    {
        return;
    }
    
    // Add client serial to global array.
    SerialAddClient(client);
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
ZSpawnOnClientDeath(client)
{
    // Add client serial to global array.
    SerialAddClient(client);
}

/**
 * The round is starting.
 */
ZSpawnOnRoundStart()
{
    // Reset serial number array.
    SerialReset();
    
    // If zspawn timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);
        
        // Reset timer handle.
        tZSpawn = INVALID_HANDLE;
    }
}

/**
 * The freeze time is ending.
 */
ZSpawnOnRoundFreezeEnd()
{
    // If infect timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);
    }
    
    // If zspawn is disabled, then stop.
    new bool:zspawn = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN]);
    if (!zspawn)
    {
        return;
    }
    
    // If timelimit is disabled, then stop.
    new bool:zspawntimelimit = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT]);
    if (!zspawntimelimit)
    {
        return;
    }
    
    // Get timelimit
    new Float:zspawntime = GetConVarFloat(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT_TIME]);
    
    // Start timer.
    tZSpawn = CreateTimer(zspawntime, ZSpawnTimer, _, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 */
ZSpawnOnRoundEnd()
{
    // If zspawn timer is running, then kill it.
    if (tZSpawn != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tZSpawn);
        
        // Reset timer handle.
        tZSpawn = INVALID_HANDLE;
    }
}

/**
 * Spawns a late-joining client into the game.
 * 
 * @param client    The client index.
 * @return          True if successful, false otherwise.
 */
bool:ZSpawnClient(client)
{
    // If zspawn is disabled, then stop.
    new bool:zspawn = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN]);
    if (!zspawn)
    {
        TranslationPrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // If client isn't on a team, then stop.
    if (!ZRIsClientOnTeam(client))
    {
        // Tell client the command may only be used when on a team.
        TranslationPrintToChat(client, "Must be on team");
        return false;
    }
    
    // If client is alive, then stop.
    if (IsPlayerAlive(client))
    {
        // Tell client the command may only be used when dead.
        TranslationPrintToChat(client, "Must be dead");
        return false;
    }
    
    // Block if client has already played during this round.
    if (SerialClientExists(client))
    {
        // Tell client the command may only be used when joining late.
        TranslationPrintToChat(client, "ZSpawn double spawn");
        return false;
    }
    
    // Check if zspawn override is enabled, and if so get overidden value.
    new bool:teamoverride = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN_TEAM_OVERRIDE]);
    new bool:teamzombie = teamoverride ? GetConVarBool(g_hCvarsList[CVAR_ZSPAWN_TEAM_ZOMBIE]) : GetConVarBool(g_hCvarsList[CVAR_RESPAWN_TEAM_ZOMBIE]);
    
    // Block is the time limit is up.
    new bool:zspawntimelimit = GetConVarBool(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT]);
    if (zspawntimelimit)
    {
        if (tZSpawn == INVALID_HANDLE)
        {
            new zspawntimelimitzombie = GetConVarInt(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT_ZOMBIE]);
            switch(zspawntimelimitzombie)
            {
                case -1:
                {
                    // Get timelimit
                    new Float:zspawntime = GetConVarFloat(g_hCvarsList[CVAR_ZSPAWN_TIMELIMIT_TIME]);
                    
                    // Tell client the timelimit for this command has expired.
                    TranslationPrintToChat(client, "ZSpawn timelimit", RoundToNearest(zspawntime));
                    return false;
                }
                case 0:
                {
                    teamzombie = false;
                }
                case 1:
                {
                    teamzombie = true;
                }
            }
        }
    }
    
    // Tell respawn module to respawn client.
    RespawnSpawnClient(client, teamzombie);
    
    return true;
}

/**
 * Command callback (zspawn)
 * Spawn into the game after joining late.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ZSpawnCommand(client, argc)
{
    // If client is console, then stop and tell them this feature is for players only.
    if (ZRIsConsole(client))
    {
        TranslationPrintToServer("Must be player");
        return Plugin_Handled;
    }
    
    // Spawn client.
    ZSpawnClient(client);
    
    // This stops the "Unknown command" message in client's console.
    return Plugin_Handled;
}

/**
 * Timer callback, resets handle.
 * 
 * @param timer     The timer handle.
 */
public Action:ZSpawnTimer(Handle:timer)
{
    // Reset timer handle.
    tZSpawn = INVALID_HANDLE;
}
