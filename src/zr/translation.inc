/**
 * ====================
 *   Zombie:Reloaded
 *   File: translations.inc
 *   Author: Greyscale
 * ==================== 
 */

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
    
    if (ZRIsValidClient(client))
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
        
        for (new x = 1; x <= MaxClients; x++)
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
 * 
 * @param client    The client index.
 * @param text      The text to print.   
 */
stock ZR_ReplyToCommandLong(client, const String:text[])
{
    decl String:partbuffer[1024];
    new pos;
    new cellswritten = 1;   // Initialize for the loop.

    while (cellswritten)
    {
        cellswritten = strcopy(partbuffer, sizeof(partbuffer), text[pos]);
        ReplyToCommand(client, partbuffer);
        pos += cellswritten;
    }
}