/**
 * ====================
 *   Zombie:Reloaded
 *   File: ambience.inc
 *   Author: Greyscale
 * ==================== 
 */

new bool:AmbienceLoaded = false;
new String:AmbienceSound[64];
new Float:AmbienceVolume;
new Handle:tAmbience[MAXPLAYERS + 1] = {INVALID_HANDLE, ...};

LoadAmbienceData()
{
    new bool:ambience = GetConVarBool(gCvars[CVAR_AMBIENCE]);
    if (!ambience)
    {
        return;
    }
    
    decl String:sound[64];
    GetConVarString(gCvars[CVAR_AMBIENCE_FILE], AmbienceSound, sizeof(AmbienceSound));
    Format(sound, sizeof(sound), "sound/%s", AmbienceSound);
    
    AmbienceLoaded = FileExists(sound, true);
    
    if (AmbienceLoaded)
    {
        AddFileToDownloadsTable(sound);
        PrecacheSound(AmbienceSound);
        AmbienceVolume = GetConVarFloat(gCvars[CVAR_AMBIENCE_VOLUME]);
        
        if (AmbienceVolume <= 0.0)
        {
            // No reason to play ambience sound if it's muted.
            AmbienceLoaded = false;
            if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_AMBIENCE)) ZR_LogMessageFormatted(-1, "ambience", "startup", "Ambience volume is muted or invalid.", LOG_FORMAT_TYPE_ERROR);
        }
        
        if (LogFlagCheck(LOG_DEBUG, LOG_MODULE_AMBIENCE)) ZR_LogMessageFormatted(-1, "ambience", "startup", "Ambience sound loaded.");
    }
    else
    {
        if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_AMBIENCE))
        {
            decl String:log_message[256];
            ZR_TranslateMessage(log_message, sizeof(log_message), "Ambience sound load failed", sound);
            ZR_LogMessageFormatted(-1, "ambience", "startup", log_message, true);
        }
    }
}

AmbienceStart(client)
{
    if (!AmbienceLoaded)
    {
        return;
    }
    
    new bool:ambience = GetConVarBool(gCvars[CVAR_AMBIENCE]);
    if (!ambience)
    {
        return;
    }
    
    new Float:delay = GetConVarFloat(gCvars[CVAR_AMBIENCE_LENGTH]);
    if (delay <= 0.0)
    {
        return;
    }
    
    if (!IsClientConnected(client) || !IsClientInGame(client))
    {
        return;
    }
    
    if (LogFlagCheck(LOG_DEBUG, LOG_MODULE_AMBIENCE)) ZR_LogMessageFormatted(client, "ambience", "start", "Starting ambience on client %d...", _, client);
    AmbiencePlay(client);
    tAmbience[client] = CreateTimer(delay, AmbienceTimer, client, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT);
}

AmbienceStop(client)
{
    if (tAmbience[client] != INVALID_HANDLE)
    {
        KillTimer(tAmbience[client]);
        tAmbience[client] = INVALID_HANDLE;
        if (IsClientConnected(client) && IsClientInGame(client))
        {
            StopSound(client, SNDCHAN_AUTO, AmbienceSound);
        }
    }
}

AmbienceStopAll()
{
    for (new client = 1; client < maxclients; client++)
    {
        AmbienceStop(client);
    }
}

AmbienceRestart(client)
{
    AmbienceStop(client);
    AmbienceStart(client);
}

AmbienceRestartAll()
{
    for (new client = 1; client < maxclients; client++)
    {
        AmbienceRestart(client);
    }
}

public Action:AmbienceTimer(Handle:timer, any:client)
{
    new bool:ambience = GetConVarBool(gCvars[CVAR_AMBIENCE]);
    if (!ambience || !AmbienceLoaded)
    {
        KillTimer(tAmbience[client]);
        tAmbience[client] = INVALID_HANDLE;
        return;
    }
    
    AmbiencePlay(client);
}

AmbiencePlay(client)
{
    EmitSoundToClient(client, AmbienceSound, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, AmbienceVolume, SNDPITCH_NORMAL, -1, NULL_VECTOR, NULL_VECTOR, true, 0.0);
    if (LogFlagCheck(LOG_DEBUG, LOG_MODULE_AMBIENCE)) ZR_LogMessageFormatted(client, "ambience", "play", "Playing ambience sound on client %d.", _, client);
}