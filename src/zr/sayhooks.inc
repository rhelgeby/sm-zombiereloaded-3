/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          sayhooks.inc
 *   Type:          Core
 *   Description:   Hook plugin say commands and redirect to their handling module.
 *
 * ============================================================================
 */

/**
 * Max number of characters in a chat text string.
 */
#define SAYHOOKS_MAX_CHAT_LENGTH 192

/**
 * @section Say command key words.
 */
#define SAYHOOKS_KEYWORD_ZMENU      "!zmenu"
#define SAYHOOKS_KEYWORD_ZADMIN     "!zadmin"
#define SAYHOOKS_KEYWORD_ZCLASS     "!zclass"
#define SAYHOOKS_KEYWORD_ZSPAWN     "!zspawn"
#define SAYHOOKS_KEYWORD_ZTELE      "!ztele"
#define SAYHOOKS_KEYWORD_ZHP        "!zhp"
#define SAYHOOKS_KEYWORD_ZMARKET    "!zmarket"
/**
 * @endsection
 */

/**
 * @section Say hook quiet flags.
 */
#define SAYHOOKS_KEYWORD_FLAG_ZMENU      1
#define SAYHOOKS_KEYWORD_FLAG_ZADMIN     2
#define SAYHOOKS_KEYWORD_FLAG_ZCLASS     4
#define SAYHOOKS_KEYWORD_FLAG_ZSPAWN     8
#define SAYHOOKS_KEYWORD_FLAG_ZTELE      16
#define SAYHOOKS_KEYWORD_FLAG_ZHP        32
#define SAYHOOKS_KEYWORD_FLAG_ZMARKET    64
/**
 * @endsection
 */

/**
 * Say hooks module init function.
 */
SayHooksInit()
{
    // Hook client's say commands.
    RegConsoleCmd("say", SayHooksCmdSay);
    RegConsoleCmd("say_team", SayHooksCmdSay);
}

/**
 * Command callback. (say, say_team)
 * Catches all client's say text and takes action on key words.
 * 
 * @param client    The client index.
 * @param argc      The number of arguments in command string.
 */
public Action:SayHooksCmdSay(client, argc)
{
    decl String:args[SAYHOOKS_MAX_CHAT_LENGTH];
    
    // Get client's command string (typically 'say "text"')
    GetCmdArgString(args, sizeof(args));
    
    // Strip away the quotes.
    ReplaceString(args, sizeof(args), "\"", "");
    
    new chatflag = SayHooksChatToFlag(args);
    
    // If chatflag is invalid, then continue.
    if (!chatflag)
    {
        return Plugin_Continue;
    }
    
    // Define success as true until otherwise changed.
    new bool:success = true;
    
    switch(chatflag)
    {
        // Client triggered ZMenu flag.
        case SAYHOOKS_KEYWORD_FLAG_ZMENU:
        {
            // Send main menu.
            MenuMain(client);
        }
        // Client triggered ZAdmin flag.
        case SAYHOOKS_KEYWORD_FLAG_ZADMIN:
        {
            success = ZRAdminMenu(client);
        }
        // Client triggered ZClass flag.
        case SAYHOOKS_KEYWORD_FLAG_ZCLASS:
        {
            // Send class menu.
            ClassMenuMain(client);
        }
        // Client triggered ZSpawn flag.
        case SAYHOOKS_KEYWORD_FLAG_ZSPAWN:
        {
            // Spawns a late-joining client into the game.
            success = ZSpawnClient(client);
        }
        // Client triggered ZTele flag.
        case SAYHOOKS_KEYWORD_FLAG_ZTELE:
        {
            success = ZTeleClient(client);
        }
        // Client triggered ZHP flag.
        case SAYHOOKS_KEYWORD_FLAG_ZHP:
        {
            // Toggle ZHP.
            success = ZHPToggle(client);
        }
        // Client triggered ZMarket flag.
        case SAYHOOKS_KEYWORD_FLAG_ZMARKET:
        {
            // Send market menu.
            success = ZMarketMenu(client);
        }
    }
    
    // If quiet cvar is disabled, then continue.
    new bool:quiet = GetConVarBool(g_hCvarsList[CVAR_SAYHOOKS_QUIET]);
    if (!quiet)
    {
        return Plugin_Continue;
    }
    
    // If filter is enabled, and the command failed to be executed, then stop.
    new bool:filter = GetConVarBool(g_hCvarsList[CVAR_SAYHOOKS_QUIET_FILTER]);
    if (filter && !success)
    {
        return Plugin_Handled;
    }
    
    // If word is flagged to be quieted, then stop.
    new quietflags = GetConVarInt(g_hCvarsList[CVAR_SAYHOOKS_QUIET_FLAGS]);
    if (quietflags & chatflag)
    {
        return Plugin_Handled;
    }
    
    return Plugin_Continue;
}

/**
 * Convert chat text into a defined flag.
 * 
 * @param chat  The chat text to convert.
 * @return      Returns flag for word given, returns 0 if matches none.
 */
SayHooksChatToFlag(const String:chat[])
{
    // Return flag for chatstring.
    
    if (StrEqual(chat, SAYHOOKS_KEYWORD_ZMENU, false))
    {
        return SAYHOOKS_KEYWORD_FLAG_ZMENU;
    }
    else if (StrEqual(chat, SAYHOOKS_KEYWORD_ZADMIN, false))
    {
        return SAYHOOKS_KEYWORD_FLAG_ZADMIN;
    }
    else if (StrEqual(chat, SAYHOOKS_KEYWORD_ZCLASS, false))
    {
        return SAYHOOKS_KEYWORD_FLAG_ZCLASS;
    }
    else if (StrEqual(chat, SAYHOOKS_KEYWORD_ZSPAWN, false))
    {
        return SAYHOOKS_KEYWORD_FLAG_ZSPAWN;
    }
    else if (StrEqual(chat, SAYHOOKS_KEYWORD_ZTELE, false))
    {
        return SAYHOOKS_KEYWORD_FLAG_ZTELE;
    }
    else if (StrEqual(chat, SAYHOOKS_KEYWORD_ZHP, false))
    {
        return SAYHOOKS_KEYWORD_FLAG_ZHP;
    }
    else if (StrEqual(chat, SAYHOOKS_KEYWORD_ZMARKET, false))
    {
        return SAYHOOKS_KEYWORD_FLAG_ZMARKET;
    }
    
    // Return 0.
    return 0;
}
