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
    Restrict_Weapon,
    Unrestrict_Weapon,
    Restrict_Group,
    Unrestrict_Group,
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
    
    decl String:restrictweapon[64];
    decl String:unrestrictweapon[64];
    decl String:restrictwgroup[64];
    decl String:unrestrictwgroup[64];
    decl String:zmarket[64];
    
    Format(restrictweapon, sizeof(restrictweapon), "%t", "Weapons menu main restrict weapon");
    Format(unrestrictweapon, sizeof(unrestrictweapon), "%t", "Weapons menu main unrestrict weapon");
    Format(restrictwgroup, sizeof(restrictwgroup), "%t", "Weapons menu main restrict weapon group");
    Format(unrestrictwgroup, sizeof(unrestrictwgroup), "%t", "Weapons menu main unrestrict weapon group");
    Format(zmarket, sizeof(zmarket), "%t", "Weapons menu main market");
    
    AddMenuItem(menu_weapons_main, "restrictweapon", restrictweapon);
    AddMenuItem(menu_weapons_main, "unrestrictweapon", unrestrictweapon);
    AddMenuItem(menu_weapons_main, "restrictwgroup", restrictwgroup);
    AddMenuItem(menu_weapons_main, "unrestrictwgroup", unrestrictwgroup);
    
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
                WeaponsMenuWeapons(client, Restrict_Weapon);
            }
            case 1:
            {
                WeaponsMenuWeapons(client, Unrestrict_Weapon);
            }
            case 2:
            {
                WeaponsMenuWeapons(client, Restrict_Group);
            }
            case 3:
            {
                WeaponsMenuWeapons(client, Unrestrict_Group);
            }
            case 4:
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
    
    SetMenuTitle(menu_weapons_weapons, "%t\n ", "Weapons menu weapons title");
    
    // If client wants to perform an action on a single weapon, show weapon list
    if (curMenuWeapons[client] == Restrict_Weapon || curMenuWeapons[client] == Unrestrict_Weapon)
    {
        decl String:weapon[WEAPONS_MAX_LENGTH];
        new Handle:arrayWeapons = INVALID_HANDLE;
        new size = WeaponsCreateWeaponArray(arrayWeapons);
        
        // x = Array index
        for (new x = 0; x < size; x++)
        {
            GetArrayString(arrayWeapons, x, weapon, sizeof(weapon));
            
            new bool:menu = WeaponsIsWeaponMenu(weapon);
            
            // If weapon restriction is blocked for the menu, disable option
            if (menu)
            {
                AddMenuItem(menu_weapons_weapons, weapon, weapon);
            }
            else
            {
                AddMenuItem(menu_weapons_weapons, weapon, weapon, ITEMDRAW_DISABLED);
            }
        }
        
        // Kill the array handle
        CloseHandle(arrayWeapons);
    }
    // If client wants to perform an action on a weapon group, show custom group list
    else if (curMenuWeapons[client] == Restrict_Group || curMenuWeapons[client] == Unrestrict_Group)
    {
        decl String:weapongroup[WEAPONS_MAX_LENGTH];
        new Handle:arrayWeaponGroups = INVALID_HANDLE;
        new size = WeaponsCreateWeaponGroupArray(arrayWeaponGroups);
        
        // x = Array index
        for (new x = 0; x < size; x++)
        {
            GetArrayString(arrayWeaponGroups, x, weapongroup, sizeof(weapongroup));
            
            AddMenuItem(menu_weapons_weapons, weapongroup, weapongroup);
        }
        
        // Kill the array handle
        CloseHandle(arrayWeaponGroups);
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
        
        // If client is restricting a weapon or group, then call the restrict function
        if (curMenuWeapons[client] == Restrict_Weapon || curMenuWeapons[client] == Restrict_Group)
        {
            output = WeaponRestrictRestrict(weapon);
            
            switch(output)
            {
                case Successful_Weapon:
                {
                    ZR_PrintToChat(0, "Restrict weapon", weapon);
                }
                case Successful_Group:
                {
                    decl String:weaponlist[128];
                    WeaponRestrictGetWeaponList(weapon, weaponlist, sizeof(weaponlist), ", ");
                    
                    ZR_PrintToChat(0, "Restrict custom weapon group", weapon, weaponlist);
                }
                case Failed_Weapon:
                {
                    ZR_PrintToChat(client, "Restrict weapon failed", weapon);
                }
                case Invalid:
                {
                    ZR_PrintToChat(client, "Weapon invalid", weapon);
                }
            }
        }
        // If client is unrestricting a weapon or group, then call the unrestrict function
        else if (curMenuWeapons[client] == Unrestrict_Weapon || curMenuWeapons[client] == Unrestrict_Group)
        {
            output = WeaponRestrictUnrestrict(weapon);
            
            switch(output)
            {
                case Successful_Weapon:
                {
                    ZR_PrintToChat(0, "Unrestrict weapon", weapon);
                }
                case Successful_Group:
                {
                    decl String:weaponlist[128];
                    WeaponRestrictGetWeaponList(weapon, weaponlist, sizeof(weaponlist), ", ");
                    
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