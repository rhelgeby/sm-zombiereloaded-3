/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volfeatures.inc
 *  Type:           Module
 *  Description:    Provides functions for managing volumetric features.
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
 * Total volumes that can be created in a map.
 */
#define ZR_VOLUMES_MAX 64

/**
 * Represent a rectangular volume.
 */
enum VolumeAttributes
{
    /* General */
    bool:vol_enabled,                   /** Volume state. */
    bool:vol_in_use,                    /** Marks if the volume is used. */
    
    /* Location */
    Float:vol_x_min,                    /** Minimum x position. */
    Float:vol_x_max,                    /** Maximum x position. */
    
    Float:vol_y_min,                    /** Minimum y position. */
    Float:vol_y_max,                    /** Maximum y position. */
    
    Float:vol_z_min,                    /** Minimum z position. */
    Float:vol_z_max,                    /** Maximum z position. */
    
    /* Style */
    VolumeEffects:vol_effect,           /** Visual effect to apply on the volume. */
    vol_effect_color[3],                /** Render color of the effect. RGB colors. */
    
    /* Data */
    VolumeFeatureTypes:vol_type,        /** The volumetric feature type. */
    vol_data_index,                     /** Index in remote feature array. */
    
    /* Behaviour */
    VolumeTeamFilters:vol_team_filter,  /** Team filtering. Trigger by certain teams, or all. */
    Float:vol_trigger_delay             /** Trigger delay. How many seconds players have to stay to trigger volume events. */
}

/**
 * Available volumetric feature types.
 */
enum VolumeFeatureTypes
{
    VolFeature_Anticamp = 0,
    VolFeature_Knockback
}

/**
 * Effects that can be applied on a volume. (Currently no effects.)
 */
enum VolumeEffects
{
    VolEffect_None = 0,
    VolEffect_Wireframe,
    VolEffect_Smoke
}

/**
 * Available team filter settings.
 */
