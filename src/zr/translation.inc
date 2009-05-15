/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          translation.inc
 *   Type:          Core
 *   Description:   Translation parsing functions.
 *
 * ============================================================================
 */

/**
 * @section Max length of different message formats.
 */
#define TRANSLATION_MAX_LENGTH_CHAT 192
#define TRANSLATION_MAX_LENGTH_CONSOLE 1024
/**
 * @endsection
 */

/**
 * Prefix on all messages printed from the plugin.
 */
#define TRANSLATION_PHRASE_PREFIX "[ZR]"

/**
 * @section Text color chars.
 */
#define TRANSLATION_TEXT_COLOR_DEFAULT "\x01"
#define TRANSLATION_TEXT_COLOR_LGREEN "\x03"
#define TRANSLATION_TEXT_COLOR_GREEN "\x04"
/**
 * @endsection
 */

/**
 * HUD text usermsg
 */
#define TRANSLATION_USERMSG_HINTTEXT "HintText"

/**
 * Load translations file here.
 */
TranslationInit()
{
    // Load translations phrases used by plugin.
    LoadTranslations("common.phrases.txt");
    LoadTranslations("zombiereloaded.phrases.txt");
}

/**
 * Translate a phrase in zombiereloaded.phrases.txt
 * 
 * @param client        The client index
 * @param translation   The translated text.
 * @param maxlen        Maximum length of the translated string.
 * @param ...           Translation formatting parameters.  
 */
stock TranslationTranslatePhrase(client, String:translation[], maxlen, any:...)
{
    // Set translation target to given target.
    SetGlobalTransTarget(client);
    
    // Dump translation into return string.
    VFormat(translation, maxlen, "%t", 3);
}

/**
 * Format the string to the plugin's style.
 * 
 * @param text      Text to format.
 * @param maxlen    Maximum length of the formatted text.
 */
stock TranslationPluginFormatString(String:text[], maxlen, bool:color = true)
{
    if (color)
    {
        // Format prefix onto the string.
        Format(text, maxlen, "@green%s @default%s", TRANSLATION_PHRASE_PREFIX, text);
        
        // Replace color tokens with CS:S color chars.
        ReplaceString(text, maxlen, "@default", TRANSLATION_TEXT_COLOR_DEFAULT);
        ReplaceString(text, maxlen, "@lgreen", TRANSLATION_TEXT_COLOR_LGREEN);
        ReplaceString(text, maxlen, "@green", TRANSLATION_TEXT_COLOR_GREEN);
        
        return;
    }
    
    // Format prefix onto the string.
    Format(text, maxlen, "%s %s", TRANSLATION_PHRASE_PREFIX, text);
}

/**
 * Print chat text to client. (with style)
 * 
 * @param client    The client index.
 * @param ...       Translation formatting parameters.  
 */
stock TranslationPrintToChat(client, any:...)
{
    // Set translation target
    SetGlobalTransTarget(client);
    
    // Translate phrase.
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    VFormat(translation, sizeof(translation), "%t", 2);
    
    // Format string to create plugin style.
    TranslationPluginFormatString(translation, sizeof(translation));
    
    // Print translated phrase to client.
    PrintToChat(client, translation);
}

/**
 * Format the string to the plugin's style.
 * 
 * @param server    True to also print text to server console, false just to clients.
 * @param ...       Translation formatting parameters.
 */
stock TranslationPrintToChatAll(bool:server, bool:admin, any:...)
{
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    
    if (server)
    {
        // Set translation target
        SetGlobalTransTarget(LANG_SERVER);
        
        // Translate phrase.
        VFormat(translation, sizeof(translation), "%t", 3);
        
        // Format string to create plugin style.
        TranslationPluginFormatString(translation, sizeof(translation), false);
        
        // Print phrase to server.
        PrintToServer(translation);
    }
    
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client isn't an admin, and we're only printing to admins, then stop.
        if (admin && !ZRIsClientAdmin(x))
        {
            continue;
        }
        
        // Set translation target
        SetGlobalTransTarget(LANG_SERVER);
        
        // Translate phrase.
        VFormat(translation, sizeof(translation), "%t", 3);
        
        // Format string to create plugin style.
        TranslationPluginFormatString(translation, sizeof(translation));
        
        // Print translated phrase to client.
        PrintToChat(x, translation);
    }
}

