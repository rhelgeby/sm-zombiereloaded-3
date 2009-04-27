/**
 * ====================
 *   Zombie:Reloaded
 *   File: zombie.inc
 *   Author: Greyscale
 * ==================== 
 */
        
HookCommands()
{
    RegConsoleCmd("nightvision", Command_NightVision);
}

public Action:Command_NightVision(client, argc)
{
    new bool:allow_disable = GetConVarBool(g_hCvarsList[CVAR_ZVISION_ALLOW_DISABLE]);
    if (!allow_disable)
    {
        return;
    }
    
    new bool:enabled = GetConVarBool(g_hCvarsList[CVAR_ENABLE]);
    if (!enabled)
    {
        return;
    }
    
    bClientOverlayOn[client] = !bClientOverlayOn[client];
    
    decl String:overlay[PLATFORM_MAX_PATH];
    ClassGetOverlayPath(client, overlay, sizeof(overlay));
    
    if (strlen(overlay) > 0)
    {
        if (ClassOverlayIsOn(client))
        {
            ClassOverlayInitialize(client, overlay);
        }
        else
        {
            ClassOverlayStop(client);
        }
    }
}

JumpBoost(client, Float:distance, Float:height)
{
    new Float:vel[3];
    
    GetPlayerVelocity(client, vel);
    
    vel[0] *= distance;
    vel[1] *= distance;
    vel[2] += height;
    
    SetPlayerVelocity(client, vel, false);
}

RemoveObjectives()
{
    decl String:classname[64];
    
    new maxentities = GetMaxEntities();
    for (new x = 0; x <= maxentities; x++)
    {
        if(!IsValidEdict(x))
        {
            continue;
        }
        
        GetEdictClassname(x, classname, sizeof(classname));
        if( StrEqual(classname, "func_bomb_target") ||
            StrEqual(classname, "func_hostage_rescue") ||
            StrEqual(classname, "c4") ||
            StrEqual(classname, "hostage_entity"))
            {
                RemoveEdict(x);
            }
    }
}