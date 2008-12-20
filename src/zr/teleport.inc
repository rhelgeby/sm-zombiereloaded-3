/**
 * ====================
 *   Zombie:Reloaded
 *   File: teleport.inc
 *   Authors: Richard Helgeby / Cpt.Moore
 * ==================== 
 */

ZTeleEnable()
{
    ztele_online = false;
    if (ztele_startup_timer != INVALID_HANDLE)
    {
        KillTimer(ztele_startup_timer);
    }
    new Float:startup_delay = GetConVarFloat(gCvars[CVAR_ZTELE_STARTUP_DELAY]);
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
        
        // Create cooldown timer if enabled.
        new cooldown = GetConVarInt(gCvars[CVAR_ZTELE_COOLDOWN]);
        if (cooldown)
        {
            ztele_countdown[client] = cooldown;
            ztele_cooldown_timer[client] = CreateTimer(1.0, Event_TeleportCooldown, client, TIMER_REPEAT);
        }
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
    // Check (on all specified clients) if a teleport/cooldown is in progress.
    // If so, kill those timers.

    // No cooldown when using this command.
    return Plugin_Handled;
}

public Action:Command_TeleSaveLocation(client, argc)
{
    return Plugin_Handled;
}

public Action:Command_TeleportToLocation(client, argc)
{
    // Don't teleport if a location isn't saved yet.
    // To do: Find or make a function to check if a vector array is a null vector.
    /*if (bufferLoc[] != NULL_VECTOR)
    {
        return Plugin_Handled;
    }
    else
    {
        ZR_PrintToChat(client, "!ztele location not set");
        return Plugin_Handled;
    }*/
}

ZTeleClientCheck(client)
{
    new bool:tele = GetConVarBool(gCvars[CVAR_ZTELE]);
    if (!tele)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        return;
    }
    
    if (!ztele_online)
    {
        ZR_PrintToChat(client, "!ztele offline");
        return;
    }
    
    // Check if there's already a teleport in process.
    if (ztele_countdown_timer[client] != INVALID_HANDLE)
    {
        ZR_PrintToChat(client, "!ztele in progress");
        return;
    }

    // Check if the cooldown isn't done yet.
    if (ztele_cooldown_timer[client] != INVALID_HANDLE)
    {
        ZR_PrintToChat(client, "!ztele cooldown");
        return;
    }

    // Check limits.
    if (IsPlayerHuman(client))
    {
        new human_limit = GetConVarInt(gCvars[CVAR_ZTELE_HUMAN_LIMIT]);
        new bool:tele_humans;
        if (human_limit == 0)
        {
            tele_humans = false;
        }
        else
        {
            tele_humans = true;
        }
        
        if (!tele_humans && zombieSpawned)
        {
            ZR_PrintToChat(client, "!ztele humans restricted");
            return;
        }
        
        if (human_limit > 0 && (ztele_count[client] >= human_limit))
        {
            ZR_PrintToChat(client, "!ztele limit reached");
            return;         
        }
    }
    else
    {
        new zombie_limit = GetConVarInt(gCvars[CVAR_ZTELE_ZOMBIE_LIMIT]);
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
            return;
        }
        
        if (zombie_limit > 0 && (ztele_count[client] >= zombie_limit))
        {
            ZR_PrintToChat(client, "!ztele limit reached");
            return;         
        }
    }
    
    TeleportClient(client);
}

TeleportClient(client, bool:no_delay = false)
{
    new teleports_left;
    new bool:teleports_unlimited = false;
    
    if (IsPlayerHuman(client))
    {
        new human_delay = GetConVarInt(gCvars[CVAR_ZTELE_HUMAN_DELAY]);
        new human_limit = GetConVarInt(gCvars[CVAR_ZTELE_HUMAN_LIMIT]);
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
        new zombie_delay = GetConVarInt(gCvars[CVAR_ZTELE_ZOMBIE_DELAY]);
        new zombie_limit = GetConVarInt(gCvars[CVAR_ZTELE_ZOMBIE_LIMIT]);
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
        ztele_count[client]++;
        TeleportEntity(client, spawnLoc[client], NULL_VECTOR, NULL_VECTOR);
        ZR_PrintToChat(client, "!ztele successful");
        if (!teleports_unlimited)
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

AbortTeleport(client)
{
    ztele_countdown[client] = -1;
    
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