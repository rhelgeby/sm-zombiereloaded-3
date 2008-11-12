/**
 * ====================
 *   Zombie:Reloaded
 *   File: admin.inc
 *   Author: Richard H
 * ==================== 
 */

#include "include/adminmenu.inc"
new SelectedClassIndex[MAXPLAYERS];

ZRAdminMenu(client)
{
    new Handle:zr_admin_menu = CreateMenu(ZRAdminMenuHandle);
    
    SetMenuTitle(zr_admin_menu, "ZR admin menu");
    
    decl String:zknockbackm[] = "Change knockback multiplier";
    decl String:zknockback[] = "Change class knockback";
    decl String:znvgs[] = "Change night vision settings";
    decl String:zinfect[] = "Infect a player";
    decl String:zspawn[] = "Spawn all players";
    decl String:zrestrict[] = "Restrict a weapon";
    decl String:zunrestrict[] = "Unrestrict a weapon";

    
    AddMenuItem(zr_admin_menu, "zknockbackm", zknockbackm);
    AddMenuItem(zr_admin_menu, "zknockback", zknockback);
    AddMenuItem(zr_admin_menu, "znvgs", znvgs);
    AddMenuItem(zr_admin_menu, "zinfect", zinfect);
    AddMenuItem(zr_admin_menu, "zspawn", zspawn);
    AddMenuItem(zr_admin_menu, "zrestrict", zrestrict, ITEMDRAW_DISABLED);
    AddMenuItem(zr_admin_menu, "zunrestrict", zunrestrict, ITEMDRAW_DISABLED);
    
    DisplayMenu(zr_admin_menu, client, MENU_TIME_FOREVER);
}

public ZRAdminMenuHandle(Handle:zr_admin_menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                ZRKnockbackMMenu(client);
            }
            case 1:
            {
                ZRClassSelectMenu(client);
            }
            case 2:
            {
                ZRNVGSMenu(client);
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
                // restrict
            }
            case 6:
            {
                // unrestrict
            }
        }
    }
    if (action == MenuAction_End)
    {
        CloseHandle(zr_admin_menu);
    }
}

ZRKnockbackMMenu(client)
{
    new Handle:zr_knockbackm_menu = CreateMenu(ZRKnockbackMHandle);
    new Float:current_knockback = GetConVarFloat(gCvars[CVAR_ZOMBIE_KNOCKBACK]);

    SetMenuTitle(zr_knockbackm_menu, "Change knockback multiplier\nCurrent value: %f\n\n", current_knockback);
    
    decl String:zincreasehalf[] = "Increase by 0.5";
    decl String:zincreasedeca[] = "Increase by 0.1";
    decl String:zdecreasedeci[] = "Decrease by 0.1";
    decl String:zdecreasehalf[] = "Decrease by 0.5";
    
    AddMenuItem(zr_knockbackm_menu, "zincreasehalf", zincreasehalf);
    AddMenuItem(zr_knockbackm_menu, "zincreasedeca", zincreasedeca);
    AddMenuItem(zr_knockbackm_menu, "zdecreasedeci", zdecreasedeci);
    AddMenuItem(zr_knockbackm_menu, "zdecreasehalf", zdecreasehalf);
    
    SetMenuExitBackButton(zr_knockbackm_menu, true);
    
    DisplayMenu(zr_knockbackm_menu, client, MENU_TIME_FOREVER);
}

public ZRKnockbackMHandle(Handle:zr_knockbackm_menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                AddToKnockbackMultiplier(0.5);
                ZRKnockbackMMenu(client);
            }
            case 1:
            {
                AddToKnockbackMultiplier(0.1);
                ZRKnockbackMMenu(client);
            }
            case 2:
            {
                AddToKnockbackMultiplier(-0.1);
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
        CloseHandle(zr_knockbackm_menu);
    }
}

ZRClassSelectMenu(client)
{
    new Handle:zr_class_select_menu = CreateMenu(ZRClassSelectHandle);
    new classindex;
    
    SetMenuTitle(zr_class_select_menu, "Select class to change:\n");
    
    for (classindex = 0; classindex < classCount; classindex++)
    {
        AddMenuItem(zr_class_select_menu, arrayClasses[classindex][data_name], arrayClasses[classindex][data_name]);
    }
    
    SetMenuExitBackButton(zr_class_select_menu, true);
    DisplayMenu(zr_class_select_menu, client, MENU_TIME_FOREVER);
}

public ZRClassSelectHandle(Handle:zr_class_select_menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        SelectedClassIndex[client] = slot;
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
        CloseHandle(zr_class_select_menu);
    }
}

