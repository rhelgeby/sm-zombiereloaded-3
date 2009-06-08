/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          ragdoll.inc
 *   Type:          Module
 *   Description:   Remove ragdolls with optional effects.
 *
 * ============================================================================
 */

#define VEFFECTS_RAGDOLL_DISSOLVE_EFFECTLESS    -1
#define VEFFECTS_RAGDOLL_DISSOLVE_ENERGY        0
#define VEFFECTS_RAGDOLL_DISSOLVE_ELECTRICALH   1
#define VEFFECTS_RAGDOLL_DISSOLVE_ELECTRICALL   2
#define VEFFECTS_RAGDOLL_DISSOLVE_CORE          3

/**
 * Variable to store ragdoll offset value.
 */
new g_iToolsRagdoll;

/**
 * Find VAmbience-specific offsets here.
 */
RagdollOnOffsetsFound()
{
    // If offset "m_iAccount" can't be found, then stop the plugin.
    g_iToolsRagdoll = FindSendPropInfo("CCSPlayer", "m_hRagdoll");
    if (g_iToolsRagdoll == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_VEffects, "Offsets", "Offset \"CCSPlayer::m_hRagdoll\" was not found.");
    }
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
RagdollOnClientDeath(client)
{
    new ragdoll = RagdollGetClientRagdoll(client);
    
    // If the ragdoll is invalid, then stop.
    if (ragdoll == -1)
    {
        return;
    }
    
    // Get the delay.
    new Float:dissolvedelay = GetConVarFloat(g_hCvarsList[CVAR_VEFFECTS_RAGDOLL_DELAY]);
    
    // If the delay is 0 or less, then remove right now.
    if (dissolvedelay <= 0)
    {
        RagdollTimer(INVALID_HANDLE, ragdoll);
        return;
    }
    
    // Create a timer to remove/dissolve ragdoll.
    CreateTimer(dissolvedelay, RagdollTimer, ragdoll, TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * Removed a ragdoll from the game following plugin settings.
 * 
 * @param ragdoll   The ragdoll index.
 */
RagdollRemove(ragdoll)
{
    // Get the dissolve type.
    new dissolve = GetConVarInt(g_hCvarsList[CVAR_VEFFECTS_RAGDOLL_DISSOLVE]);
    
    if (dissolve == VEFFECTS_RAGDOLL_DISSOLVE_EFFECTLESS)
    {
        // Remove entity from world.
        RemoveEdict(ragdoll);
        return;
    }
    
    // Prep the ragdoll for dissolving.
    decl String:targetname[64];
    Format(targetname, sizeof(targetname), "zr_dissolve_%d", ragdoll);
    DispatchKeyValue(ragdoll, "targetname", targetname);
    
    // Prep the dissolve entity.
    new dissolver = CreateEntityByName("env_entity_dissolver");
    
    // Set the target to the ragdoll.
    DispatchKeyValue(dissolver, "target", targetname);
    
    // Set the dissolve type.
    decl String:dissolvetype[16];
    Format(dissolvetype, sizeof(dissolvetype), "%d", dissolve);
    DispatchKeyValue(dissolver, "dissolvetype", dissolvetype);
    
    // Tell the entity to dissolve the ragdoll.
    AcceptEntityInput(dissolver, "Dissolve");
    
    // Remove the dissolver.
    RemoveEdict(dissolver); 
}

/**
 * Timer callback.  Removed a client's ragdoll.
 * 
 * @param timer     The timer handle. 
 * @param ragdoll   The ragdoll index.
 */
public Action:RagdollTimer(Handle:timer, any:ragdoll)
{
    // If ragdoll removal is disabled, then stop.
    new bool:ragdollremove = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_RAGDOLL_REMOVE]);
    if (!ragdollremove)
    {
        return;
    }
    
    // If the ragdoll is already gone, then stop.
    if (!IsValidEdict(ragdoll))
    {
        return;
    }
    
    // Remove the ragdoll.
    RagdollRemove(ragdoll);
}

/** Finds the ragdoll entity of a client.
 * @param client    The client index.
 * @return          The entity index of the client's ragdoll, -1 if none exists.
 */
RagdollGetClientRagdoll(client)
{
    return GetEntDataEnt2(client, g_iToolsRagdoll);
}