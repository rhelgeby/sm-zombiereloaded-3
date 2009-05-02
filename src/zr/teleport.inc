/**
 * ====================
 *   Zombie:Reloaded
 *   File: teleport.inc
 *   Authors: Richard Helgeby, Cpt.Moore
 * ==================== 
 */

new Float:spawnLoc[MAXPLAYERS + 1][3];
new Float:bufferLoc[MAXPLAYERS + 1][3];
new bool:ztele_spawned[MAXPLAYERS + 1] = {false, ...};
new bool:bufferLocSaved[MAXPLAYERS + 1] = {false, ...};
new ztele_countdown[MAXPLAYERS + 1] = {-1, ...};
new ztele_count[MAXPLAYERS + 1];
new bool:ztele_online = false;
new Handle:ztele_startup_timer = INVALID_HANDLE;
new Handle:ztele_countdown_timer[MAXPLAYERS + 1] = {INVALID_HANDLE, ...};
new Handle:ztele_cooldown_timer[MAXPLAYERS + 1] = {INVALID_HANDLE, ...};

ZTeleEnable()
{
    ztele_online = false;
    if (ztele_startup_timer != INVALID_HANDLE)
    {
        KillTimer(ztele_startup_timer);
    }
    new Float:startup_delay = GetConVarFloat(g_hCvarsList[CVAR_ZTELE_STARTUP_DELAY]);
    if (startup_delay > 0)
    {
        ztele_startup_timer = CreateTimer(startup_delay, Event_TeleporterStartup);
    }
    else
    {
        ztele_online = true;
    }
}

ZTeleReset()
{
    ztele_online = false;

    if (ztele_startup_timer != INVALID_HANDLE)
    {
        KillTimer(ztele_startup_timer);
        ztele_startup_timer = INVALID_HANDLE;
    }

    for (new client = 1; client <= MAXPLAYERS; client++)
    {
        spawnLoc[client] = NULL_VECTOR;
        ztele_spawned[client] = false;
        ztele_countdown[client] = -1;
        ztele_count[client] = 0;
        bufferLoc[client] = NULL_VECTOR;
        bufferLocSaved[client] = false;
        
        // Stop any cooldown or teleportation in progress.
        if (ztele_countdown_timer[client] != INVALID_HANDLE)
        {
            KillTimer(ztele_countdown_timer[client]);
            ztele_countdown_timer[client] = INVALID_HANDLE;
        }
        if (ztele_cooldown_timer[client] != INVALID_HANDLE)
        {
            KillTimer(ztele_cooldown_timer[client]);
            ztele_cooldown_timer[client] = INVALID_HANDLE;
        }
    }
}

ZTeleResetClient(client, bool:soft_reset = false)
{
    if (!soft_reset)
    {
        spawnLoc[client] = NULL_VECTOR;
        ztele_spawned[client] = false;
    }
    ztele_countdown[client] = -1;
    ztele_count[client] = 0;
    
    // Stop any cooldown or teleportation in progress.
    if (ztele_countdown_timer[client] != INVALID_HANDLE)
    {
        KillTimer(ztele_countdown_timer[client]);
        ztele_countdown_timer[client] = INVALID_HANDLE;
    }
    if (ztele_cooldown_timer[client] != INVALID_HANDLE)
    {
        KillTimer(ztele_cooldown_timer[client]);
        ztele_cooldown_timer[client] = INVALID_HANDLE;
    }
}

ZTeleClientSpawned(client)
{
    if (IsFakeClient(client))
    {
        return;
    }

    ZTeleResetClient(client, true);
    
    // Store location if not already stored.
    if (!ztele_spawned[client])
    {
        GetClientAbsOrigin(client, spawnLoc[client]);
    }
}

TeleportOnClientInfected(client)
{
    ztele_count[client] = 0;
    AbortTeleport(client);
}

public Action:Event_TeleporterStartup(Handle:timer)
{
    ztele_online = true;
    ztele_startup_timer = INVALID_HANDLE;
}

