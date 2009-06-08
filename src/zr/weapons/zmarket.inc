/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          zmarket.inc
 *   Type:          Module
 *   Description:   ZMarket module, provides menu of weapons to buy from.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/* *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

/**
 * Variable to store buyzone offset value.
 */
new g_iToolsInBuyZone;

/**
 * Array to store the client's current weapon type within menu.
 */
new g_iZMarketCurType[MAXPLAYERS + 1];

/**
 * Array handle to store the amount of purchases left for a weapon.
 */
new Handle:g_hZMarketPurchaseCount[MAXPLAYERS + 1];

/**
 * Array to store a weapon of each type last purchased from ZMarket.
 */
new String:g_strZMarketLastWeapon[MAXPLAYERS + 1][WeaponsSlot][WEAPONS_MAX_LENGTH];

/**
 * Array to store clients' auto-rebuy setting.
 */
new bool:g_bZMarketAutoRebuy[MAXPLAYERS + 1];

/**
 * Initialize market data.
 */
ZMarketInit()
{
}

/**
 * Find ZMarket-specific offsets here.
 */
ZMarketOnOffsetsFound()
{
    // If offset "m_bInBuyZone" can't be found, then stop the plugin.
    g_iToolsInBuyZone = FindSendPropInfo("CCSPlayer", "m_bInBuyZone");
    if (g_iToolsInBuyZone == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CCSPlayer::m_bInBuyZone\" was not found.");
    }
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
ZMarketClientInit(client)
{
    // If purchase count data exists, then destroy before creating again.
    if (g_hZMarketPurchaseCount[client] != INVALID_HANDLE)
    {
        CloseHandle(g_hZMarketPurchaseCount[client]);
    }
    
    // Create a new array handle to store purchase count data for client.
    g_hZMarketPurchaseCount[client] = CreateTrie();
    
    // Initialize auto-rebuy data.
    g_bZMarketAutoRebuy[client] = false;
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
ZMarketOnClientDisconnect(client)
{
    // Destroy ZMarket array data for client.
    CloseHandle(g_hZMarketPurchaseCount[client]);
    g_hZMarketPurchaseCount[client] = INVALID_HANDLE;
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
ZMarketOnClientSpawn(client)
{
    // Reset purchase counts for client.
    ZMarketResetPurchaseCount(client);
    
    // If auto-rebuy is enabled, then force client to rebuy weapons.
    if (g_bZMarketAutoRebuy[client])
    {
        ZMarketRebuy(client);
    }
}

/**
 * Reset the purchase count(s) for a client.
 * 
 * @param client    The client index.
 */
ZMarketResetPurchaseCount(client)
{
    // Clear out the trie of all data.
    ClearTrie(g_hZMarketPurchaseCount[client]);
}

/**
 * Set the purchases left for a client of a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon name.
 * @param value     The amount of purchases left for the weapon.
 */
ZMarketSetPurchaseCount(client, const String:weapon[], value, bool:add = false)
{
    // Initialize variable (value is 0)
    new purchasemax;
    
    // If we are adding, then update variable with current ammo value.
    if (add)
    {
        purchasemax = ZMarketGetPurchaseCount(client, weapon);
    }
    
    // Set value in client's trie.
    SetTrieValue(g_hZMarketPurchaseCount[client], weapon, purchasemax + value);
}

/**
 * Get the purchases left for a client of a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon name.
 * @param return    The amount of purchases left for the weapon.
 */
ZMarketGetPurchaseCount(client, const String:weapon[])
{
    // Get value in client's trie.
    new value;
    GetTrieValue(g_hZMarketPurchaseCount[client], weapon, value);
    return value;
}

/**
 * Sends weapon type list to client.
 * @param client    The client index.
 * @return          True if the menu was sent successfully, false if not.
 */
bool:ZMarketMenuTypes(client)
{
    // If weapons module is disabled, then stop.
    new bool:weapons = GetConVarBool(g_hCvarsList[CVAR_WEAPONS]);
    if (!weapons)
    {
        // Telle client feature is disabled.
        TranslationPrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // If ZMarket module is disabled, then stop.
    new bool:zmarket = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET]);
    if (!zmarket)
    {
        // Telle client feature is disabled.
        TranslationPrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // Create menu handle.
    new Handle:menu_zmarket_types = CreateMenu(ZMarketMenuTypesHandle);
    
    // Set translation target as client.
    SetGlobalTransTarget(client);
    
    // Set menu title.
    SetMenuTitle(menu_zmarket_types, "%t\n ", "Weapons menu zmarket types title");
    
    // If rebuy is enabled, then add it to the menu.
    new bool:zmarketrebuy = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_REBUY]);
    new bool:zmarketrebuyauto = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_REBUY_AUTO]);
    
    // If rebuy or auto-rebuy is enabled, then add it to the menu.
    if (zmarketrebuy || zmarketrebuyauto)
    {
        // Add "Rebuy" option.
        decl String:rebuy[WEAPONS_MAX_LENGTH];
        Format(rebuy, sizeof(rebuy), "%t", "Weapons menu zmarket types rebuy");
        AddMenuItem(menu_zmarket_types, "Rebuy", rebuy);
    }
    
    // If auto-rebuy is enabled, then add it to the menu.
    if (zmarketrebuyauto)
    {
        // Get current auto-rebuy setting.
        decl String:rebuyautosetting[8];
        ConfigBoolToSetting(g_bZMarketAutoRebuy[client], rebuyautosetting, sizeof(rebuyautosetting));
        
        // Add "Auto-Rebuy" option.
        decl String:rebuyauto[WEAPONS_MAX_LENGTH];
        Format(rebuyauto, sizeof(rebuyauto), "%t", "Weapons menu zmarket types auto-rebuy", rebuyautosetting);
        AddMenuItem(menu_zmarket_types, "Auto-Rebuy", rebuyauto);
    }
    
    decl String:typename[WEAPONS_MAX_LENGTH];
    
    // x = Array index.
    new size = GetArraySize(arrayWeaponTypes);
    for (new x = 0; x < size; x++)
    {
        // Get name of type.
        RestrictWeaponTypeGetName(x, typename, sizeof(typename));
        
        // Add item to menu.
        AddMenuItem(menu_zmarket_types, typename, typename);
    }
    
    // If there are no weapons, add an "(Empty)" line.
    if (size == 0)
    {
        decl String:empty[64];
        Format(empty, sizeof(empty), "%t", "Menu empty");
        
        AddMenuItem(menu_zmarket_types, "empty", empty, ITEMDRAW_DISABLED);
    }
    
    // Set exit back button.
    SetMenuExitBackButton(menu_zmarket_types, true);
        
    DisplayMenu(menu_zmarket_types, client, MENU_TIME_FOREVER);
    
    // Menu sent successfully.
    return true;
}

