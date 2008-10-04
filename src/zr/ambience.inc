/**
 * ====================
 *   Zombie:Reloaded
 *   File: ambience.inc
 *   Author: Greyscale
 * ==================== 
 */

new bool:soundValid = false;

new Handle:tAmbience = INVALID_HANDLE;

LoadAmbienceData()
{
    new bool:ambience = GetConVarBool(gCvars[CVAR_AMBIENCE]);
    if (!ambience)
    {
        return;
    }
    
    decl String:sound[64];
    GetConVarString(gCvars[CVAR_AMBIENCE_FILE], sound, sizeof(sound));
    Format(sound, sizeof(sound), "sound/%s", sound);
    
    soundValid = FileExists(sound, true);
    
    if (soundValid)
    {
        AddFileToDownloadsTable(sound);
    }
    else
    {
        ZR_LogMessage("Ambient sound load failed", sound);
    }
}

RestartAmbience()
{
    if (tAmbience != INVALID_HANDLE)
    {
        CloseHandle(tAmbience);
    }
    
    CreateTimer(0.0, AmbienceLoop, _, TIMER_FLAG_NO_MAPCHANGE);
}

public Action:AmbienceLoop(Handle:timer)
{
    new bool:ambience = GetConVarBool(gCvars[CVAR_AMBIENCE]);
    
    if (!ambience || !soundValid)
    {
        return;
    }
    
    decl String:sound[64];
    GetConVarString(gCvars[CVAR_AMBIENCE_FILE], sound, sizeof(sound));
    
    EmitAmbience(sound);
    
    new Float:delay = GetConVarFloat(gCvars[CVAR_AMBIENCE_LENGTH]);
    tAmbience = CreateTimer(delay, AmbienceLoop, _, TIMER_FLAG_NO_MAPCHANGE);
}

StopAmbience()
{
    new bool:ambience = GetConVarBool(gCvars[CVAR_AMBIENCE]);
    
    if (!ambience)
    {
        return;
    }
    
    decl String:sound[64];
    GetConVarString(gCvars[CVAR_AMBIENCE_FILE], sound, sizeof(sound));
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        StopSound(x, SNDCHAN_AUTO, sound);
    }
}

EmitAmbience(const String:sound[])
{
    PrecacheSound(sound);
    
    StopAmbience();
    
    new Float:volume = GetConVarFloat(gCvars[CVAR_AMBIENCE_VOLUME]);
    EmitSoundToAll(sound, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, volume, SNDPITCH_NORMAL, -1, NULL_VECTOR, NULL_VECTOR, true, 0.0);
}