enum VolumeTeamFilters
{
    VolTeam_All = 0,
    VolTeam_Humans,
    VolTeam_Zombies
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
 * Specifies whether the volumetric features module is enabled or not. Synced
 * with zr_vol CVAR.
 */
new bool:VolEnabled;

/**
 * Counter for trigger delay.
 */
new Float:VolPlayerCountDown[MAXPLAYERS + 1][ZR_VOLUMES_MAX];

/**
 * The handle for a timer that updates player locations. This is the main timer
 * and any feature events can't be updated faster than this interval.
 *
 * Note: Some features may have its own timer.
 */
new Handle:hVolUpdateTimer;

/**
 * The handle for a timer that do count down on trigger delays.
 */
new Handle:hVolTriggerTimer;

/**
 * Cached interval value for trigger timer.
 */
new Float:VolTriggerInterval;

#include "zr/volfeatures/volevents"
#include "zr/volfeatures/volgenericattributes"
#include "zr/volfeatures/volcommands"
#include "zr/volfeatures/volanticamp"


/**
 * Initialize volumetric features.
 */
VolLoad()
{
    // Cache CVAR value.
    VolEnabled = GetConVarBool(g_hCvarsList[CVAR_VOL]);
}

/**
 * Function alias for fully stopping volumetric features.
 */
VolDisable()
{
    VolEnabled = false;
    VolStopUpdateTimer();
    
    // TODO: Send disable/stop event to volumes with their own timers.
}

/**
 * Starts the update timer.
 *
 * @return      True if timer is started, false otherwise.
 */
bool:VolStartUpdateTimer()
{
    // Check if volumetric features is enabled.
    if (!VolEnabled)
    {
        // Volumetric features disabled.
        return false;
    }
    
    // Stop timer if it exist.
    VolStopUpdateTimer();
    
    // Get update interval.
    new Float:interval = GetConVarFloat(g_hCvarsList[CVAR_VOL_UPDATE_INTERVAL]);
    
    // Validate interval.
    if (interval > 0.0)
    {
        // Create a new timer.
        hVolUpdateTimer = CreateTimer(interval, Event_VolUpdateTimer, _, TIMER_REPEAT);
        
        // Also start the trigger delay timer.
        VolStartTriggerTimer();
        
        // Volumetric features started.
        return true;
    }
    else
    {
        // Volumetric features disabled.        
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
    
    // Also stop trigger delay timer.
    VolStopTriggerTimer();
    
    // Reset all trigger delay counters.
    VolResetCountDown();
}

/**
 * Starts the update timer if it exists.
 *
 * @return      True if timer is started, false otherwise.
 */
bool:VolStartTriggerTimer()
{
    // Make sure existing timer is killed.
    VolStopTriggerTimer();
    
    // Get trigger interval and cache it.
    VolTriggerInterval = GetConVarFloat(g_hCvarsList[CVAR_VOL_TRIGGER_INTERVAL]);
    
    // Validate interval.
    if (VolTriggerInterval > 0.0)
    {
        // Start the timer.
        hVolTriggerTimer = CreateTimer(VolTriggerInterval, Event_VolUpdateTimer, _, TIMER_REPEAT);
        
        // Trigger timer started.
        return true;
    }
    else
    {
        // Trigger timer not running. Either disabled or invalid interval.
        return false;
    }
}

/**
 * Kills the trigger delay timer if it exists.
 */
VolStopTriggerTimer()
{
    // Kill the timer if it's running.
    if (hVolTriggerTimer != INVALID_HANDLE)
    {
        KillTimer(hVolTriggerTimer);
        hVolTriggerTimer = INVALID_HANDLE;
    }
}

/**
 * Resets volume trigger delay counters on one or more players.
 *
 * @param client    Optional. Specifies a single player to reset. Default is
 *                  -1, all players.
 */
VolResetCountDown(client = -1)
{
    // Check if a client is specified.
    if (client > -1)
    {
        // Reset volume counters.
        for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
        {
            VolPlayerCountDown[client][volumeIndex] = -1.0;
        }
    }
    else
    {
        // Reset all volume counters.
        for (new clientIndex = 0; clientIndex <= MAXPLAYERS + 1; clientIndex++)
        {
            for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
            {
                VolPlayerCountDown[clientIndex][volumeIndex] = -1.0;
            }
        }
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
        for (client = 1; client <= MaxClients; client++)
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
    
    new Float:trigger_delay;
    
    // Loop through all players.
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
            
            // Check if client entered the volume.
            else if (!newState && oldState)
            {
                // Get trigger delay value.
                trigger_delay = Volumes[volumeIndex][vol_trigger_delay];
                
                // Check if the volume has a trigger delay.
                if (trigger_delay > 0.0)
                {
                    // Set count down value.
                    VolPlayerCountDown[client][volumeIndex] = trigger_delay;
                }
                else
                {
                    // No trigger delay, trigger event instantly.
                    VolOnPlayerEnter(client, volumeIndex);
                }
            }
            
            // Check if client left the volume.
            else if (newState && !oldState)
            {
                // Make sure count down value is reset.
                VolPlayerCountDown[client][volumeIndex] = -1.0;
                
                // Trigger event.
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
 * Note: Does not validate index.
 *
 * @param volumeIndex   The volume index.
 * @return              True if in use, false otherwise.
 */
bool:VolIsInUse(volumeIndex)
{
    return Volumes[volumeIndex][vol_in_use];
}

/**
 * Validates a volume index.
 *
 * @param volumeIndex   The volume index.
 * @return              True if valid, false otherwise.
 */
bool:VolIsValidIndex(volumeIndex)
{
    if (volumeIndex >= 0 && volumeIndex < ZR_VOLUMES_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
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

/**
 * Callback for trigger delay timer.
 */
public Action:Event_VolTriggerTimer(Handle:timer)
{
    new Float:countDown;
    
    // Loop through all players.
    for (new client = 1; client <= MaxClients; client++)
    {
        // Loop through all volumes.
        for (new volumeIndex = 0; volumeIndex < ZR_VOLUMES_MAX; volumeIndex++)
        {
            // Get count down value.
            countDown = VolPlayerCountDown[client][volumeIndex];
            
            // Check if volume trigger delay is enabled.
            if (countDown > 0.0)
            {
                // Substract by trigger interval.
                countDown -= VolTriggerInterval;
                
                // Check if zero or below.
                if (countDown <= 0.0)
                {
                    // Trigger volume enter event.
                    VolOnPlayerEnter(client, volumeIndex);
                    
                    // Reset count down value.
                    VolPlayerCountDown[client][volumeIndex] = -1.0;
                }
            }
        }
    }
}

/**
 * Called when zr_vol CVAR is changed.
 */
public VolEnabledChanged(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    new bool:isEnabled = bool:StringToInt(newvalue);
    
    if (isEnabled)
    {
        // Volumetric features is enabled.
        VolEnabled = true;
        
        // Start timers.
        VolStartUpdateTimer();
    }
    else
    {
        // Volumetric features is disabled.
        VolDisable();
    }
}
