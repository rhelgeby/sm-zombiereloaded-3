/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          zombiesounds.inc
 *  Type:          Core 
 *  Description:   Zombie sound effects.
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
 * @section Defines for min/max moaning sound file index.
 */
#define SOUND_MOAN_PATH "npc/zombie/zombie_voice_idle%d.wav" // %d is sound file index
#define SOUND_MOAN_MIN 1
#define SOUND_MOAN_MAX 14
/**
 * @endsection
 */

/**
 * @section Defines for min/max groan sound file index.
 */
#define SOUND_GROAN_PATH "npc/zombie/zombie_pain%d.wav" // %d is sound file index
#define SOUND_GROAN_MIN 1
#define SOUND_GROAN_MAX 6
/**
 * @endsection
 */

/**
 * @section Defines for min/max death sound file index.
 */
#define SOUND_DEATH_PATH "npc/zombie/zombie_die%d.wav" // %d is sound file index
#define SOUND_DEATH_MIN 1
#define SOUND_DEATH_MAX 3
/**
 * @endsection
 */

/**
 * Zombie sound types
 */
enum ZombieSounds
{
    Moan, /** Zombie's moan periodically */
    Groan, /** When zombie is hurt */
    Death, /** When a zombie is killed */
}
 
/**
 * Array for storing zombie moaning timer handles per client.
 */
new Handle:tSEffectsMoan[MAXPLAYERS + 1];

/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
ZombieSoundsClientInit(client)
{
    // Reset timer handle.
    tSEffectsMoan[client] = INVALID_HANDLE;
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
ZombieSoundsOnClientSpawn(client)
{
    // If timer is running, kill it.
    if (tSEffectsMoan[client] != INVALID_HANDLE)
    {
        KillTimer(tSEffectsMoan[client]);
    }
    
    // Reset timer handle.
    tSEffectsMoan[client] = INVALID_HANDLE;
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
ZombieSoundsOnClientDeath(client)
{
    // If timer is running, kill it.
    if (tSEffectsMoan[client] != INVALID_HANDLE)
    {
        KillTimer(tSEffectsMoan[client]);
    }
    
    // Reset timer handle.
    tSEffectsMoan[client] = INVALID_HANDLE;
    
    // If player isn't a zombie, then stop.
    if (!InfectIsClientInfected(client))
    {
        return;
    }
    
    // If death sound cvar is disabled, then stop.
    new bool:death = GetConVarBool(g_hCvarsList[CVAR_SEFFECTS_DEATH]);
    if (!death)
    {
        return;
    }
    
    // Get random death sound.
    decl String:sound[SOUND_MAX_PATH];
    ZombieSoundsGetRandomSound(sound, Death);
    
    SEffectsEmitSoundFromClient(client, sound);
}

/**
 * Client has been hurt.
 * 
 * @param client    The client index.
 */
ZombieSoundsOnClientHurt(client)
{
    // If player isn't a zombie, then stop.
    if (!InfectIsClientInfected(client))
    {
        return;
    }
    
    // Get groan factor, if 0, then stop.
    new groan = GetConVarInt(g_hCvarsList[CVAR_SEFFECTS_GROAN]);
    if (!groan)
    {
        return;
    }
    
    // 1 in 'groan' chance of groaning.
    if (GetRandomInt(1, groan) == 1)
    {
        // Get random groan sound.
        decl String:sound[SOUND_MAX_PATH];
        ZombieSoundsGetRandomSound(sound, Groan);
        
        SEffectsEmitSoundFromClient(client, sound);
    }
}

/**
 * Client has been infected.
 * 
 * @param client    The client index.
 */
ZombieSoundsOnClientInfected(client)
{
    // If interval is set to 0, then stop.
    new Float:interval = GetConVarFloat(g_hCvarsList[CVAR_SEFFECTS_MOAN]);
    if (!interval)
    {
        return;
    }
    
    // If timer is currently running, kill it.
    if (tSEffectsMoan[client] != INVALID_HANDLE)
    {
        KillTimer(tSEffectsMoan[client]);
    }
    
    // Start repeating timer.
    tSEffectsMoan[client] = CreateTimer(interval, ZombieSoundsMoanTimer, client, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
}

/**
 * Gets a random zombie sound from hl2 folder.
 *
 * @param sound     The randomly picked sound.
 * @param soundtype The type of sound to get. (See enum ZombieSounds)
 * @return          True if sound was successfully picked, false otherwise. 
 */    
bool:ZombieSoundsGetRandomSound(String:sound[], ZombieSounds:soundtype)
{
    new soundmin;
    new soundmax;
    decl String:soundpath[SOUND_MAX_PATH];
    
    switch(soundtype)
    {
        // Find moan sound.
        case Moan:
        {
            // Copy min and max
            soundmin = SOUND_MOAN_MIN;
            soundmax = SOUND_MOAN_MAX;
            
            // Copy path
            strcopy(soundpath, sizeof(soundpath), SOUND_MOAN_PATH);
        }
        // Find groan sound. (zombie shot)
        case Groan:
        {
            // Copy min and max
            soundmin = SOUND_GROAN_MIN;
            soundmax = SOUND_GROAN_MAX;
            
            // Copy path
            strcopy(soundpath, sizeof(soundpath), SOUND_GROAN_PATH);
        }
        // Find death sound.
        case Death:
        {
            // Copy min and max
            soundmin = SOUND_DEATH_MIN;
            soundmax = SOUND_DEATH_MAX;
            
            // Copy path
            strcopy(soundpath, sizeof(soundpath), SOUND_DEATH_PATH);
        }
        // Invalid case given.
        default:
        {
            // No handled case was given.
            return false;
        }
    }
    
    // Pick a random integer between min and max sound file index.
    new randsound = GetRandomInt(soundmin, soundmax);
    
    // Format random index into sound path.
    Format(sound, SOUND_MAX_PATH, soundpath, randsound);
    
    // Found sound.
    return true;
}

/**
 * Timer callback, repeats a moaning sound on zombies.
 * 
 * @param timer     The timer handle.
 * @param client    The client index.
 */
public Action:ZombieSoundsMoanTimer(Handle:timer, any:client)
{
    // If client isn't in-game or client is no longer a zombie, then stop.
    if (!IsClientInGame(client) || !InfectIsClientInfected(client))
    {
        // Reset timer handle.
        tSEffectsMoan[client] = INVALID_HANDLE;
        
        // Stop timer.
        return Plugin_Stop;
    }
    
    // Get random moan sound.
    decl String:sound[SOUND_MAX_PATH];
    ZombieSoundsGetRandomSound(sound, Moan);
    
    // Emit sound from client.
    SEffectsEmitSoundFromClient(client, sound, SNDLEVEL_SCREAMING);
    
    // Allow timer to continue.
    return Plugin_Continue;
}