public Action:Event_TeleportCountdown(Handle:timer, any:client)
{
    ztele_countdown[client]--;
    if (ztele_countdown[client] <= 0)
    {
        KillTimer(ztele_countdown_timer[client]);
        ztele_countdown_timer[client] = INVALID_HANDLE;
        
        // Do teleport.
        TeleportClient(client, true);
    }
    else if ((ztele_countdown[client] % 3) == 0)
    {
        // Display countdown message.
        ZR_PrintToChat(client, "!ztele time left", ztele_countdown[client]);
    }
}

public Action:Event_TeleportCooldown(Handle:Timer, any:client)
{
    ztele_countdown[client]--;
    if (ztele_countdown[client] <= 0)
    {
        KillTimer(ztele_cooldown_timer[client]);
        ztele_cooldown_timer[client] = INVALID_HANDLE;
    }
}

public Action:Command_Teleport(client, argc)
{
    decl String:arg1[MAX_TARGET_LENGTH];
    decl String:target_name_list[192];
    decl String:target_name[192];
    decl String:client_name[192];
    new bool:tn_is_ml;
    
    GetClientName(client, client_name, sizeof(client_name));

    if (argc >= 1)
    {
        GetCmdArg(1, arg1, sizeof(arg1));
        new target_list[MAXPLAYERS];
        new target_count;
        
        if ((target_count = ProcessTargetString(
                arg1,
                client,
                target_list,
                MAXPLAYERS,
                COMMAND_FILTER_ALIVE,
                target_name_list,
                sizeof(target_name_list),
                tn_is_ml)) <= 0)
        {
            ReplyToTargetError(client, target_count);
            return Plugin_Handled;
        }
        
        for (new i = 0; i < target_count; i++)
        {
            if (IsPlayerAlive(client))
            {
                AbortTeleport(target_list[i]);
                TeleportClient(target_list[i], true, true);
                GetClientName(target_list[i], target_name, sizeof(target_name));
                if (LogCheckFlag(LOG_GAME_EVENTS, LOG_MODULE_TELEPORT)) LogMessageFormatted(client, "teleport", "manual teleport", "\"%s\" teleported \"%s\" to spawn.", true, client_name, target_name);
            }
        }
    }
    else
    {
        if (IsPlayerAlive(client))
        {
            AbortTeleport(client);
            TeleportClient(client, true, true);
            if (LogCheckFlag(LOG_GAME_EVENTS, LOG_MODULE_TELEPORT)) LogMessageFormatted(client, "teleport", "manual teleport", "\"%s\" self-teleported to spawn.", true, client_name);
        }
    }
    
    if (tn_is_ml)
    {
        ShowActivity2(client, "[ZR] ", "%t teleported to spawn.", target_name_list);
    }
    else
    {
        ShowActivity2(client, "[ZR] ", "%s teleported to spawn.", target_name);
    }
    
    return Plugin_Handled;
}

public Action:Command_TeleSaveLocation(client, argc)
{
    new String:target_name[MAX_TARGET_LENGTH];
    new String:target_client;
    
    if (argc >= 1)
    {
        GetCmdArg(1, target_name, sizeof(target_name));
        target_client = FindTarget(client, target_name);
    }
    else
    {
        target_client = client;
    }
    
    if (target_client > 0 && target_client <= MAXPLAYERS)
    {
        GetClientAbsOrigin(target_client, bufferLoc[client]);
        bufferLocSaved[client] = true;
        GetClientName(target_client, target_name, sizeof(target_name));
        ReplyToCommand(client, "Saved location to %s (x:%f, y:%f, z:%f).", target_name, bufferLoc[client][0], bufferLoc[client][1], bufferLoc[client][2]);

    }
    else
    {
        ReplyToCommand(client, "Unable to target %s", target_name);
    }
    
    return Plugin_Handled;
}

