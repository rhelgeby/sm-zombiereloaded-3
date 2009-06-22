/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          attributes.inc
 *  Type:          Core
 *  Description:   Retrieving class attributes from certain caches.
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

/*
 * ------------------------------------
 *
 *         GENERAL ATTRIBUTES
 *
 * ------------------------------------
 */
 
/**
 * Checks if the specified class is enabled.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if it's enabled, false otherwise.
 */
stock bool:ClassIsEnabled(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_enabled];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_enabled];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_enabled];
        }
    }
    return false;
}

/**
 * Gets the team ID for the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return          The team ID if successful, -1 otherwise.
 */
stock ClassGetTeamID(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_team];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_team];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_team];
        }
    }
    return -1;
}

/**
 * Checks if the specified class is marked as the default class for its current
 * team.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if it's default for its current team, false otherwise.
 */
stock bool:ClassGetTeamDefault(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_team_default];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_team_default];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_team_default];
        }
    }
    return false;
}

/**
 * Gets flags for the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          Class flags, or -1 if failed.
 */
stock ClassGetFlags(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_flags];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_flags];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_flags];
        }
    }
    return -1;
}

/**
 * Do bitwise compare on flags in the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param flags     Class flags to check.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if the flags are set, false otherwise or if failed.
 */
stock bool:ClassHasFlags(index, flags, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return bool:(ClassData[index][class_flags] & flags);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return bool:(ClassDataCache[index][class_flags] & flags);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return bool:(ClassPlayerCache[index][class_flags] & flags);
        }
    }
    return false;
}

/**
 * Gets the class name to be displayed in the class menu.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return          Number of cells written. -1 on error.
 */
stock ClassGetName(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][class_name]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][class_name]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][class_name]);
        }
    }
    
    return -1;
}

/**
 * Gets the menu description string for the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
stock ClassGetDescription(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][class_description]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][class_description]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][class_description]);
        }
    }
    return -1;
}



/*
 * ------------------------------------
 *
 *          MODEL ATTRIBUTES
 *
 * ------------------------------------
 */

/**
 * Gets the model path string from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
stock ClassGetModelPath(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][class_model_path]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][class_model_path]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][class_model_path]);
        }
    }
    return -1;
}

/**
 * Gets the initial alpha value from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The initial alpha value from the specified class. -1 on error.
 */
stock ClassGetAlphaInitial(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_alpha_initial];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_alpha_initial];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_alpha_initial];
        }
    }
    return -1;
}

/**
 * Gets the alpha value when damaged, from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The alpha value when damaged, from the specified class. -1 on
 *          error.
 */
stock ClassGetAlphaDamaged(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_alpha_damaged];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_alpha_damaged];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_alpha_damaged];
        }
    }
    return -1;
}

/**
 * Gets the damage amount needed to change alpha, from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The damage amount needed to change alpha, from the specified class.
 *          -1 on error.
 */
stock ClassGetAlphaDamage(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_alpha_damage];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_alpha_damage];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_alpha_damage];
        }
    }
    return -1;
}



/*
 * ------------------------------------
 *
 *           HUD ATTRIBUTES
 *
 * ------------------------------------
 */

/**
 * Gets the overlay path from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param buffer    The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
stock ClassGetOverlayPath(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(buffer, maxlen, ClassData[index][class_overlay_path]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(buffer, maxlen, ClassDataCache[index][class_overlay_path]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(buffer, maxlen, ClassPlayerCache[index][class_overlay_path]);
        }
    }
    return -1;    
}

/**
 * Gets the night vision setting from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The night vision setting from the specified class. False on error.
 */
stock bool:ClassGetNvgs(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_nvgs];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_nvgs];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_nvgs];
        }
    }
    return false;
}

/**
 * Gets the field of view value from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The field of view value from the specified class. -1 on error.
 */
stock ClassGetFOV(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_fov];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_fov];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_fov];
        }
    }
    return -1;
}



