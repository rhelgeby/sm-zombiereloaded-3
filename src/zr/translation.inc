/**
 * ====================
 *   Zombie:Reloaded
 *   File: translations.inc
 *   Author: Greyscale
 * ==================== 
 */

#define LOG_FORMAT_TYPE_SIMPLE     0      // Simple log message, no module or block info.
#define LOG_FORMAT_TYPE_FULL       1      // Full log message, with module and block info.
#define LOG_FORMAT_TYPE_ERROR      2      // Full log message, but log to error log instead.

FormatTextString(String:text[], maxlen)
{
    Format(text, maxlen, "@green[%t] @default%s", "ZR", text);
    
    ReplaceString(text, maxlen, "@default","\x01");
    ReplaceString(text, maxlen, "@lgreen","\x03");
    ReplaceString(text, maxlen, "@green","\x04");
}

stock ZR_PrintToChat(client, any:...)
{
    decl String:phrase[192];
    
    if (client)
    {
        SetGlobalTransTarget(client);
        
        VFormat(phrase, sizeof(phrase), "%t", 2);
        FormatTextString(phrase, sizeof(phrase));
        
        PrintToChat(client, phrase);
    }
    else
    {
        SetGlobalTransTarget(client);
        
        VFormat(phrase, sizeof(phrase), "%t", 2);
        FormatTextString(phrase, sizeof(phrase));
        
        PrintToServer(phrase);
        
        new maxplayers = GetMaxClients();
        for (new x = 1; x <= maxplayers; x++)
        {
            if (IsClientInGame(x))
            {
                SetGlobalTransTarget(x);
                
                VFormat(phrase, sizeof(phrase), "%t", 2);
                FormatTextString(phrase, sizeof(phrase));
                
                PrintToChat(x, phrase);
            }
        }
    }
}

stock ZR_PrintCenterText(client, any:...)
{
    SetGlobalTransTarget(client);
    
    decl String:phrase[192];
    
    VFormat(phrase, sizeof(phrase), "%t", 2);
    
    PrintCenterText(client, phrase);
}

stock ZR_HudHint(client, any:...)
{
    SetGlobalTransTarget(client);
    
    decl String:phrase[192];
    
    VFormat(phrase, sizeof(phrase), "%t", 2);
    
    new Handle:hHintText = StartMessageOne("HintText", client);
    if (hHintText != INVALID_HANDLE)
    {
        BfWriteByte(hHintText, -1); 
        BfWriteString(hHintText, phrase);
        EndMessage();
    }
}

stock ZR_PrintToServer(any:...)
{
    SetGlobalTransTarget(LANG_SERVER);
    
    decl String:phrase[192];
    decl String:buffer[192];
    
    VFormat(phrase, sizeof(phrase), "%t", 1);
    Format(buffer, sizeof(buffer), "[%t] %s", "ZR", phrase);
    
    PrintToServer(buffer);
}

stock ZR_LogMessage(any:...)
{
    SetGlobalTransTarget(LANG_SERVER);
    
    decl String:phrase[192];
    
    VFormat(phrase, sizeof(phrase), "%t", 1);
    
    LogMessage(phrase);
}

stock ZR_TranslateMessage(String:buffer[], maxlen, any:...)
{
    SetGlobalTransTarget(LANG_SERVER);
    VFormat(buffer, maxlen, "%t", 3);
}

/**
 * Logs a formatted message with module and block info depending, on the type.
 *
 * @param client    Specifies the client who triggered the event/command. Use
 *                  -1 for core events like validation, etc.
 * @param module    what module the log event belongs to.
 * @param block     What function or code block the log is triggered from.
 * @param message   Log message. Formatted string.
 * @param type      Optional. What logging type or style to use. Options:
 *                  LOG_FORMAT_TYPE_SIMPLE - Simple, no module or block info.
 *                  LOG_FORMAT_TYPE_FULL - Full, with module and block info.
 *                  LOG_FORMAT_TYPE_ERROR - Full, but log to error log instead.
 * @param any...    Formatting parameters.
 */
stock ZR_LogMessageFormatted(client, const String:module[], const String:block[], const String:message[], type = LOG_FORMAT_TYPE_FULL, any:...)
{
    decl String:buffer[2048];
    decl String:text[2048];

    if (client == 0 && LogHasFlag(LOG_IGNORE_CONSOLE))
    {
        return;
    }
    
    switch (type)
    {
        case LOG_FORMAT_TYPE_SIMPLE:
        {
            VFormat(buffer, sizeof(buffer), message, 6);
            Format(text, sizeof(text), "Log -- %s", message);
            LogMessage(text);
        }
        case LOG_FORMAT_TYPE_FULL:
        {
            VFormat(buffer, sizeof(buffer), message, 6);
            Format(text, sizeof(text), "Log (%s : %s) -- %s", module, block, buffer);
            LogMessage(text);
        }
        case LOG_FORMAT_TYPE_ERROR:
        {
            VFormat(buffer, sizeof(buffer), message, 6);
            Format(text, sizeof(text), "Log (%s : %s) -- %s", module, block, buffer);
            LogError(text);
        }
    }

    if (LogHasFlag(LOG_TO_ADMINS))
    {
        ZR_PrintToAdminChat(text);
    }
    if (client > 0 && LogHasFlag(LOG_TO_CLIENT) && IsClientConnected(client) && IsClientInGame(client))
    {
        PrintToConsole(client, "[ZR] %s", text);
    }
}

stock ZR_ReplyToCommand(client, any:...)
{
    decl String:phrase[192];
    
    SetGlobalTransTarget(client);
        
    VFormat(phrase, sizeof(phrase), "%t", 2);
    FormatTextString(phrase, sizeof(phrase));
    
    ReplyToCommand(client, phrase);
}

/**
 * Adds support for printing long strings.
 */
stock ZR_ReplyToCommandLong(client, const String:text[])
{
    decl String:partbuffer[1000];
    new pos;
    new cellswritten = 1;   // Initialize for the loop.

    while (cellswritten)
    {
        cellswritten = strcopy(partbuffer, sizeof(partbuffer), text[pos]);
        ReplyToCommand(client, partbuffer);
        pos += cellswritten;
    }
}

stock ZR_PrintToAdminChat(String:message[])
{
    decl String:buffer[256];
    Format(buffer, sizeof(buffer), "[ZR] %s", message);
    for (new client = 1; client < maxclients; client++)
    {
        if (IsClientConnected(client) && IsClientInGame(client) && GetAdminFlag(GetUserAdmin(client), Admin_Generic))
        {
            PrintToChat(client, buffer);
        }
    }
}