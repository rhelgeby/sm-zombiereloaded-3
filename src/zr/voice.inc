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