/*
 * ------------------------------------
 *
 *              EFFECTS
 *
 * ------------------------------------
 */

/**
 * Gets the napalm setting from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  True if the class has napalm grenades, false otherwise.
 */
stock bool:ClassGetHasNapalm(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_has_napalm];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_has_napalm];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_has_napalm];
        }
    }
    return false;
}

/**
 * Gets the napalm grenades time from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The napalm grenades time from the specified class. -1.0 on error.
 */
stock Float:ClassGetNapalmTime(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_napalm_time];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_napalm_time];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_napalm_time] * ClassGetAttributeMultiplier(index, ClassM_NapalmTime);
        }
    }
    return -1.0;
}



/*
 * ------------------------------------
 *
 *          PLAYER BEHAVIOUR
 *
 * ------------------------------------
 */

/**
 * Gets the immunity mode to the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Current immunity mode to the specified class. -1 on error.
 */
stock ClassGetImmunityMode(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_immunity_mode];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_immunity_mode];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_immunity_mode];
        }
    }
    return -1;
}

/**
 * Gets the immunity amount to the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Current immunity amount to the specified class. -1.0 on error.
 */
stock Float:ClassGetImmunityAmount(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_immunity_amount];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_immunity_amount];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_immunity_amount];
        }
    }
    return -1.0;
}

/**
 * Gets the no fall damage setting from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The no fall damage setting from the specified class. False on
 * error.
 */
stock bool:ClassGetNoFallDamage(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_no_fall_damage];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_no_fall_damage];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_no_fall_damage];
        }
    }
    return false;
}

/**
 * Gets the health points from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Health points from the specified class. -1 on error.
 */
stock ClassGetHealth(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_health];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_health];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return RoundToCeil(ClassPlayerCache[index][class_health] * ClassGetAttributeMultiplier(index, ClassM_Health));
        }
    }
    return -1;
}

/**
 * Gets the health regen interval time from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health regen interval time from the specified class. -1.0 on
 *          error.
 */
stock Float:ClassGetHealthRegenInterval(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_health_regen_interval];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_health_regen_interval];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_health_regen_interval] * ClassGetAttributeMultiplier(index, ClassM_HealthRegenInterval);
        }
    }
    return -1.0;
}

/**
 * Gets the health regen amount value from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health regen amount value from the specified class. -1 on
 *          error.
 */
stock ClassGetHealthRegenAmount(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_health_regen_amount];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_health_regen_amount];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return RoundToCeil(ClassPlayerCache[index][class_health_regen_amount] * ClassGetAttributeMultiplier(index, ClassM_HealthRegenAmount));
        }
    }
    return -1;
}

/**
 * Gets the health infect gain value from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health infect gain value from the specified class. -1 on
 *          error.
 */
stock ClassGetHealthInfectGain(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_health_infect_gain];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_health_infect_gain];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return RoundToCeil(ClassPlayerCache[index][class_health_infect_gain] * ClassGetAttributeMultiplier(index, ClassM_HealthInfectGain));
        }
    }
    return -1;
}

/**
 * Gets the kill bonus points from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The kill bonus points from the specified class.
 */
stock ClassGetKillBonus(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_kill_bonus];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_kill_bonus];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_kill_bonus];
        }
    }
    return -1;
}

/**
 * Gets the running speed value from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The running speed value from the specified class. -1.0 on error.
 */
stock Float:ClassGetSpeed(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_speed];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_speed];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_speed] * ClassGetAttributeMultiplier(index, ClassM_Speed);
        }
    }
    return -1.0;
}

/**
 * Gets the knock back boost from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The knock back boost from the specified class. 0.0 on error.
 */
stock Float:ClassGetKnockback(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_knockback];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_knockback];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_knockback] * ClassGetAttributeMultiplier(index, ClassM_Knockback);
        }
    }
    return 0.0;
}

/**
 * Gets the jump height boost from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The jump height boost from the specified class. -1.0 on error.
 */
