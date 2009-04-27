/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        (Core) event.inc
 *   Description: Event hooking and forwarding.
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
    ZR_PrintToChat(0, "Round objective");
    
    // Forward event to sub-modules.
    RoundEndOnRoundStart();
    InfectOnRoundStart();
    VEffectsOnRoundStart();
    SEffectsOnRoundStart();
    AntiStickOnRoundStart();
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
    ZTeleEnable();

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
    RoundEndOnRoundEnd(reason);
    InfectOnRoundEnd();
    RespawnOnRoundEnd();
    ZTeleReset();
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
    SetPlayerFOV(index, 90);
    ClientCommand(index, "r_screenoverlay \"\"");
    
    // Check if client is on a team.
    if (ZRIsClientOnTeam(index))
    {
        // Remove night vision.
        NightVisionOn(index, false);
        NightVision(index, false);
        
        if (g_bZombieSpawned)
        {
            if (ZRIsClientOnTeam(index, CS_TEAM_T))
            {
                CS_SwitchTeam(index, CS_TEAM_CT);
                CS_RespawnPlayer(index);
            }
        }
        else
        {
            SetPlayerAlpha(index, 255);
        }
    }
    
    // Forward event to modules.
    InfectOnClientSpawn(index);
    ClassOnClientSpawn(index);          // Module event depends on infect module.
    SEffectsOnClientSpawn(index);
    AccountOnClientSpawn(index);
    SpawnProtectOnClientSpawn(index);
    RespawnOnClientSpawn(index);
    ZHPOnClientSpawn(index);
    ZTeleClientSpawned(index);
    
    ZR_PrintToChat(index, "!zmenu reminder");
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
            AddPlayerScore(attacker, bonus);
        }
    }
        
    // Forward event to modules.
    ClassOnClientDeath(index);
    RoundEndOnClientDeath();
    SEffectsOnClientDeath(index);
    SpawnProtectOnClientDeath(index);
    RespawnOnClientDeath(index, attacker, weapon);
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
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    
    new Float:distance = ClassGetJumpDistance(client);
    new Float:height = ClassGetJumpHeight(client);
    
    JumpBoost(client, distance, height);
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
