/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        soundeffects.inc
 *   Description: Basic sound-management API.
 *
 * ============================================================================
 */

/**
 * Maximum sound path length.
 */
#define SOUND_MAX_PATH 128

/**
 * Ambient sound channel.
 */
#define SOUND_AMBIENT_CHANNEL 8

#include "zr/soundeffects/zombiesounds"

/**
 * Emits an ambient sound
 */
SEffectsEmitAmbientSound(const String:sound[], Float:soundvolume = 1.0)
{
    // Precache sound before playing.
    PrecacheSound(sound);
    
    // Emit ambient sound.
    EmitSoundToAll(sound, SOUND_FROM_PLAYER, SOUND_AMBIENT_CHANNEL, _, _, soundvolume);
}

/**
 * Stop an ambient sound
 */
SEffectsStopAmbientSound(const String:sound[])
{
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // Stop ambient sound.
        StopSound(x, SOUND_AMBIENT_CHANNEL, sound);
    }
}

/**
 * Emits a sound from a client.
 * 
 * @param client    The client index.
 * @param sound     The sound file relative to the sound/ directory.
 * @param level     The attenuation of the sound.
 */
SEffectsEmitSoundFromClient(client, const String:sound[], level = SNDLEVEL_NORMAL)
{
    // Precache sound before playing.
    PrecacheSound(sound);
    
    // Emit sound from client.
    EmitSoundToAll(sound, client, _, level);
}