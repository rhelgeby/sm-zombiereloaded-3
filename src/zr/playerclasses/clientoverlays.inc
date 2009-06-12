/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          clientoverlays.inc
 *  Type:          Core 
 *  Description:   Handles overlays on clients, as a part of class attributes.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * @section Suicide intercept defines.
 */
#define CLASSOVERLAY_TOGGLE_MAX_CMDS 5
#define CLASSOVERLAY_TOGGLE_MAX_LENGTH 16
/**
 * @endsection
 */
 
/**
 * Array to store default class overlay enable flag.
 */
new bool:h_bClassOverlay[MAXPLAYERS + 1];

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
ClassOverlayClientInit(client)
{
    // Get overlay toggle cvar values.
    new bool:overlaytoggle = GetConVarBool(g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLE]);
    new bool:overlaydefault = GetConVarBool(g_hCvarsList[CVAR_CLASSES_OVERLAY_DEFAULT]);
    
    // Apply default value if toggle is enabled, default to true if toggle is disabled.
    h_bClassOverlay[client] = overlaytoggle ? overlaydefault : true;
}

/**
 * Hook commands related to overlay here.
 */
ClassOverlayOnCommandsHook()
{
    // Create command callbacks (intercepts) for listed suicide commands.
    decl String:togglecmds[CLASSOVERLAY_TOGGLE_MAX_CMDS * CLASSOVERLAY_TOGGLE_MAX_LENGTH];
    GetConVarString(g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLECMDS], togglecmds, sizeof(togglecmds));
    
    // Create array to store cmds
    new String:arrayCmds[CLASSOVERLAY_TOGGLE_MAX_CMDS][CLASSOVERLAY_TOGGLE_MAX_LENGTH];
    
    // Explode string into array indexes.
    new cmdcount = ExplodeString(togglecmds, ", ", arrayCmds, CLASSOVERLAY_TOGGLE_MAX_CMDS, CLASSOVERLAY_TOGGLE_MAX_LENGTH);
    
    // x = Array index.
    // arrayCmds[x] = suicide command.
    for (new x = 0; x <= cmdcount - 1; x++)
    {
        // Prepare intercept for this command.
        RegConsoleCmd(arrayCmds[x], ClassOverlayEnableCommand);
    }
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
ClassOverlayOnClientSpawn(client)
{
    // If overlay toggle is enabled and class has an overlay, then send center text.
    new bool:overlaytoggle = GetConVarBool(g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLE]);
    
    decl String:overlaypath[PLATFORM_MAX_PATH];
    ClassGetOverlayPath(client, overlaypath, sizeof(overlaypath));
    
    if (overlaytoggle && overlaypath[0])
    {
        decl String:togglecmds[CLASSOVERLAY_TOGGLE_MAX_CMDS * CLASSOVERLAY_TOGGLE_MAX_LENGTH];
        GetConVarString(g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLECMDS], togglecmds, sizeof(togglecmds));
        
        TranslationPrintHUDText(client, "Classes overlay toggle", togglecmds);
    }
    
    // Update class overlay.
    OverlaysClientUpdateOverlay(client, OVERLAYS_CHANNEL_CLASSES);
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
ClassOverlayOnClientDeath(client)
{
    // Disable overlay.
    OverlaysClientSetChannelState(client, OVERLAYS_CHANNEL_CLASSES, true, false, false, true);
}

/**
 * Client has been infected.
 * 
 * @param client    The client index.
 */
ClassOverlayOnClientInfected(client)
{
    // If overlay toggle is enabled and class has an overlay, then send center text.
    new bool:overlaytoggle = GetConVarBool(g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLE]);
    
    decl String:overlaypath[PLATFORM_MAX_PATH];
    ClassGetOverlayPath(client, overlaypath, sizeof(overlaypath));
    
    if (overlaytoggle && overlaypath[0])
    {
        decl String:togglecmds[CLASSOVERLAY_TOGGLE_MAX_CMDS * CLASSOVERLAY_TOGGLE_MAX_LENGTH];
        GetConVarString(g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLECMDS], togglecmds, sizeof(togglecmds));
        
        TranslationPrintCenterText(client, "Classes overlay toggle", togglecmds);
    }
}

ClassOverlayInitialize(client, const String:overlay[])
{
    if (IsFakeClient(client))
    {
        return;
    }
    
    // If overlay path is empty, then disable channel, then stop.
    if (!overlay[0])
    {
        OverlaysClientSetChannelState(client, OVERLAYS_CHANNEL_CLASSES, true, false, false, true);
        return;
    }
    
    // Display class overlays.
    OverlaysClientSetChannelPath(client, OVERLAYS_CHANNEL_CLASSES, overlay);
    OverlaysClientSetChannelState(client, OVERLAYS_CHANNEL_CLASSES, true, false, h_bClassOverlay[client]);
}

/**
 * Command callback. (See zr_classes_overlay_togglecmds)
 * Toggles nightvision of a client.
 *   
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ClassOverlayEnableCommand(client, argc)
{
    // If overlay toggle is disabled, then stop.
    new bool:overlaytoggle = GetConVarBool(g_hCvarsList[CVAR_CLASSES_OVERLAY_TOGGLE]);
    if (!overlaytoggle)
    {
        return;
    }
    
    // Toggle current overlay channel, and retrieve new value.
    h_bClassOverlay[client] = OverlaysClientSetChannelState(client, OVERLAYS_CHANNEL_CLASSES, true, true);
}
