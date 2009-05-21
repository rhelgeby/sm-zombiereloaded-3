/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          commands.inc
 *   Type:          Core
 *   Description:   Console command creation and hooking.
 *
 * ============================================================================
 */

/**
 * Commands are created/hooked.
 */
CommandsInit()
{
    // Forward event to modules. (create commands)
    ConfigOnCommandsCreate();
    ClassOnCommandsCreate();
    WeaponsOnCommandsCreate();
    
    // Forward event to modules. (hook commands)
    ClassOnCommandsHook();
    DamageOnCommandsHook();
}

/*CreateCommands()
{
    RegAdminCmd("zr_infect", Command_Infect, ADMFLAG_GENERIC, "Infects the specified player");
    RegAdminCmd("zr_spawn", Command_Respawn, ADMFLAG_GENERIC, "Respawns the specified player following auto-respawning rules");
    
    
    //RegAdminCmd("zr_set_class_knockback", Command_SetClassKnockback, ADMFLAG_GENERIC, "Sets the knockback to the specified class. Usage: zr_set_class_knockback <class name> <value>");
    //RegAdminCmd("zr_get_class_knockback", Command_GetClassKnockback, ADMFLAG_GENERIC, "Gets the knockback to the specified class. Usage: zr_get_class_knockback <class name>");
    
    RegAdminCmd("zr_admin", Command_AdminMenu, ADMFLAG_GENERIC, "Displays the admin menu for Zombie: Reloaded.");
    //RegAdminCmd("zr_knockback_m", Command_KnockbackMMenu, ADMFLAG_GENERIC, "Displays the knockback multiplier menu.");
    
    RegAdminCmd("zr_anticamp_create_volume", Command_AnticampCreateVolume, ADMFLAG_GENERIC, "Creates a rectangular hurt volume between two points. Usage: ht_create_volume <damage> <interval> <x1> <y1> <z1> <x2> <y2> <z2>");
    RegAdminCmd("zr_anticamp_remove_volume", Command_AnticampRemoveVolume, ADMFLAG_GENERIC, "Removes a volume. Use zr_anticamp_list to list volumes. Usage: zr_anticamp_remove_volume <volume index>");
    RegAdminCmd("zr_anticamp_list", Command_AnticampList, ADMFLAG_GENERIC, "List current volumes.");
    
    RegConsoleCmd("zr_log_flags", Command_LogFlags, "List available logging flags.");
}*/

/*public Action:Command_Infect(client, argc)
{
    new bool:enabled = GetConVarBool(g_hCvarsList[CVAR_ENABLE]);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    decl String:target_name_list[MAX_TARGET_LENGTH];
    new targets[MAXPLAYERS];
    new bool:tn_is_ml;
    
    new tcount = ProcessTargetString(arg1, client, targets, MAXPLAYERS, COMMAND_FILTER_ALIVE, target_name_list, sizeof(target_name_list), tn_is_ml);
    if (tcount <= 0)
    {
        ReplyToTargetError(client, tcount);
        return Plugin_Handled;
    }
    
    decl String:target_name[64];
    decl String:client_name[64];
    
    if (client > 0)
    {
        GetClientName(client, client_name, sizeof(client_name));
    }
    else
    {
        client_name = "Console\0";
    }

    for (new x = 0; x < tcount; x++)
    {
        InfectClient(targets[x]);
        if (LogCheckFlag(LOG_GAME_EVENTS, LOG_MODULE_COMMANDS))
        {
            GetClientName(targets[x], target_name, sizeof(target_name));
            LogMessageFormatted(client, "admin commands", "infect", "\"%s\" infected \"%s\".", true, client_name, target_name);
        }
    }
    
    return Plugin_Handled;
}

public Action:Command_Respawn(client, argc)
{
    new bool:enabled = GetConVarBool(g_hCvarsList[CVAR_ENABLE]);
    if (argc < 1 || !enabled)
    {
        return Plugin_Handled;
    }
    
    decl String:arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    
    decl String:target_name_list[MAX_TARGET_LENGTH];
    new targets[MAXPLAYERS];
    new bool:tn_is_ml;
    
    new tcount = ProcessTargetString(arg1, client, targets, MAXPLAYERS, COMMAND_FILTER_DEAD, target_name_list, sizeof(target_name_list), tn_is_ml);
    if (tcount <= 0)
    {
        ReplyToTargetError(client, tcount);
        return Plugin_Handled;
    }
    
    decl String:client_name[64];
    decl String:target_name[64];
    
    if (client > 0)
    {
        GetClientName(client, client_name, sizeof(client_name));
    }
    else
    {
        client_name = "Console\0";
    }
    
    new team;
    for (new x = 0; x < tcount; x++)
    {
        team = GetClientTeam(targets[x]);
        if (team == CS_TEAM_T || team == CS_TEAM_CT)
        {
            if (LogCheckFlag(LOG_GAME_EVENTS, LOG_MODULE_COMMANDS))
            {
                GetClientName(targets[x], target_name, sizeof(target_name));
                LogMessageFormatted(targets[x], "admin commands", "spawn", "\"%s\" spawned player \"%s\".", true, client_name, target_name);
            }
            
            RespawnSpawnClient(targets[x]);
        }
    }
    
    return Plugin_Handled;
}*/