stock Float:ClassGetJumpHeight(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_jump_height];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_jump_height];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_jump_height] * ClassGetAttributeMultiplier(index, ClassM_JumpHeight);
        }
    }
    return -1.0;
}

/**
 * Gets the jump distance boost from the specified class.
 *
 * Note: Multiplier is used if cache type is player cache.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The jump distance boost from the specified class. -1.0 on error.
 */
stock Float:ClassGetJumpDistance(index, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return ClassData[index][class_jump_distance];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return ClassDataCache[index][class_jump_distance];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_jump_distance] * ClassGetAttributeMultiplier(index, ClassM_JumpDistance);
        }
    }
    return -1.0;
}

/**
 * Gets the attribute flag that represent the specified attribute.
 *
 * @param attributename     The attribute name.
 * @return                  The flag that represent the specified attribute.
 *                          -1 on error.
 */
stock ClassAttributeNameToFlag(const String:attributename[])
{
    // Check attribute names.
    if (StrEqual(attributename, "enabled", false))
    {
        return ZR_CLASS_ENABLED;
    }
    else if (StrEqual(attributename, "team", false))
    {
        return ZR_CLASS_TEAM;
    }
    else if (StrEqual(attributename, "team_default", false))
    {
        return ZR_CLASS_TEAM_DEFAULT;
    }
    else if (StrEqual(attributename, "flags", false))
    {
        return ZR_CLASS_FLAGS;
    }
    else if (StrEqual(attributename, "name", false))
    {
        return ZR_CLASS_NAME;
    }
    else if (StrEqual(attributename, "description", false))
    {
        return ZR_CLASS_DESCRIPTION;
    }
    else if (StrEqual(attributename, "model_path", false))
    {
        return ZR_CLASS_MODEL_PATH;
    }
    else if (StrEqual(attributename, "alpha_initial", false))
    {
        return ZR_CLASS_ALPHA_INITIAL;
    }
    else if (StrEqual(attributename, "alpha_damaged", false))
    {
        return ZR_CLASS_ALPHA_DAMAGED;
    }
    else if (StrEqual(attributename, "alpha_damage", false))
    {
        return ZR_CLASS_ALPHA_DAMAGE;
    }
    else if (StrEqual(attributename, "overlay_path", false))
    {
        return ZR_CLASS_OVERLAY_PATH;
    }
    else if (StrEqual(attributename, "nvgs", false))
    {
        return ZR_CLASS_NVGS;
    }
    else if (StrEqual(attributename, "fov", false))
    {
        return ZR_CLASS_FOV;
    }
    else if (StrEqual(attributename, "has_napalm", false))
    {
        return ZR_CLASS_HAS_NAPALM;
    }
    else if (StrEqual(attributename, "napalm_time", false))
    {
        return ZR_CLASS_NAPALM_TIME;
    }
    else if (StrEqual(attributename, "immunity_mode", false))
    {
        return ZR_CLASS_IMMUNITY_MODE;
    }
    else if (StrEqual(attributename, "immunity_amount", false))
    {
        return ZR_CLASS_IMMUNITY_AMOUNT;
    }
    else if (StrEqual(attributename, "no_fall_damage", false))
    {
        return ZR_CLASS_NO_FALL_DAMAGE;
    }
    else if (StrEqual(attributename, "health", false))
    {
        return ZR_CLASS_HEALTH;
    }
    else if (StrEqual(attributename, "health_regen_interval", false))
    {
        return ZR_CLASS_HEALTH_REGEN_INTERVAL;
    }
    else if (StrEqual(attributename, "health_regen_amount", false))
    {
        return ZR_CLASS_HEALTH_REGEN_AMOUNT;
    }
    else if (StrEqual(attributename, "health_infect_gain", false))
    {
        return ZR_CLASS_HEALTH_INFECT_GAIN;
    }
    else if (StrEqual(attributename, "kill_bonus", false))
    {
        return ZR_CLASS_KILL_BONUS;
    }
    else if (StrEqual(attributename, "speed", false))
    {
        return ZR_CLASS_SPEED;
    }
    else if (StrEqual(attributename, "knockback", false))
    {
        return ZR_CLASS_KNOCKBACK;
    }
    else if (StrEqual(attributename, "jump_height", false))
    {
        return ZR_CLASS_JUMP_HEIGHT;
    }
    else if (StrEqual(attributename, "jump_distance", false))
    {
        return ZR_CLASS_JUMP_DISTANCE;
    }
    
    // Invalid attribute name.
    return -1;
}

