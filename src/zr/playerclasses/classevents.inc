/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          classevents.inc
 *  Type:          Core
 *  Description:   Functions for handling class related events.
 *
 * ============================================================================
 */

/* ------------------------------------
 *
 * GAME EVENTS
 *
 * ------------------------------------
 */

/**
 * Create class-related cookies here.
 */
ClassOnCookiesCreate()
{
    // Forward event to sub-modules.
    ClassOverlayOnCookiesCreate();
}

/**
 * Called when all modules are done loading.
 */
ClassOnModulesLoaded()
{
    // Set default classes on all player slots.
    ClassClientSetDefaultIndexes();
}

/**
 * Called when a client connects to the server (OnClientPutInServer).
 */
ClassClientInit(client)
{
    // Check if there are valid classes and the client is valid.
    // To prevent bots or SourceTV causing errors. If classes are invalid this
    // event was executed before classes were loaded. 
    if (ClassValidated && ZRIsClientValid(client))
    {
        // Set default class indexes on the player.
        ClassClientSetDefaultIndexes(client);
    }
    
    // Forward event to sub-modules
    ClassOverlayClientInit(client);
}

/**
 * Called a client disconnects.
 */
ClassOnClientDisconnect(client)
{
    // Disable class attributes with timers.
    ClassHealthRegenStop(client);
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
ClassOnClientSpawn(client)
{
    // Check if the player is alive.
    if (!IsPlayerAlive(client))
    {
        return;
    }
    
    // Check if there are valid classes. Block this event if classes aren't
    // done loading.
    if (!ClassValidated)
    {
        return;
    }
    
    // Reset attributes by triggering death event.
    ClassOnClientDeath(client);
    
    new bool:randomclass = GetConVarBool(g_hCvarsList[CVAR_CLASSES_RANDOM]);

    decl String:steamid[16];
    decl String:classname[64];
    
    // Assign random classes if enabled. Always do it for bots.
    GetClientAuthString(client, steamid, sizeof(steamid));
    if (randomclass || StrEqual(steamid, "BOT"))
    {
        // Get random classes for each type.
        new randomzombie = ClassGetRandomClass(ZR_CLASS_TEAM_ZOMBIES);
        new randomhuman = ClassGetRandomClass(ZR_CLASS_TEAM_HUMANS);
        
        // Mark zombie class as selected.
        ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES] = randomzombie;
        ClassGetName(randomzombie, classname, sizeof(classname), ZR_CLASS_TEAM_ZOMBIES);
        TranslationPrintToChat(client, "Classes random assignment", classname);
        
        // Mark human class as selected.
        ClassSelected[client][ZR_CLASS_TEAM_HUMANS] = randomhuman;
        ClassGetName(randomhuman, classname, sizeof(classname), ZR_CLASS_TEAM_HUMANS);
        TranslationPrintToChat(client, "Classes random assignment", classname);
        
        // Update player cache with the human class data, and apply it.
        ClassReloadPlayerCache(client, randomhuman);
        ClassApplyAttributes(client);
    }
    
    // Check if the player should spawn in admin mode.
    if (ClassPlayerAdminMode[client])
    {
        // Mark player as in admin mode.
        ClassPlayerInAdminMode[client] = true;
        
        // Update player cache with the admin class and apply attributes.
        new adminindex = ClassPlayerNextAdminClass[client];
        ClassReloadPlayerCache(client, adminindex);
        ClassApplyAttributes(client);
    }
    else
    {
        // Mark player as not in admin mode.
        ClassPlayerInAdminMode[client] = false;
        
        // Apply class attributes for the currently active class.
        ClassReloadPlayerCache(client, ClassGetActiveIndex(client));
        ClassApplyAttributes(client);
    }
}

/**
 * Client died. Stops timers and reset certain attributes. Call this event to
 * clean up class related stuff.
 * 
 * @param client    The client index.
 */
ClassOnClientDeath(client)
{
    // Disable class attributes with timers.
    ClassHealthRegenStop(client);
    
    // Set client's FOV back to normal.
    ToolsSetClientDefaultFOV(client, 90);
    
    // Forward event to sub-modules.
    ClassOverlayOnClientDeath(client);
}

/**
 * Client got infected. Reloads class attributes.
 * 
 * @param client    The client index.
 */
ClassOnClientInfected(client, bool:motherzombie = false)
{
    new classindex = ClassGetActiveIndex(client);
    new isadmin;
    new motherindex;
    
    decl String:motherzombiesetting[64];
    
    // Disable class attributes with timers.
    ClassHealthRegenStop(client);
    
    // Check if it's a mother zombie.
    if (motherzombie)
    {
        // Set negative admin flag if client is admin, so it's removed in
        // special class flags.
        isadmin = ZRIsClientAdmin(client) ? -ZR_CLASS_FLAG_ADMIN_ONLY : 0;
        
        // Get default mother zombie setting.
        GetConVarString(g_hCvarsList[CVAR_CLASSES_DEFAULT_M_ZOMB], motherzombiesetting, sizeof(motherzombiesetting));
        
        if (StrEqual(motherzombiesetting, "disabled", false))
        {
            // Do nothing. Keep current class.
        }
        else if (StrEqual(motherzombiesetting, "random", false))
        {
            // Get random regular zombie class. Remove admin flag if admin.
            motherindex = ClassGetRandomClass(ZR_CLASS_TEAM_ZOMBIES, _, _, ZR_CLASS_SPECIALFLAGS + isadmin);
        }
        else if (StrEqual(motherzombiesetting, "motherzombies", false))
        {
            // Get random mother zombie class.
            motherindex = ClassGetRandomClass(ZR_CLASS_TEAM_ZOMBIES, _, ZR_CLASS_FLAG_MOTHER_ZOMBIE, isadmin);
        }
        else
        {
            // Assume it's a class name. Get index for the specified class name.
            motherindex = ClassGetIndex(motherzombiesetting);
            
            // Validate index.
            if (ClassValidateIndex(motherindex))
            {
                // Change class.
                classindex = motherindex;
            }
        }
    }
    
    // Update the players cache with zombie attributes.
    ClassReloadPlayerCache(client, classindex);
    
    // Apply the new attributes.
    ClassApplyAttributes(client, motherzombie);
}
