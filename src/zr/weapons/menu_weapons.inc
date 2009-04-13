/**
 * ====================
 *   Zombie:Reloaded
 *   File: menu_weapons.inc
 *   Author: Greyscale
 * ==================== 
 */

/**
 * Weapons Menus
 */
enum WeaponsMenu
{
    Weapon,
    WeaponGroup,
}

/**
 * Array to store the client's current weapon menu
 */
new WeaponsMenu:curMenuWeapons[MAXPLAYERS + 1];

/**
 * Sends main weapon menu to client.
 * @param client    The client index.
 */  
WeaponsMenuMain(client)
{
    // Create menu handle
    new Handle:menu_weapons_main = CreateMenu(WeaponsMenuMainHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_weapons_main, "%t\n ", "Weapons menu main title");
    
    decl String:toggleweaponrestriction[64];
    decl String:togglewgrouprestriction[64];
    
    decl String:zmarket[64];
    
    Format(toggleweaponrestriction, sizeof(toggleweaponrestriction), "%t", "Weapons menu main toggle weapon restrict");
    Format(togglewgrouprestriction, sizeof(togglewgrouprestriction), "%t", "Weapons menu main toggle weapon group restrict");
    Format(zmarket, sizeof(zmarket), "%t", "Weapons menu main market");
    
    AddMenuItem(menu_weapons_main, "toggleweaponrestriction", toggleweaponrestriction);
    AddMenuItem(menu_weapons_main, "togglewgrouprestriction", togglewgrouprestriction, ITEMDRAW_DISABLED);
    
    // Disable market option if market isn't installed
    if (market)
    {
        AddMenuItem(menu_weapons_main, "zmarket", zmarket);
    }
    else
    {
        AddMenuItem(menu_weapons_main, "zmarket", zmarket, ITEMDRAW_DISABLED);
    }
    
    // Create a "Back" button to the weapons main menu
    SetMenuExitBackButton(menu_weapons_main, true);
    
    // Send menu
    DisplayMenu(menu_weapons_main, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapons main menu, and handles it.
 * @param menu_weapons_main     Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public WeaponsMenuMainHandle(Handle:menu_weapons_main, MenuAction:action, client, slot)
{
    // Client selected an option
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                WeaponsMenuWeapons(client, Weapon);
            }
            case 1:
            {
                WeaponsMenuWeapons(client, WeaponGroup);
            }
            case 2:
            {
                // WeaponsMenuMarket(client);
            }
        }
    }
    // Client closed the menu
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button
        if (slot == MenuCancel_ExitBack)
        {
            ZRAdminMenu(client);
        }
    }
    // Client hit "Exit" button
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_weapons_main);
    }
}

/**
 * Sends weapon list menu to client.
 * @param client    The client index.
 */ 
