/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        classmenus.inc
 *   Description: Provides functions for managing class menus.
 *   Author:      Richard Helgeby, Greyscale
 *
 * ============================================================================
 */

/*

Menu layouts

---------------------------------------
Class selection:

Admin mode is enabled!

1. Select Zombie Class
   - Classic
2. Select Human Class
   - Regular Human
3. Select Admin Class       <-- Only active for admins
   - Hidden Admin
(spacer)
4. Toggle Admin Mode
   - Disabled

0. Exit
---------------------------------------

*/

/**
 * Displays the main class menu with the players class settings.
 */
ClassMenuMain(client)
{
    new Handle:classmenu = CreateMenu(ClassMenuMainHandle);
    
    SetGlobalTransTarget(client);
    SetMenuTitle(classmenu, "%t\n", "!zclass title");
    
    decl String:zombieclass[128];
    decl String:humanclass[128];
    decl String:adminclass[128];
    
    decl String:zombieselect[128];
    decl String:humanselect[128];
    decl String:adminselect[128];
        
    decl String:inadminmnode[128];
    decl String:adminmnode[128];
    decl String:toggleadminmnode[128];
    
    // Check if the player is in admin mode.
    if (ClassPlayerInAdminMode(client))
    {
        // Notify the player.
        Format(adminmode, sizeof(adminmode), "%t\n", "!zclass admin mode enabled");
        AddMenuItem(classmenu, "", adminmode, ITEMDRAW_RAWLINE);
    }
    
    // List zombie class options.
    ClassGetName(ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES, zombieclass, sizeof(zombieclass), ZR_CLASS_CACHE_MODIFIED);
    Format(zombieselect, sizeof(zombieselect), "%t\n-%s", "!zclass zombie", zombieclass);
    AddMenuItem(classmenu, "", zombieselect);
    
    // List human class options.
    ClassGetName(client, humanclass, sizeof(zombieclass));
    Format(zombieselect, sizeof(zombieselect), "%t\n-%s", "!zclass human", humanclass);
    AddMenuItem(classmenu, "", zombieselect);
    
    // List admin class options.
    ClassGetName(client, adminclass, sizeof(adminclass));
    Format(adminselect, sizeof(adminselect), "%t\n-%s", "!zclass admin", adminclass);
    AddMenuItem(classmenu, "", adminselect);
    
    if (IsClientAdmin(client))
    {
        // Show admin mode toggle option.
        AddMenuItem(classmenu, "", " ", ITEMDRAW_SPACER);
        
        // TODO: Translate or use core phrases!
        if (ClassPlayerAdminMode[client])
        {
            Format(adminmnode, sizeof(adminmnode), "Enabled");
        }
        else
        {
            Format(adminmnode, sizeof(adminmnode), "Disabled");
        }
        
        Format(toggleadminmode, sizeof(toggleadminmode), "%t\n-%s", "!zclass admin mode toggle", adminmode);
    }
    
    /*for (new x = 0; x < classCount; x++)
    {
        GetClassName(x, display, sizeof(display));
        GetClassMenuDescription(x, menu_description, sizeof(menu_description));
        
        if (pNextClass[client] == -1)
        {
            if (x == pClass[client])
            {
                Format(display, sizeof(display), "%s (current)", display);
            }
        }
        else if (x == pNextClass[client])
        {
            Format(display, sizeof(display), "%s (current)", display);
        }
        
        Format(display, sizeof(display), "%s\n %s", display, menu_description);
        
        AddMenuItem(menu_classes, "", display);
    }
    
    SetMenuExitBackButton(menu_classes, true);*/
    
    DisplayMenu(classmenu, client, MENU_TIME_FOREVER);
}

ClassMenuMainHandle(Handle:classmenu, MenuAction:action, client, slot)
{
    
}