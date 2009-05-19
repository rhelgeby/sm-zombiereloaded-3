/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          event.inc
 *   Type:          Core
 *   Description:   Event hooking and forwarding.
 *
 * ============================================================================
 */

/**
 * Init function for event module.
 */
EventInit()
{
    // Hook all events used by plugin.
    EventHook();
}

/**
 * Hook events used by plugin.
 * 
 * @param unhook    If true, then unhook all events, if false, then hook.
 */ 
EventHook(bool:unhook = false)
{
    // If unhook is true, then continue.
    if (unhook)
    {
        // Unhook all events.
        UnhookEvent("round_start", EventRoundStart);
        UnhookEvent("round_freeze_end", EventRoundFreezeEnd);
        UnhookEvent("round_end", EventRoundEnd);
        UnhookEvent("player_team", EventPlayerTeam, EventHookMode_Pre);
        UnhookEvent("player_spawn", EventPlayerSpawn);
        UnhookEvent("player_hurt", EventPlayerHurt);
        UnhookEvent("player_death", EventPlayerDeath);
        UnhookEvent("player_jump", EventPlayerJump);
        UnhookEvent("weapon_fire", EventWeaponFire);
        
        // Stop after unhooking events.
        return;
    }
    
    // Hook all events used by plugin.
    HookEvent("round_start", EventRoundStart);
    HookEvent("round_freeze_end", EventRoundFreezeEnd);
    HookEvent("round_end", EventRoundEnd);
    HookEvent("player_team", EventPlayerTeam, EventHookMode_Pre);
    HookEvent("player_spawn", EventPlayerSpawn);
    HookEvent("player_hurt", EventPlayerHurt);
    HookEvent("player_death", EventPlayerDeath);
    HookEvent("player_jump", EventPlayerJump);
    HookEvent("weapon_fire", EventWeaponFire);
}

/**
 * Event callback (round_start)
 * The round is starting.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventRoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Print round objective to all clients.
    TranslationPrintToChatAll(true, false, "General round objective");
    
    // Forward event to sub-modules.
    OverlaysOnRoundStart();
    WeaponsOnRoundStart();
    RoundEndOnRoundStart();
    InfectOnRoundStart();
    SEffectsOnRoundStart();
    AntiStickOnRoundStart();
    ZSpawnOnRoundStart();
    VolOnRoundStart();
}

/**
 * Event callback (round_freeze_end)
 * The freeze time is ending.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventRoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    RemoveObjectives();
    
    // Forward events to modules.
    RoundEndOnRoundFreezeEnd();
    InfectOnRoundFreezeEnd();
    ZSpawnOnRoundFreezeEnd();
}

/**
 * Event callback (round_end)
 * The round is ending.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventRoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new reason = GetEventInt(event, "reason");
    
    // Forward event to modules.
    WeaponsOnRoundEnd();
    RoundEndOnRoundEnd(reason);
    InfectOnRoundEnd();
    SEffectsOnRoundEnd();
    RespawnOnRoundEnd();
    ZSpawnOnRoundEnd();
    VolOnRoundEnd();
}

/**
 * Event callback (player_team)
 * Client is joining a team.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventPlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new team = GetEventInt(event, "team");
    
    // Forward event to modules.
    InfectOnClientTeam(index, team);
    
    return Plugin_Handled;
}

/**
 * Event callback (player_spawn)
 * Client is spawning into the game.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventPlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    
    // Reset FOV and overlay.
    ToolsSetClientDefaultFOV(index, 90);
    ClientCommand(index, "r_screenoverlay \"\"");
    
    // Check if client is on a team.
    if (ZRIsClientOnTeam(index))
    {
        // Turn off nightvision.
        ToolsClientNightVision(index, false, false);
        
        // Take nightvision away.
        ToolsClientNightVision(index, false);
        
        if (g_bZombieSpawned)
        {
            if (ZRIsClientOnTeam(index, CS_TEAM_T))
            {
                CS_SwitchTeam(index, CS_TEAM_CT);
                CS_RespawnPlayer(index);
            }
        }
    }
    
    // Forward event to modules.
    InfectOnClientSpawn(index);
    ClassOnClientSpawn(index);      // Module event depends on infect module.
    RestrictOnClientSpawn(index);
    SEffectsOnClientSpawn(index);
    AccountOnClientSpawn(index);
    SpawnProtectOnClientSpawn(index);
    RespawnOnClientSpawn(index);
    ZTeleOnClientSpawn(index);
    ZHPOnClientSpawn(index);
    
    TranslationPrintToChat(index, "General zmenu reminder");
    
    // Fire post player_spawn event.
    CreateTimer(0.0, EventPlayerSpawnPost, index);
}

/**
 * Event callback (player_spawn)
 * Client is spawning into the game. *Post
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventPlayerSpawnPost(Handle:timer, any:index)
{
    // If client isn't in-game, then stop.
    if (!IsClientInGame(index))
    {
        return;
    }
    
    // Forward event to modules.
    SpawnProtectOnClientSpawnPost(index);
}

/**
 * Event callback (player_hurt)
 * Client is being hurt.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventPlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));
    new hitgroup = GetEventInt(event, "hitgroup");
    new dmg_health = GetEventInt(event, "dmg_health");
    
    decl String:weapon[WEAPONS_MAX_LENGTH];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // Forward event to modules.
    ClassAlphaUpdate(index);
    InfectOnClientHurt(index, attacker, weapon);
    SEffectsOnClientHurt(index);
    KnockbackOnClientHurt(index, attacker, weapon, hitgroup, dmg_health);
    NapalmOnClientHurt(index, weapon);
    ZHPOnClientHurt(index);
}

/**
 * Event callback (player_death)
 * Client has been killed.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventPlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get the weapon name.
    decl String:weapon[WEAPONS_MAX_LENGTH];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // If client is being infected, then stop.
    if (StrEqual(weapon, "zombie_claws_of_death", false))
    {
        return;
    }
    
    // Get all required event info.
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));

    // Extinguish any flames to stop burning sounds.
    ExtinguishEntity(index);
    
    // If the attacker is valid, then continue.
    if (ZRIsClientValid(attacker))
    {
        // If the client is a zombie, then continue.
        if (InfectIsClientInfected(index))
        {
            // Add kill bonus to attacker's score.
            new bonus = ClassGetKillBonus(attacker);
            new score = ToolsClientScore(index, true, false);
            ToolsClientScore(index, true, true, score + bonus);
        }
    }
        
    // Forward event to modules.
    ClassOnClientDeath(index);
    RoundEndOnClientDeath();
    SEffectsOnClientDeath(index);
    SpawnProtectOnClientDeath(index);
    RespawnOnClientDeath(index, attacker, weapon);
    ZSpawnOnClientDeath(index);
    ZTeleOnClientDeath(index);
    ZHPOnClientDeath(index);
}

/**
 * Event callback (player_jump)
 * Client is jumping.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventPlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    
    // Forward event to modules.
    JumpBoostOnClientJump(index);
}

/**
 * Event callback (weapon_fire)
 * Weapon has been fired.
 * 
 * @param event     The event handle.
 * @param name      Name of the event.
 * @dontBroadcast   If true, event is broadcasted to all clients, false if not.
 */
public Action:EventWeaponFire(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    decl String:weapon[32];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // Forward event to modules.
    NapalmOnWeaponFire(weapon);
}
