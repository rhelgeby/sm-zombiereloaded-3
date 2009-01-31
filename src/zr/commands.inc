/**
 * ====================
 *   Zombie:Reloaded
 *   File: commands.inc
 *   Author: Greyscale
 * ====================
 */
 
CreateCommands()
{
    RegAdminCmd("zr_infect", Command_Infect, ADMFLAG_GENERIC, "Infects the specified player");
    RegAdminCmd("zr_spawn", Command_Respawn, ADMFLAG_GENERIC, "Respawns the specified player following auto-respawning rules");

    RegAdminCmd("zr_teleport", Command_Teleport, ADMFLAG_GENERIC, "Teleports one or more players to spawn. Usage: zr_teleport <target>");
    RegAdminCmd("zr_tele_saveloc", Command_TeleSaveLocation, ADMFLAG_GENERIC, "Saves your or a players location to a buffer. Usage: zr_tele_saveloc [#userid|name]");
    RegAdminCmd("zr_tele_loc", Command_TeleportToLocation, ADMFLAG_GENERIC, "Teleports you or a player to the saved location. Usage: zr_tele_loc [#userid|name]");
    RegAdminCmd("zr_tele_abort", Command_TeleportAbort, ADMFLAG_GENERIC, "Aborts a teleportation or cooldown on a client. Usage: zr_tele_abort <target>");

    RegAdminCmd("zr_restrict", Command_Restrict, ADMFLAG_GENERIC, "Restrict a specified weapon");
    RegAdminCmd("zr_unrestrict", Command_UnRestrict, ADMFLAG_GENERIC, "Unrestrict a specified weapon");

    RegAdminCmd("zr_set_class_knockback", Command_SetClassKnockback, ADMFLAG_GENERIC, "Sets the knockback to the specified class. Usage: zr_set_class_knockback <class name> <value>");
    RegAdminCmd("zr_get_class_knockback", Command_GetClassKnockback, ADMFLAG_GENERIC, "Gets the knockback to the specified class. Usage: zr_get_class_knockback <class name>");

    RegAdminCmd("zr_admin", Command_AdminMenu, ADMFLAG_GENERIC, "Displays the admin menu for Zombie: Reloaded.");
    RegAdminCmd("zr_knockback_m", Command_KnockbackMMenu, ADMFLAG_GENERIC, "Displays the knockback multiplier menu.");
    RegAdminCmd("zr_teleadmin", Command_TeleMenu, ADMFLAG_GENERIC, "Displays the teleport admin menu for Zombie: Reloaded.");
    
    RegAdminCmd("zr_anticamp_create_volume", Command_AnticampCreateVolume, ADMFLAG_GENERIC, "Creates a rectangular hurt volume between two points. Usage: ht_create_volume <damage> <interval> <x1> <y1> <z1> <x2> <y2> <z2>");
    RegAdminCmd("zr_anticamp_remove_volume", Command_AnticampRemoveVolume, ADMFLAG_GENERIC, "Removes a volume. Use zr_anticamp_list to list volumes. Usage: zr_anticamp_remove_volume <volume index>");
    RegAdminCmd("zr_anticamp_list", Command_AnticampList, ADMFLAG_GENERIC, "List current volumes.");
    
    RegConsoleCmd("zr_log_flags", Command_LogFlags, "List available logging flags.");
}

public Action:Command_Infect(client, argc)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    decl String:target_name[MAX_TARGET_LENGTH];
    new targets[MAXPLAYERS];
    new bool:tn_is_ml;
    
    new tcount = ProcessTargetString(arg1, client, targets, MAXPLAYERS, COMMAND_FILTER_ALIVE, target_name, sizeof(target_name), tn_is_ml);
    if (tcount <= 0)
    {
        ReplyToTargetError(client, tcount);
        return Plugin_Handled;
    }
    
    for (new x = 0; x < tcount; x++)
    {
        Zombify(targets[x], 0);
    }
    
    return Plugin_Handled;
}

public Action:Command_Respawn(client, argc)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    decl String:target_name[MAX_TARGET_LENGTH];
    new targets[MAXPLAYERS];
    new bool:tn_is_ml;
    
    new tcount = ProcessTargetString(arg1, client, targets, MAXPLAYERS, COMMAND_FILTER_DEAD, target_name, sizeof(target_name), tn_is_ml);
    if (tcount <= 0)
    {
        ReplyToTargetError(client, tcount);
        return Plugin_Handled;
    }
    
    new team;
    for (new x = 0; x < tcount; x++)
    {
        team = GetClientTeam(targets[x]);
        if (team == CS_TEAM_T || team == CS_TEAM_CT)
        {
            if (LogHasFlag(LOG_GAME_EVENTS)) ZR_LogMessageFormatted(targets[x], "Commands", "Command_Respawn", "ZSpawn spawned player %d.", true, targets[x]);
            RespawnPlayer(targets[x]);
        }
    }
    
    return Plugin_Handled;
}

public Action:Command_Restrict(client, argc)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    new WepRestrictQuery:output = RestrictWeapon(arg1);
    
    if (output == Existing)
    {
        ZR_ReplyToCommand(client, "Weapon already restricted", arg1);
    }
    
    return Plugin_Handled;
}

