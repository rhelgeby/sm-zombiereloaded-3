/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        apply.inc
 *   Description: Functions for applying attributes and effects on a client.
 *   Author:      Richard Helgeby
 *
 * ============================================================================
 */

/**
 * Applies all class attributes on a player. Changing model, hp, speed, health,
 * effects etc. The players current team will be used to get the class index.
 *
 * @param client        The player to apply attributes on.
 * @param improved      Optional. Gives advantages or improvements in some
 *                      attributes. To be used on mother zombies. Default is
 *                      false.
 * @return  True if all success on applying all attributes, false otherwise.
 */
bool:ClassApplyAttributes(client, bool:improved = false)
{
    new classindex = ClassGetActiveIndex(client);
    
    if (classindex < 0)
    {
        return false;
    }
    
    ClassApplyModel(client, classindex);
    ClassApplyAlpha(client, classindex);
    ClassApplyOverlay(client, classindex);
}

/**
 * Changes the model on a player.
 *
 * @param client        The client index.
 * @param classindex    The class to read from.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                      ZR_CLASS_CACHE_PLAYER (default) - Player cache.
 * @return  True on success, false otherwise.
 */
bool:ClassApplyModel(client, classindex, cachetpye = ZR_CLASS_CACHE_PLAYER)
{
    decl String:modelpath[256];
    
    // Get the model path from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        ClassGetModelPath(client, modelpath, sizeof(modelpath), cachetype)
    }
    else
    {
        ClassGetModelPath(classindex, modelpath, sizeof(modelpath), cachetype)
    }
    
    // Check if the user specified a random model.
    if (strcmp(modelpath, "random", false) == 0)
    {
        // TODO: Make a function that gets a random model from the specified team.
        new randmodel = GetRandomInt(0, GetArraySize(arrayModels) - 1);
        GetArrayString(arrayModels, randmodel, modelpath, sizeof(modelpath));
        Format(modelpath, sizeof(modelpath), "%s.mdl", modelpath);
    }
    
    SetPlayerModel(client, modelpath);
    return true;
}

/**
 * Sets transparency on a player.
 *
 * @param client        The client index.
 * @param classindex    The class to read from.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                      ZR_CLASS_CACHE_PLAYER (default) - Player cache.
 * @return  True on success, false otherwise.
 */
bool:ClassApplyAlpha(client, classindex, cachetype = ZR_CLASS_CACHE_PLAYER)
{
	new alpha;
	
	// Get the alpha value from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        alpha = ClassGetAlphaSpawn(client, cachetype);
    }
    else
    {
        alpha = ClassGetAlphaSpawn(classindex, cachetype);
    }
    
    if (alpha < 0)
    {
        return false;
    }
    
	SetPlayerAlpha(client, alpha);
	return true;
}

/**
 * Apply the overlay on a player if not applied.
 *
 * @param client        The client index.
 * @param classindex    The class to read from.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                      ZR_CLASS_CACHE_PLAYER (default) - Player cache.
 * @return  True on success, false otherwise.
 */
bool:ClassApplyOverlay(client, classindex, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    decl String:overlay[256];
    
    // Validate DirectX requirements.
    if (dxLevel[client] < DXLEVEL_MIN)
    {
        // DirectX version is too old.
        // TODO: Log warning?
        return false;
    }
    
    // Get the overlay path from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        ClassGetOverlayPath(client, overlay, sizeof(overlay), cachetype);
    }
    else
    {
        ClassGetOverlayPath(classindex, overlay, sizeof(overlay), cachetype);
    }
    
    ClassOverlayInitialize(client, overlay);
    return true;
}
