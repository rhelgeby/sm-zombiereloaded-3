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

#include "include/adminmenu.inc"
//new curMenuClass[MAXPLAYERS + 1];

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
    
    decl String:knockbackm[64];
    decl String:knockback[64];
    decl String:nvgs[64];
    decl String:infect[64];
    decl String:zspawn[64];
    decl String:ztele[64];
    decl String:weapons[64];
    decl String:logflags[64];
    
    Format(knockbackm, sizeof(knockbackm), "%t", "!zadmin main knockbackm");
    Format(knockback, sizeof(knockback), "%t", "!zadmin main knockback");
    Format(nvgs, sizeof(nvgs), "%t", "!zadmin main nvgs");
    Format(infect, sizeof(infect), "%t", "!zadmin main infect");
    Format(zspawn, sizeof(zspawn), "%t", "!zadmin main spawn");
    Format(ztele, sizeof(ztele), "%t", "!zadmin main tele");
    Format(weapons, sizeof(weapons), "%t", "!zadmin main weapons");
    Format(logflags, sizeof(logflags), "%t", "!zadmin main logflags");
    
    AddMenuItem(menu_zadmin, "knockbackm", knockbackm, ITEMDRAW_DISABLED);
    AddMenuItem(menu_zadmin, "knockback", knockback, ITEMDRAW_DISABLED);
    AddMenuItem(menu_zadmin, "nvgs", nvgs, ITEMDRAW_DISABLED);
    AddMenuItem(menu_zadmin, "infect", infect);
    AddMenuItem(menu_zadmin, "zspawn", zspawn);
    AddMenuItem(menu_zadmin, "ztele", ztele, ITEMDRAW_DISABLED);
    AddMenuItem(menu_zadmin, "weapons", weapons);
    AddMenuItem(menu_zadmin, "logflags", logflags);
    
    SetMenuExitBackButton(menu_zadmin, true);
    
    DisplayMenu(menu_zadmin, client, MENU_TIME_FOREVER);
    
    return true;
}

public ZRAdminMenuHandle(Handle:menu_admin, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                //ZRKnockbackMMenu(client);
            }
            case 1:
            {
                //ZRClassSelectMenu(client);
            }
            case 2:
            {
                //ZRNVGSMenu(client);
            }
            case 3:
            {
                ZRInfectMenu(client);
            }
            case 4:
            {
                ZRSpawnAll(client);
            }
            case 5:
            {
                ZRZTeleMenu(client);
            }
            case 6:
            {
                WeaponsMenuMain(client);
            }
            case 7:
            {
                ZRLogFlagsMenu(client);
            }
        }
    }
    
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            MenuMain(client);
        }
    }
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_admin);
    }
}

/**
 * Needs to be recoded to support new modules.
 */ 
