/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        classevents.inc
 *   Description: Functions for handling class related events.
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
 * To be called when a client connect to the server.
 * (OnClientPutInServer)
 */
ClassClientInit(client)
{
    if (ZRIsValidClient(client))
    {
        // Set default class indexes on the player.
        ClassClientSetDefaultIndexes(client);
    }
}

ClassOnClientDisconnect(client)
{
    // Stop timers related to class attributes.
    ClassOverlayStop(client);
}

ClassOnClientSpawn(client)
{
    if (!IsPlayerAlive(client))
    {
        // The client isn't alive.
        return;
    }
    
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
        ZR_PrintToChat(client, "Auto-assign", classname);
        
        // Mark human class as selected.
        ClassSelected[client][ZR_CLASS_TEAM_HUMANS] = randomhuman;
        ClassGetName(randomhuman, classname, sizeof(classname), ZR_CLASS_TEAM_HUMANS);
        ZR_PrintToChat(client, "Auto-assign", classname);
        
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
        // Not in admin mode, spawn as a zombie or human.
        
        // Mark player as not in admin mode.
        ClassPlayerInAdminMode[client] = false;
        
        // Apply class attributes for the currently active class.
        ClassReloadPlayerCache(client, ClassGetActiveIndex(client));
        ClassApplyAttributes(client);
    }
}

ClassOnClientDeath(client)
{
    // Reset certain attributes to not make spectating disorted.
    ClassHealthRegenStop(client);
    ClassOverlayStop(client);
    
    SetPlayerFOV(client, 90);
    NightVisionOn(client, false);
}

ClassOnClientInfected(client, bool:motherzombie = false)
{
    new classindex = ClassGetActiveIndex(client);
    
    // Update the players cache with zombie attributes.
    ClassReloadPlayerCache(client, classindex);
    
    // Apply the new attributes.
    ClassApplyAttributes(client, motherzombie);
}


/* ------------------------------------
 *
 * PLAYER COMMANDS
 *
 * ------------------------------------
 */

