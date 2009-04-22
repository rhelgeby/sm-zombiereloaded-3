/**
 * ====================
 *   Zombie:Reloaded
 *   File: events.inc
 *   Author: Greyscale
 * ==================== 
 */

HookEvents()
{
    HookEvent("round_start", RoundStart);
    HookEvent("round_freeze_end", RoundFreezeEnd);
    HookEvent("round_end", RoundEnd);
    HookEvent("player_team", PlayerTeam, EventHookMode_Pre);
    HookEvent("player_spawn", PlayerSpawn);
    HookEvent("player_hurt", PlayerHurt);
    HookEvent("player_death", PlayerDeath);
    HookEvent("player_jump", PlayerJump);
    HookEvent("weapon_fire", WeaponFire);
}

UnhookEvents()
{
    UnhookEvent("round_start", RoundStart);
    UnhookEvent("round_freeze_end", RoundFreezeEnd);
    UnhookEvent("round_end", RoundEnd);
    UnhookEvent("player_team", PlayerTeam, EventHookMode_Pre);
    UnhookEvent("player_spawn", PlayerSpawn);
    UnhookEvent("player_hurt", PlayerHurt);
    UnhookEvent("player_death", PlayerDeath);
    UnhookEvent("player_jump", PlayerJump);
    UnhookEvent("weapon_fire", WeaponFire);
}

public Action:RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{   
    ChangeLightStyle();
    
    ZR_PrintToChat(0, "Round objective");
    
    // Forward event to sub-modules.
    RoundEndOnRoundStart();
    InfectOnRoundStart();
    SEffectsOnRoundStart();
    AntiStickOnRoundStart();
}

public Action:RoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    RemoveObjectives();
    
    // Forward events to modules.
    RoundEndOnRoundFreezeEnd();
    InfectOnRoundFreezeEnd();
    ZTeleEnable();

}

public Action:RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new reason = GetEventInt(event, "reason");
    
    // Forward event to modules.
    RoundEndOnRoundEnd(reason);
    InfectOnRoundEnd();
    ZTeleReset();
}

public Action:PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new team = GetEventInt(event, "team");
    
    // Forward event to modules.
    InfectOnClientTeam(index, team);
    
    return Plugin_Handled;
}

public Action:PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
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
    ClassOnClientSpawn(index);
    InfectOnClientSpawn(index);
    SEffectsOnClientSpawn(index);
    AccountOnClientSpawn(index);
    SpawnProtectOnClientSpawn(index);
    RespawnOnClientSpawn(index);
    ZHPOnClientSpawn(index);
    ZTeleClientSpawned(index);
    
    ZR_PrintToChat(index, "!zmenu reminder");
}

public Action:PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new index = GetClientOfUserId(GetEventInt(event, "userid"));
    new attacker = GetClientOfUserId(GetEventInt(event, "attacker"));
    new hitgroup = GetEventInt(event, "hitgroup");
    new dmg_health = GetEventInt(event, "dmg_health");
    
    decl String:weapon[32];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // Forward event to modules.
    ClassAlphaUpdate(index);
    InfectOnClientHurt(index, attacker, weapon);
    SEffectsOnClientHurt(index);
    KnockbackOnClientHurt(index, attacker, weapon, hitgroup, dmg_health);
    NapalmOnClientHurt(index, weapon);
    ZHPOnClientHurt(index);
}

public Action:PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get the weapon name.
    decl String:weapon[32];
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
    if (ZRIsValidClient(attacker))
    {
        // If the client is a zombie, then continue.
        if (IsPlayerZombie(index))
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

public Action:PlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    
    new Float:distance = ClassGetJumpDistance(client);
    new Float:height = ClassGetJumpHeight(client);
        
    JumpBoost(client, distance, height);
}

public Action:WeaponFire(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Get all required event info.
    decl String:weapon[32];
    GetEventString(event, "weapon", weapon, sizeof(weapon));
    
    // Forward event to modules.
    NapalmOnWeaponFire(weapon);
}
