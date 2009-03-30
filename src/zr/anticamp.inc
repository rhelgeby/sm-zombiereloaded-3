/**
 * ====================
 *   Zombie:Reloaded
 *   File: anticamp.inc
 *   Authors: Richard Helgeby
 * ==================== 
 */

#define MAX_VOLUMES 32

enum volume
{
    Float:x_min,
    Float:x_max,
    Float:y_min,
    Float:y_max,
    Float:z_min,
    Float:z_max,
    volume_damage,
    Float:volume_interval,
    Handle:volume_timer,
    bool:volume_in_use
}

new volumes[MAX_VOLUMES][volume];
new volume_count;

new Float:player_loc[MAXPLAYERS + 1][3];

new Handle:hUpdateTimer;

Anticamp_Startup(bool:no_reset = false)
{
    new Float:interval = GetConVarFloat(gCvars[CVAR_ANTICAMP_UPDATE_INTERVAL]);
    
    // Create timer for updating player locations.
    if (hUpdateTimer != INVALID_HANDLE)
    {
        KillTimer(hUpdateTimer);
        hUpdateTimer = INVALID_HANDLE;
    }

    if (interval > 0.0)
    {
        hUpdateTimer = CreateTimer(interval, Event_UpdatePlayerLocations, 0, TIMER_REPEAT);
    }
    
    if (no_reset)
    {
        // Start existing hurt timers.
        for (new vol_index = 0; vol_index < MAX_VOLUMES; vol_index++)
        {
            if (volumes[vol_index][volume_in_use])
            {
                if (volumes[vol_index][volume_timer] != INVALID_HANDLE)
                {
                    KillTimer(volumes[vol_index][volume_timer]);
                }
                volumes[vol_index][volume_timer] = CreateTimer(volumes[vol_index][volume_interval], Event_HurtPlayers, vol_index, TIMER_REPEAT);
            }
        }
    }
    else
    {
        ResetVolumes();
    }
}

Anticamp_Disable()
{
    // Kill update timer.
    if (hUpdateTimer != INVALID_HANDLE)
    {
        KillTimer(hUpdateTimer);
        hUpdateTimer = INVALID_HANDLE;
    }
    
    // Kill hurt timers.
    for (new vol = 0; vol < MAX_VOLUMES; vol++)
    {
        if (volumes[vol][volume_timer] != INVALID_HANDLE)
        {
            KillTimer(volumes[vol][volume_timer]);
            volumes[vol][volume_timer] = INVALID_HANDLE;
        }
    }
}

public UpdateIntervalHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    new Float:interval = StringToFloat(newValue);
    if (hUpdateTimer != INVALID_HANDLE)
    {
        KillTimer(hUpdateTimer);
        hUpdateTimer  = INVALID_HANDLE;
    }
    
    if (interval > 0.0)
    {
        hUpdateTimer = CreateTimer(interval, Event_UpdatePlayerLocations, 0, TIMER_REPEAT);
    }
}

public AnticampHook(Handle:convar, const String:oldValue[], const String:newValue[])
{
    new anticamp_enabled = StringToInt(newValue);
    if (anticamp_enabled)
    {
        Anticamp_Startup(true);
    }
    else
    {
        Anticamp_Disable();
    }
}

