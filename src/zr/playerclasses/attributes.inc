/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          attributes.inc
 *   Type:          Core
 *   Description:   Retrieving class attributes from certain caches.
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
bool:ClassIsEnabled(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
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
ClassGetTeamID(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
bool:ClassGetTeamDefault(index, cachetype = ZR_CLASS_CACHE_MODIFIED)
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
ClassGetName(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
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
ClassGetDescription(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
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
ClassGetModelPath(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
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
ClassGetAlphaInitial(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
ClassGetAlphaDamaged(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
ClassGetAlphaDamage(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
ClassGetOverlayPath(index, String:buffer[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
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
bool:ClassGetNvgs(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
ClassGetFOV(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
 * Gets the napalm grenades time from the specified class.
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
Float:ClassGetNapalmTime(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_napalm_time];
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
ClassGetImmunityMode(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
Float:ClassGetImmunityAmount(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
bool:ClassGetNoFallDamage(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Health points from the specified class. -1 on error.
 */
ClassGetHealth(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_health];
        }
    }
    return -1;
}

/**
 * Gets the health regen interval time from the specified class.
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
Float:ClassGetHealthRegenInterval(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_health_regen_interval];
        }
    }
    return -1.0;
}

/**
 * Gets the health regen amount value from the specified class.
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
ClassGetHealthRegenAmount(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_health_regen_amount];
        }
    }
    return -1;
}

/**
 * Gets the health infect gain value from the specified class.
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
ClassGetHealthInfectGain(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_health_infect_gain];
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
ClassGetKillBonus(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The running speed value from the specified class. -1.0 on error.
 */
Float:ClassGetSpeed(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_speed];
        }
    }
    return -1.0;
}

/**
 * Gets the knock back boost from the specified class.
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
Float:ClassGetKnockback(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_knockback];
        }
    }
    return 0.0;
}

/**
 * Gets the jump height boost from the specified class.
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
Float:ClassGetJumpHeight(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_jump_height];
        }
    }
    return -1.0;
}

/**
 * Gets the jump distance boost from the specified class.
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
Float:ClassGetJumpDistance(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
            return ClassPlayerCache[index][class_jump_distance];
        }
    }
    return -1.0;
}

/**
 * Gets the attribute flag that represent the specified attribute.
 *
 * @param attributename     The attribute name.
 * @return                  The flag that reporesent the specified attribute.
 *                          -1 on error.
 */
ClassAttributeNameToFlag(const String:attributename[])
{
    // Check attribute names.
    if (StrEqual(attributename, "enabled", false))
    {
        return ZR_CLASS_FLAG_ENABLED;
    }
    else if (StrEqual(attributename, "team", false))
    {
        return ZR_CLASS_FLAG_TEAM;
    }
    else if (StrEqual(attributename, "team_default", false))
    {
        return ZR_CLASS_FLAG_TEAM_DEFAULT;
    }
    else if (StrEqual(attributename, "name", false))
    {
        return ZR_CLASS_FLAG_NAME;
    }
    else if (StrEqual(attributename, "description", false))
    {
        return ZR_CLASS_FLAG_DESCRIPTION;
    }
    else if (StrEqual(attributename, "model_path", false))
    {
        return ZR_CLASS_FLAG_MODEL_PATH;
    }
    else if (StrEqual(attributename, "alpha_initial", false))
    {
        return ZR_CLASS_FLAG_ALPHA_INITIAL;
    }
    else if (StrEqual(attributename, "alpha_damaged", false))
    {
        return ZR_CLASS_FLAG_ALPHA_DAMAGED;
    }
    else if (StrEqual(attributename, "alpha_damage", false))
    {
        return ZR_CLASS_FLAG_ALPHA_DAMAGE;
    }
    else if (StrEqual(attributename, "overlay_path", false))
    {
        return ZR_CLASS_FLAG_OVERLAY_PATH;
    }
    else if (StrEqual(attributename, "nvgs", false))
    {
        return ZR_CLASS_FLAG_NVGS;
    }
    else if (StrEqual(attributename, "fov", false))
    {
        return ZR_CLASS_FLAG_FOV;
    }
    else if (StrEqual(attributename, "napalm_time", false))
    {
        return ZR_CLASS_FLAG_NAPALM_TIME;
    }
    else if (StrEqual(attributename, "immunity_mode", false))
    {
        return ZR_CLASS_FLAG_IMMUNITY_MODE;
    }
    else if (StrEqual(attributename, "immunity_amount", false))
    {
        return ZR_CLASS_FLAG_IMMUNITY_AMOUNT;
    }
    else if (StrEqual(attributename, "no_fall_damage", false))
    {
        return ZR_CLASS_FLAG_NO_FALL_DAMAGE;
    }
    else if (StrEqual(attributename, "health", false))
    {
        return ZR_CLASS_FLAG_HEALTH;
    }
    else if (StrEqual(attributename, "health_regen_interval", false))
    {
        return ZR_CLASS_FLAG_HEALTH_REGEN_INTERVAL;
    }
    else if (StrEqual(attributename, "health_regen_amount", false))
    {
        return ZR_CLASS_FLAG_HEALTH_REGEN_AMOUNT;
    }
    else if (StrEqual(attributename, "health_infect_gain", false))
    {
        return ZR_CLASS_FLAG_HEALTH_INFECT_GAIN;
    }
    else if (StrEqual(attributename, "kill_bonus", false))
    {
        return ZR_CLASS_FLAG_KILL_BONUS;
    }
    else if (StrEqual(attributename, "speed", false))
    {
        return ZR_CLASS_FLAG_SPEED;
    }
    else if (StrEqual(attributename, "knockback", false))
    {
        return ZR_CLASS_FLAG_KNOCKBACK;
    }
    else if (StrEqual(attributename, "jump_height", false))
    {
        return ZR_CLASS_FLAG_JUMP_HEIGHT;
    }
    else if (StrEqual(attributename, "jump_distance", false))
    {
        return ZR_CLASS_FLAG_JUMP_DISTANCE;
    }
    
    // Invalid attribute name.
    return -1;
}

/**
 * Returns the datatype used in the specified attribute.
 *
 * @param attributeflag     A flag specifying the attribute to check.
 * @return      The data type used in the specified attribute, or
 *              ClassType_InvalidType if failed.
 */
ClassDataTypes:ClassGetAttributeType(attributeflag)
{
    switch (attributeflag)
    {
        // Boolean.
        case ZR_CLASS_FLAG_ENABLED,
              ZR_CLASS_FLAG_NVGS,
              ZR_CLASS_FLAG_NO_FALL_DAMAGE:
        {
            return ClassDataType_Boolean;
        }
        
        // Integer.
        case ZR_CLASS_FLAG_ALPHA_INITIAL,
              ZR_CLASS_FLAG_ALPHA_DAMAGED,
              ZR_CLASS_FLAG_ALPHA_DAMAGE,
              ZR_CLASS_FLAG_FOV,
              ZR_CLASS_FLAG_IMMUNITY_MODE,
              ZR_CLASS_FLAG_HEALTH,
              ZR_CLASS_FLAG_HEALTH_REGEN_AMOUNT,
              ZR_CLASS_FLAG_HEALTH_INFECT_GAIN,
              ZR_CLASS_FLAG_KILL_BONUS:
        {
            return ClassDataType_Integer;
        }
        
        // Float.
        case ZR_CLASS_FLAG_NAPALM_TIME,
              ZR_CLASS_FLAG_IMMUNITY_AMOUNT,
              ZR_CLASS_FLAG_HEALTH_REGEN_INTERVAL,
              ZR_CLASS_FLAG_SPEED,
              ZR_CLASS_FLAG_KNOCKBACK,
              ZR_CLASS_FLAG_JUMP_HEIGHT,
              ZR_CLASS_FLAG_JUMP_DISTANCE:
        {
            return ClassDataType_Float;
        }
        
        // String.
        case ZR_CLASS_FLAG_NAME,
              ZR_CLASS_FLAG_DESCRIPTION,
              ZR_CLASS_FLAG_MODEL_PATH,
              ZR_CLASS_FLAG_OVERLAY_PATH:
        {
            return ClassDataType_String;
        }
    }
    
    // Invalid flag or multiple flags combined.
    return ClassDataType_InvalidType;
}
