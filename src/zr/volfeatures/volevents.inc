/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          volevents.inc
 *   Type:          Module
 *   Description:   Handles volumetric feature events.
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
    // TODO: Check if volfeatures is enabled first.

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
    // TODO: Check if volfeatures is enabled first.
    
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
    VolUpdatePlayerLocation(client);
}

/**
 * Called when the round starts. Main enable event for volumetric features.
 */
VolOnRoundStart()
{
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
