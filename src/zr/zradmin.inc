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
    decl String:ztele[] = "Teleporter commands";
    decl String:zrestrict[] = "Restrict a weapon";
    decl String:zunrestrict[] = "Unrestrict a weapon";
    decl String:zlogflags[] = "Logging flags";
    
    AddMenuItem(zr_admin_menu, "zknockbackm", zknockbackm);
    AddMenuItem(zr_admin_menu, "zknockback", zknockback);
    AddMenuItem(zr_admin_menu, "znvgs", znvgs);
    AddMenuItem(zr_admin_menu, "zinfect", zinfect);
    AddMenuItem(zr_admin_menu, "zspawn", zspawn);
    AddMenuItem(zr_admin_menu, "ztele", ztele);
    AddMenuItem(zr_admin_menu, "zrestrict", zrestrict, ITEMDRAW_DISABLED);
    AddMenuItem(zr_admin_menu, "zunrestrict", zunrestrict, ITEMDRAW_DISABLED);
    AddMenuItem(zr_admin_menu, "zlogflags", zlogflags);
    
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
                ZRTeleMenu(client);
            }
            case 6:
            {
                // restrict
            }
            case 7:
            {
                // unrestrict
            }
            case 8:
            {
                ZRLogFlagsMenu(client);
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
        if (IsClientConnected(c) && IsClientInGame(c))
        {
            ZSpawn(c);
        }
    }
    ZRAdminMenu(client);
}

ZRTeleMenu(client)
{
    new Handle:zr_tele_menu = CreateMenu(ZRTeleHandle);
    
    decl String:ztele_player[] = "Teleport players";
    decl String:ztele_abort[] = "Abort teleport on players";
    decl String:ztele_save[] = "Save player location";
    decl String:ztele_teleloc[] = "Teleport player to saved location";
    
    SetMenuTitle(zr_tele_menu, "Teleport menu");
    AddMenuItem(zr_tele_menu, "ztele_player", ztele_player);
    AddMenuItem(zr_tele_menu, "ztele_abort", ztele_abort);
    AddMenuItem(zr_tele_menu, "ztele_save", ztele_save);
    AddMenuItem(zr_tele_menu, "ztele_teleloc", ztele_teleloc);
    
    SetMenuExitBackButton(zr_tele_menu, true);
    DisplayMenu(zr_tele_menu, client, MENU_TIME_FOREVER);
}

public ZRTeleHandle(Handle:zr_tele_menu , MenuAction:action, client, slot)
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
        CloseHandle(zr_tele_menu);
    }
}

