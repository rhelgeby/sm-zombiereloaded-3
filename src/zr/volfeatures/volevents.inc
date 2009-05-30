/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volevents.inc
 *  Type:           Module
 *  Description:    Handles generic events for volumetric features.
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
 * Called when a player enters a volume.
 *
 * @param client        The client index.
 * @param volumeIndex   The volume index.
 */
VolOnPlayerEnter(client, volumeIndex)
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }

    // Forward event to features.
    // VolAnticampStart(client, volume);
}

/**
 * Called when a player leaves a volume.
 *
 * @param client        The client index.
 * @param volumeIndex   The volume index.
 */
VolOnPlayerLeave(client, volumeIndex)
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }
    
    // Forward event to features.
    // VolAnticampStop(client, volume);
}

/**
 * Called when a player spawned. Used for initializing player data.
 *
 * @param client    The client index.
 */
VolOnPlayerSpawn(client)
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }
    
    // Cache player location.
    VolUpdatePlayerLocation(client);
}

/**
 * Called when a player disconnects.
 *
 * @param client    The client index.
 */
VolOnPlayerDisconnect(client)
{
    // Disable trigger delay counters.
    VolResetCountDown(client);
}

/**
 * Called when the round starts. Main enable event for volumetric features.
 */
VolOnRoundStart()
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return;
    }
    
    // Start main timer.
    VolStartUpdateTimer();
}

/**
 * Called when the round ends. Main disable event for volumetric features.
 */
VolOnRoundEnd()
{
    // Stop main timer.
    VolStopUpdateTimer();
    
    // Forward stop event to features.
    // VolAnticampStop();
}

/**
 * Called when a volume is disabled.
 * @param volumeIndex   The volume index.
 */
VolOnVolumeDisabled(volumeIndex)
{
    // Forward stop event to features.
    
}

/**
 * Called when a volume is enabled.
 * @param volumeIndex   The volume index.
 */
VolOnVolumeEnabled(volumeIndex)
{
    // Forward start event to features.
    
}
