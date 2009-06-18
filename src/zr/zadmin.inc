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

bool:ZRAdminMenu(client)
{
    if (!ZRIsClientAdmin(client))
    {
        TranslationPrintToChat(client, "Must be admin");
        return false;
    }
    
    new Handle:menu = CreateMenu(ZRAdminMenuHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu, "%t\n ", "!zadmin title");
    
    decl String:classmultipliers[64];
    //decl String:infect[64];
    //decl String:zspawn[64];
    //decl String:ztele[64];
    decl String:weapons[64];
    //decl String:logflags[64];
    
    Format(classmultipliers, sizeof(classmultipliers), "%t", "!zadmin class multipliers");
    //Format(infect, sizeof(infect), "%t", "!zadmin main infect");
    //Format(zspawn, sizeof(zspawn), "%t", "!zadmin main spawn");
    //Format(ztele, sizeof(ztele), "%t", "!zadmin main tele");
    Format(weapons, sizeof(weapons), "%t", "!zadmin weapons");
    //Format(logflags, sizeof(logflags), "%t", "!zadmin main logflags");
    
    AddMenuItem(menu, "classmultipliers", classmultipliers);
    //AddMenuItem(menu_zadmin, "infect", infect);
    //AddMenuItem(menu_zadmin, "zspawn", zspawn);
    //AddMenuItem(menu_zadmin, "ztele", ztele, ITEMDRAW_DISABLED);
    AddMenuItem(menu, "weapons", weapons);
    //AddMenuItem(menu_zadmin, "logflags", logflags);
    
    // Set "Back" button.
    SetMenuExitBackButton(menu, true);
    
    // Send menu to client.
    DisplayMenu(menu, client, MENU_TIME_FOREVER);
    
    return true;
}

public ZRAdminMenuHandle(Handle:menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        // Create variable to possible resend menu later.
        new bool:resend = true;
        
        switch(slot)
        {
            case 0:
            {
                resend = !ClassTeamSelect(client);
            }
            
            // Weapon management.
            case 1:
            {
                resend = !WeaponsMenuMain(client);
            }
        }
        
        // Re-send menu if selection failed.
        if (resend)
        {
            ZRAdminMenu(client);
        }
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
        CloseHandle(menu);
    }
}

/*ZRInfectMenu(client)
{
    new Handle:menu_infect = CreateMenu(ZRInfectHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_infect, "%t\n ", "!zadmin infect title");
    
    AddTargetsToMenu(menu_infect, client, true, true);
    
    SetMenuExitBackButton(menu_infect, true);
    
    DisplayMenu(menu_infect, client, MENU_TIME_FOREVER);
}*/

/*public ZRInfectHandle(Handle:menu_infect, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
         decl String:info[32];
         new userid, target;
         
         GetMenuItem(menu_infect, slot, info, sizeof(info));
         userid = StringToInt(info);

         if ((target = GetClientOfUserId(userid)) == 0)
         {
             ReplyToCommand(client, "[ZR] Player no longer available");
         }
         else if (!CanUserTarget(client, target))
         {
             ReplyToCommand(client, "[ZR] Unable to target player");
         }
         else if (!IsPlayerAlive(target))
         {
             ReplyToCommand(client, "[ZR] Player is dead");
         }
         else
         {
             decl String:name[64];
             GetClientName(target, name, sizeof(name));
             InfectClient(target);
             ShowActivity2(client, "[ZR] ", "Infected %s", name);
             ZRInfectMenu(client);
         }
    }
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            ZRAdminMenu(client);
        }
    }
    if (action == MenuAction_End)
    {
        CloseHandle(menu_infect);
    }
}*/

/*ZRSpawnAll(client)
{
    // x = client index.
    for (new x = 1; x < MaxClients; x++)
    {
        if (IsClientInGame(x))
        {
            ZSpawnClient(x);
        }
    }
    ZRAdminMenu(client);
}*/

/*ZRZTeleMenu(client)
{
    new Handle:menu_ztele = CreateMenu(ZRTeleHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_ztele, "%t\n ", "!zadmin ztele title");
    
    decl String:ztele_spawntele[64];
    decl String:ztele_abort[64];
    decl String:ztele_save[64];
    decl String:ztele_tele[64];
    
    Format(ztele_spawntele, sizeof(ztele_spawntele), "%t", "!zadmin ztele spawn tele");
    Format(ztele_abort, sizeof(ztele_abort), "%t", "!zadmin ztele abort");
    Format(ztele_save, sizeof(ztele_save), "%t", "!zadmin ztele save");
    Format(ztele_tele, sizeof(ztele_tele), "%t", "!zadmin ztele tele");
    
    AddMenuItem(menu_ztele, "ztele_spawntele", ztele_spawntele);
    AddMenuItem(menu_ztele, "ztele_abort", ztele_abort);
    AddMenuItem(menu_ztele, "ztele_save", ztele_save);
    AddMenuItem(menu_ztele, "ztele_tele", ztele_tele);
    
    SetMenuExitBackButton(menu_ztele, true);
    DisplayMenu(menu_ztele, client, MENU_TIME_FOREVER);
}*/

/*public ZRTeleHandle(Handle:menu_ztele , MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                // Teleport player.
            }
            case 1:
            {
                // Abort teleport.
            }
            case 2:
            {
                // Save location.
            }
            case 3:
            {
                // Teleport to location.
            }
        }
    }
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            ZRAdminMenu(client);
        }
    }
    if (action == MenuAction_End)
    {
        CloseHandle(menu_ztele);
    }
}*/

/*ZRLogFlagsMenu(client)
{
    new Handle:menu_log_flags = CreateMenu(ZRLogFlagsMenuHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_log_flags, "%t\n ", "!zadmin log flags title");
    
    //new client_flags = GetUserFlagBits(client);
    //new item_state = (client_flags & ADMFLAG_ROOT) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;

    //decl String:z_log_core[64];
    
    //Format(z_log_core, sizeof(z_log_core), "Log core events (%d)", LogCheckFlag(LOG_CORE_EVENTS));
    
    //AddMenuItem(menu_log_flags, z_log_core, z_log_core, item_state);
    
    SetMenuExitBackButton(menu_log_flags, true);
    DisplayMenu(menu_log_flags, client, MENU_TIME_FOREVER);
}*/

/*public ZRLogFlagsMenuHandle(Handle:menu_log_flags, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
        }
    }
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            ZRAdminMenu(client);
        }
    }
    if (action == MenuAction_End)
    {
        CloseHandle(menu_log_flags);
    }
}*/
