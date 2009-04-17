/**
 * ====================
 *   Zombie:Reloaded
 *   File: sayhooks.inc
 *   Author: Greyscale
 * ==================== 
 */

HookChatCmds()
{
    RegConsoleCmd("say", SayCommand);
    RegConsoleCmd("say_team", SayCommand);
}

public Action:SayCommand(client, argc)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (!client || !enabled)
    {
        return Plugin_Continue;
    }
    
    decl String:args[192];
    
    GetCmdArgString(args, sizeof(args));
    ReplaceString(args, sizeof(args), "\"", "");
    
    if (StrEqual(args, "!zmenu", false))
    {
        MainMenu(client);
    }
    
    if (StrEqual(args, "!zadmin", false))
    {
        ZRAdminMenu(client);
    }
    
    else if (StrEqual(args, "!zclass", false))
    {
        ClassMenuMain(client);
    }
    
    else if (StrEqual(args, "!zmarket", false))
    {
        // Send market menu.
        ZMarketSend(client);
    }
    
    else if (StrEqual(args, "!zspawn", false))
    {
        ZSpawn(client);
    }
    
    else if (StrEqual(args, "!tp", false) ||
             StrEqual(args, "!ztele", false) ||
             StrEqual(args, "!tele", false) ||
             StrEqual(args, "!teleport", false))
    {
        ZTele(client);
    }
    
    else if (StrEqual(args, "!teleabort", false))
    {
        AbortTeleport(client, false);
    }
    
    else if (StrEqual(args, "!zhp", false))
    {
        // Toggle ZHP.
        ZHPToggle(client);
    }
    else if (StrContains(args, "teleport", false) != -1
             || StrContains(args, "stuck", false) != -1
             || StrContains(args, "help", false) != -1)
    {
        ZR_PrintToChat(client, "!ztele stuck");
    }
    
    return Plugin_Continue;
}

ZSpawn(client)
{
    new bool:spawn = GetConVarBool(gCvars[CVAR_ZSPAWN]);
    if (!spawn)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        return;
    }
    
    new team = GetClientTeam(client);
    if (team != CS_TEAM_T && team != CS_TEAM_CT)
    {
        return;
    }
    
    if (IsPlayerAlive(client))
    {
        return;
    }

    RespawnSpawnClient(client);
}