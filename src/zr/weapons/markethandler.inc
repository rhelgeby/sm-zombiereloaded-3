/**
 * ====================
 *   Zombie:Reloaded
 *   File: markethandler.inc
 *   Author: Greyscale
 * ==================== 
 */

/**
 * (Market) Forward called when a client selects a weapon from the market.
 * 
 * @param client    The client index.
 * @param weaponid  The unique weapon ID used for market natives.
 * @return          True to allow market to take over, false to block purchase. 
 */
public bool:Market_OnWeaponSelected(client, String:weaponid[])
{
    // If player is dead or weaponid is invalid, then stop
    if (!weaponid[0] || !IsPlayerAlive(client))
    {
        return false;
    }
    
    // If player is a zombie, then stop
    if (IsPlayerZombie(client))
    {
        ZR_PrintToChat(client, "Zombie cant use weapon");
        
        return false;
    }
    
    // If player is using the rebuy option then allow
    if (StrEqual(weaponid, "rebuy"))
    {
        return true;
    }
    
    decl String:display[64];
    decl String:weapon[WEAPONS_MAX_LENGTH];
    new price;
    
    // If the market plugin can't find info about the weapon, then stop
    if (!Market_GetWeaponIDInfo(weaponid, display, weapon, price))
    {
        return false;
    }
    
    // Strip "weapon_" from entity name
    ReplaceString(weapon, sizeof(weapon), "weapon_", "");
    
    // If the weapon is restricted, then stop
    if (WeaponRestrictIsRestricted(weapon))
    {
        ZR_PrintToChat(client, "Weapon is restricted", weapon);
        
        return false;
    }
    
    // Check if buyzone cvar is enabled, and if the client is in a buyzone
    new bool:buyzone = GetConVarBool(gCvars[CVAR_ZMARKET_BUYZONE]); 
    if (!IsClientInBuyZone(client) && buyzone)
    {
        ZR_PrintCenterText(client, "Market out of buyzone");
        
        return false;
    }
    
    return true;
}

/**
 * (Market) Forward called one frame after a client selects a weapon from the market.
 * 
 * @param client    The client index.
 * @param allowed   True when the weapon was purchased successfully, false otherwise.
 */
public Market_PostOnWeaponSelected(client, &bool:allowed)
{
    // If the purchase wasn't allowed, then stop
    if (!allowed)
    {
        return;
    }
    
    // Resend market menu
    ZMarket(client);
}