/**
 * Called when client selects option in the weapons list menu, and handles it.
 * @param menu_zmarket_types    Handle of the menu being used.
 * @param action                The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                The client index.
 * @param slot                  The slot index selected (starting from 0).
 */ 
public ZMarketMenuTypesHandle(Handle:menu_zmarket_types, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        decl String:weapontype[WEAPONS_MAX_LENGTH];
        GetMenuItem(menu_zmarket_types, slot, weapontype, sizeof(weapontype));
        
        if (StrEqual(weapontype, "Rebuy"))
        {
            // Force client to rebuy weapons.
            ZMarketRebuy(client);
            
            // Resend menu.
            ZMarketMenuTypes(client);
            
            return;
        }
        
        if (StrEqual(weapontype, "Auto-Rebuy"))
        {
            // Toggle rebuy.
            g_bZMarketAutoRebuy[client] = !g_bZMarketAutoRebuy[client];
            
            // Resend menu.
            ZMarketMenuTypes(client);
            
            return;
        }
        
        // Get weapon index.
        new weapontypeindex = RestrictTypeToIndex(weapontype);
        
        // If weapon type index is -1, then something went very wrong.
        if (weapontypeindex == -1)
        {
            // Resend menu.
            ZMarketMenuTypes(client);
            
            return;
        }
        
        // Menu slot index is = weapon type index.
        g_iZMarketCurType[client] = weapontypeindex;
        
        // Send weapons of the selected type in a menu to client.
        ZMarketMenuTypeWeapons(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            MenuMain(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zmarket_types);
    }
}

