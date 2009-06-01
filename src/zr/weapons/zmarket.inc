/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          
 *   Type:          Module
 *   Description:   
 *
 * ============================================================================
 */

/**
 * Variable to store buyzone offset value.
 */
new g_iToolsInBuyZone;

/**
 * Initialize market data.
 */
ZMarketInit()
{
}

/**
 * Find ZMarket-specific offsets here.
 */
ZMarketOnOffsetsFound()
{
    // If offset "m_bInBuyZone" can't be found, then stop the plugin.
    g_iToolsInBuyZone = FindSendPropInfo("CCSPlayer", "m_bInBuyZone");
    if (g_iToolsInBuyZone == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CCSPlayer::m_bInBuyZone\" was not found.");
    }
}

/**
 * Checks if a client is in a buyzone.
 * 
 * @param client    The client index.
 */
stock bool:ZMarketIsClientInBuyZone(client)
{
    // Return if client is in buyzone.
    return bool:GetEntData(client, g_iToolsInBuyZone);
}