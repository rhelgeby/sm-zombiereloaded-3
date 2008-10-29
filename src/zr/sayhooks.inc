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
    
    else if (StrEqual(args, "!ztele", false))
    {
        ZTele(client);
    }
    
    else if (StrEqual(args, "!zstuck", false))
    {
        ZStuck(client);
    }
    
    else if (StrEqual(args, "!zhp", false))
    {
        ZHP(client);
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

public bool:Market_OnWeaponSelected(client, String:weaponid[])
{
    if (!weaponid[0] || !IsPlayerAlive(client))
    {
        return false;
    }
    
    if (IsPlayerZombie(client))
    {
        ZR_PrintToChat(client, "Zombie cant use weapon");
        
        return false;
    }
    
    if (StrEqual(weaponid, "rebuy"))
    {
        return true;
    }
    
    decl String:display[64];
    decl String:weapon[32];
    new price;
    
    if (!Market_GetWeaponIDInfo(weaponid, display, weapon, price))
    {
        return false;
    }
    
    ReplaceString(weapon, sizeof(weapon), "weapon_", "");
    
    if (IsWeaponRestricted(weapon))
    {
        ZR_PrintToChat(client, "Weapon is restricted", weapon);
        
        return false;
    }
    
    new bool:buyzone = GetConVarBool(gCvars[CVAR_ZMARKET_BUYZONE]); 
    if (!IsClientInBuyZone(client) && buyzone)
    {
        ZR_PrintCenterText(client, "Market out of buyzone");
        
        return false;
    }
    
    return true;
}

public Market_PostOnWeaponSelected(client, &bool:allowed)
{
    if (!allowed)
    {
        return;
    }
    
    ZMarket(client);
}

ZSpawn(client)
{
    if (GetConVarBool(gCvars[CVAR_DEBUG])) ZR_DebugPrintToConsole(client, "ZSpawn: Spawn request:");
    new bool:spawn = GetConVarBool(gCvars[CVAR_ZSPAWN]);
    if (!spawn)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        if (GetConVarBool(gCvars[CVAR_DEBUG])) ZR_DebugPrintToConsole(client, "ZSpawn: Not enabled.");
        return;
    }
    
    new team = GetClientTeam(client);
    if (team != CS_TEAM_T && team != CS_TEAM_CT)
    {
        if (GetConVarBool(gCvars[CVAR_DEBUG])) ZR_DebugPrintToConsole(client, "ZSpawn: Player is not a T or CT.");
        return;
    }
    
    if (IsPlayerAlive(client))
    {
        if (GetConVarBool(gCvars[CVAR_DEBUG])) ZR_DebugPrintToConsole(client, "ZSpawn: Player appears to be alive.");
        return;
    }
    
    if (IsPlayerInList(client))
    {
        if (GetConVarBool(gCvars[CVAR_DEBUG])) ZR_DebugPrintToConsole(client, "ZSpawn: Player is already in the list (= maybe spawned, or bug).");
        return;
    }

    if (GetConVarBool(gCvars[CVAR_DEBUG])) ZR_DebugPrintToConsole(client, "ZSpawn: Spawned player");
    RespawnPlayer(client);
    
    AddPlayerToList(client);
}

ZTele(client)
{
    new bool:tele = GetConVarBool(gCvars[CVAR_ZTELE]);
    if (!tele)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        
        return;
    }
    
    if (!IsPlayerZombie(client) && zombieSpawned)
    {
        ZR_PrintToChat(client, "!ztele humans restricted");
        
        return;
    }
    
    if (!IsPlayerAlive(client) || tHandles[client][TTELE] != INVALID_HANDLE)
    {
        return;
    }
            
    new count = teleCount[client];
    new limit = GetConVarInt(gCvars[CVAR_ZTELE_LIMIT]);
    
    if (limit > 0)
    {
        if (count < limit)
        {
            tHandles[client][TTELE] = CreateTimer(3.0, Teleport, client, TIMER_FLAG_NO_MAPCHANGE);
            teleCount[client]++;
            ZR_PrintToChat(client, "!ztele amount", limit - teleCount[client]);
        }
        else
        {
            ZR_PrintToChat(client, "!ztele limit reached");
        }
    }
    else
    {
        tHandles[client][TTELE] = CreateTimer(3.0, Teleport, client, TIMER_FLAG_NO_MAPCHANGE);
        ZR_PrintToChat(client, "!ztele amount unlimited");
    }
}

public Action:Teleport(Handle:timer, any:index)
{
    TeleportEntity(index, spawnLoc[index], NULL_VECTOR, NULL_VECTOR);
    
    tHandles[index][TTELE] = INVALID_HANDLE;
}

ZStuck(client)
{
    new bool:stuck = GetConVarBool(gCvars[CVAR_ZTELE]);
    if (!stuck)
    {
        ZR_PrintToChat(client, "Feature is disabled");
        
        return;
    }
    
    if (!IsPlayerAlive(client))
    {
        return;
    }
    
    new Float:clientloc[3];
    new Float:stuckloc[3];
    
    GetClientAbsOrigin(client, clientloc);
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (!IsClientInGame(x) || !IsPlayerAlive(x))
        {
            continue;
        }
        
        GetClientAbsOrigin(x, stuckloc);
        if (GetVectorDistance(clientloc, stuckloc) <= 60)
        {
            NoCollide(x, true);
            CreateTimer(0.5, CollisionOn, x, TIMER_FLAG_NO_MAPCHANGE);
        }
    }
    
    new Float:x = GetRandomFloat(-150.0, 150.0);
    new Float:y = GetRandomFloat(-150.0, 150.0);
    
    new Float:nudge[3];
    
    nudge[0] = x;
    nudge[1] = y;
    
    SetPlayerVelocity(client, nudge);
}

public Action:CollisionOn(Handle:timer, any:index)
{
    if (!IsClientInGame(index))
    {
        return;
    }
    
    NoCollide(index, false);
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