/*public Action:Command_AdminMenu(client, argc)
{
    if (ZRIsClientValid(client))
    {
        ZRAdminMenu(client);
    }
    else
    {
        // BAD!
        // ReplyToCommand(client, "This menu cannot be used from the console.");
    }
    return Plugin_Handled;
}

public Action:Command_KnockbackMMenu(client, argc)
{
    if (ZRIsClientValid(client))
    {
        // Disabled, under construction.
        // ZRKnockbackMMenu(client);
    }
    else
    {
        // Tsk tsk no translation? :P
        //ReplyToCommand(client, "This menu cannot be used from the console.");
    }
    return Plugin_Handled;
}

public Action:Command_TeleMenu(client, argc)
{
    if (ZRIsClientValid(client))
    {
        ZRZTeleMenu(client);
    }
    else
    {
        // BAD!
        ReplyToCommand(client, "This menu cannot be used from the console.");
    }
    return Plugin_Handled;
}

public Action:Command_LogFlags(client, argc)
{
    decl String:message[2048];
    message[0] = 0;
    
    StrCat(message, sizeof(message), "LOG_CORE_EVENTS            (1)      - Log core events like executing files, error messages, etc.\n");
    StrCat(message, sizeof(message), "LOG_GAME_EVENTS            (2)      - Log game events like admin commands, suicide prevention and anticamp kills.\n");
    StrCat(message, sizeof(message), "LOG_PLAYER_COMMANDS        (4)      - Log commands made by the player.\n");
    StrCat(message, sizeof(message), "LOG_DEBUG                  (8)      - Enable debug messages (if they exist).\n");
    StrCat(message, sizeof(message), "LOG_DEBUG_DETAIL           (16)     - Detailed debug messages. May cause spam.\n");
    StrCat(message, sizeof(message), "LOG_DEBUG_MAX_DETAIL       (32)     - Low level detailed debug messages. Causes spam! Only enable right before and after testing.\n");
    StrCat(message, sizeof(message), "LOG_LOG_TO_ADMINS          (64)     - Display log messages to admin chat.\n");
    StrCat(message, sizeof(message), "LOG_LOG_TO_CLIENT          (128)    - Display log messages to the client that executed the event/command.\n");
    StrCat(message, sizeof(message), "LOG_IGNORE_CONSOLE         (256)    - Don't log messages from client 0 (console).\n");
    StrCat(message, sizeof(message), "LOG_MODULES_ENABLED        (512)    - Enable detailed log control for developers. Module logs overrides previous flags.\n");
    StrCat(message, sizeof(message), "LOG_MODULE_ZOMBIE          (1024)   - zombie.inc");

    ReplyToCommand(client, message);
    message[0] = 0;

    StrCat(message, sizeof(message), "LOG_MODULE_AMBIENTSOUNDS   (2048)   - ambientsounds.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_OVERLAYS        (4096)   - overlays.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_SAYTRIGGERS     (8192)   - sayhooks.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_TELEPORT        (16384)  - teleport.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_CLASSES         (32768)  - classes.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_WEAPONRESTICT   (65536)  - weaponrestrict.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_COMMANDS        (131072) - commands.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_ANTICAMP        (262144) - anticamp.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_DAMAGE          (524288) - damage.inc\n");
    StrCat(message, sizeof(message), "LOG_MODULE_OFFSETS         (524288) - offsets.inc");
    
    ReplyToCommand(client, message);
    return Plugin_Handled;
}*/
