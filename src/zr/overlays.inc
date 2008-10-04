/**
 * ====================
 *   Zombie:Reloaded
 *   File: overlays.inc
 *   Author: Greyscale
 * ==================== 
 */

ShowOverlays(Float:time, ZTeam:winner)
{
    new bool:overlays = GetConVarBool(gCvars[CVAR_OVERLAYS]);
    if (overlays)
    {
        decl String:overlay[64];
        if (winner == Human)
        {
            GetConVarString(gCvars[CVAR_OVERLAYS_HUMAN], overlay, sizeof(overlay));
        }
        else if (winner == Zombie)
        {
            GetConVarString(gCvars[CVAR_OVERLAYS_ZOMBIE], overlay, sizeof(overlay));
        }
            
        new maxplayers = GetMaxClients();
        for (new x = 1; x <= maxplayers; x++)
        {
            if (IsClientInGame(x))
            {
                DisplayClientOverlay(x, overlay);
            }
        }
        
        CreateTimer(time, KillOverlays);
    }
}

public Action:KillOverlays(Handle:timer)
{
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (IsClientInGame(x))
        {
            ClientCommand(x, "r_screenoverlay \"\"");
        }
    }
}