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

stock ZR_ReplyToCommand(client, any:...)
{
    decl String:phrase[192];
    
    SetGlobalTransTarget(client);
        
    VFormat(phrase, sizeof(phrase), "%t", 2);
    FormatTextString(phrase, sizeof(phrase));
    
    ReplyToCommand(client, phrase);
}

stock ZR_PrintToAdminChat(String:message[])
{
    decl String:buffer[192];
    Format(buffer, sizeof(buffer), "[ZR] %s", message);
    for (new client = 1; client < maxclients; client++)
    {
        if (IsClientConnected(client) && IsClientInGame(client) && GetUserAdmin(client) != INVALID_ADMIN_ID)
        {
            PrintToChat(client, buffer);
        }
    }
}