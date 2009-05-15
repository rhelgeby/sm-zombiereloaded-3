/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          volfeatures.inc
 *   Type:          Module
 *   Description:   Provides functions for managing volumetric features.
 *
 * ============================================================================
 */

/**
 * Total volumes that can be created in a map.
 */
#define ZR_VOLUMES_MAX 64

/**
 * Represent a rectangular volume.
 */
enum VolumeAttributes
{
    /* General */
    bool:vol_enabled,               /** Volume state. */
    bool:vol_in_use,                /** Marks if the volume is used. */
    
    /* Location */
    Float:vol_x_min,                /** Minimum x position. */
    Float:vol_x_max,                /** Maximum x position. */
    
    Float:vol_y_min,                /** Minimum y position. */
    Float:vol_y_max,                /** Maximum y position. */
    
    Float:vol_z_min,                /** Minimum z position. */
    Float:vol_z_max,                /** Maximum z position. */
    
    /* Data */
    VolumeEffects:vol_effect,       /** Visual effect to apply on the volume. */
    VolumeFeatureTypes:vol_type,    /** The volumetric feature type. */
    vol_data_index                  /** Index in remote feature array. */
}

/**
 * Available volumetric feature types.
 */
enum VolumeFeatureTypes
{
    VolFeature_Anticamp = 0,
    VolFeature_Knockback
}

enum VolumeEffects
{
    VolEffect_None = 0,
    VolEffect_Wireframe,
    VolEffect_Smoke
}

/**
 * Volumes.
 */
new Volumes[ZR_VOLUMES_MAX][VolumeAttributes];

/**
 * Total number of volumes.
 */
new VolumeCount;

/**
 * List of player locations. Updated by a timer.
 */
new Float:VolPlayerLoc[MAXPLAYERS + 1][3];

/**
 * Specifies whether the volumetric features module is enabled or not. To be
 * synced with zr_volfeatures_enabled CVAR.
 */
new bool:VolEnabled;

/**
 * Timer handle for timer that updates player locations. This is the main timer
 * and any feature events can't be updated faster than this interval.
 *
 * Note: Some features may have its own timer.
 */
new Handle:hVolUpdateTimer;

#include "zr/volfeatures/volevents"
#include "zr/volfeatures/volanticamp"

/**
 * Starts the update timer.
 */
bool:VolStartUpdateTimer()
{
    // TODO: Read from CVAR (zr_volfeatures_enabled).
    VolEnabled = true;
    
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return false;
    }
    
    // TODO: Read from CVAR (zr_volfeatures_interval).
    new Float:interval = 1.0;
    
    // Validate interval.
    if (interval > 0.0)
    {
        // Stop timer if it exist.
        VolStopUpdateTimer();
        
        // Create a new timer.
        hVolUpdateTimer = CreateTimer(interval, Event_VolUpdateTimer, _, TIMER_REPEAT);
        
        // Volumetric features started.
        return true;
    }
    else
    {
        // Volumetric features disabled. Do explicit stop.
        VolStopUpdateTimer();
        
        return false;
    }
}

/**
 * Kills the update timer if it exists.
 */
VolStopUpdateTimer()
{
    // Kill the timer if it's running.
    if (hVolUpdateTimer != INVALID_HANDLE)
    {
        KillTimer(hVolUpdateTimer);
        hVolUpdateTimer = INVALID_HANDLE;
    }
}

/**
 * Updates all player locations. Used for initialization.
 *
 * Note: If a client is specified, it's NOT validated. This function assumes
 *       the specified client is in game and alive.
 *
 * @param client    Optional. Specify single client to be updated. Default is
 *                  -1.
 */
VolUpdatePlayerLocation(client = -1)
{
    if (client <= 0)
    {
        // Assume the client is valid and save location in array.
        GetClientAbsOrigin(client, VolPlayerLoc[client]);
    }
    else
    {
        for (new client = 1; client <= MaxClients; client++)
        {
            // Check if client is in game and alive.
            if (!IsClientConnected(client) || !IsClientInGame(client) || !IsPlayerAlive(client))
            {
                return;
            }
            
            // Save location in array.
            GetClientAbsOrigin(client, VolPlayerLoc[client]);
        }
    }
}

/**
 * Updates player locations and trigger events for each player that enter or
 * leave a volume.
 */
