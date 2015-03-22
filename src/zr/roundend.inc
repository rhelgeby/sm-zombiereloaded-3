/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          roundend.inc
 *  Type:          Core
 *  Description:   Handles round end actions.
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
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
 * @section All round end reasons.
 */
#define ROUNDEND_TARGET_BOMBED                          0        // Target Successfully Bombed!
#define ROUNDEND_VIP_ESCAPED                            1        // The VIP has escaped!
#define ROUNDEND_VIP_ASSASSINATED                       2        // VIP has been assassinated!
#define ROUNDEND_TERRORISTS_ESCAPED                     3        // The terrorists have escaped!
#define ROUNDEND_CTS_PREVENTESCAPE                      4        // The CT's have prevented most of the terrorists from escaping!
#define ROUNDEND_ESCAPING_TERRORISTS_NEUTRALIZED        5        // Escaping terrorists have all been neutralized!
#define ROUNDEND_BOMB_DEFUSED                           6        // The bomb has been defused!
#define ROUNDEND_CTS_WIN                                7        // Counter-Terrorists Win!
#define ROUNDEND_TERRORISTS_WIN                         8        // Terrorists Win!
#define ROUNDEND_ROUND_DRAW                             9        // Round Draw!
#define ROUNDEND_ALL_HOSTAGES_RESCUED                   10       // All Hostages have been rescued!
#define ROUNDEND_TARGET_SAVED                           11       // Target has been saved!
#define ROUNDEND_HOSTAGES_NOT_RESCUED                   12       // Hostages have not been rescued!
#define ROUNDEND_TERRORISTS_NOT_ESCAPED                 13       // Terrorists have not escaped!
#define ROUNDEND_VIP_NOT_ESCAPED                        14       // VIP has not escaped!
#define ROUNDEND_GAME_COMMENCING                        15       // Game Commencing!
/**
 * @endsection
 */

/**
 * Delay between round ending and new round starting. (Normal)
 */
#define ROUNDEND_DELAY 5.0
 
/**
 * Possible round end outcomes.
 */
enum RoundEndOutcome
{
    Restart,        /** Round is restarting. */
    Draw,           /** Round has ended in unexpected way. */
    HumansWin,      /** Humans have killed all zombies. */
    ZombiesWin,     /** Zombies have infected all humans. */
}

/**
 * Global variable to store round win timer handle.
 */
new Handle:tRoundEnd = INVALID_HANDLE;

/**
 * Map is starting.
 */ 
RoundEndOnMapStart()
{
    // Reset timer handle.
    tRoundEnd = INVALID_HANDLE;
}

/**
 * Client has been killed.
 */
RoundEndOnClientDeath()
{
    // Terminate the round if the last player was killed.
    new RoundEndOutcome:outcome;
    if (RoundEndGetRoundStatus(outcome))
    {
        RoundEndTerminateRound(ROUNDEND_DELAY, outcome);
    }
}

/**
 * Client has been infected.
 */
RoundEndOnClientInfected()
{
    // Terminate the round if the last player was infected.
    new RoundEndOutcome:outcome;
    if (RoundEndGetRoundStatus(outcome))
    {
        RoundEndTerminateRound(ROUNDEND_DELAY, outcome);
    }
}

/**
 * The round is starting.
 */
RoundEndOnRoundStart()
{
    // Stop all overlays.
    RoundEndOverlayStop();
    
    // If round end timer is running, then kill it.
    if (tRoundEnd != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tRoundEnd);
        
        // Reset timer handle.
        tRoundEnd = INVALID_HANDLE;
    }
}

/**
 * The freeze time is ending.
 */