/**
 * Print console text to client. (with style)
 * 
 * @param client    The client index.
 * @param ...       Translation formatting parameters.  
 */
stock TranslationPrintToConsole(client, any:...)
{
    // Set translation target
    SetGlobalTransTarget(client);
    
    // Translate phrase.
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    VFormat(translation, sizeof(translation), "%t", 2);
    
    // Format string to create plugin style.
    TranslationPluginFormatString(translation, sizeof(translation), false);
    
    // Print translated phrase to client.
    PrintToConsole(client, translation);
}

/**
 * Format the string to the plugin's style.
 * 
 * @param server    True to also print text to server console, false just to clients.
 * @param ...       Translation formatting parameters.
 */
stock TranslationPrintToConsoleAll(bool:server, bool:admin, any:...)
{
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    
    if (server)
    {
        // Set translation target
        SetGlobalTransTarget(LANG_SERVER);
        
        // Translate phrase.
        VFormat(translation, sizeof(translation), "%t", 3);
        
        // Format string to create plugin style.
        TranslationPluginFormatString(translation, sizeof(translation), false);
        
        // Print phrase to server.
        PrintToServer(translation);
    }
    
    // x = client index.
    for (new x = 1; x <= MaxClients; x++)
    {
        // If client isn't in-game, then stop.
        if (!IsClientInGame(x))
        {
            continue;
        }
        
        // If client isn't an admin, and we're only printing to admins, then stop.
        if (admin && !ZRIsClientAdmin(x))
        {
            continue;
        }
        
        // Set translation target
        SetGlobalTransTarget(LANG_SERVER);
        
        // Translate phrase.
        VFormat(translation, sizeof(translation), "%t", 3);
        
        // Format string to create plugin style.
        TranslationPluginFormatString(translation, sizeof(translation), false);
        
        // Print translated phrase to client.
        PrintToConsole(x, translation);
    }
}

/**
 * Print center text to client. (with style)
 * 
 * @param client    The client index.
 * @param ...       Translation formatting parameters.  
 */
stock TranslationPrintCenterText(client, any:...)
{
    // Set translation target
    SetGlobalTransTarget(client);
    
    // Translate phrase.
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    VFormat(translation, sizeof(translation), "%t", 2);
    
    // Print translated phrase to client.
    PrintCenterText(client, translation);
}

/**
 * Print HUD text to client. (with style)
 * 
 * @param client    The client index.
 * @param ...       Translation formatting parameters.  
 */
stock TranslationPrintHUDText(client, any:...)
{
    // Set translation target
    SetGlobalTransTarget(client);
    
    // Translate phrase.
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    VFormat(translation, sizeof(translation), "%t", 2);
    
    // Print translated phrase to client.
    
    // If hint text usermsg is invalid, then stop.
    new Handle:hHintText = StartMessageOne(TRANSLATION_USERMSG_HINTTEXT, client);
    if (hHintText == INVALID_HANDLE)
    {
        return;
    }
    
    BfWriteByte(hHintText, -1); 
    BfWriteString(hHintText, translation);
    EndMessage();
}

/**
 * Print text to server. (with style)
 * 
 * @param ...       Translation formatting parameters.  
 */
stock TranslationPrintToServer(any:...)
{
    // Set translation target
    SetGlobalTransTarget(LANG_SERVER);
    
    // Translate phrase.
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    VFormat(translation, sizeof(translation), "%t", 1);
    
    // Format string to create plugin style.
    TranslationPluginFormatString(translation, sizeof(translation), false);
    
    // Print translated phrase to client.
    PrintToServer(translation);
}

/**
 * Print chat text to client. (with style)
 * 
 * @param client    The client index.
 * @param ...       Translation formatting parameters.  
 */
stock TranslationReplyToCommand(client, any:...)
{
    // Set translation target
    SetGlobalTransTarget(client);
    
    // Translate phrase.
    decl String:translation[TRANSLATION_MAX_LENGTH_CHAT];
    VFormat(translation, sizeof(translation), "%t", 2);
    
    // Format string to create plugin style.
    TranslationPluginFormatString(translation, sizeof(translation));
    
    // Print translated phrase to client.
    ReplyToCommand(client, translation);
}