public Action:Command_TeleportToLocation(client, argc)
{
    decl String:client_name[64];
    decl String:target_name[64];
    new target_client;
    new Float:empty_vector[3] = {0.0, 0.0, 0.0};
    
    // Don't teleport if a location isn't saved yet.
    if (bufferLocSaved[client])
    {
        if (argc >= 1)
        {
            GetCmdArg(1, target_name, sizeof(target_name));
            target_client = FindTarget(client, target_name);
        }
        else
        {
            target_client = client;
        }
        
        if (target_client > 0 && target_client <= MAXPLAYERS)
        {
            if (client > 0)
            {
                GetClientName(client, client_name, sizeof(client_name));
            }
            else
            {
                client_name = "Console\0";
            }
            GetClientName(target_client, target_name, sizeof(target_name));
            
            if (IsPlayerAlive(target_client))
            {
                AbortTeleport(target_client);
                TeleportEntity(target_client, bufferLoc[client], NULL_VECTOR, empty_vector);
                ZR_PrintToChat(client, "!ztele successful");
                if (target_client != client) ZR_PrintToChat(target_client, "!ztele successful");
                if (LogCheckFlag(LOG_GAME_EVENTS, LOG_MODULE_TELEPORT)) LogMessageFormatted(client, "teleport", "custom teleport", "\"%s\" teleported \"%s\".", true, client_name, target_name);
            }
            else
            {
                ReplyToCommand(client, "Player \"%s\" is dead. Only alive players can be teleported.", target_name);
            }
        }
        else
        {
            ReplyToCommand(client, "Unable to target \"%s\"", target_name);
        }
    }
    else
    {
        ReplyToCommand(client, "Location not set.");
    }
    
    return Plugin_Handled;
}

public Action:Command_TeleportAbort(client, argc)
{
    new String:arg1[MAX_TARGET_LENGTH];
    new String:target_name[MAX_TARGET_LENGTH];
    new bool:tn_is_ml;
    
    if (argc >= 1)
    {
        GetCmdArg(1, arg1, sizeof(arg1));
        new target_list[MAXPLAYERS];
        new target_count;
        
        if ((target_count = ProcessTargetString(
                arg1,
                client,
                target_list,
                MAXPLAYERS,
                COMMAND_FILTER_ALIVE,
                target_name,
                sizeof(target_name),
                tn_is_ml)) <= 0)
        {
            ReplyToTargetError(client, target_count);
            return Plugin_Handled;
        }
        
        for (new i = 0; i < target_count; i++)
        {
            AbortTeleport(target_list[i]);
            LogAction(client, target_list[i], "%L aborted teleport on %L.", client, target_list[i]);
        }
    }
    else
    {
        AbortTeleport(client);
        GetClientName(client, target_name, sizeof(target_name));
        LogAction(client, client, "[ZR] Player %s teleported  %s to spawn.", target_name);
    }
    
    if (tn_is_ml)
    {
        ShowActivity2(client, "[ZR] ", "Aborted teleport and cooldown on %t.", target_name);
    }
    else
    {
        ShowActivity2(client, "[ZR] ", "Aborted teleport and cooldown on %s.", target_name);
    }
    
    return Plugin_Handled;
}

bool:ZTele(client)
{
    // Check if the teleporter is disabled.
    new bool:tele = GetConVarBool(g_hCvarsList[CVAR_ZTELE]);
    if (!tele)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // Check if the teleporter is online.
    if (!ztele_online)
    {
        ZR_PrintToChat(client, "!ztele offline");
        return false;
    }
    
    // Check if the player is alive and is not a spectactor.
    new team = GetClientTeam(client);
    if (!IsPlayerAlive(client) || (team != CS_TEAM_T && team != CS_TEAM_CT))
    {
        ZR_PrintToChat(client, "Must be alive");
        return false;
    }
    
    // Check if there's already a teleport in process.
    if (ztele_countdown_timer[client] != INVALID_HANDLE)
    {
        ZR_PrintToChat(client, "!ztele in progress");
        return false;
    }

    // Check if the cooldown isn't done yet.
    if (ztele_cooldown_timer[client] != INVALID_HANDLE)
    {
        ZR_PrintToChat(client, "!ztele cooldown");
        return false;
    }

    // Check limits.
    if (InfectIsClientHuman(client))
    {
        new human_limit = GetConVarInt(g_hCvarsList[CVAR_ZTELE_HUMAN_LIMIT]);
        new bool:tele_humans;
        if (human_limit == 0)
        {
            tele_humans = false;
        }
        else
        {
            tele_humans = true;
        }
        
        if (!tele_humans && g_bZombieSpawned)
        {
            ZR_PrintToChat(client, "!ztele humans restricted");
            return false;
        }
        
        if (human_limit > 0 && (ztele_count[client] >= human_limit))
        {
            ZR_PrintToChat(client, "!ztele limit reached");
            return false;    
        }
    }
    else
    {
        new zombie_limit = GetConVarInt(g_hCvarsList[CVAR_ZTELE_ZOMBIE_LIMIT]);
        new bool:tele_zombies;
        if (zombie_limit == 0)
        {
            tele_zombies = false;
        }
        else
        {
            tele_zombies = true;
        }
        
        if (!tele_zombies)
        {
            ZR_PrintToChat(client, "!ztele zombies restricted");
            return false;
        }
        
        if (zombie_limit > 0 && (ztele_count[client] >= zombie_limit))
        {
            ZR_PrintToChat(client, "!ztele limit reached");
            return false;
        }
    }
    
    TeleportClient(client);
    return true;
}

