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

#include "zr/soundeffects/ambientsounds"
#include "zr/soundeffects/zombiesounds"

/**
 * Load sound effects data.
 */
SEffectsLoad()
{
    // Load ambient sound cvars.
    AmbientSoundsLoad();
}

/**
 * Map is starting.
 */ 
SEffectsOnMapStart()
{
    // Forward event to sub-modules.
    AmbientSoundsOnMapStart();
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
SEffectsClientInit(client)
{
    // Forward event to sub-modules.
    AmbientSoundsClientInit(client);
    ZombieSoundsClientInit(client);
}

/**
 * The round is starting.
 */
SEffectsOnRoundStart()
{
    // Forward event to sub-modules.
    AmbientSoundsOnRoundStart();
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
SEffectsOnClientSpawn(client)
{
    // Forward event to sub-modules.
    //AmbientSoundsOnClientSpawn(client);
    ZombieSoundsOnClientSpawn(client);
}

/**
 * Fired one frame after client spawns into the game.
 * 
 * @param client    The client index.
 */
SEffectsOnClientSpawnPost(client)
{
    AmbientSoundsOnClientSpawnPost(client);
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
SEffectsOnClientDeath(client)
{
    // Forward event to sub-modules.
    ZombieSoundsOnClientDeath(client);
}

/**
 * Client has been hurt.
 * 
 * @param client    The client index.
 */
SEffectsOnClientHurt(client)
{
    // Forward event to sub-modules.
    ZombieSoundsOnClientHurt(client);
}

/**
 * Client has been infected.
 * 
 * @param client    The client index.
 */
SEffectsOnClientInfected(client)
{
    // Forward event to sub-modules.
    ZombieSoundsOnClientInfected(client);
}
    
/**
 * Emits an ambient sound
 * 
 * @param sound         The path to the sound file (relative to sounds/)
 * @param soundvolume   The volume of the sound (0.0 - 1.0)
 * @param client        (Optional) Client index to play sound to.
 */
SEffectsEmitAmbientSound(const String:sound[], Float:ambientvolume = 1.0, client = -1)
{
    // Precache sound before playing.
    PrecacheSound(sound);
    
    // Stop sound before playing again.
    SEffectsStopAmbientSound(sound);
    
    if (ZRIsClientValid(client))
    {
        // Emit ambient sound.
        EmitSoundToClient(client, sound, SOUND_FROM_PLAYER, SOUND_AMBIENT_CHANNEL, _, _, ambientvolume);
        
        // Flag client that sound is playing.
        bAmbientSoundsIsPlaying[client] = true;
    }
    else
    {
        for (new x = 1; x <= MaxClients; x++)
        {
            // If client isn't in-game, then stop.
            if (!IsClientInGame(x))
            {
                continue;
            }
            
            // Emit ambient sound.
            EmitSoundToClient(x, sound, SOUND_FROM_PLAYER, SOUND_AMBIENT_CHANNEL, _, _, ambientvolume);
            
            // Flag client that sound is playing.
            bAmbientSoundsIsPlaying[x] = true;
        }
    }
}

/**
 * Stop an ambient sound
 *  
 * @param sound     The path to the sound file (relative to sounds/) 
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
 * Replay an ambient sound
 * 
 * @param sound     The path to the sound file (relative to sounds/)
 */ 

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