VolUpdatePlayerChanges()
{
    new bool:volumeStates[ZR_VOLUMES_MAX];
    new bool:volumeNewStates[ZR_VOLUMES_MAX];
    
    new bool:newState;
    new bool:oldState;
    
    for (new client = 1; client <= MaxClients; client++)
    {
        // Check if client is in game and alive.
        if (!IsClientConnected(client) || !IsClientInGame(client) || !IsPlayerAlive(client))
        {
            return;
        }
        
        // Get the current volume states.
        VolGetPlayerStates(client, volumeStates, sizeof(volumeStates));
        
        // Update player location cache.
        GetClientAbsOrigin(client, VolPlayerLoc[client]);
        
        // Get new volume states.
        VolGetPlayerStates(client, volumeNewStates, sizeof(volumeNewStates));
        
        // Loop through each volume and compare states.
        for (new volumeIndex = 0; volumeIndex < VolumeCount; volumeIndex++)
        {
            newState = volumeNewStates[volumeIndex];
            oldState = volumeStates[volumeIndex];
            
            // Compare new states with old states.
            if (newState == oldState)
            {
                // No change. Skip to next volume.
                break;
            }
            else if (!newState && oldState)
            {
                // Client entered volume. Trigger event.
                VolOnPlayerEnter(client, volumeIndex);
            }
            else if (newState && !oldState)
            {
                // Client left the volume. Trigger event.
                VolOnPlayerLeave(client, volumeIndex);
            }
        }
    }
}

/**
 * Returns wether a position is within a certain location.
 *
 * @param loc       The position to check.
 * @param min       Minimum x, y and z values of the location.
 * @param max       Maximum x, y and z values of the location.
 * @return          True if the position is within min and max values. False
 *                  otherwise.
 */
bool:IsPositionInLocation(Float:pos[3], Float:min[3], Float:max[3])
{
    // Cache location to avoid re-indexing arrays.
    new Float:posX = pos[0];
    new Float:posY = pos[1];
    new Float:posZ = pos[2];
    
    // Check if within x boundaries.
    if ((posX >= min[0]) && (posX <= max[0]))
    {
        // Check if within y boundaries.
        if ((posY >= min[1]) && (posY <= max[1]))
        {
            // Check if within x boundaries.
            if ((posZ >= min[2]) && (posZ <= max[2]))
            {
                // The player is within the location boundaries.
                return true;
            }
        }
    }
    
    // The player is outside the location boundaries.
    return false;
}

/**
 * Returns wether a volume is marked as in use.
 *
 * @param volumeIndex   The volume index.
 * @return              True if in use, false otherwise.
 */
bool:VolIsInUse(volumeIndex)
{
    return Volumes[volumeIndex][vol_in_use];
}

/**
 * Gets the first free volume index.
 *
 * @return      The first free volume index if successful, or -1 if there are
 *              no free volumes.
 */
VolGetFreeVolume()
{
    // Loop through all volumes.
    for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
    {
        // Check if it's free.
        if (!VolIsInUse(volumeIndex))
        {
            return volumeIndex;
        }
    }
    
    // No free volumes found.
    return -1;
}

/**
 * Gets wether a client is within volumes or not. Result is stored in a boolean
 * array.
 *
 * @param client    The client index.
 * @param buffer    Destination buffer.
 * @param maxlen    Size of destination buffer.
 * @return          Number of volumes the client is within.
 */
VolGetPlayerStates(client, bool:buffer[], maxlen)
{
    new volumeBuffer[VolumeAttributes];
    new volCount;
    
    new Float:volMinBuffer[3];
    new Float:volMaxBuffer[3];
    
    // Loop through all available volumes.
    for (new volumeIndex = 0; volumeIndex < VolumeCount && volumeIndex < maxlen; volumeIndex++)
    {
        if (VolIsInUse(volumeIndex))
        {
            // Chache volume to avoid re-indexing.
            volumeBuffer = Volumes[volumeIndex];
            
            // Get min positions.
            volMinBuffer[0] = volumeBuffer[vol_x_min];
            volMinBuffer[1] = volumeBuffer[vol_y_min];
            volMinBuffer[2] = volumeBuffer[vol_z_min];
            
            // Get max positions.
            volMaxBuffer[0] = volumeBuffer[vol_x_min];
            volMaxBuffer[1] = volumeBuffer[vol_y_min];
            volMaxBuffer[2] = volumeBuffer[vol_z_min];
            
            // Check the cached player location.
            if (IsPositionInLocation(VolPlayerLoc[client], volMinBuffer, volMaxBuffer))
            {
                // Mark player as in volume.
                buffer[volumeIndex] = true;
                volCount++;
            }
            else
            {
                // Do explicit reset.
                buffer[volumeIndex] = false;
            }
        }
    }
    
    return volCount;
}

/**
 * Callback for update timer. This is the main timer in volumetric features.
 */
public Action:Event_VolUpdateTimer(Handle:timer)
{
    VolUpdatePlayerChanges();
}
