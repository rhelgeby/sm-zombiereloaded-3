/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        classmenus.inc
 *   Description: Provides functions for managing class menus.
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

/* ------------------------------------
 *
 * MAIN CLASS MENU
 *
 * ------------------------------------
 */

/**
 * Displays the main class menu with the players class settings.
 *
 * @param client    The client index.
 */
ClassMenuMain(client)
{
    new Handle:menu = CreateMenu(ClassMenuMainHandle);
    new itemdraw = (ZRIsClientAdmin(client)) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
    
    SetGlobalTransTarget(client);
    SetMenuTitle(menu, "%t\n", "!zclass title");
    
    decl String:zombieclass[128];
    decl String:humanclass[128];
    decl String:adminclass[128];
    
    decl String:zombieselect[128];
    decl String:humanselect[128];
    decl String:adminselect[128];
        
    decl String:inadminmnode[128];
    decl String:adminmode[128];
    decl String:toggleadminmode[128];
    
    // Check if the player is in admin mode.
    if (ClassPlayerInAdminMode[client])
    {
        // Notify the player.
        Format(inadminmnode, sizeof(inadminmnode), "%t\n", "!zclass admin mode enabled");
        AddMenuItem(menu, "", inadminmnode, ITEMDRAW_RAWLINE);
    }
    
    // List zombie class options.
    ClassGetName(ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES], zombieclass, sizeof(zombieclass), ZR_CLASS_CACHE_MODIFIED);
    Format(zombieselect, sizeof(zombieselect), "%t\n    %s", "!zclass zombie", zombieclass);
    AddMenuItem(menu, "", zombieselect);
    
    // List human class options.
    ClassGetName(ClassSelected[client][ZR_CLASS_TEAM_HUMANS], humanclass, sizeof(humanclass), ZR_CLASS_CACHE_MODIFIED);
    Format(humanselect, sizeof(humanselect), "%t\n    %s", "!zclass human", humanclass);
    AddMenuItem(menu, "", humanselect);
    
    if (ZRIsClientAdmin(client))
    {
        // List admin class options.
        ClassGetName(ClassSelected[client][ZR_CLASS_TEAM_ADMINS], adminclass, sizeof(adminclass), ZR_CLASS_CACHE_MODIFIED);
        Format(adminselect, sizeof(adminselect), "%t\n    %s", "!zclass admin", adminclass);
        AddMenuItem(menu, "", adminselect, itemdraw);
        
        // Set admin mode status string.
        if (ClassPlayerAdminMode[client])
        {
            Format(adminmode, sizeof(adminmode), "%t", "On");
        }
        else
        {
            Format(adminmode, sizeof(adminmode), "%t", "Off");
        }
        
        // Show admin mode toggle option.
        Format(toggleadminmode, sizeof(toggleadminmode), "%t\n    %s", "!zclass admin mode toggle", adminmode);
        AddMenuItem(menu, "", toggleadminmode);
    }
    
    SetMenuExitBackButton(menu, true);
    DisplayMenu(menu, client, MENU_TIME_FOREVER);
}

/**
 * Main class menu handle.
 */
public ClassMenuMainHandle(Handle:menu, MenuAction:action, client, slot)
{
    switch (action)
    {
        case MenuAction_Select:
        {
            switch(slot)
            {
                case 0:
                {
                    ClassMenuSelect(client, ZR_CLASS_TEAM_ZOMBIES);
                }
                case 1:
                {
                    ClassMenuSelect(client, ZR_CLASS_TEAM_HUMANS);
                }
                case 2:
                {
                    ClassMenuSelect(client, ZR_CLASS_TEAM_ADMINS);
                }
                case 3:
                {
                    // ClassMenuToggleAdmin(client);
                }
            }
        }
        case MenuAction_End:
        {
            CloseHandle(menu);
        }
        case MenuAction_Cancel:
        {
            if (slot == MenuCancel_ExitBack)
            {
                MainMenu(client);
            }
        }
    }
}

/* ------------------------------------
 *
 * ZOMBIE CLASS SELECTION MENU
 *
 * ------------------------------------
 */

/**
 * Displays the class selection menu.
 *
 * @param client    The client index.
 * @param teamid    What class types to display.
 */
ClassMenuSelect(client, teamid)
{
    new Handle:menu = CreateMenu(ClassMenuSelectHandle);
    new arraycount;
    new classindex;
    
    decl String:title[64];
    decl String:classname[64];
    decl String:description[256];
    decl String:menuitem[320];
    
    SetGlobalTransTarget(client);
    
    // Set correct menu title depending on team ID.
    switch (teamid)
    {
        case ZR_CLASS_TEAM_ZOMBIES:
        {
            Format(title, sizeof(title), "%t\n", "!zclass zombie");
        }
        case ZR_CLASS_TEAM_HUMANS:
        {
            Format(title, sizeof(title), "%t\n", "!zclass human");
        }
        case ZR_CLASS_TEAM_ADMINS:
        {
            Format(title, sizeof(title), "%t\n", "!zclass admin");
        }
    }
    SetMenuTitle(menu, title);
    
    // Create buffer array.
    new Handle:classarray = CreateArray();
    
    // Copy all class indexes into the array, with the specified team filter.
    if (ClassAddToArray(classarray, teamid))
    {
        // Get number of classes.
        arraycount = GetArraySize(classarray);
        
        // Loop through each class.
        for (new i = 0; i < arraycount; i++)
        {
            // Get index, name and description.
            classindex = GetArrayCell(classarray, i);
            ClassGetName(classindex, classname, sizeof(classname), ZR_CLASS_CACHE_MODIFIED);
            ClassGetDescription(classindex, description, sizeof(description), ZR_CLASS_CACHE_MODIFIED);
            
            // Add menu item.
            Format(menuitem, sizeof(menuitem), "%s\n    %s", classname, description);
            AddMenuItem(menu, classname, menuitem);
        }
    }
    else
    
    SetMenuExitBackButton(menu, true);
    DisplayMenu(menu, client, MENU_TIME_FOREVER);
}

/**
 * Class selection menu handle.
 */
public ClassMenuSelectHandle(Handle:menu, MenuAction:action, client, slot)
{
    decl String:classname[64];
    new classindex;
    new teamid;
    new bool:autoclose = GetConVarBool(gCvars[CVAR_MENU_AUTOCLOSE]);
    
    switch (action)
    {
        case MenuAction_Select:
        {
            // Get class name from the information string.
            GetMenuItem(menu, slot, classname, sizeof(classname));
            
            // Solve class index from the name.
            classindex = ClassGetIndex(classname);
            
            // Solve teamid from the class index.
            teamid = ClassGetTeamID(classindex, ZR_CLASS_CACHE_MODIFIED);
            
            // Set the players active class to the specified class.
            ClassSelected[client][teamid] = classindex;
        }
        case MenuAction_Cancel:
        {
            if (slot == MenuCancel_ExitBack)
            {
                ClassMenuMain(client);
            }
        }
        case MenuAction_End:
        {
            CloseHandle(menu);
        }
    }
    
    // Redisplay the main class menu if autoclose is disabled.
    if (!autoclose)
    {
        ClassMenuMain(client);
    }
}


ClassMenuToggleAdmin(client)
{
}
