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
 * Array to store the client's current weapon group menu
 */ 
new String:curMenuGroup[WEAPONS_MAX_LENGTH][MAXPLAYERS + 1];

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
    AddMenuItem(menu_weapons_main, "togglewgrouprestriction", togglewgrouprestriction);
    
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
                WeaponsMenuMarket(client);
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
                
                strcopy(display, sizeof(display), weapon);
                
                if (RestrictIsWeaponRestricted(weapon))
                {
                    Format(display, sizeof(display), "%s*", weapon);
                }
                
                // If weapon restriction is blocked for the menu, disable option
                new bool:menu = WeaponsIsWeaponMenu(weapon);
                
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
                
                if (RestrictIsPartialRestricted(weapongroup))
                {
                    Format(display, sizeof(display), "%s*", weapongroup);
                }
                else if (RestrictIsGroupRestricted(weapongroup))
                {
                    Format(display, sizeof(display), "%s**", weapongroup);
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
        
        switch(curMenuWeapons[client])
        {
            // Client is restricting a single weapon
            case Weapon:
            {
                new WpnRestrictQuery:output;
                
                if (!RestrictIsWeaponRestricted(weapon))
                {
                    output = RestrictRestrict(weapon);
                    RestrictPrintRestrictOutput(client, output, weapon, false);
                }
                else
                {
                    output = RestrictUnrestrict(weapon);
                    RestrictPrintUnrestrictOutput(client, output, weapon, false);
                }
                
                // Resend menu
                WeaponsMenuWeapons(client, curMenuWeapons[client]);
            }
            // Client is accessing a weapon group
            case WeaponGroup:
            {
                // Send weapon group menu
                WeaponsMenuWeaponGroup(client, weapon);
            }
        }
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

WeaponsMenuWeaponGroup(client, const String:weapongroup[])
{
    strcopy(curMenuGroup[client], WEAPONS_MAX_LENGTH, weapongroup);
    
    // Create menu handle
    new Handle:menu_weapons_groupweapon = CreateMenu(WeaponsMenuWeaponGroupHandle);
    
    SetMenuTitle(menu_weapons_groupweapon, "%t\n ", "Weapons menu weapon group title", weapongroup);
    
    decl String:restrictall[64];
    decl String:unrestrictall[64];
    
    Format(restrictall, sizeof(restrictall), "%t", "Weapons menu weapon group restrict all");
    Format(unrestrictall, sizeof(unrestrictall), "%t", "Weapons menu weapon group unrestrict all");
    
    if (RestrictIsGroupRestricted(weapongroup))
    {
        AddMenuItem(menu_weapons_groupweapon, "restrictall", restrictall, ITEMDRAW_DISABLED);
    }
    else
    {
        AddMenuItem(menu_weapons_groupweapon, "restrictall", restrictall);
    }
    
    if (RestrictIsGroupUnrestricted(weapongroup))
    {
        AddMenuItem(menu_weapons_groupweapon, "unrestrictall", unrestrictall, ITEMDRAW_DISABLED);
    }
    else
    {
        AddMenuItem(menu_weapons_groupweapon, "unrestrictall", unrestrictall);
    }
    
    decl String:groupweapon[WEAPONS_MAX_LENGTH];
    decl String:display[WEAPONS_MAX_LENGTH + 1];
    new Handle:arrayGroupWeapons = INVALID_HANDLE;
    new size = RestrictCreateGroupWeaponsArray(arrayGroupWeapons, weapongroup);
    
    // x = Array index
    for (new x = 0; x < size; x++)
    {
        GetArrayString(arrayGroupWeapons, x, groupweapon, sizeof(groupweapon));
        
        strcopy(display, sizeof(display), groupweapon);
        
        if (RestrictIsWeaponRestricted(groupweapon))
        {
            Format(display, sizeof(display), "%s*", groupweapon);
        }
        
        AddMenuItem(menu_weapons_groupweapon, groupweapon, display);
    }
    
    // Kill the array handle
    CloseHandle(arrayGroupWeapons);
    
    SetMenuExitBackButton(menu_weapons_groupweapon, true);
        
    DisplayMenu(menu_weapons_groupweapon, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapon group menu, and handles it.
 * @param menu_weapons_main     Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public WeaponsMenuWeaponGroupHandle(Handle:menu_weapons_groupweapon, MenuAction:action, client, slot)
{
    // Client selected an option
    if (action == MenuAction_Select)
    {
        switch(slot)
        {   
            case 0:
            {
                new WpnRestrictQuery:output = RestrictRestrict(curMenuGroup[client]);
                RestrictPrintRestrictOutput(client, output, curMenuGroup[client], false);
            }
            case 1:
            {
                new WpnRestrictQuery:output = RestrictUnrestrict(curMenuGroup[client]);
                RestrictPrintUnrestrictOutput(client, output, curMenuGroup[client], false);
            }
            
            default:
            {
                new WpnRestrictQuery:output;
                
                decl String:groupweapon[WEAPONS_MAX_LENGTH];
                GetMenuItem(menu_weapons_groupweapon, slot, groupweapon, sizeof(groupweapon));
                
                if (!RestrictIsWeaponRestricted(groupweapon))
                {
                    output = RestrictRestrict(groupweapon);
                    RestrictPrintRestrictOutput(client, output, groupweapon, false);
                }
                else
                {
                    output = RestrictUnrestrict(groupweapon);
                    RestrictPrintUnrestrictOutput(client, output, groupweapon, false);
                }
            }
        }
        
        // Resend menu
        WeaponsMenuWeaponGroup(client, curMenuGroup[client]);
    }
    // Client closed the menu
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button
        if (slot == MenuCancel_ExitBack)
        {
            WeaponsMenuWeapons(client, curMenuWeapons[client]);
        }
    }
    // Client hit "Exit" button
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_weapons_groupweapon);
    }
}

/**
 * Sends market options menu to client.
 * @param client    The client index.
 */  
WeaponsMenuMarket(client)
{
    // Create menu handle
    new Handle:menu_weapons_market = CreateMenu(WeaponsMenuMarketHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_weapons_market, "%t\n ", "Weapons menu market title");
    
    decl String:togglebuyzone[64];
    
    decl String:curSetting[8];
    BoolToConfigOption(GetConVarBool(gCvars[CVAR_ZMARKET_BUYZONE]), curSetting, sizeof(curSetting));
    
    Format(togglebuyzone, sizeof(togglebuyzone), "%t", "Weapons menu market toggle buyzone", curSetting);
    
    AddMenuItem(menu_weapons_market, "togglebuyzone", togglebuyzone);
    
    // Create a "Back" button to the weapons main menu
    SetMenuExitBackButton(menu_weapons_market, true);
    
    // Send menu
    DisplayMenu(menu_weapons_market, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapons main menu, and handles it.
 * @param menu_weapons_main     Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public WeaponsMenuMarketHandle(Handle:menu_weapons_market, MenuAction:action, client, slot)
{
    // Client selected an option
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                if (GetConVarBool(gCvars[CVAR_ZMARKET_BUYZONE]))
                {
                    SetConVarBool(gCvars[CVAR_ZMARKET_BUYZONE], false);
                }
                else
                {
                    SetConVarBool(gCvars[CVAR_ZMARKET_BUYZONE], true);
                }
            }
        }
        
        // Resend menu
        WeaponsMenuMarket(client);
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
        CloseHandle(menu_weapons_market);
    }
}