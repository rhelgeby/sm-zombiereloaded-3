/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          voice.inc
 *  Type:          Module
 *  Description:   Alter listening/speaking states of humans/zombies.
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
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
VoiceOnClientSpawn(client)
{
    // Give client proper verbal permissions.
    VoiceUpdateClient(client);
}

/**
 * Client has been infected.
 * 
 * @param client    The client index.
 */
VoiceOnClientInfected(client)
{
    // Give client proper verbal permissions.
    VoiceUpdateClient(client);
}

/**
 * Client has been turned back human.
 * 
 * @param client    The client index.
 */
VoiceOnClientHuman(client)
{
    // Give client proper verbal permissions.
    VoiceUpdateClient(client);
}

/**
 * Allow all clients to listen and speak with each other.
 */
stock VoiceAllTalk()
{
    // x = Receiver index.
    // y = Sender index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If receiver isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        for (new y = 1; y <= MaxClients; y++)
        {
            // If sender isn't in-game, then stop.
            if (!IsClientInGame(y))
            {
                continue;
            }
            
            // No need to alter listening/speaking flags between one client.
            if (x == y)
            {
                continue;
            }
            
            // Receiver (x) can now hear the sender (y).
            SetClientListening(x, y, true);
        }
    }
}

/**
 * Set which team the client is allowed to listen/speak with.
 * 
 * @param client    The client index.
 * @param zombie    True to permit verbal communication to zombies only, false for humans only.
 */
stock VoiceSetClientTeam(client, bool:zombie)
{
    // x = Client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If sender isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // Client can only listen/speak if the sender is on their team.
        new bool:canlisten = (zombie == InfectIsClientInfected(x));
        
        // (Dis)allow clients to listen/speak with each other.
        SetClientListening(client, x, canlisten);
        SetClientListening(x, client, canlisten);
    }
}

/**
 * Update a client's listening/speaking status.
 * 
 * @param client    The client index.
 */
stock VoiceUpdateClient(client)
{
    // Set the client's listening/speaking status to their current team.
    VoiceSetClientTeam(client, InfectIsClientInfected(client));
}