public Action:Command_UnRestrict(client, argc)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    new WepRestrictQuery:output = UnRestrictWeapon(arg1);
    
    if (output == Invalid)
    {
        ZR_ReplyToCommand(client, "Weapon invalid", arg1);
    }
    
    return Plugin_Handled;
}

public Action:Command_SetClassKnockback(client, argc)
{
    if (argc < 2)
    {
        if (client == 0)
        {
            PrintToServer("Sets the specified class knockback. Usage: zr_set_class_knockback <classname> <knockback>");
            return Plugin_Handled;
        }
        else
        {
            PrintToConsole(client, "Sets the specified class knockback. Usage: zr_set_class_knockback <classname> <knockback>");
            return Plugin_Handled;
        }
    }

    decl String:classname[64];
    decl String:knockback_arg[8];
    new classindex;
    new Float:knockback;

    GetCmdArg(1, classname, sizeof(classname));
    GetCmdArg(2, knockback_arg, sizeof(knockback_arg));
    classindex = GetClassIndex(classname);
    knockback = StringToFloat(knockback_arg);

    if (classindex < 0)
    {
        if (client == 0)
        {
            PrintToServer("Could not find the class %s.", classname);
            return Plugin_Handled;
        }
        else
        {
            PrintToConsole(client, "Could not find the class %s.", classname);
            return Plugin_Handled;
        }
    }

    arrayClasses[classindex][data_knockback] = knockback;
    return Plugin_Handled;
}

public Action:Command_GetClassKnockback(client, argc)
{
    if (argc < 1)
    {
        if (client == 0)
        {
            PrintToServer("Gets the specified class knockback. Usage: zr_get_class_knockback <classname>");
            return Plugin_Handled;
        }
        else
        {
            PrintToConsole(client, "Gets the specified class knockback. Usage: zr_get_class_knockback <classname>");
            return Plugin_Handled;
        }
    }

    decl String:classname[64];
    new classindex;
    new Float:knockback;

    GetCmdArg(1, classname, sizeof(classname));
    classindex = GetClassIndex(classname);

    if (classindex < 0)
    {
        if (client == 0)
        {
            PrintToServer("Could not find the class %s.", classname);
            return Plugin_Handled;
        }
        else
        {
            PrintToConsole(client, "Could not find the class %s.", classname);
            return Plugin_Handled;
        }
    }

    knockback = arrayClasses[classindex][data_knockback];

    if (client == 0)
    {
        PrintToServer("Current knockback for %s: %f", classname, knockback);
    }
    else
    {
       PrintToConsole(client, "Current knockback for %s: %f", classname, knockback);
    }

    return Plugin_Handled;
}

public Action:Command_AdminMenu(client, argc)
{
    if (IsClientPlayer(client))
    {
        ZRAdminMenu(client);
    }
    else
    {
        PrintToServer("This menu cannot be used in the console. Client: %d", client);
        if (client > 0) PrintToConsole(client, "You cannot use this menu. Client: %d", client);
    }
    return Plugin_Handled;
}

public Action:Command_KnockbackMMenu(client, argc)
{
    if (IsClientPlayer(client))
    {
        ZRKnockbackMMenu(client);
    }
    else
    {
        PrintToServer("This menu cannot be used in the console. Client: %d", client);
        if (client > 0) PrintToConsole(client, "You cannot use this menu. Client: %d", client);
    }
    return Plugin_Handled;
}

public Action:Command_TeleMenu(client, argc)
{
    if (IsClientPlayer(client))
    {
        ZRTeleMenu(client);
    }
    else
    {
        PrintToServer("This menu cannot be used in the console. Client: %d", client);
        if (client > 0) PrintToConsole(client, "You cannot use this menu. Client: %d", client);
    }
    return Plugin_Handled;
}

public Action:Command_LogFlags(client, argc)
{
    decl String:message[2048];
    
    StrCat(message, sizeof(message), "LOG_FLAG_CORE_EVENTS (1)        - Log core events like executing files, restricting weapons, etc.\n");
    StrCat(message, sizeof(message), "LOG_FLAG_GAME_EVENTS (2)        - Log infections.\n");
    StrCat(message, sizeof(message), "LOG_FLAG_PLAYER_COMMANDS (4)    - Log zspawn, teleports, class change, etc.\n");
    StrCat(message, sizeof(message), "LOG_FLAG_DEBUG (8)              - Enable debug messages (if they exist).\n");
    StrCat(message, sizeof(message), "LOG_FLAG_DEBUG (16)             - Detailed debug messages. May cause spam.\n");
    StrCat(message, sizeof(message), "LOG_FLAG_DEBUG_MAX_DETAIL (32)  - Low level detailed debug messages. Causes spam! Only enable right before and after testing.\n");
    StrCat(message, sizeof(message), "LOG_FLAG_LOG_TO_ADMINS (64)     - Display log messages to admin chat.\n");
    StrCat(message, sizeof(message), "LOG_FLAG_LOG_TO_CLIENT (128)    - Display log messages to the client that executed the event/command.\n");
    
    ReplyToCommand(client, message);
}