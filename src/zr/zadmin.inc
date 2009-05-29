/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          zadmin.inc
 *   Type:          Core
 *   Description:   Handle admin functions and menus.
 *
 * ============================================================================
 */

bool:ZRAdminMenu(client)
{
    if (!ZRIsClientAdmin(client))
    {
        TranslationPrintToChat(client, "Must be admin");
        return false;
    }
    
    new Handle:menu_zadmin = CreateMenu(ZRAdminMenuHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_zadmin, "%t\n ", "!zadmin main title");
    
    //decl String:knockbackm[64];
    //decl String:knockback[64];
    //decl String:nvgs[64];
    //decl String:infect[64];
    //decl String:zspawn[64];
    //decl String:ztele[64];
    decl String:weapons[64];
    //decl String:logflags[64];
    
    //Format(knockbackm, sizeof(knockbackm), "%t", "!zadmin main knockbackm");
    //Format(knockback, sizeof(knockback), "%t", "!zadmin main knockback");
    //Format(nvgs, sizeof(nvgs), "%t", "!zadmin main nvgs");
    //Format(infect, sizeof(infect), "%t", "!zadmin main infect");
    //Format(zspawn, sizeof(zspawn), "%t", "!zadmin main spawn");
    //Format(ztele, sizeof(ztele), "%t", "!zadmin main tele");
    Format(weapons, sizeof(weapons), "%t", "!zadmin main weapons");
    //Format(logflags, sizeof(logflags), "%t", "!zadmin main logflags");
    
    //AddMenuItem(menu_zadmin, "knockbackm", knockbackm, ITEMDRAW_DISABLED);
    //AddMenuItem(menu_zadmin, "knockback", knockback, ITEMDRAW_DISABLED);
    //AddMenuItem(menu_zadmin, "nvgs", nvgs, ITEMDRAW_DISABLED);
    //AddMenuItem(menu_zadmin, "infect", infect);
    //AddMenuItem(menu_zadmin, "zspawn", zspawn);
    //AddMenuItem(menu_zadmin, "ztele", ztele, ITEMDRAW_DISABLED);
    AddMenuItem(menu_zadmin, "weapons", weapons);
    //AddMenuItem(menu_zadmin, "logflags", logflags);
    
    // Set "Back" button.
    SetMenuExitBackButton(menu_zadmin, true);
    
    // Send menu to client.
    DisplayMenu(menu_zadmin, client, MENU_TIME_FOREVER);
    
    return true;
}

public ZRAdminMenuHandle(Handle:menu_admin, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            // Weapon management.
            case 0:
            {
                WeaponsMenuMain(client);
            }
        }
        
        // TODO: Resend menu if feature is disabled or unopenable.
    }
    
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            // Exit back to main menu.
            MenuMain(client);
        }
    }
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_admin);
    }
}