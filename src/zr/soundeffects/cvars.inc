/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          soundeffects/cvars.inc
 *  Type:          Module Component
 *  Description:   Console variables for Sound Effects.
 *
 *  Copyright (C) 2009-2015  Greyscale, Richard Helgeby
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

Handle SoundEffectsCvar_PathMoan;
Handle SoundEffectsCvar_PathPain;
Handle SoundEffectsCvar_PathDie;
Handle SoundEffectsCvar_MoanMax;
Handle SoundEffectsCvar_PainMax;
Handle SoundEffectsCvar_DieMax;

void SoundEffects_OnCvarsCreate()
{
    SoundEffectsCvar_PathMoan               = CreateConVar("zr_soundeffects_path_moan",             "npc/zombie/zombie_voice_idle%d.wav",   "Sound, relative to \"sound\" folder, to play from player when moaning.");
    SoundEffectsCvar_PathPain               = CreateConVar("zr_soundeffects_path_pain",             "npc/zombie/zombie_pain%d.wav",         "Sound, relative to \"sound\" folder, to play from player when hurt.");
    SoundEffectsCvar_PathDie                = CreateConVar("zr_soundeffects_path_die",              "npc/zombie/zombie_die%d.wav",          "Sound, relative to \"sound\" folder, to play from player when killed.");
    SoundEffectsCvar_MoanMax                = CreateConVar("zr_soundeffects_moan_max",              "14",                                   "Max moaning sound file index.");
    SoundEffectsCvar_PainMax                = CreateConVar("zr_soundeffects_pain_max",              "6",                                    "Max groan sound file index.");
    SoundEffectsCvar_DieMax                 = CreateConVar("zr_soundeffects_die_max",               "3",                                    "Max death sound file index.");
}

void SoundEffects_GetSoundEffectsPath(String:sound[], ZombieSounds:soundtype)
{
    decl String:effectsPath[SOUND_MAX_PATH];
    
    switch(soundtype)
    {
        case Moan:
            GetConVarString(SoundEffectsCvar_PathMoan, effectsPath, SOUND_MAX_PATH);
        case Groan:
            GetConVarString(SoundEffectsCvar_PathPain, effectsPath, SOUND_MAX_PATH);
        case Death:
            GetConVarString(SoundEffectsCvar_PathDie, effectsPath, SOUND_MAX_PATH);
    }
    
    Format(sound, SOUND_MAX_PATH, effectsPath);
}

int SoundEffects_GetSoundEffectsMax(ZombieSounds:soundtype)
{
    switch(soundtype)
    {
        case Moan:
            return GetConVarInt(SoundEffectsCvar_MoanMax);
        case Groan:
            return GetConVarInt(SoundEffectsCvar_PainMax);
        case Death:
            return GetConVarInt(SoundEffectsCvar_DieMax);
    }
    
    return 1;
}