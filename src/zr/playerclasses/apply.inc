/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          apply.inc
 *  Type:          Core
 *  Description:   Functions for applying attributes and effects on a client.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    decl String:modelpath[PLATFORM_MAX_PATH];
    
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
    
    // Check if the user specified no change.
    else if (strcmp(modelpath, "default", false) == 0)
    {
        // Don't change the model.
        return true;
    }
    
    SetEntityModel(client, modelpath);
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
    
    ToolsSetClientAlpha(client, alpha);
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
    decl String:overlaypath[PLATFORM_MAX_PATH];
    
    // Get the overlay path from the specified cache.
    if (cachetype == ZR_CLASS_CACHE_PLAYER)
    {
        ClassGetOverlayPath(client, overlaypath, sizeof(overlaypath), cachetype);
    }
    else
    {
        ClassGetOverlayPath(classindex, overlaypath, sizeof(overlaypath), cachetype);
    }
    
    ClassOverlayInitialize(client, overlaypath);
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
    
    ToolsClientNightVision(client, nvgs);
    ToolsClientNightVision(client, nvgs, false);
    
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
    
    ToolsSetClientDefaultFOV(client, fov);
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
    
    ToolsSetClientLMV(client, speed);
    return true;
}
