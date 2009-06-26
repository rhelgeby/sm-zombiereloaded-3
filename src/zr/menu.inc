/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          menu.inc
 *  Type:          Core
 *  Description:   Base menu functions for the plugin.
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

/**
 * Create commands specific to ZMenu.
 */
MenuOnCommandsCreate()
{
    // Register ZMenu command.
    RegConsoleCmd(SAYHOOKS_KEYWORD_ZMENU, ZMenuCommand, "Opens ZR's main menu.");
}

/**
 * Command callback (zmenu)
 * Opens ZR's main menu.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ZMenuCommand(client, argc)
{
    // If client is console, then stop and tell them this feature is for players only.
    if (ZRIsConsole(client))
    {
        TranslationPrintToServer("Must be player");
        return Plugin_Handled;
    }
    
    // Send main menu.
    ZMenuMain(client);
    
    // This stops the "Unknown command" message in client's console.
    return Plugin_Handled;
}

/**
 * Show main menu to client.
 * 
 * @param client    The client index.
 */
ZMenuMain(client)
{
    // Create menu handle.
    new Handle:menu_main = CreateMenu(ZMenuMainHandle);
    
    // Make client global translations target.
    SetGlobalTransTarget(client);
    
    decl String:publictrigger[4];
    decl String:silenttrigger[4];
    
    // Get public/silent chat triggers.
    SayHooksGetPublicChatTrigger(publictrigger, sizeof(publictrigger));
    SayHooksGetSilentChatTrigger(silenttrigger, sizeof(silenttrigger));
    
    // Set menu title.
    SetMenuTitle(menu_main, "%t\n ", "Menu main title", publictrigger, silenttrigger);
    
    // Initialize menu lines.
    decl String:zadmin[256];
    decl String:zclass[256];
    decl String:zcookies[256];
    decl String:zspawn[256];
    decl String:ztele[256];
    decl String:zhp[256];
    decl String:zmarket[256];
    
    // Translate each line into client's language.
    Format(zadmin, sizeof(zadmin), "%t", "Menu main zadmin", SAYHOOKS_KEYWORD_ZMENU);
    Format(zclass, sizeof(zclass), "%t", "Menu main zclass", SAYHOOKS_KEYWORD_ZADMIN);
    Format(zcookies, sizeof(zcookies), "%t", "Menu main zcookies", SAYHOOKS_KEYWORD_ZCOOKIES);
    Format(zspawn, sizeof(zspawn), "%t", "Menu main zspawn", SAYHOOKS_KEYWORD_ZCLASS);
    Format(ztele, sizeof(ztele), "%t", "Menu main ztele", SAYHOOKS_KEYWORD_ZTELE);
    Format(zhp, sizeof(zhp), "%t", "Menu main zhp", SAYHOOKS_KEYWORD_ZHP);
    Format(zmarket, sizeof(zmarket), "%t", "Menu main zmarket", SAYHOOKS_KEYWORD_ZMARKET);
    
    // Add items to menu.
    
    // Disable option if client isn't an admin.
    new bool:admin = ZRIsClientAdmin(client);
    AddMenuItem(menu_main, "zadmin", zadmin, MenuGetItemDraw(admin));
    
    AddMenuItem(menu_main, "zclass", zclass);
    AddMenuItem(menu_main, "zcookies", zcookies);
    AddMenuItem(menu_main, "zspawn", zspawn);
    AddMenuItem(menu_main, "ztele", ztele);
    AddMenuItem(menu_main, "zhp", zhp);
    AddMenuItem(menu_main, "zmarket", zmarket);
    
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
public ZMenuMainHandle(Handle:menu, MenuAction:action, client, slot)
{
    // Client selected an option.
    if (action == MenuAction_Select)
    {
        // Create variable to possible resend menu later.
        new bool:resend = true;
        
        switch(slot)
        {
            // Selected ZAdmin.
            case 0:
            {
                // Copy return to resend variable.
                resend = !ZAdminMenu(client);
            }
            // Select ZClass.
            case 1:
            {
                // Send ZClass menu
                ClassMenuMain(client);
                
                // Don't resend this menu.
                resend = false;
            }
            // Select ZCookies.
            case 2:
            {
                // Send ZCookies menu
                ZCookiesMenuMain(client);
                
                // Don't resend this menu.
                resend = false;
            }
            // Select ZSpawn.
            case 3:
            {
                // Send ZSpawn command from client.
                ZSpawnClient(client);
            }
            // Select ZTele.
            case 4:
            {
                // Copy return to resend variable.
                resend = !ZTeleClient(client);
            }
            // Select ZHP.
            case 5:
            {
                // Toggle ZHP.
                ZHPToggle(client);
            }
            // Select ZMarket.
            case 6:
            {
                // Send ZMarket menu.
                resend = !ZMarketMenuMain(client);
            }
        }
        
        // Resend is still true, then resend menu.
        if (resend)
        {
            ZMenuMain(client);
        }
    }
    // Client exited menu.
    if (action == MenuAction_End)
    {
        CloseHandle(menu);
    }
}

/**
 * Shows a list of all clients to a client, different handlers can be used for this, as well as title.
 * 
 * @param client    The client index.
 * @param handler   The menu handler.
 * @param any       Title is a translations phrase.  
 */
stock MenuClientList(client, MenuHandler:handler, any:...)
{
    // Create menu handle.
    new Handle:menu_clients = CreateMenu(handler);
    
    // Set client as translation target.
    SetGlobalTransTarget(client);
    
    // Translate phrase.
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    VFormat(translation, sizeof(translation), "%t", 3);
    
    // Set menu title to the translated phrase.
    SetMenuTitle(menu_clients, translation);
    
    decl String:clientoption[MAX_NAME_LENGTH];
    decl String:clientuserid[8];
    
    // x = Client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // Get client info.
        GetClientName(x, clientoption, sizeof(clientoption));
        IntToString(GetClientUserId(x), clientuserid, sizeof(clientuserid));
        
        // Add option to menu.
        AddMenuItem(menu_clients, clientuserid, clientoption);
    }
    
    // Create a "Back" button to the main admin menu.
    SetMenuExitBackButton(menu_clients, true);
    
    // Send menu.
    DisplayMenu(menu_clients, client, MENU_TIME_FOREVER);
}

/**
 * Gets the client index of the selected client in the menu.
 * 
 * @param menu  The menu handle.
 * @param slot  The menu slot that was selected. 
 * @return      The client index, 0 if the selected client is no longer in the server.
 */
stock MenuGetClientIndex(Handle:menu, slot)
{
    // Get menu slot's information.
    decl String:clientuserid[8];
    GetMenuItem(menu, slot, clientuserid, sizeof(clientuserid));
    
    // Return the targetted client through their userid which was set into the menu slot's info param.
    return GetClientOfUserId(StringToInt(clientuserid));
}

/**
 * Return itemdraw flag for SM menus.
 * 
 * @param condition     If this is true, item will be drawn normally.
 */
stock MenuGetItemDraw(bool:condition)
{
    return condition ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
}
