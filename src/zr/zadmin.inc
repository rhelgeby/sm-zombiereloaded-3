/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          zadmin.inc
 *  Type:          Core
 *  Description:   Handle admin functions and menus.
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
 * Create commands specific to ZAdmin.
 */
ZAdminOnCommandsCreate()
{
    // Register ZAdmin command.
    RegConsoleCmd(SAYHOOKS_KEYWORD_ZADMIN, ZAdminCommand, "Opens ZR admin menu.");
}

/**
 * Command callback (zadmin)
 * Opens ZR admin menu.
 * 
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ZAdminCommand(client, argc)
{
    // If client is console, then stop and tell them this feature is for players only.
    if (ZRIsConsole(client))
    {
        TranslationPrintToServer("Must be player");
        return Plugin_Handled;
    }
    
    // Send admin menu.
    ZAdminMenu(client);
    
    // This stops the "Unknown command" message in client's console.
    return Plugin_Handled;
}

/**
 * Main admin menu.
 * 
 * @param client    The client index.
 */
bool:ZAdminMenu(client)
{
    // If client isn't an admin, then stop.
    if (!ZRIsClientAdmin(client))
    {
        TranslationPrintToChat(client, "Must be admin");
        return false;
    }
    
    // Create menu handle.
    new Handle:menu_zadmin = CreateMenu(ZAdminMenuHandle);
    
    // Set translation target as the client.
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_zadmin, "%t\n ", "ZAdmin main title");
    
    decl String:classmultipliers[64];
    decl String:weapons[64];
    decl String:hitgroups[64];
    decl String:infect[64];
    decl String:zspawn[64];
    decl String:ztele[64];
    //decl String:logflags[64];
    
    Format(classmultipliers, sizeof(classmultipliers), "%t", "ZAdmin main class multipliers");
    Format(weapons, sizeof(weapons), "%t", "ZAdmin main weapons");
    Format(hitgroups, sizeof(hitgroups), "%t", "ZAdmin main hitgroups");
    Format(infect, sizeof(infect), "%t", "ZAdmin main zombie");
    Format(zspawn, sizeof(zspawn), "%t", "ZAdmin main force zspawn");
    Format(ztele, sizeof(ztele), "%t", "ZAdmin main force ztele");
    //Format(logflags, sizeof(logflags), "%t", "!zadmin main logflags");
    
    // Get conditions for options.
    new bool:hitgroups = GetConVarBool(g_hCvarsList[CVAR_HITGROUPS]);
    
    // Add items to menu.
    AddMenuItem(menu_zadmin, "classmultipliers", classmultipliers);
    AddMenuItem(menu_zadmin, "weapons", weapons);
    AddMenuItem(menu_zadmin, "hitgroups", hitgroups, MenuGetItemDraw(hitgroups));
    AddMenuItem(menu_zadmin, "infect", infect);
    AddMenuItem(menu_zadmin, "zspawn", zspawn);
    AddMenuItem(menu_zadmin, "ztele", ztele);
    //AddMenuItem(menu_zadmin, "logflags", logflags);
    
    // Set "Back" button.
    SetMenuExitBackButton(menu_zadmin, true);
    
    // Send menu to client.
    DisplayMenu(menu_zadmin, client, MENU_TIME_FOREVER);
    
    return true;
}

/**
 * Menu callback (zadmin)
 * Handles options selected in the admin menu.
 * 
 * @param menu      The menu handle.
 * @param action    Action client is doing in menu.
 * @param client    The client index.
 * @param slot      The menu slot selected. (starting from 0)
 */
public ZAdminMenuHandle(Handle:menu_zadmin, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        // Create variable to possible resend menu later.
        new bool:resend = true;
        
        switch(slot)
        {
            // Class multipliers.
            case 0:
            {
                resend = !ClassTeamSelect(client);
            }
            // Weapon management.
            case 1:
            {
                resend = !WeaponsMenuMain(client);
            }
            // Hitgroup management.
            case 2:
            {
                resend = !HitgroupsMenuHitgroups(client);
            }
            // Zombie management.
            case 3:
            {
                // We're not resending this menu.
                resend = false;
                
                // Send list of clients to infect.
                InfectMenuClients(client);
            }
            // Force ZSpawn.
            case 4:
            {
                // We're not resending this menu.
                resend = false;
                
                // Send list of clients to infect.
                MenuClientList(client, ZSpawnForceHandle, "ZSpawn clients title");
            }
            // Force ZTele.
            case 5:
            {
                // We're not resending this menu.
                resend = false;
                
                // Send list of clients to infect.
                MenuClientList(client, ZTeleForceHandle, "ZTele clients title");
            }
        }
        
        // Re-send menu if selection failed.
        if (resend)
        {
            ZAdminMenu(client);
        }
    }
    
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            // Exit back to main menu.
            ZMenuMain(client);
        }
    }
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_zadmin);
    }
}