/**
 * Converts a attribute name to a multiplier type.
 *
 * @param attributename     The attribute name.
 * @return                  The multiplier that represent the specified attribute.
 *                          ClassM_Invalid if failed.
 */
stock ClassMultipliers:ClassAttributeNameToMultiplier(const String:attributename[])
{
    // Check attribute names.
    if (StrEqual(attributename, "napalm_time", false))
    {
        return ClassM_NapalmTime;
    }
    else if (StrEqual(attributename, "health", false))
    {
        return ClassM_Health;
    }
    else if (StrEqual(attributename, "health_regen_interval", false))
    {
        return ClassM_HealthRegenInterval;
    }
    else if (StrEqual(attributename, "health_regen_interval", false))
    {
        return ClassM_HealthRegenAmount;
    }
    else if (StrEqual(attributename, "health_infect_gain", false))
    {
        return ClassM_HealthInfectGain;
    }
    else if (StrEqual(attributename, "speed", false))
    {
        return ClassM_Speed;
    }
    else if (StrEqual(attributename, "knockback", false))
    {
        return ClassM_Knockback;
    }
    else if (StrEqual(attributename, "jump_height", false))
    {
        return ClassM_JumpHeight;
    }
    else if (StrEqual(attributename, "jump_distance", false))
    {
        return ClassM_JumpDistance;
    }
    
    // Invalid attribute name or not a multiplier.
    return ClassM_Invalid;
}

/**
 * Returns the datatype used in the specified attribute.
 *
 * @param attributeflag     A flag specifying the attribute to check.
 * @return      The data type used in the specified attribute, or
 *              ClassType_InvalidType if failed.
 */
stock ClassDataTypes:ClassGetAttributeType(attributeflag)
{
    switch (attributeflag)
    {
        // Boolean.
        case ZR_CLASS_ENABLED,
              ZR_CLASS_NVGS,
              ZR_CLASS_HAS_NAPALM,
              ZR_CLASS_NO_FALL_DAMAGE:
        {
            return ClassDataType_Boolean;
        }
        
        // Integer.
        case ZR_CLASS_FLAGS,
              ZR_CLASS_ALPHA_INITIAL,
              ZR_CLASS_ALPHA_DAMAGED,
              ZR_CLASS_ALPHA_DAMAGE,
              ZR_CLASS_FOV,
              ZR_CLASS_IMMUNITY_MODE,
              ZR_CLASS_HEALTH,
              ZR_CLASS_HEALTH_REGEN_AMOUNT,
              ZR_CLASS_HEALTH_INFECT_GAIN,
              ZR_CLASS_KILL_BONUS:
        {
            return ClassDataType_Integer;
        }
        
        // Float.
        case ZR_CLASS_NAPALM_TIME,
              ZR_CLASS_IMMUNITY_AMOUNT,
              ZR_CLASS_HEALTH_REGEN_INTERVAL,
              ZR_CLASS_SPEED,
              ZR_CLASS_KNOCKBACK,
              ZR_CLASS_JUMP_HEIGHT,
              ZR_CLASS_JUMP_DISTANCE:
        {
            return ClassDataType_Float;
        }
        
        // String.
        case ZR_CLASS_NAME,
              ZR_CLASS_DESCRIPTION,
              ZR_CLASS_MODEL_PATH,
              ZR_CLASS_OVERLAY_PATH:
        {
            return ClassDataType_String;
        }
    }
    
    // Invalid flag or multiple flags combined.
    return ClassDataType_InvalidType;
}
