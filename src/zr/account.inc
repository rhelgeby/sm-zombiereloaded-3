/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          account.inc
 *   Type:          Module
 *   Description:   Handles client's accounts. (cash)
 *
 * ============================================================================
 */

/**
 * Maximum limit for cash in CS:S.
 */
#define ACCOUNT_CASH_MAX 16000

/**
 * Variable to store account offset value.
 */
new g_iToolsAccount;

/**
 * Find account-specific offsets here.
 */
AccountOnOffsetsFound()
{
    // If offset "m_iAccount" can't be found, then stop the plugin.
    g_iToolsAccount = FindSendPropInfo("CCSPlayer", "m_iAccount");
    if (g_iToolsAccount == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Account, "Offsets", "Offset \"CCSPlayer::m_iAccount\" was not found.");
    }
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
AccountOnClientSpawn(client)
{
    // If cashfill cvar is disabled, then stop.
    new bool:accountcashfill = GetConVarBool(g_hCvarsList[CVAR_ACCOUNT_CASHFILL]);
    if (!accountcashfill)
    {
        return;
    }
    
    // Get cash value.
    new cash = GetConVarInt(g_hCvarsList[CVAR_ACCOUNT_CASHFILL_VALUE]);
    
    // Set client's account.
    AccountSetClientCash(client, cash);
}

/**
 * Get's a client's account value (cash)
 * 
 * @param client    The client index.
 * @return          The client's current account value.
 */
stock AccountGetClientCash(client)
{
    // Set client's cash.
    return GetEntData(client, g_iToolsAccount, 4);
}

/**
 * Set's a client's account value (cash)
 * 
 * @param client    The client index.
 * @param value    The value to set to.
 */
stock AccountSetClientCash(client, value)
{
    // If value if below 0, then set to 0.
    if (value < 0)
    {
        value = 0;
    }
    // If value is above max, then set to max.
    else if (value > ACCOUNT_CASH_MAX)
    {
        value = ACCOUNT_CASH_MAX;
    }
    
    // Set client's cash.
    SetEntData(client, g_iToolsAccount, value, 4);
}
