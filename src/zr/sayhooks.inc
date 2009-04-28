/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        sayhooks.inc
 *   Description: Hook plugin say commands and redirect to their handling module.
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
    
    // If client triggered the zmenu keyword, then send menu.
    if (StrEqual(args, SAYHOOKS_KEYWORD_ZMENU, false))
    {
        MenuMain(client);
    }
    
    // If client triggered the zmenu keyword, then send menu.
    else if (StrEqual(args, SAYHOOKS_KEYWORD_ZADMIN, false))
    {
        ZRAdminMenu(client);
    }
    
    // If client triggered the zmenu keyword, then send menu.
    else if (StrEqual(args, SAYHOOKS_KEYWORD_ZCLASS, false))
    {
        ClassMenuMain(client);
    }
    
    // If client triggered the zmenu keyword, then send menu.
    else if (StrEqual(args, SAYHOOKS_KEYWORD_ZSPAWN, false))
    {
        // Spawns a late-joining client into the game.
        ZSpawnClient(client);
    }
    
    // If client triggered the zmenu keyword, then send menu.
    else if (StrEqual(args, SAYHOOKS_KEYWORD_ZTELE, false))
    {
        ZTele(client);
    }
    
    // If client triggered the zmenu keyword, then send menu.
    else if (StrEqual(args, SAYHOOKS_KEYWORD_ZHP, false))
    {
        // Toggle ZHP.
        ZHPToggle(client);
    }
    
    // If client triggered the zmenu keyword, then send menu.
    else if (StrEqual(args, SAYHOOKS_KEYWORD_ZMARKET, false))
    {
        // Send market menu.
        ZMarketSend(client);
    }
    
    return Plugin_Continue;
}