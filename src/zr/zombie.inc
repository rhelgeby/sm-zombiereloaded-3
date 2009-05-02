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
    
    bClientOverlayOn[client] = !bClientOverlayOn[client];
    
    decl String:overlay[PLATFORM_MAX_PATH];
    ClassGetOverlayPath(client, overlay, sizeof(overlay));
    
    if (strlen(overlay) > 0)
    {
        if (bClientOverlayOn[client])
        {
            ClassOverlayInitialize(client, overlay);
        }
        else
        {
            ClassOverlayStop(client);
        }
    }
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
