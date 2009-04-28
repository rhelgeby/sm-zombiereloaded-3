/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        account.inc
 *   Description: (Module) Handles client's accounts. (cash)
 *
 * ============================================================================
 */

/**
 * Maximum limit for cash in CS:S.
 */
#define ACCOUNT_CASH_MAX 16000

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
 * Set's a client's account value (cash)
 * 
 * @param client    The client index.
 * @param value    The value to set to.
 */
AccountSetClientCash(client, value)
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