RoundEndOnRoundFreezeEnd()
{
    // Calculate round length, in seconds.
    // Get mp_roundtime. (in minutes)
    new Float:roundtime = GetConVarFloat(FindConVar("mp_roundtime"));
    
    // Convert to seconds.
    roundtime *= 60.0;
    
    // Subtract one second if running CS: GO to prevent round draw when round
    // ends. For some reason the timing doesn't match the actual round end.
    // Thanks to Jargon.
    if (g_Game == Game_CSGO)
    {
        roundtime--;
    }
    
    // Start timer.
    tRoundEnd = CreateTimer(roundtime, RoundEndTimer, _, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * The round is ending.
 * 
 * @param reason    Reason the round has ended.
 */
RoundEndOnRoundEnd(reason)
{
    // If round end timer is running, then kill it.
    if (tRoundEnd != INVALID_HANDLE)
    {
        // Kill timer.
        KillTimer(tRoundEnd);
        
        // Reset timer handle.
        tRoundEnd = INVALID_HANDLE;
    }
    
    // Tell plugin no zombies have been spawned.
    g_bZombieSpawned = false;
    
    // Get outcome of the round.
    new RoundEndOutcome:outcome = RoundEndReasonToOutcome(reason);
    
    // Update team scores.
    new teamscore;
    switch(outcome)
    {
        // Zombies won the round.
        case ZombiesWin:
        {
            // Increment T score.
            teamscore = GetTeamScore(CS_TEAM_T);
            SetTeamScore(CS_TEAM_T, ++teamscore);
        }
        // Humans won the round.
        case HumansWin:
        {
            // Increment CT score.
            teamscore = GetTeamScore(CS_TEAM_CT);
            SetTeamScore(CS_TEAM_CT, ++teamscore);
        }
    }
    
    // Display the overlay to all clients.
    RoundEndOverlayStart(outcome);
    
    // Balance teams if enabled.
    if (GetConVarBool(g_hCvarsList[CVAR_ROUNDEND_BALANCE_TEAMS]))
    {
        RoundEndBalanceTeams();
    }
}

/**
 * Convert a round_end reason, to a round winner, or draw.
 * 
 * @param reason    The round_end reason.
 * @return          The winner of the round. (see enum RoundEndOutcome)
 */ 
RoundEndOutcome:RoundEndReasonToOutcome(reason)
{
    switch(reason)
    {
        // CTs won the round.
        case ROUNDEND_CTS_WIN:
        {
            return HumansWin;
        }
        // Ts won the round.
        case ROUNDEND_TERRORISTS_WIN:
        {
            return ZombiesWin;
        }
        // Unexpected case.
        default:
        {
            return Draw;
        }
    }
    
    // Return draw to satisfy compiler. (code will never reach this point.)
    return Draw;
}

/**
 * Timer callback, called when round time reaches 0.
 *
 * @param timer     The timer handle.
 */
public Action:RoundEndTimer(Handle:timer)
{
    // Set the global timer handle variable to INVALID_HANDLE.
    tRoundEnd = INVALID_HANDLE;
    
    // If there aren't clients on both teams, then stop.
    if (!ZRTeamHasClients())
    {
        return;
    }
    
    // Terminate the round with humans as the winner.
    RoundEndTerminateRound(ROUNDEND_DELAY, HumansWin);
}

/**
 * Checks if the round is over.
 * 
 * @param outcome   Set to the outcome of the round, if round is over.
 * @return          True if the round is over, false otherwise.
 */
bool:RoundEndGetRoundStatus(&RoundEndOutcome:outcome)
{
    // If zombie hasn't spawned, then stop.
    if (!InfectHasZombieSpawned())
    {
        // Round isn't over.
        return false;
    }
    
    // Initialize count variables
    new zombiecount;
    new humancount;
    
    // Count valid clients. (true to only allow living clients)
    ZRCountValidClients(zombiecount, humancount, true);
    
    // If there are no clients on either teams, then stop.
    if (!zombiecount && !humancount)
    {
        // Round isn't active.
        return false;
    }
    
    // If there are clients on both teams, then stop.
    if (zombiecount && humancount)
    {
        // Round isn't over.
        return false;
    }
    
    // We know here, that either zombiecount or humancount is 0. (not both)
    
    // If there are zombies, then zombies won the round.
    if (zombiecount)
    {
        outcome = ZombiesWin;
    }
    // If there are no zombies, that means there must be humans, they win the round.
    else
    {
        outcome = HumansWin;
    }
    
    // Round is over.
    return true;
}

/**
 * Ends the round with the given outcome and delay.
 * 
 * @param delay     Delay before new round starts. 
 * @param outcome   The outcome of the round.
 */
RoundEndTerminateRound(Float:delay, RoundEndOutcome:outcome = Restart)
{
    switch(outcome)
    {
        // Round is restarting.
        case Restart:
        {
            CS_TerminateRound(delay, CSRoundEnd_GameStart, false);
        }
        // Round was a draw.
        case Draw:
        {
            CS_TerminateRound(delay, CSRoundEnd_Draw, false);
        }
        // Zombies won.
        case ZombiesWin:
        {
            CS_TerminateRound(delay, CSRoundEnd_TerroristWin, false);
        }
        // Humans won.
        case HumansWin:
        {
            CS_TerminateRound(delay, CSRoundEnd_CTWin, false);
        }
    }
}

/**
 * Balances teams.
 */
RoundEndBalanceTeams()
{
    // Create eligible player list.
    new Handle:arrayEligibleClients = INVALID_HANDLE;
    new eligibleclients = ZRCreateEligibleClientList(arrayEligibleClients, true);
    
    // If there are no eligible client's then stop.
    if (!eligibleclients)
    {
        // Destroy handle.
        CloseHandle(arrayEligibleClients);
        
        return;
    }
    
    new client;
    
    // Move all clients to T
    
    // x = Array index.
    // client = client index.
    for (new x = 0; x < eligibleclients; x++)
    {
        // Get client stored in array index.
        client = GetArrayCell(arrayEligibleClients, x);
        
        // Switch client to T
        CS_SwitchTeam(client, CS_TEAM_T);
    }
    
    // Move every other client back to CT
    
    // x = array index
    // client = client index.
    for (new x = 0; x < eligibleclients; x += 2)
    {
        // Get client stored in array index.
        client = GetArrayCell(arrayEligibleClients, x);
        
        // Switch client to CT
        CS_SwitchTeam(client, CS_TEAM_CT);
    }
    
    // Destroy handle.
    CloseHandle(arrayEligibleClients);
}

/**
 * Displays overlays to clients, depending on the outcome.
 * 
 * @param time      Time to display overlays.
 * @param outcome   The outcome of the round.
 */
RoundEndOverlayStart(RoundEndOutcome:outcome)
{
    // If round end overlays are disabled, then stop.
    new bool:overlay = GetConVarBool(g_hCvarsList[CVAR_ROUNDEND_OVERLAY]);
    if (!overlay)
    {
        return;
    }
    
    decl String:overlaypath[PLATFORM_MAX_PATH];
    
    switch(outcome)
    {
        // Show "zombies win" overlay.
        case ZombiesWin:
        {
            GetConVarString(g_hCvarsList[CVAR_ROUNDEND_OVERLAY_ZOMBIE], overlaypath, sizeof(overlaypath));
        }
        // Show "humans win" overlay.
        case HumansWin:
        {
            GetConVarString(g_hCvarsList[CVAR_ROUNDEND_OVERLAY_HUMAN], overlaypath, sizeof(overlaypath));
        }
        // Show no overlay.
        default:
        {
            strcopy(overlaypath, sizeof(overlaypath), "");
        }
    }
    
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client is fake (or bot), then stop.
        if (IsFakeClient(x))
        {
            continue;
        }
        
        OverlaysClientSetChannelPath(x, OVERLAYS_CHANNEL_ROUNDEND, overlaypath);
        OverlaysClientSetChannelState(x, OVERLAYS_CHANNEL_ROUNDEND, true, false, true);
    }
}

RoundEndOverlayStop()
{
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client is fake (or bot), then stop.
        if (IsFakeClient(x))
        {
            continue;
        }
        
        // Disable roundend overlay channel.
        OverlaysClientSetChannelState(x, OVERLAYS_CHANNEL_ROUNDEND, true, false, false, true);
    }
}