/**
 * Sends a list of weapons of a certain type in a menu to the client.
 * @param client    The client index.
 */
ZMarketMenuTypeWeapons(client)
{
    // Create menu handle.
    new Handle:menu_zmarket_typeweapons = CreateMenu(ZMarketMenuTypeWeaponsHandle);
    
    // Get name of current weapon type.
    decl String:typename[WEAPONS_MAX_LENGTH];
    RestrictWeaponTypeGetName(g_iZMarketCurType[client], typename, sizeof(typename));
    
    // Set menu title.
    SetMenuTitle(menu_zmarket_typeweapons, "%t\n ", "Weapons menu zmarket types weapon type title", typename);
    
    decl String:typeweapon[WEAPONS_MAX_LENGTH];
    decl String:typeweapondisplay[WEAPONS_MAX_LENGTH];
    decl String:display[128];
    
    // Get an array populated with all weapons of the given type.
    new Handle:arrayTypeWeapons;
    new count = RestrictGetTypeWeapons(g_iZMarketCurType[client], arrayTypeWeapons);
    
    // x = Array index.
    for (new x = 0; x < count; x++)
    {
        // Get weapon index to check restricted status of.
        new weaponindex = GetArrayCell(arrayTypeWeapons, x);
        
        // Get name of weapon.
        WeaponsGetName(weaponindex, typeweapon, sizeof(typeweapon));
        
        // Check if client is buying the weapon or ammo for it, and get the price of the item.
        new bool:hasweapon = WeaponsClientHasWeapon(client, typeweapon);
        
        // DO ITEM PRICE STUFF HERE.
        
        new itemprice;
        
        // Do appropriate formatting for the type of item client is buying.
        new WeaponsSlot:slot = WeaponsGetSlot(weaponindex);
        if (!hasweapon || slot == Slot_Invalid || slot == Slot_Projectile)
        {
            // Get the weapon price.
            itemprice = WeaponsGetZMarketPrice(weaponindex);
        }
        else
        {
            // Get the weapon's ammo price.
            itemprice = WeaponsGetAmmoPrice(weaponindex);
            
            // Tell client they are buying ammo.
            Format(typeweapondisplay, sizeof(typeweapondisplay), "%s %t", typeweapon, "Weapons menu zmarket types weapon ammo");
        }
        
        // If the itemprice is invalid, then set to known integer to be later replaced.
        if (itemprice < 0)
        {
            itemprice = -1;
        }
        
        // DO RESTRICTED WEAPON STUFF HERE.
        
        // Get restricted status on the weapon.
        new bool:restricted = RestrictIsWeaponRestricted(weaponindex);
        
        // If weapon is restricted then format "[]" around it.
        strcopy(typeweapondisplay, sizeof(typeweapondisplay), typeweapon);
        if (restricted)
        {
            Format(typeweapondisplay, sizeof(typeweapondisplay), "[%s]", typeweapon);
        }
        
        // DO PURCHASE MAX STUFF HERE.
        
        // Get the purchase count information for this weapon.
        new purchasemax = WeaponsGetZMarketPurchaseMax(weaponindex);
        new purchasecount = ZMarketGetPurchaseCount(client, typeweapon);
        new purchasesleft = purchasemax - purchasecount;
        
        // If the purchases left for the weapon goes below 0, fix it.
        if (purchasesleft < 0)
        {
            purchasesleft = 0;
        }
        
        // If max purchases is disabled for the weapon, then set as known integer to be later replaced.
        if (purchasemax <= 0)
        {
            purchasesleft = -1;
        }
        
        // Format all the information together.
        
        // Format price onto the menu entry.
        Format(display, sizeof(display), "%t", "Weapons menu zmarket types weapon info", typeweapondisplay, itemprice, purchasesleft);
        
        // Remove the known invalid number from the string, and replace with N/A, and remove currency symbol.
        ReplaceString(display, sizeof(display), "$-1", "N/A");
        ReplaceString(display, sizeof(display), "-1", "N/A");
        
        // Disable option if it isn't toggleable.
        new bool:itemdraw = ((itemprice > -1) && !restricted && (purchasemax <= 0 || purchasesleft > 0));
        AddMenuItem(menu_zmarket_typeweapons, typeweapon, display, MenuGetItemDraw(itemdraw));
    }
    
    // Destroy the array handle.
    CloseHandle(arrayTypeWeapons);
    
    // Set menu back button.
    SetMenuExitBackButton(menu_zmarket_typeweapons, true);
    
    // Display menu to client.
    DisplayMenu(menu_zmarket_typeweapons, client, MENU_TIME_FOREVER);
}

