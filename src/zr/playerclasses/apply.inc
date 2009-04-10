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
    ClassApplyNightVision(client, classindex);
    ClassApplyFOV(client, classindex);
    ClassApplyHealth(client, classindex, improved);
    ClassApplyHealthRegen(client, classindex);
    ClassApplySpeed(client, classindex);
    
    return true;
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
bool:ClassApplyModel(client, classindex, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    decl String:modelpath[256];
    
    // Get the model path from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        ClassGetModelPath(client, modelpath, sizeof(modelpath), cachetype);
    }
    else
    {
        ClassGetModelPath(classindex, modelpath, sizeof(modelpath), cachetype);
    }
    
    // Check if the user specified a random model.
    if (strcmp(modelpath, "random", false) == 0)
    {
        // TODO: Make a function that gets a random model from the specified team.
        new randmodel = GetRandomInt(0, GetArraySize(arrayModels) - 1);
        GetArrayString(arrayModels, randmodel, modelpath, sizeof(modelpath));
        Format(modelpath, sizeof(modelpath), "%s.mdl", modelpath);
    }
    
    // TODO: Add support for keeping the default cs model ("default").
    
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
        alpha = ClassGetAlphaInitial(client, cachetype);
    }
    else
    {
        alpha = ClassGetAlphaInitial(classindex, cachetype);
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

/**
 * Gives night vision to a player.
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
bool:ClassApplyNightVision(client, classindex, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    new bool:nvgs;
    
    // Get the night vision setting from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        nvgs = ClassGetNvgs(client, cachetype);
    }
    else
    {
        nvgs = ClassGetNvgs(classindex, cachetype);
    }
    
    NightVision(client, nvgs);
    
    // Turn on night vision if applied.
    if (nvgs)
    {
        NightVisionOn(client, nvgs);
    }
    return true;
}

/**
 * Sets the field of view setting on a player.
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
bool:ClassApplyFOV(client, classindex, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    new fov;
    
    // Get the field of view setting from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        fov = ClassGetFOV(client, cachetype);
    }
    else
    {
        fov = ClassGetFOV(classindex, cachetype);
    }
    
    SetPlayerFOV(client, fov);
    return true;
}

/**
 * Gives health points on a player.
 *
 * @param client        The client index.
 * @param classindex    The class to read from.
 * @param boost         Double health boost. Default: false
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                      ZR_CLASS_CACHE_PLAYER (default) - Player cache.
 * @return  True on success, false otherwise.
 */
bool:ClassApplyHealth(client, classindex, bool:boost = false, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    new health;
    
    // Get the health points from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        health = ClassGetHealth(client, cachetype);
    }
    else
    {
        health = ClassGetHealth(classindex, cachetype);
    }
    
    if (boost)
    {
        health *= 2;
    }
    
    SetEntityHealth(client, health);
    return true;
}

/**
 * Applies health regeneration on a player if enabled.
 *
 * @param client        The client index.
 * @param classindex    The class to read from.
 * @param boost         Double health boost. Default: false
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                      ZR_CLASS_CACHE_PLAYER (default) - Player cache.
 * @return  True if applied, false otherwise.
 */
bool:ClassApplyHealthRegen(client, classindex, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    new Float:interval;
    new amount;
    new max;
    
    // Get the health regeneration info from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        interval = ClassGetHealthRegenInterval(client, cachetype);
        amount = ClassGetHealthRegenAmount(client, cachetype);
        max = ClassGetHealth(client, cachetype);
    }
    else
    {
        interval = ClassGetHealthRegenInterval(classindex, cachetype);
        amount = ClassGetHealthRegenAmount(classindex, cachetype);
        max = ClassGetHealth(classindex, cachetype);
    }
    
    if (interval > 0)
    {
        ClassHealthRegenInitialize(client, interval, amount, max);
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Sets the players speed.
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
bool:ClassApplySpeed(client, classindex, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    new Float:speed;
    
    // Get the health points from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        speed = ClassGetSpeed(client, cachetype);
    }
    else
    {
        speed = ClassGetSpeed(classindex, cachetype);
    }
    
    SetPlayerSpeed(client, speed);
    return true;
}
