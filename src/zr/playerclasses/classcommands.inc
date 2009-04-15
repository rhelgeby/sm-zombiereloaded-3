/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        classcommands.inc
 *   Description: Console commands for working with classes.
 *
 * ============================================================================
 */

/**
 * Dumps class data at a specified index in the specified cache.
 */
public Action:Command_ClassDump(client, argc)
{
    decl String:syntax[1024];
    syntax[0] = 0;
    
    if (argc < 2)
    {
        // Write syntax info.
        StrCat(syntax, sizeof(syntax), "Dumps class data at a specified index in the specified cache. Usage: zr_class_dump <cachetype> <index|targetname>\n\n");
        StrCat(syntax, sizeof(syntax), "Cache types:\n");
        StrCat(syntax, sizeof(syntax), "original - Unmodified class data\n");
        StrCat(syntax, sizeof(syntax), "modified - Newest class data\n");
        StrCat(syntax, sizeof(syntax), "player   - Players class data\n");
        ReplyToCommand(client, syntax);
        
        return Plugin_Handled;
    }
    
    new cachetype = -1;
    new index = -1;
    
    decl String:type[64];
    decl String:target[64];
    decl String:buffer[2048];
    
    // Quick initialize buffer.
    buffer[0] = 0;
    
    // Get cache type.
    GetCmdArg(1, type, sizeof(type));
    
    // Set cache type depending on parameter setting.
    if (StrEqual(type, "original", false))
    {
        cachetype = ZR_CLASS_CACHE_ORIGINAL;
    }
    else if (StrEqual(type, "modified", false))
    {
        cachetype = ZR_CLASS_CACHE_MODIFIED;
    }
    else if (StrEqual(type, "player", false))
    {
        cachetype = ZR_CLASS_CACHE_PLAYER;
        
        // Get client index.
        GetCmdArg(2, target, sizeof(target));
        index = FindTarget(client, target, _, false);
        
        // Check if failed.
        if (index < 0)
        {
            ReplyToCommand(client, "Invalid target name.");
            return Plugin_Handled;
        }
    }
    
    // Check if cachetype is valid.
    if (cachetype < 0)
    {
        ReplyToCommand(client, "Invalid cache type.");
        return Plugin_Handled;
    }
    
    // Validate class index.
    if (cachetype != ZR_CLASS_CACHE_PLAYER)
    {
        // Get class index.
        GetCmdArg(2, target, sizeof(target));
        index = StringToInt(target);
        
        if (!ClassValidateIndex(index))
        {
            ReplyToCommand(client, "Invalid class index.");
            return Plugin_Handled;
        }
    }
    
    // Dump the specified cache.
    ReplyToCommand(client, "DUMPING CACHE: \"%s\" (%d classes total)\n========================================\n", type, ClassCount);
    ClassDumpData(index, cachetype, buffer, sizeof(buffer));
    ZR_ReplyToCommandLong(client, buffer);
    
    return Plugin_Handled;

}