/*
 *  Note: free_tele only works if no_delay is true.
 */
TeleportClient(client, bool:no_delay = false, bool:free_tele = false, bool:no_cooldown = false)
{
    new teleports_left;
    new bool:teleports_unlimited = false;
    new Float:empty_vector[3] = {0.0, 0.0, 0.0};

    if (InfectIsClientHuman(client))
    {
        new human_delay = GetConVarInt(g_hCvarsList[CVAR_ZTELE_HUMAN_DELAY]);
        new human_limit = GetConVarInt(g_hCvarsList[CVAR_ZTELE_HUMAN_LIMIT]);
        if (human_delay > 0)
        {
            ztele_countdown[client] = human_delay;
        }
        else
        {
            no_delay = true;
        }
        
        if (human_limit > 0)
        {
            teleports_left = human_limit - ztele_count[client] - 1;
        }
        else
        {
            teleports_unlimited = true;
        }
    }
    else
    {
        new zombie_delay = GetConVarInt(g_hCvarsList[CVAR_ZTELE_ZOMBIE_DELAY]);
        new zombie_limit = GetConVarInt(g_hCvarsList[CVAR_ZTELE_ZOMBIE_LIMIT]);
        if (zombie_delay > 0)
        {
            ztele_countdown[client] = zombie_delay;
        }
        else
        {
            no_delay = true;
        }
        
        if (zombie_limit > 0)
        {
            teleports_left = zombie_limit - ztele_count[client] - 1;
        }
        else
        {
            teleports_unlimited = true;
        }
    }
    
    if (no_delay)
    {
        ztele_countdown[client] = -1;
        if (!free_tele) ztele_count[client]++;
        
        TeleportEntity(client, spawnLoc[client], NULL_VECTOR, empty_vector);
        
        // Create cooldown timer if enabled.
        new cooldown = GetConVarInt(g_hCvarsList[CVAR_ZTELE_COOLDOWN]);
        if (!no_cooldown && cooldown)
        {
            ztele_countdown[client] = cooldown;
            ztele_cooldown_timer[client] = CreateTimer(1.0, Event_TeleportCooldown, client, TIMER_REPEAT);
        }

        ZR_PrintToChat(client, "!ztele successful");
        if (!teleports_unlimited && !free_tele)
        {
            ZR_PrintToChat(client, "!ztele amount", teleports_left);
        }
    }
    else
    {
        ztele_countdown_timer[client] = CreateTimer(1.0, Event_TeleportCountdown, client, TIMER_REPEAT);
        if (!teleports_unlimited)
        {
            ZR_PrintToChat(client, "!ztele amount", teleports_left);
        }
    }
}

AbortTeleport(client, bool:abort_cooldown = true)
{
    ztele_countdown[client] = -1;
    
    // Stop any cooldown or teleportation in progress.
    if (ztele_countdown_timer[client] != INVALID_HANDLE)
    {
        KillTimer(ztele_countdown_timer[client]);
        ztele_countdown_timer[client] = INVALID_HANDLE;
    }
    if (abort_cooldown && ztele_cooldown_timer[client] != INVALID_HANDLE)
    {
        KillTimer(ztele_cooldown_timer[client]);
        ztele_cooldown_timer[client] = INVALID_HANDLE;
    }
}