ZRLogFlagsMenu(client)
{
    new Handle:zr_log_flags_menu = CreateMenu(ZRLogFlagsMenuHandle);
    new client_flags = GetUserFlagBits(client);
    new item_disabled = (client_flags & ADMFLAG_ROOT) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;

    decl String:z_log_core[64];
    decl String:z_log_game[64];
    decl String:z_log_player[64];
    decl String:z_log_debug[64];
    decl String:z_log_debug_detail[64];
    decl String:z_log_debug_max[64];
    decl String:z_log_admins[64];
    decl String:z_log_client[64];
    decl String:z_log_ignore_console[64];
    decl String:z_log_modules_enabled[64];
    decl String:z_log_module_zombie[64];
    decl String:z_log_module_ambience[64];
    decl String:z_log_module_overlays[64];
    decl String:z_log_module_saytriggers[64];
    decl String:z_log_module_teleport[64];
    decl String:z_log_module_classes[64];
    decl String:z_log_module_weaponrestrict[64];
    decl String:z_log_module_commands[64];
    decl String:z_log_module_anticamp[64];
    decl String:z_log_module_damagecontrol[64];
    decl String:z_log_module_offsets[64];

    Format(z_log_core, sizeof(z_log_core), "Log core events (%d)", LogHasFlag(LOG_CORE_EVENTS));
    Format(z_log_game, sizeof(z_log_game), "Log game events (%d)", LogHasFlag(LOG_GAME_EVENTS));
    Format(z_log_player, sizeof(z_log_player), "Log player commands (%d)", LogHasFlag(LOG_PLAYER_COMMANDS));
    Format(z_log_debug, sizeof(z_log_debug), "Log debug messages (%d)", LogHasFlag(LOG_DEBUG));
    Format(z_log_debug_detail, sizeof(z_log_debug_detail), "Log detailed debug messages (%d)", LogHasFlag(LOG_DEBUG_DETAIL));
    Format(z_log_debug_max, sizeof(z_log_debug_max), "Log low level debug messages (%d)", LogHasFlag(LOG_DEBUG_MAX_DETAIL));
    Format(z_log_admins, sizeof(z_log_admins), "Also log to admin chat (%d)", LogHasFlag(LOG_TO_ADMINS));
    Format(z_log_client, sizeof(z_log_client), "Also log to client console (%d)", LogHasFlag(LOG_TO_CLIENT));
    Format(z_log_ignore_console, sizeof(z_log_ignore_console), "Don't log from messages the console (%d)", LogHasFlag(LOG_IGNORE_CONSOLE));
    Format(z_log_modules_enabled, sizeof(z_log_modules_enabled), "Module based log control (%d)", LogHasFlag(LOG_MODULES_ENABLED));
    Format(z_log_module_zombie, sizeof(z_log_module_zombie), "Zombie (%d)", LogHasFlag(LOG_MODULE_ZOMBIE));
    Format(z_log_module_ambience, sizeof(z_log_module_ambience), "Ambience (%d)", LogHasFlag(LOG_MODULE_AMBIENCE));
    Format(z_log_module_overlays, sizeof(z_log_module_overlays), "Overlays (%d)", LogHasFlag(LOG_MODULE_OVERLAYS));
    Format(z_log_module_saytriggers, sizeof(z_log_module_saytriggers), "Chat commands (%d)", LogHasFlag(LOG_MODULE_SAYTRIGGERS));
    Format(z_log_module_teleport, sizeof(z_log_module_teleport), "Teleporter (%d)", LogHasFlag(LOG_MODULE_TELEPORT));
    Format(z_log_module_classes, sizeof(z_log_module_classes), "Classes (%d)", LogHasFlag(LOG_MODULE_CLASSES));
    Format(z_log_module_weaponrestrict, sizeof(z_log_module_weaponrestrict), "Weapon restrictions (%d)", LogHasFlag(LOG_MODULE_WEAPONRESTICT));
    Format(z_log_module_commands, sizeof(z_log_module_commands), "Admin commands (%d)", LogHasFlag(LOG_MODULE_COMMANDS));
    Format(z_log_module_anticamp, sizeof(z_log_module_anticamp), "Anticamp (%d)", LogHasFlag(LOG_MODULE_ANTICAMP));
    Format(z_log_module_damagecontrol, sizeof(z_log_module_damagecontrol), "Damage control (suicides) (%d)", LogHasFlag(LOG_MODULE_DAMAGECONTROL));
    Format(z_log_module_offsets, sizeof(z_log_module_offsets), "Offsets (properties) (%d)", LogHasFlag(LOG_MODULE_OFFSETS));
    
    SetMenuTitle(zr_log_flags_menu, "Toggle logging flags");
    AddMenuItem(zr_log_flags_menu, z_log_core, z_log_core, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_game, z_log_game, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_player, z_log_player, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_debug, z_log_debug, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_debug_detail, z_log_debug_detail, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_debug_max, z_log_debug_max, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_admins, z_log_admins);
    AddMenuItem(zr_log_flags_menu, z_log_client, z_log_client);
    AddMenuItem(zr_log_flags_menu, z_log_ignore_console, z_log_ignore_console, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_modules_enabled, z_log_modules_enabled, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_zombie, z_log_module_zombie, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_ambience, z_log_module_ambience, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_overlays, z_log_module_overlays, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_saytriggers, z_log_module_saytriggers, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_teleport, z_log_module_teleport, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_classes, z_log_module_classes, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_weaponrestrict, z_log_module_weaponrestrict, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_commands, z_log_module_commands, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_anticamp, z_log_module_anticamp, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_damagecontrol, z_log_module_damagecontrol, item_disabled);
    AddMenuItem(zr_log_flags_menu, z_log_module_offsets, z_log_module_offsets, item_disabled);
    
    SetMenuExitBackButton(zr_log_flags_menu, true);
    DisplayMenu(zr_log_flags_menu, client, MENU_TIME_FOREVER);
}