/**
 * Called when client selects option in the weapon group menu, and handles it.
 * @param menu_zmarket_typeweapons  Handle of the menu being used.
 * @param action                    The action done on the menu (see menus.inc, enum MenuAction).
 * @param client                    The client index.
 * @param slot                      The slot index selected (starting from 0).
 */ 
public ZMarketMenuTypeWeaponsHandle(Handle:menu_zmarket_typeweapons, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Get name of current weapon type.
        decl String:typename[WEAPONS_MAX_LENGTH];
        RestrictWeaponTypeGetName(g_iZMarketCurType[client], typename, sizeof(typename));
        
        decl String:typeweapon[WEAPONS_MAX_LENGTH];
        GetMenuItem(menu_zmarket_typeweapons, slot, typeweapon, sizeof(typeweapon));
        
        // Equip weapon on client.
        ZMarketEquip(client, typeweapon);
        
        // Open types menu.
        ZMarketMenuTypes(client);
    }
    // Client closed the menu.
    if (action == MenuAction_Cancel)
    {
        // Client hit "Back" button.
        if (slot == MenuCancel_ExitBack)
        {
            ZMarketMenuTypes(client);
        }
    }
    // Client hit "Exit" button.
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zmarket_typeweapons);
    }
}

/**
 * Equip a weapon on a client.
 * 
 * @param client    The client index.
 * @param weapon    The weapon to equip (must be in weapons config file)
 * @param rebuy     (Optional) If client is rebuying, ammo will be ignored.
 */
