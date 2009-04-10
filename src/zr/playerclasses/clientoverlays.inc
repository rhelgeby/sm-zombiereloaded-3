/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        clientoverlays.inc
 *   Description: Handles overlays on clients, as a part of class attributes.
 *   Author:      Richard Helgeby
 *
 * ============================================================================
 */

/**
 * Timer handles for redisplaying overlays on clients.
 */
new Handle:tOverlay[MAXPLAYERS + 1];

/**
 * Specifies if a client have a overlay.
 */
new bClientHasOverlay[MAXPLAYERS + 1];

/**
 * Tells wether the overlay is on or not.
 */
new bClientOverlayOn[MAXPLAYERS + 1];

/**
 * Path to the currently active overlay.
 */
new String:ActiveOverlay[MAXPLAYERS + 1][256];

bool:ClientHasOverlay(client)
{
    return bClientHasOverlay[client];
}

ClassOverlayInitialize(client, const String:overlay[])
{
    if (IsFakeClient(client))
    {
        return;
    }
    
    if (strlen(overlay) == 0)
    {
        bClientHasOverlay[client] = false;
    }
    else
    {
        bClientHasOverlay[client] = true;
        strcopy(ActiveOverlay[client], PLATFORM_MAX_PATH, overlay);
        
        ClassOverlayStart(client);
    }
}

ClassOverlayStart(client)
{
    // Kill timer if it exist.
    if (tOverlay[client] != INVALID_HANDLE)
    {
        KillTimer(tOverlay[client]);
        tOverlay[client] = INVALID_HANDLE;
    }
    
    ClientCommand(client, "r_screenoverlay \"%s\"", ActiveOverlay[client]);
    bClientOverlayOn[client] = true;
    
    new Float:redisplay = GetConVarFloat(gCvars[CVAR_OVERLAY_REDISPLAY]);
    tOverlay[client] = CreateTimer(redisplay, ClassOverlayTimer, client, TIMER_REPEAT);
}

ClassOverlayStop(client)
{
    // Kill timer if it exist.
    if (tOverlay[client] != INVALID_HANDLE)
    {
        KillTimer(tOverlay[client]);
        tOverlay[client] = INVALID_HANDLE;
    }
    
    // Disable client overlay.
    ClientCommand(client, "r_screenoverlay \"\"");
    bClientOverlayOn[client] = false;
}

public Action:ClassOverlayTimer(Handle:timer, any:client)
{
    if (!IsClientInGame(client) || !IsPlayerZombie(client))
    {
        tOverlay[client] = INVALID_HANDLE;
        return Plugin_Stop;
    }
    
    ClientCommand(client, "r_screenoverlay \"%s\"", ActiveOverlay[client]);
    return Plugin_Continue;
}