public Action:Command_AnticampCreateVolume(client, argc)
{
    // Get a unused volume index.
    new vol_index = GetFreeVolumeIndex();
    if (vol_index == -1)
    {
        ReplyToCommand(client, "Maximum volumes reached. Max: %d", MAX_VOLUMES);
        return Plugin_Handled;
    }
    
    // Check arguments.
    if (argc < 8)
    {
        ReplyToCommand(client, "Too few parameters specified. Usage: zr_anticamp_create_volume <damage> <interval> <x1> <y1> <z1> <x2> <y2> <z2>");
        return Plugin_Handled;
    }
    
    decl String:arg_buffer[32];
    
    new vol_damage;
    new Float:vol_interval;
    new Float:x1;
    new Float:y1;
    new Float:z1;
    new Float:x2;
    new Float:y2;
    new Float:z2;
    
    // Get arguments.
    GetCmdArg(1, arg_buffer, sizeof(arg_buffer));
    vol_damage = StringToInt(arg_buffer);
    
    GetCmdArg(2, arg_buffer, sizeof(arg_buffer));
    vol_interval = StringToFloat(arg_buffer);
    
    GetCmdArg(3, arg_buffer, sizeof(arg_buffer));
    x1 = StringToFloat(arg_buffer);
    
    GetCmdArg(4, arg_buffer, sizeof(arg_buffer));
    y1 = StringToFloat(arg_buffer);
    
    GetCmdArg(5, arg_buffer, sizeof(arg_buffer));
    z1 = StringToFloat(arg_buffer);
    
    GetCmdArg(6, arg_buffer, sizeof(arg_buffer));
    x2 = StringToFloat(arg_buffer);
    
    GetCmdArg(7, arg_buffer, sizeof(arg_buffer));
    y2 = StringToFloat(arg_buffer);
    
    GetCmdArg(8, arg_buffer, sizeof(arg_buffer));
    z2 = StringToFloat(arg_buffer);

    // Check damage.
    if (vol_damage == 0)
    {
        ReplyToCommand(client, "The damage specified is zero or invalid.");
        return Plugin_Handled;
    }
    
    // Check interval.
    if (vol_interval <= 0.0)
    {
        ReplyToCommand(client, "The interval specified is zero or invalid.");
        return Plugin_Handled;
    }
    
    // Check if both locations are equal.
    if (FloatCompare(x1, x2) == 0)
    {
        if (FloatCompare(y1, y2) == 0)
        {
            if (FloatCompare(z1, z2) == 0)
            {
                ReplyToCommand(client, "Cannot add hurt volume. Both locations are equal.");
                return Plugin_Handled;
            }
        }
    }

    // Sort out max and min values.
    if (FloatCompare(x1, x2) == 1)
    {
        volumes[vol_index][x_max] = x1;
        volumes[vol_index][x_min] = x2;
    }
    else
    {
        volumes[vol_index][x_max] = x2;
        volumes[vol_index][x_min] = x1;
    }
    
    if (FloatCompare(y1, y2) == 1)
    {
        volumes[vol_index][y_max] = y1;
        volumes[vol_index][y_min] = y2;
    }
    else
    {
        volumes[vol_index][y_max] = y2;
        volumes[vol_index][y_min] = y1;
    }
    
    if (FloatCompare(z1, z2) == 1)
    {
        volumes[vol_index][z_max] = z1;
        volumes[vol_index][z_min] = z2;
    }
    else
    {
        volumes[vol_index][z_max] = z2;
        volumes[vol_index][z_min] = z1;
    }
    
    volumes[vol_index][volume_damage] = vol_damage;
    volumes[vol_index][volume_interval] = vol_interval;
    volumes[vol_index][volume_timer] = CreateTimer(vol_interval, Event_HurtPlayers, vol_index, TIMER_REPEAT);
    volumes[vol_index][volume_in_use] = true;
    
    ReplyToCommand(client, "Volume created:\nid: %d\nx min:%f\ny min:%f\nz min:%f\nx max:%f\ny max:%f\nz max:%f",
        vol_index,
        volumes[vol_index][x_min],
        volumes[vol_index][y_min],
        volumes[vol_index][z_min],
        volumes[vol_index][x_max],
        volumes[vol_index][y_max],
        volumes[vol_index][z_max]);
    
    volume_count++;
    
    return Plugin_Handled;
}

public Action:Command_AnticampRemoveVolume(client, argc)
{
    new vol_index;
    decl String:arg_buffer[32];
    //decl String:client_name[192];
    new Handle:vol_timer;
    
    if (argc < 1)
    {
        ReplyToCommand(client, "Volume ID not specified. Usage: zr_anticamp_remove_volume <volume id>");
        return Plugin_Handled;
    }
    
    GetCmdArg(1, arg_buffer, sizeof(arg_buffer));
    vol_index = StringToInt(arg_buffer);
    
    if (IsValidVolume(vol_index))
    {
        vol_timer = volumes[vol_index][volume_timer];
        if (vol_timer != INVALID_HANDLE)
        {
            KillTimer(vol_timer);
            volumes[vol_index][volume_timer] = INVALID_HANDLE;
        }
        volumes[vol_index][volume_in_use] = false;
        ReplyToCommand(client, "Removed volume %d.", vol_index);
        
        if (LogFlagCheck(LOG_GAME_EVENTS, LOG_MODULE_ANTICAMP))
        {
            ZR_LogMessageFormatted(client, "anticamp", "remove volume", "\"%L\" removed volume %d.", true, client, vol_index);
        }
    }
    else
    {
        ReplyToCommand(client, "Invalid volume ID.");
        return Plugin_Handled;
    }
    
    return Plugin_Handled;
}