stock bool:ZMarketEquip(client, const String:weapon[], bool:rebuy = false)
{
    // If client is a zombie, then stop.
    if (InfectIsClientInfected(client))
    {
        TranslationPrintToChat(client, "Must be human");
        return false;
    }
    
    new bool:zmarketbuyzone = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]);
    if (zmarketbuyzone && !ZMarketIsClientInBuyZone(client))
    {
        TranslationPrintToChat(client, "Weapons zmarket buyzone");
        return false;
    }
    
    // Get weapon index.
    new weaponindex = WeaponsNameToIndex(weapon);
    
    // If weapon index is -1, then something went very wrong.
    if (weaponindex == -1)
    {
        return false;
    }
    
    // Get slot index of the weapon being purchased.
    new WeaponsSlot:slot = WeaponsGetSlot(weaponindex);
    
    // Get the appropriate price of the item being purchased.
    new bool:hasweapon = WeaponsClientHasWeapon(client, weapon);
    new itemprice = (hasweapon && slot != Slot_Invalid && slot != Slot_Projectile) ? WeaponsGetAmmoPrice(weaponindex) : WeaponsGetZMarketPrice(weaponindex);
    
    // If the weapon price is below 0, then something went wrong.
    if (itemprice < 0)
    {
        return false;
    }
    
    // Get client's current money.
    new cash = AccountGetClientCash(client);
    
    // If client doesn't have enough money, tell them, resend menu, and then stop.
    if (cash < itemprice)
    {
        TranslationPrintToChat(client, "Not enough money");
        return false;
    }
    
    // Set client's new cash after purchase.
    AccountSetClientCash(client, cash - itemprice);
    
    // Check to make sure the weapon isn't restricted.
    new bool:restricted = RestrictIsWeaponRestricted(weaponindex);
    if (restricted)
    {
        TranslationPrintToChat(client, "Weapon is restricted", weapon);
        return false;
    }
    
    // Get the purchase count information for this weapon.
    new purchasemax = WeaponsGetZMarketPurchaseMax(weaponindex);
    new purchasecount = ZMarketGetPurchaseCount(client, weapon);
    new purchasesleft = purchasemax - purchasecount;
    if (purchasemax > 0 && purchasesleft <= 0)
    {
        TranslationPrintToChat(client, "Weapons zmarket purchase max", weapon, purchasemax);
        return false;
    }
    
    // Get a list of the client's current weapons.
    new weapons[WeaponsSlot];
    WeaponsGetClientWeapons(client, weapons);
    
    // Check if client is buying the weapon or ammo for it.
    if (!hasweapon || slot == Slot_Invalid || slot == Slot_Projectile)
    {
        
        // Check if the slot is valid and NOT a projectile (grenade).
        if (slot != Slot_Invalid && slot != Slot_Projectile)
        {
            // If there is already a weapon in the slot, then force client to drop it.
            if (weapons[slot] > -1)
            {
                // Force client to drop the weapon.
                WeaponsForceClientDrop(client, weapons[slot]);
            }
        }
        
        // Format name into entity name.
        decl String:weaponentity[WEAPONS_MAX_LENGTH];
        
        // If the slot is invalid, this means the item is not a usable weapon, it's equipment.
        if (slot == Slot_Invalid)
        {
            Format(weaponentity, sizeof(weaponentity), "item_%s", weapon);
        }
        else
        {
            Format(weaponentity, sizeof(weaponentity), "weapon_%s", weapon);
        }
        
        // Give client the weapon.
        GivePlayerItem(client, weaponentity);
        
        // Copy weapon to array for this slot.
        strcopy(g_strZMarketLastWeapon[client][slot], sizeof(g_strZMarketLastWeapon), weapon);
        
        // Add 1 to the client's purchase count.
        ZMarketSetPurchaseCount(client, weapon, 1, true);
        
        // Tell client they bought a weapon.
        TranslationPrintToChat(client, "Weapons zmarket purchase", weapon);
    }
    else if (!rebuy)
    {
        // Get ammo type and stop if it's invalid.
        decl String:weaponammo[WEAPONS_MAX_LENGTH];
        WeaponsGetAmmoType(weaponindex, weaponammo, sizeof(weaponammo));
        if (!weaponammo[0])
        {
            return false;
        }
        
        // Give ammo and preserve client's clip ammo value.
        new clip = WeaponAmmoGetAmmo(weapons[slot], true);
        GivePlayerItem(client, weaponammo);
        WeaponAmmoSetAmmo(weapons[slot], true, clip);
    }
    
    return true;
}

/**
 * Force a client to rebuy their weapons.
 * 
 * @param client    The client index.
 */
ZMarketRebuy(client)
{
    // If client is a zombie, then stop.
    if (InfectIsClientInfected(client))
    {
        TranslationPrintToChat(client, "Must be human");
        return;
    }
    
    new bool:zmarketbuyzone = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]);
    if (zmarketbuyzone && !ZMarketIsClientInBuyZone(client))
    {
        TranslationPrintToChat(client, "Weapons zmarket buyzone");
        return;
    }
    
    // x = Weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        new bool:equipped = ZMarketEquip(client, g_strZMarketLastWeapon[client][x], true);
        
        // Remove weapon from last weapons.
        if (!equipped)
        {
            strcopy(g_strZMarketLastWeapon[client][x], sizeof(g_strZMarketLastWeapon), "");
        }
    }
}

/**
 * Checks if a client is in a buyzone.
 * 
 * @param client    The client index.
 */
stock bool:ZMarketIsClientInBuyZone(client)
{
    // Return if client is in buyzone.
    return bool:GetEntData(client, g_iToolsInBuyZone);
}