/**
 * ====================
 *   Zombie:Reloaded
 *   File: weaponrestrict.inc
 *   Author: Greyscale
 * ==================== 
 */

new Handle:restrictedWeapons = INVALID_HANDLE;

new gRestrict[MAXPLAYERS+1];

enum WepRestrictQuery
{
    Successful,  /** Weapon (un)restrict query was successful */
    Invalid,  /** Weapon invalid */
    Existing,  /** Already restricted */
}
    
InitWeaponRestrict()
{
    RegConsoleCmd("buy", BuyHook);
    
    restrictedWeapons = CreateArray(32, 0);
}

ClientHookUse(client)
{
    gRestrict[client] = Hacks_Hook(client, HACKS_HTYPE_WEAPON_CANUSE, Weapon_CanUse, false);
}

ClientUnHookUse(client)
{
    Hacks_Unhook(gRestrict[client]);
}

public Action:BuyHook(client, argc)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (!enabled)
    {
        return Plugin_Continue;
    }
    
    if (IsPlayerHuman(client))
    {
        decl String:weapon[64];
        GetCmdArg(1, weapon, sizeof(weapon));
        
        ReplaceString(weapon, sizeof(weapon), "weapon_", "");
        
        if (IsWeaponRestricted(weapon))
        {
            ZR_PrintToChat(client, "Weapon is restricted", weapon);
            return Plugin_Handled;
        }
    }
    else
    {
        ZR_PrintToChat(client, "Zombie cant use weapon");
        return Plugin_Handled;
    }
    
    return Plugin_Continue;
}

WepRestrictQuery:RestrictWeapon(const String:weapon[])
{
    if (IsWeaponGroup(weapon))
    {
        RestrictWeaponGroup(weapon);
        
        ZR_PrintToChat(0, "Weapon group has been restricted", weapon);
        
        return Successful;
    }
    
    if (!IsWeaponRestricted(weapon))
    {
        PushArrayString(restrictedWeapons, weapon);
        
        ZR_PrintToChat(0, "Weapon has been restricted", weapon);
        
        return Successful;
    }
    
    return Existing;
}

RestrictWeaponGroup(const String:group[])
{
    if (StrEqual(group, "pistols", false))
    {
        PushArrayString(restrictedWeapons, "glock");
        PushArrayString(restrictedWeapons, "usp");
        PushArrayString(restrictedWeapons, "p228");
        PushArrayString(restrictedWeapons, "deagle");
        PushArrayString(restrictedWeapons, "elite");
        PushArrayString(restrictedWeapons, "fiveseven");
    }
    else if (StrEqual(group, "shotguns", false))
    {
        PushArrayString(restrictedWeapons, "m3");
        PushArrayString(restrictedWeapons, "xm1014");
    }
    else if (StrEqual(group, "smgs", false))
    {
        PushArrayString(restrictedWeapons, "tmp");
        PushArrayString(restrictedWeapons, "mac10");
        PushArrayString(restrictedWeapons, "mp5navy");
        PushArrayString(restrictedWeapons, "ump45");
        PushArrayString(restrictedWeapons, "p90");
    }
    else if (StrEqual(group, "rifles", false))
    {
        PushArrayString(restrictedWeapons, "galil");
        PushArrayString(restrictedWeapons, "famas");
        PushArrayString(restrictedWeapons, "ak47");
        PushArrayString(restrictedWeapons, "m4a1");
        PushArrayString(restrictedWeapons, "sg552");
        PushArrayString(restrictedWeapons, "aug");
    }
    else if (StrEqual(group, "snipers", false))
    {
        PushArrayString(restrictedWeapons, "scout");
        PushArrayString(restrictedWeapons, "sg550");
        PushArrayString(restrictedWeapons, "g3sg1");
        PushArrayString(restrictedWeapons, "awp");
    }
}
    
WepRestrictQuery:UnRestrictWeapon(const String:weapon[])
{
    if (IsWeaponGroup(weapon))
    {
        UnRestrictWeaponGroup(weapon);
        
        ZR_PrintToChat(0, "Weapon group has been unrestricted", weapon);
        
        return Successful;
    }
    
    new index = GetRestrictedWeaponIndex(weapon);
    
    if (index > -1)
    {
        RemoveFromArray(restrictedWeapons, index);
        
        ZR_PrintToChat(0, "Weapon has been unrestricted", weapon);
        
        return Successful;
    }

    return Invalid;
}

UnRestrictWeaponGroup(const String:group[])
{
    if (StrEqual(group, "pistols", false))
    {
        UnRestrictWeapon("glock");
        UnRestrictWeapon("usp");
        UnRestrictWeapon("p228");
        UnRestrictWeapon("deagle");
        UnRestrictWeapon("elite");
        UnRestrictWeapon("fiveseven");
    }
    else if (StrEqual(group, "shotguns", false))
    {
        UnRestrictWeapon("m3");
        UnRestrictWeapon("xm1014");
    }
    else if (StrEqual(group, "smgs", false))
    {
        UnRestrictWeapon("tmp");
        UnRestrictWeapon("mac10");
        UnRestrictWeapon("mp5navy");
        UnRestrictWeapon("ump45");
        UnRestrictWeapon("p90");
    }
    else if (StrEqual(group, "rifles", false))
    {
        UnRestrictWeapon("galil");
        UnRestrictWeapon("famas");
        UnRestrictWeapon("ak47");
        UnRestrictWeapon("m4a1");
        UnRestrictWeapon("sg552");
        UnRestrictWeapon("aug");
    }
    else if (StrEqual(group, "snipers", false))
    {
        UnRestrictWeapon("scout");
        UnRestrictWeapon("sg550");
        UnRestrictWeapon("g3sg1");
        UnRestrictWeapon("awp");
    }
}

bool:IsWeaponRestricted(const String:weapon[])
{
    for (new x = 0; x < GetArraySize(restrictedWeapons); x++)
    {
        decl String:restrictedweapon[32];
        GetArrayString(restrictedWeapons, x, restrictedweapon, sizeof(restrictedweapon));
        
        if (StrEqual(weapon, restrictedweapon, false))
        {
            return true;
        }
    }
    
    return false;
}

GetRestrictedWeaponIndex(const String:weapon[])
{
    for (new x = 0; x < GetArraySize(restrictedWeapons); x++)
    {
        decl String:restrictedweapon[32];
        GetArrayString(restrictedWeapons, x, restrictedweapon, sizeof(restrictedweapon));
        ReplaceString(restrictedweapon, sizeof(restrictedweapon), "weapon_", "");
        
        if (StrEqual(weapon, restrictedweapon, false))
        {
            return x;
        }
    }
    
    return -1;
}

bool:IsWeaponGroup(const String:weapon[])
{
  return (StrEqual(weapon, "pistols", false) || StrEqual(weapon, "shotguns", false) || StrEqual(weapon, "smgs", false) || StrEqual(weapon, "rifles", false) || StrEqual(weapon, "snipers", false));
}

public Weapon_CanUse(client, weapon, dummy1, dummy2, dummy3, dummy4)
{
    new bool:enabled = GetConVarBool(gCvars[CVAR_ENABLE]);
    if (!enabled)
    {
        return Hacks_Continue;
    }
    
    new String:weaponname[32];
    GetEdictClassname(weapon, weaponname, sizeof(weaponname));
    
    ReplaceString(weaponname, sizeof(weaponname), "weapon_", "");
    
    if (IsWeaponRestricted(weaponname))
    {
        return 0;
    }
    
    if (IsPlayerZombie(client) && !StrEqual(weaponname, "knife"))
    {
        return 0;
    }
    
    return Hacks_Continue;
}