public Action:Command_AnticampList(client, argc)
{
    decl String:buffer[2048];
    decl String:line[192];
    buffer[0] = 0;
    
/*
id  damage  interval    x_min   y_min   z_min   x_max   y_max   z_max
0   5       1           73      -535    28      -251    -732    76
*/
    StrCat(buffer, sizeof(buffer), "id  damage  interval    x_min   y_min   z_min   x_max   y_max   z_max\n");
    
    for (new vol_index = 0; vol_index < MAX_VOLUMES; vol_index++)
    {
        if (volumes[vol_index][volume_in_use])
        {
            Format(line, sizeof(line), "%-3d %-7d %-11.1f %-7.1f %-7.1f %-7.1f %-7.1f %-7.1f %-7.1f\n",
                vol_index,
                volumes[vol_index][volume_damage],
                volumes[vol_index][volume_interval],
                volumes[vol_index][x_min],
                volumes[vol_index][y_min],
                volumes[vol_index][z_min],
                volumes[vol_index][x_max],
                volumes[vol_index][y_max],
                volumes[vol_index][y_max]);
            StrCat(buffer, sizeof(buffer), line);
        }
    }
    
    ReplyToCommand(client, buffer);
    return Plugin_Handled;
}

UpdatePlayerLocations()
{
    new client;
    for (client = 1; client <= maxclients; client++)
    {
        if (IsClientConnected(client) && IsClientInGame(client))
        {
            GetClientAbsOrigin(client, player_loc[client]);
        }
    }
}

HurtPlayersInVolume(volume_index)
{
    new client;
    new client_health;
    decl String:client_name[64];
    decl String:buffer[192];
    new anticamp_echo = GetConVarBool(gCvars[CVAR_ANTICAMP_ECHO]);
    
    for (client = 1; client <= maxclients; client++)
    {
        if (IsClientConnected(client) &&
            IsClientInGame(client) &&
            IsPlayerAlive(client) &&
            IsPlayerHuman(client))
        {
            if (IsPlayerInVolume(client, volume_index))
            {
                ZR_PrintToChat(client, "Unfair camping");
                client_health = GetClientHealth(client) - volumes[volume_index][volume_damage];
                if (client_health > 0)
                {
                    SetEntityHealth(client, client_health);
                }
                else
                {
                    ForcePlayerSuicide(client);
                    
                    GetClientName(client, client_name, sizeof(client_name));
                    SetGlobalTransTarget(client);
                    Format(buffer, sizeof(buffer), "%T", "Unfair camper slayed", LANG_SERVER, client_name, volume_index);

                    if (LogFlagCheck(LOG_GAME_EVENTS, LOG_MODULE_ANTICAMP)) ZR_LogMessageFormatted(client, "anticamp", "kill", "%s", true, buffer);
                    if (anticamp_echo)
                    {
                        FormatTextString(buffer, sizeof(buffer));
                        ZR_PrintToAdminChat(buffer);
                    }
                }
            }
        }
    }
}

bool:IsPlayerInVolume(client, volume_index)
{
    new Float:player_loc_x = player_loc[client][0];
    new Float:player_loc_y = player_loc[client][1];
    new Float:player_loc_z = player_loc[client][2];
    new log;
    log = LogFlagCheck(LOG_DEBUG_MAX_DETAIL, LOG_MODULE_ANTICAMP);
    
    if ((player_loc_x >= volumes[volume_index][x_min]) && (player_loc_x <= volumes[volume_index][x_max]))
    {
        if (log) ZR_LogMessageFormatted(client, "anticamp", "IsPlayerInVolume", "Client %d matches X values.", true, client);
        if ((player_loc_y >= volumes[volume_index][y_min]) && (player_loc_y <= volumes[volume_index][y_max]))
        {
            if (log) ZR_LogMessageFormatted(client, "anticamp", "IsPlayerInVolume", "Client %d matches Y values.", true, client);
            if ((player_loc_z >= volumes[volume_index][z_min]) && (player_loc_z <= volumes[volume_index][z_max]))
            {
                if (log) ZR_LogMessageFormatted(client, "anticamp", "IsPlayerInVolume", "Client %d matches Z values.", true, client);
                return true;
            }
        }
    }
    return false;
}

GetFreeVolumeIndex()
{
    for(new vol_index = 0; vol_index < MAX_VOLUMES; vol_index++)
    {
        if (!volumes[vol_index][volume_in_use])
        {
            return vol_index;
        }
    }
    return -1;
}

bool:IsValidVolume(vol_index)
{
    if (vol_index >= 0 && vol_index < MAX_VOLUMES)
    {
        return true;
    }
    else
    {
        return false;
    }
}

ResetVolumes()
{
    for (new vol_index = 0; vol_index < MAX_VOLUMES; vol_index++)
    {
        volumes[vol_index][volume_in_use] = false;
        if (volumes[vol_index][volume_timer] != INVALID_HANDLE)
        {
            KillTimer(volumes[vol_index][volume_timer]);
            volumes[vol_index][volume_timer] = INVALID_HANDLE;
        }
    }
}

public Action:Event_UpdatePlayerLocations(Handle:Timer)
{
    UpdatePlayerLocations();
}

public Action:Event_HurtPlayers(Handle:Timer, any:volume_index)
{
    HurtPlayersInVolume(volume_index);
}
