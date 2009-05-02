/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        markethandler.inc
 *   Description: Handles market (optional plugin) API, natives, and forwards.
 *
 * ============================================================================
 */

/**
 * Global variable set to true if market plugin is installed
 */ 
new bool:g_bMarket;

/**
 * Sends market menu to client.
 * 
 * @param client    The client index.
 */    
bool:ZMarketMenu(client)
{
    // If market is disabled, then stop.
    if (!g_bMarket)
    {
        // Tell client market is disabled.
        ZR_PrintToChat(client, "Feature is disabled");
        return false;
    }
    
    // If player is dead, then stop.
    if (!IsPlayerAlive(client))
    {
        // Tell player they must be alive.
        ZR_PrintToChat(client, "Must be alive");
        return false;
    }
    
    // Check buyzone cvar to see if client has to be in a buyzone to use.
    new bool:buyzone = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]); 
    if (!ZMarketIsClientInBuyZone(client) && buyzone)
    {
        // Tell client they must be in a buyzone.
        ZR_PrintCenterText(client, "Market out of buyzone");
        return false;
    }
    
    // Set translate target to client.
    SetGlobalTransTarget(client);
    
    // Format title and rebuy lines.
    decl String:title[64];
    decl String:rebuy[64];
    
    Format(title, sizeof(title), "%t\n ", "Market title");
    Format(rebuy, sizeof(rebuy), "%t\n ", "Market rebuy");
    
    // Send market menu.
    Market_Send(client, title, rebuy);
    
    // Successfully sent the market menu.
    return true;
}

/**
 * Checks if a client is in a buyzone.
 * 
 * @param client    The client index.
 */
bool:ZMarketIsClientInBuyZone(client)
{
    // Return if client is in buyzone.
    return bool:GetEntData(client, g_iToolsInBuyZone);
}

/**
 * (Market) Forward called when a client selects a weapon from the market.
 * 
 * @param client    The client index.
 * @param weaponid  The unique weapon ID used for market natives.
 * @return          True to allow market to take over, false to block purchase. 
 */
public bool:Market_OnWeaponSelected(client, String:weaponid[])
{
    // If player is dead or weaponid is invalid, then stop.
    if (!weaponid[0] || !IsPlayerAlive(client))
    {
        return false;
    }
    
    // If player is a zombie, then stop.
    if (InfectIsClientInfected(client))
    {
        ZR_PrintToChat(client, "Zombie cant use weapon");
        
        return false;
    }
    
    // If player is using the rebuy option then allow.
    if (StrEqual(weaponid, "rebuy"))
    {
        return true;
    }
    
    decl String:display[64];
    decl String:weapon[WEAPONS_MAX_LENGTH];
    new price;
    
    // If the market plugin can't find info about the weapon, then stop.
    if (!Market_GetWeaponIDInfo(weaponid, display, weapon, price))
    {
        return false;
    }
    
    // Strip "weapon_" from entity name.
    ReplaceString(weapon, sizeof(weapon), "weapon_", "");
    
    // If the weapon is restricted, then stop.
    if (RestrictIsWeaponRestricted(weapon))
    {
        ZR_PrintToChat(client, "Weapon is restricted", weapon);
        
        return false;
    }
    
    // Check if buyzone cvar is enabled, and if the client is in a buyzone.
    new bool:buyzone = GetConVarBool(g_hCvarsList[CVAR_WEAPONS_ZMARKET_BUYZONE]); 
    if (!ZMarketIsClientInBuyZone(client) && buyzone)
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
    // If the purchase wasn't allowed, then stop.
    if (!allowed)
    {
        return;
    }
    
    // Resend market menu.
    ZMarketMenu(client);
}
