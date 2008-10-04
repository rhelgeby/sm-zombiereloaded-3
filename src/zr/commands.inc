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
    
    RegAdminCmd("zr_restrict", Command_Restrict, ADMFLAG_GENERIC, "Restrict a specified weapon");
    RegAdminCmd("zr_unrestrict", Command_UnRestrict, ADMFLAG_GENERIC, "Unrestrict a specified weapon");
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
    
    for (new x = 0; x < tcount; x++)
    {
        RespawnPlayer(targets[x]);
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