WeaponsMenuWeapons(client, WeaponsMenu:type)
{
    // Set the current action client is performing on a weapon (see enum WeaponsMenu)
    curMenuWeapons[client] = type;
    
    // Create menu handle
    new Handle:menu_weapons_weapons = CreateMenu(WeaponsMenuWeaponsHandle);
    
    SetGlobalTransTarget(client);
    
    // If client wants to perform an action on a single weapon, show weapon list
    switch(curMenuWeapons[client])
    {
        case Weapon:
        {
            SetMenuTitle(menu_weapons_weapons, "%t\n ", "Weapons menu weapons weapon title");
            
            decl String:weapon[WEAPONS_MAX_LENGTH];
            decl String:display[WEAPONS_MAX_LENGTH + 1];
            new Handle:arrayWeapons = INVALID_HANDLE;
            new size = WeaponsCreateWeaponArray(arrayWeapons);
            
            // x = Array index
            for (new x = 0; x < size; x++)
            {
                GetArrayString(arrayWeapons, x, weapon, sizeof(weapon));
                
                new bool:menu = WeaponsIsWeaponMenu(weapon);
                
                strcopy(display, sizeof(display), weapon);
                
                if (RestrictIsWeaponRestricted(weapon))
                {
                    Format(display, sizeof(display), "%s*", weapon);
                }
                
                // If weapon restriction is blocked for the menu, disable option
                if (menu)
                {
                    AddMenuItem(menu_weapons_weapons, weapon, display);
                }
                else
                {
                    AddMenuItem(menu_weapons_weapons, weapon, display, ITEMDRAW_DISABLED);
                }
            }
            
            // Kill the array handle
            CloseHandle(arrayWeapons);
        }
        // If client wants to perform an action on a weapon group, show custom group list
        case WeaponGroup:
        {
            SetMenuTitle(menu_weapons_weapons, "%t\n ", "Weapons menu weapons group title");
            
            decl String:weapongroup[WEAPONS_MAX_LENGTH];
            decl String:display[WEAPONS_MAX_LENGTH + 1];
            new Handle:arrayWeaponGroups = INVALID_HANDLE;
            new size = RestrictCreateGroupArray(arrayWeaponGroups);
            
            // x = Array index
            for (new x = 0; x < size; x++)
            {
                GetArrayString(arrayWeaponGroups, x, weapongroup, sizeof(weapongroup));
                
                strcopy(display, sizeof(display), weapongroup);
                
                if (RestrictIsGroupRestricted(weapongroup))
                {
                    Format(display, sizeof(display), "%s*", weapongroup);
                }
                
                AddMenuItem(menu_weapons_weapons, weapongroup, display);
            }
            
            // Kill the array handle
            CloseHandle(arrayWeaponGroups);
        }
    }
    SetMenuExitBackButton(menu_weapons_weapons, true);
        
    DisplayMenu(menu_weapons_weapons, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapons list menu, and handles it.
 * @param menu_weapons_main     Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public WeaponsMenuWeaponsHandle(Handle:menu_weapons_weapons, MenuAction:action, client, slot)
{
    // Client selected an option
    if (action == MenuAction_Select)
    {
        decl String:weapon[WEAPONS_MAX_LENGTH];
        GetMenuItem(menu_weapons_weapons, slot, weapon, sizeof(weapon));
        
        new WpnRestrictQuery:output;
        
        // Set restrict to true if were restricting a weapon/group, false if not
        new bool:restrict;
        if (RestrictIsWeaponGroup(weapon))
        {
            restrict = RestrictIsGroupRestricted(weapon);
        }
        else
        {
            restrict = RestrictIsWeaponRestricted(weapon);
        }
        
        if (!restrict)
        {
            decl String:display[WEAPONS_MAX_LENGTH];
            
            output = RestrictRestrict(weapon, display);
            
            switch(output)
            {
                case Successful_Weapon:
                {
                    ZR_PrintToChat(0, "Restrict weapon", weapon);
                }
                case Successful_Group:
                {
                    decl String:weaponlist[128];
                    RestrictGetGroupWeapons(weapon, weaponlist, sizeof(weaponlist), ", ");
                    
                    ZR_PrintToChat(0, "Restrict custom weapon group", weapon, weaponlist);
                }
                case Failed_Weapon:
                {
                    ZR_PrintToChat(client, "Restrict weapon failed", weapon);
                }
                case Failed_Group:
                {
                    decl String:weaponlist[128];
                    RestrictGetGroupWeapons(weapon, weaponlist, sizeof(weaponlist), ", ");
                    
                    ZR_PrintToChat(client, "Restrict custom weapon group failed", weapon, weaponlist);
                }
                case Invalid:
                {
                    ZR_PrintToChat(client, "Weapon invalid", weapon);
                }
            }
        }
        else
        {
            decl String:display[WEAPONS_MAX_LENGTH];
            
            output = RestrictUnrestrict(weapon, display);
            
            switch(output)
            {
                case Successful_Weapon:
                {
                    ZR_PrintToChat(0, "Unrestrict weapon", weapon);
                }
                case Successful_Group:
                {
                    decl String:weaponlist[128];
                    RestrictGetGroupWeapons(weapon, weaponlist, sizeof(weaponlist), ", ");
                    
                    ZR_PrintToChat(0, "Unrestrict custom weapon group", weapon, weaponlist);
                }
                case Failed_Weapon:
                {
                    ZR_PrintToChat(client, "Unrestrict weapon failed", weapon);
                }
                case Invalid:
                {
                    ZR_PrintToChat(client, "Weapon invalid", weapon);
                }
            }
        }
        
        // Resend menu
        WeaponsMenuWeapons(client, curMenuWeapons[client]);
    }
    // Client closed the menu
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button
        if (slot == MenuCancel_ExitBack)
        {
            WeaponsMenuMain(client);
        }
    }
    // Client hit "Exit" button
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_weapons_weapons);
    }
}