/*ZRKnockbackMMenu(client)
{
    new Handle:menu_knockbackm = CreateMenu(ZRKnockbackMHandle);
    new Float:curknockback = GetConVarFloat(g_hCvarsList[CVAR_ZOMBIE_KNOCKBACK]);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_knockbackm, "%t\n ", "!zadmin knockbackm title", curknockback);
    
    decl String:knockbackmincrease1[64];
    decl String:knockbackmdecrease1[64];
    decl String:knockbackmincrease2[64];
    decl String:knockbackmdecrease2[64];
    
    Format(knockbackmincrease1, sizeof(knockbackmincrease1), "%t", "!zadmin knockbackm increase", "0.1");
    Format(knockbackmdecrease1, sizeof(knockbackmdecrease1), "%t", "!zadmin knockbackm decrease", "0.1");
    Format(knockbackmincrease2, sizeof(knockbackmincrease2), "%t", "!zadmin knockbackm increase", "0.5");
    Format(knockbackmdecrease2, sizeof(knockbackmdecrease2), "%t", "!zadmin knockbackm decrease", "0.5");
    
    AddMenuItem(menu_knockbackm, "knockbackmincrease1", knockbackmincrease1);
    AddMenuItem(menu_knockbackm, "knockbackmdecrease1", knockbackmdecrease1);
    AddMenuItem(menu_knockbackm, "knockbackmincrease2", knockbackmincrease2);
    AddMenuItem(menu_knockbackm, "knockbackmdecrease2", knockbackmdecrease2);
    
    SetMenuExitBackButton(menu_knockbackm, true);
    
    DisplayMenu(menu_knockbackm, client, MENU_TIME_FOREVER);
}

public ZRKnockbackMHandle(Handle:menu_knockbackm, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                AddToKnockbackMultiplier(0.1);
                ZRKnockbackMMenu(client);
            }
            case 1:
            {
                AddToKnockbackMultiplier(-0.1);
                ZRKnockbackMMenu(client);
            }
            case 2:
            {
                AddToKnockbackMultiplier(0.5);
                ZRKnockbackMMenu(client);
            }
            case 3:
            {
                AddToKnockbackMultiplier(-0.5);
                ZRKnockbackMMenu(client);
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
        CloseHandle(menu_knockbackm);
    }
}

ZRClassSelectMenu(client)
{
    new Handle:menu_class = CreateMenu(ZRClassSelectHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_class, "%t\n ", "!zadmin class title");
    
    // x = index of class
    for (new x = 0; x < classCount; x++)
    {
        AddMenuItem(menu_class, arrayClasses[x][data_name], arrayClasses[x][data_name]);
    }
    
    SetMenuExitBackButton(menu_class, true);
    DisplayMenu(menu_class, client, MENU_TIME_FOREVER);
}

public ZRClassSelectHandle(Handle:menu_class, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        curMenuClass[client] = slot;
        ZRClassKnockbackMenu(client, slot);
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
        CloseHandle(menu_class);
    }
}

ZRClassKnockbackMenu(client, classindex)
{
    new Handle:menu_knockback = CreateMenu(ZRClassKnockbackHandle);
    
    new Float:curknockback = arrayClasses[classindex][data_knockback];
    
    new String:classname[64];
    GetClassName(classindex, classname, sizeof(classname));
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_knockback, "%t\n ", "!zadmin knockback title", classname, curknockback);
    
    decl String:knockbackincrease1[64];
    decl String:knockbackdecrease1[64];
    decl String:knockbackincrease2[64];
    decl String:knockbackdecrease2[64];
    
    Format(knockbackincrease1, sizeof(knockbackincrease1), "%t", "!zadmin knockback increase", "0.1");
    Format(knockbackdecrease1, sizeof(knockbackdecrease1), "%t", "!zadmin knockback decrease", "0.1");
    Format(knockbackincrease2, sizeof(knockbackincrease2), "%t", "!zadmin knockback increase", "0.5");
    Format(knockbackdecrease2, sizeof(knockbackdecrease2), "%t", "!zadmin knockback decrease", "0.5");
    
    AddMenuItem(menu_knockback, "knockbackincrease1", knockbackincrease1);
    AddMenuItem(menu_knockback, "knockbackdecrease1", knockbackdecrease1);
    AddMenuItem(menu_knockback, "knockbackincrease2", knockbackincrease2);
    AddMenuItem(menu_knockback, "knockbackdecrease2", knockbackdecrease2);
    
    SetMenuExitBackButton(menu_knockback, true);
    
    DisplayMenu(menu_knockback, client, MENU_TIME_FOREVER);
}

public ZRClassKnockbackHandle(Handle:menu_knockback, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                AddToClassKnockback(curMenuClass[client], 0.1);
            }
            case 1:
            {
                AddToClassKnockback(curMenuClass[client], -0.1);
            }
            case 2:
            {
                AddToClassKnockback(curMenuClass[client], 0.5);
            }
            case 3:
            {
                AddToClassKnockback(curMenuClass[client], -0.5);
            }
        }
        
        ZRClassKnockbackMenu(client, curMenuClass[client]);
    }
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            ZRClassSelectMenu(client);
        }
    }
    if (action == MenuAction_End)
    {
        CloseHandle(menu_knockback);
    }
}

ZRNVGSMenu(client)
{
    new Handle:menu_nvgs = CreateMenu(ZRNVGSHandle);
    new curnvgs = GetConVarInt(g_hCvarsList[CVAR_ZOMBIE_NVGS]);

    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_nvgs, "%t\n ", "!zadmin nvgs title", curnvgs);
    
    decl String:nooverride[64];
    decl String:disable[64];
    decl String:enable[64];
    
    Format(nooverride, sizeof(nooverride), "%t", "!zadmin nvgs no override");
    Format(disable, sizeof(disable), "%t", "!zadmin nvgs disable");
    Format(enable, sizeof(enable), "%t", "!zadmin nvgs enable");
    
    AddMenuItem(menu_nvgs, "nooverride", nooverride);
    AddMenuItem(menu_nvgs, "disable", disable);
    AddMenuItem(menu_nvgs, "enable", enable);
    
    SetMenuExitBackButton(menu_nvgs, true);
    
    DisplayMenu(menu_nvgs, client, MENU_TIME_FOREVER);
}

public ZRNVGSHandle(Handle:menu_nvgs, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                SetConVarInt(g_hCvarsList[CVAR_ZOMBIE_NVGS], -1);
                ZRNVGSMenu(client);
            }
            case 1:
            {
                SetConVarInt(g_hCvarsList[CVAR_ZOMBIE_NVGS], 0);
                ZRNVGSMenu(client);
            }
            case 2:
            {
                SetConVarInt(g_hCvarsList[CVAR_ZOMBIE_NVGS], 1);
                ZRNVGSMenu(client);
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
        CloseHandle(menu_nvgs);
    }
}*/

ZRInfectMenu(client)
{
    new Handle:menu_infect = CreateMenu(ZRInfectHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_infect, "%t\n ", "!zadmin infect title");
    
    AddTargetsToMenu(menu_infect, client, true, true);
    
    SetMenuExitBackButton(menu_infect, true);
    
    DisplayMenu(menu_infect, client, MENU_TIME_FOREVER);
}

public ZRInfectHandle(Handle:menu_infect, MenuAction:action, client, slot)
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
}

ZRSpawnAll(client)
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
}

ZRZTeleMenu(client)
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
}

public ZRTeleHandle(Handle:menu_ztele , MenuAction:action, client, slot)
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
}

ZRLogFlagsMenu(client)
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
}

public ZRLogFlagsMenuHandle(Handle:menu_log_flags, MenuAction:action, client, slot)
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
}
