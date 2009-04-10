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
        ZClass(client);
    }
    
    else if (StrEqual(args, "!zmarket", false))
    {
        ZMarket(client);
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
        ZHP(client);
    }
    else if (StrContains(args, "teleport", false) != -1
             || StrContains(args, "stuck", false) != -1
             || StrContains(args, "help", false) != -1)
    {
        ZR_PrintToChat(client, "!ztele stuck");
    }
    
    return Plugin_Continue;
}

bool:ZClass(client)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (!classes)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        
        return false;
    }
    
    new bool:randomclass = GetConVarBool(gCvars[CVAR_CLASSES_RANDOM]);
    if (randomclass)
    {
        ZR_PrintToChat(client, "Random class is enabled");
        
        return false;
    }
    
    ClassMenu(client);
    
    return true;
}

bool:ZMarket(client)
{
    if (!market)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        
        return false;
    }
    
    if (!IsPlayerAlive(client))
    {
        ZR_PrintToChat(client, "Must be alive");
        
        return false;
    }
    
    new bool:buyzone = GetConVarBool(gCvars[CVAR_ZMARKET_BUYZONE]); 
    if (!IsClientInBuyZone(client) && buyzone)
    {
        ZR_PrintCenterText(client, "Market out of buyzone");
        
        return false;
    }
    
    SetGlobalTransTarget(client);
    
    decl String:title[64];
    decl String:rebuy[64];
    
    Format(title, sizeof(title), "%t\n ", "Market title");
    Format(rebuy, sizeof(rebuy), "%t\n ", "Market rebuy");
    
    Market_Send(client, title, rebuy);
    
    return true;
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
    
    if (IsPlayerInList(client))
    {
        return;
    }

    RespawnPlayer(client);
    
    AddPlayerToList(client);
}

ZHP(client)
{
    new bool:zhp = GetConVarBool(gCvars[CVAR_ZHP]);
    if (!zhp)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        
        return;
    }
    
    if (dispHP[client])
    {
        ZR_PrintToChat(client, "!zhp disabled");
    }
    else
    {
        ZR_PrintToChat(client, "!zhp enabled");
        UpdateHPDisplay(client);
    }
    
    dispHP[client] = !dispHP[client];
}