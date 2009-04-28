/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        menu.inc
 *   Description: Base menu functions for the plugin.
 *
 * ============================================================================
 */

/**
 * Show main menu to client.
 * 
 * @param client    The client index.
 */
MenuMain(client)
{
    // Create menu handle.
    new Handle:menu_main = CreateMenu(MenuMainHandle);
    
    // Make client global translations target.
    SetGlobalTransTarget(client);
    
    // Set menu title.
    SetMenuTitle(menu_main, "%t\n ", "!zmenu title");
    
    // Initialize menu lines.
    decl String:zadmin[64];
    decl String:zclass[64];
    decl String:zspawn[64];
    decl String:ztele[64];
    decl String:zhp[64];
    decl String:zmarket[64];
    
    // Translate each line into client's language.
    Format(zadmin, sizeof(zadmin), "%t", "Menu main zadmin");
    Format(zclass, sizeof(zclass), "%t", "Menu main zclass");
    Format(zspawn, sizeof(zspawn), "%t", "Menu main zspawn");
    Format(ztele, sizeof(ztele), "%t", "Menu main ztele");
    Format(zhp, sizeof(zhp), "%t", "Menu main zhp");
    Format(zmarket, sizeof(zmarket), "%t", "Menu main zmarket");
    
    // Add items to menu.
    
    // Disable option if client isn't an admin.
    new bool:admin = ZRIsClientAdmin(client);
    AddMenuItem(menu_main, "zadmin", zadmin, MenuGetItemDraw(admin));
    
    AddMenuItem(menu_main, "zclass", zclass);
    AddMenuItem(menu_main, "zspawn", zspawn);
    AddMenuItem(menu_main, "ztele", ztele);
    AddMenuItem(menu_main, "zhp", zhp);
    AddMenuItem(menu_main, "zmarket", zmarket, MenuGetItemDraw(g_bMarket));
    
    // Display menu to client.
    DisplayMenu(menu_main, client, MENU_TIME_FOREVER);
}

/**
 * Menu callback (main)
 * Redirects client to selected option's handle code.
 * 
 * @param menu      The menu handle.
 * @param action    Action client is doing in menu.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public MenuMainHandle(Handle:menu, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Create variable to possible resend menu later.
        new bool:resend = true;
        
        switch(slot)
        {
            // Selected zadmin.
            case 0:
            {
                // Copy return to resend variable.
                resend = !ZRAdminMenu(client);
            }
            case 1:
            {
                // Send class menu
                ClassMenuMain(client);
                
                // Don't resend this menu.
                resend = false;
            }
            case 2:
            {
                // Copy return to resend variable.
                resend = !ZMarketSend(client);
            }
            case 3:
            {
                // Send zspawn command from client.
                ZSpawnClient(client);
            }
            case 4:
            {
                // Copy return to resend variable.
                resend = !ZTele(client);
            }
            case 5:
            {
                // Toggle ZHP.
                ZHPToggle(client);
            }
        }
        
        // Resend is still true, then resend menu.
        if (resend)
        {
            MenuMain(client);
        }
    }
    // Client exited menu.
    if (action == MenuAction_End)
    {
        CloseHandle(menu);
    }
}

/**
 * Return itemdraw flag for SM menus.
 * 
 * @param condition     If this is true, item will be drawn normally.
 */
MenuGetItemDraw(bool:condition)
{
    return condition ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
}