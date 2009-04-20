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
    
    // Get number of enabled classes per team.
    new zombiecount = ClassCountTeam(ZR_CLASS_TEAM_ZOMBIES);
    new humancount = ClassCountTeam(ZR_CLASS_TEAM_ZOMBIES);
    new admincount = ClassCountTeam(ZR_CLASS_TEAM_ZOMBIES);
    
    // Set draw style on class options depending on number of enabled classes.
    new zombie_itemdraw = (zombiecount > 1) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
    new human_itemdraw = (humancount > 1) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
    new admin_itemdraw = (admincount > 1) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
    
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
    AddMenuItem(menu, "", zombieselect, zombie_itemdraw);
    
    // List human class options.
    ClassGetName(ClassSelected[client][ZR_CLASS_TEAM_HUMANS], humanclass, sizeof(humanclass), ZR_CLASS_CACHE_MODIFIED);
    Format(humanselect, sizeof(humanselect), "%t\n    %s", "!zclass human", humanclass);
    AddMenuItem(menu, "", humanselect, human_itemdraw);
    
    // Only display admin class options for admins.
    if (ZRIsClientAdmin(client))
    {
        // List admin class options.
        ClassGetName(ClassSelected[client][ZR_CLASS_TEAM_ADMINS], adminclass, sizeof(adminclass), ZR_CLASS_CACHE_MODIFIED);
        Format(adminselect, sizeof(adminselect), "%t\n    %s", "!zclass admin", adminclass);
        AddMenuItem(menu, "", adminselect, admin_itemdraw);
        
        // Set admin mode status string.
        if (ClassPlayerAdminMode[client])
        {
            Format(adminmode, sizeof(adminmode), "%t", "On");
        }
        else
        {
            Format(adminmode, sizeof(adminmode), "%t", "Off");
        }
        
        // Spacer. ITEMDRAW_SPACER not used because it use a slot.
        AddMenuItem(menu, "", " ", ITEMDRAW_RAWLINE);
        
        // Show admin mode toggle option.
        Format(toggleadminmode, sizeof(toggleadminmode), "%t\n    %s", "!zclass admin mode toggle", adminmode);
        AddMenuItem(menu, "", toggleadminmode, admin_itemdraw);
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
                    // ClassToggleAdminMode(client);
                    ClassMenuMain(client);
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
    {
        // No classes found. Display message. The main class menu should
        // prevent this from happening, but we print a message just in case.
        Format(menuitem, sizeof(menuitem), "%t\n", "!zclass not found");
        AddMenuItem(menu, classname, menuitem, ITEMDRAW_RAWLINE);
    }
    
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
    new bool:autoclose = GetConVarBool(g_hCvarsList[CVAR_MENU_AUTOCLOSE]);
    
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

ClassToggleAdminMode(client)
{
    // TODO: Move to new file, adminmode.inc.
}