ZRClassKnockbackMenu(client, classindex)
{
    new Handle:zr_knockback_menu = CreateMenu(ZRClassKnockbackHandle);
    new Float:current_knockback = arrayClasses[classindex][data_knockback];
    new String:classname[64];
    GetClassName(classindex, classname, sizeof(classname));

    SetMenuTitle(zr_knockback_menu, "Change %s knockback\nCurrent value: %f\n\n", classname, current_knockback);
    
    decl String:zincreasehalf[] = "Increase by 0.5";
    decl String:zincreasedeca[] = "Increase by 0.1";
    decl String:zdecreasedeci[] = "Decrease by 0.1";
    decl String:zdecreasehalf[] = "Decrease by 0.5";
    
    AddMenuItem(zr_knockback_menu, "zincreasehalf", zincreasehalf);
    AddMenuItem(zr_knockback_menu, "zincreasedeca", zincreasedeca);
    AddMenuItem(zr_knockback_menu, "zdecreasedeci", zdecreasedeci);
    AddMenuItem(zr_knockback_menu, "zdecreasehalf", zdecreasehalf);
    
    SetMenuExitBackButton(zr_knockback_menu, true);
    
    DisplayMenu(zr_knockback_menu, client, MENU_TIME_FOREVER);
}

public ZRClassKnockbackHandle(Handle:zr_knockback_menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                AddToClassKnockback(SelectedClassIndex[client], 0.5);
                ZRClassKnockbackMenu(client, SelectedClassIndex[client]);
            }
            case 1:
            {
                AddToClassKnockback(SelectedClassIndex[client], 0.1);
                ZRClassKnockbackMenu(client, SelectedClassIndex[client]);
            }
            case 2:
            {
                AddToClassKnockback(SelectedClassIndex[client], -0.1);
                ZRClassKnockbackMenu(client, SelectedClassIndex[client]);
            }
            case 3:
            {
                AddToClassKnockback(SelectedClassIndex[client], -0.5);
                ZRClassKnockbackMenu(client, SelectedClassIndex[client]);
            }
        }
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
        CloseHandle(zr_knockback_menu);
    }
}

ZRNVGSMenu(client)
{
    new Handle:zr_nvgs_menu = CreateMenu(ZRNVGSHandle);
    new current_nvgs = GetConVarInt(gCvars[CVAR_ZOMBIE_NVGS]);

    SetMenuTitle(zr_nvgs_menu, "Change night vision settings\nCurrent value: %i\n", current_nvgs);
    
    decl String:znooverride[] = "-1: No override/Default";
    decl String:zoff[] = "0: Never give nvgs";
    decl String:zon[] = "1: Always give nvgs";
    
    AddMenuItem(zr_nvgs_menu, "znooverride", znooverride);
    AddMenuItem(zr_nvgs_menu, "zoff", zoff);
    AddMenuItem(zr_nvgs_menu, "zon", zon);
    
    SetMenuExitBackButton(zr_nvgs_menu, true);
    
    DisplayMenu(zr_nvgs_menu, client, MENU_TIME_FOREVER);
}

public ZRNVGSHandle(Handle:zr_nvgs_menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                SetConVarInt(gCvars[CVAR_ZOMBIE_NVGS], -1);
                ZRNVGSMenu(client);
            }
            case 1:
            {
                SetConVarInt(gCvars[CVAR_ZOMBIE_NVGS], 0);
                ZRNVGSMenu(client);
            }
            case 2:
            {
                SetConVarInt(gCvars[CVAR_ZOMBIE_NVGS], 1);
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
        CloseHandle(zr_nvgs_menu);
    }
}

ZRInfectMenu(client)
{
    new Handle:zr_infect_menu = CreateMenu(ZRInfectHandle);

    SetMenuTitle(zr_infect_menu, "Infect a player:");
    AddTargetsToMenu(zr_infect_menu, client, true, true);
    SetMenuExitBackButton(zr_infect_menu, true);
    DisplayMenu(zr_infect_menu, client, MENU_TIME_FOREVER);
}

public ZRInfectHandle(Handle:zr_infect_menu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
         decl String:info[32];
         new userid, target;
         
         GetMenuItem(zr_infect_menu, slot, info, sizeof(info));
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
             Zombify(target, 0);
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
        CloseHandle(zr_infect_menu);
    }
}

ZRSpawnAll(client)
{
    for (new c = 1; c < maxclients; c++)
    {
        ZSpawn(c);
    }
    ZRAdminMenu(client);
}

AddToKnockbackMultiplier(Float:value)
{
    new Float:current_val = GetConVarFloat(gCvars[CVAR_ZOMBIE_KNOCKBACK]);
    SetConVarFloat(gCvars[CVAR_ZOMBIE_KNOCKBACK], current_val + value);
}

AddToClassKnockback(classindex, Float:value)
{
    arrayClasses[classindex][data_knockback] = arrayClasses[classindex][data_knockback] + value;
}