public ZRLogFlagsMenuHandle(Handle:zr_log_flags_menu , MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
            {
                ToggleLogFlag(LOG_CORE_EVENTS);
                ZRLogFlagsMenu(client);
            }
            case 1:
            {
                ToggleLogFlag(LOG_GAME_EVENTS);
                ZRLogFlagsMenu(client);
            }
            case 2:
            {
                ToggleLogFlag(LOG_PLAYER_COMMANDS);
                ZRLogFlagsMenu(client);
            }
            case 3:
            {
                ToggleLogFlag(LOG_DEBUG);
                ZRLogFlagsMenu(client);
            }
            case 4:
            {
                ToggleLogFlag(LOG_DEBUG_DETAIL);
                ZRLogFlagsMenu(client);
            }
            case 5:
            {
                ToggleLogFlag(LOG_DEBUG_MAX_DETAIL);
                ZRLogFlagsMenu(client);
            }
            case 6:
            {
                ToggleLogFlag(LOG_TO_ADMINS);
                ZRLogFlagsMenu(client);
            }
            case 7:
            {
                ToggleLogFlag(LOG_TO_CLIENT);
                ZRLogFlagsMenu(client);
            }
            case 8:
            {
                ToggleLogFlag(LOG_IGNORE_CONSOLE);
                ZRLogFlagsMenu(client);
            }
            case 9:
            {
                ToggleLogFlag(LOG_MODULES_ENABLED);
                ZRLogFlagsMenu(client);
            }
            case 10:
            {
                ToggleLogFlag(LOG_MODULE_ZOMBIE);
                ZRLogFlagsMenu(client);
            }
            case 11:
            {
                ToggleLogFlag(LOG_MODULE_AMBIENCE);
                ZRLogFlagsMenu(client);
            }
            case 12:
            {
                ToggleLogFlag(LOG_MODULE_OVERLAYS);
                ZRLogFlagsMenu(client);
            }
            case 13:
            {
                ToggleLogFlag(LOG_MODULE_SAYTRIGGERS);
                ZRLogFlagsMenu(client);
            }
            case 14:
            {
                ToggleLogFlag(LOG_MODULE_TELEPORT);
                ZRLogFlagsMenu(client);
            }
            case 15:
            {
                ToggleLogFlag(LOG_MODULE_CLASSES);
                ZRLogFlagsMenu(client);
            }
            case 16:
            {
                ToggleLogFlag(LOG_MODULE_WEAPONRESTICT);
                ZRLogFlagsMenu(client);
            }
            case 17:
            {
                ToggleLogFlag(LOG_MODULE_COMMANDS);
                ZRLogFlagsMenu(client);
            }
            case 18:
            {
                ToggleLogFlag(LOG_MODULE_ANTICAMP);
                ZRLogFlagsMenu(client);
            }
            case 19:
            {
                ToggleLogFlag(LOG_MODULE_DAMAGECONTROL);
                ZRLogFlagsMenu(client);
            }
            case 20:
            {
                ToggleLogFlag(LOG_MODULE_OFFSETS);
                ZRLogFlagsMenu(client);
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
        CloseHandle(zr_log_flags_menu );
    }
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

ToggleLogFlag(flag)
{
    new log_flags;
    log_flags = GetConVarInt(gCvars[CVAR_LOG]);
    if (log_flags & flag)
    {
        log_flags = log_flags - flag;
    }
    else
    {
        log_flags = log_flags + flag;
    }
    SetConVarInt(gCvars[CVAR_LOG], log_flags);
}