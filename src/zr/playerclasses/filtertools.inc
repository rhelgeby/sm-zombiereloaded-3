/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        filtertools.inc
 *   Description: Class system: Validating, getting indexes or lists
 *   Author:      Richard Helgeby
 *
 * ============================================================================
 */

/**
 * Validates the team requirements in a class cache and check that theres at
 * least one class for each team. Minium team requirements are zombies and
 * humans. The admin team is optinal and not validated.
 *
 * @param cachetype Optional. Specifies what class cache to validate. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL (default, unchanged class data),
 *                  ZR_CLASS_CACHE_MODIFIED (modified class data).
 * @return          True if validation was successful, false otherwise.
 */
bool:ClassValidateTeamRequirements(cachetype = ZR_CLASS_CACHE_ORIGINAL)
{
    new zombieindex;
    new humanindex;
    
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Test if a zombie and human class was found.
    zombieindex = ClassGetFirstClass(ZR_CLASS_TEAM_ZOMBIES, _, cachetype);
    humanindex = ClassGetFirstClass(ZR_CLASS_TEAM_HUMANS, _, cachetype);
    
    // Validate indexes.
    if (ClassValidateIndex(zombieindex) && ClassValidateIndex(humanindex))
    {
        return true;
    }
    
    return false;
}

/**
 * Validates that there's a class marked as team default for each team.
 *
 * @param cachetype Optional. Specifies what class cache to validate. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL (default, unchanged class data),
 *                  ZR_CLASS_CACHE_MODIFIED (modified class data).
 * @return          True if validation was successful, false otherwise.
 */
bool:ClassValidateTeamDefaults(cachetype = ZR_CLASS_CACHE_ORIGINAL)
{
    new zombieindex;
    new humanindex;
    
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Test if a default zombie and human class was found.
    zombieindex = ClassGetDefaultClass(ZR_CLASS_TEAM_ZOMBIES, _, cachetype);
    humanindex = ClassGetDefaultClass(ZR_CLASS_TEAM_HUMANS, _, cachetype);
    
    // Validate indexes.
    if (ClassValidateIndex(zombieindex) && ClassValidateIndex(humanindex))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Validates all the class attributes in the original class data array, to
 * check if they have invalid values. Boolean settings are not validated.
 *
 * @param classindex    The index of the class to validate.
 * @return              A value with attribute error flags.
 */
ClassValidateAttributes(classindex)
{
    // TODO: Validate immunity mode and amount.
    // TODO: Validate jump values.
    
    new flags;
    
    // Name.
    if (strlen(ClassData[classindex][class_name]) == 0)
    {
        flags += ZR_CLASS_ATTRIB_ERR_NAME;
    }
    
    // Description.
    if (strlen(ClassData[classindex][class_description]) == 0)
    {
        flags += ZR_CLASS_ATTRIB_ERR_DESCRIPTION;
    }
    
    // Model path.
    decl String:model_path[256];
    if (strcopy(model_path, sizeof(model_path), ClassData[classindex][class_model_path]) == 0)
    {
        flags += ZR_CLASS_ATTRIB_ERR_MODEL_PATH;
    }
    else
    {
        // Check if default or random model is specified.
        if (strcmp(model_path, "random", false) != 0 && strcmp(model_path, "default", false) != 0)
        {
            // Check if the file exists.
            if (!FileExists(model_path))
            {
                flags += ZR_CLASS_ATTRIB_ERR_MODEL_PATH;
            }
        }        
    }
    
    // Alpha, initial.
    new alpha_initial = ClassData[classindex][class_alpha_initial];
    if (!(alpha_initial >= 0 && alpha_initial <= 255))
    {
        flags += ZR_CLASS_ATTRIB_ERR_ALPHA_INITIAL;
    }
    
    // Alpha, damaged.
    new alpha_damaged = ClassData[classindex][class_alpha_damaged];
    if (!(alpha_damaged >= 0 && alpha_damaged <= 255))
    {
        flags += ZR_CLASS_ATTRIB_ERR_ALPHA_DAMAGED;
    }
    
    // Alpha, damage.
    new alpha_damage = ClassData[classindex][class_alpha_damage];
    if (!(alpha_damage >= 0 && alpha_damage <= 65536))
    {
        flags += ZR_CLASS_ATTRIB_ERR_ALPHA_DAMAGE;
    }
    
    // Overlay path.
    decl String:overlay_path[256];
    decl String:overlay[256];
    if (strcopy(overlay_path, sizeof(overlay_path), ClassData[classindex][class_overlay_path]) > 0)
    {
        // Check if the file exists.
        Format(overlay, sizeof(overlay), "materials/%s.vmt", overlay_path);
        if (!FileExists(overlay))
        {
            flags += ZR_CLASS_ATTRIB_ERR_OVERLAY_PATH;
        }
    }
    
    // Field of view.
    new fov = ClassData[classindex][class_fov];
    if (!(fov > 15 && fov < 180))
    {
        flags += ZR_CLASS_ATTRIB_ERR_FOV;
    }
    
    // Napalm time.
    new Float:napalm_time = ClassData[classindex][class_napalm_time];
    if (!(napalm_time >= 0.0 && napalm_time <= 900.0))
    {
        flags += ZR_CLASS_ATTRIB_ERR_NAPALM_TIME;
    }
    
    // Health regen interval.
    new Float:regen_interval = ClassData[classindex][class_health_regen_interval];
    if (!(regen_interval >= 0.0 && regen_interval <= 900.0))
    {
        flags += ZR_CLASS_ATTRIB_ERR_HEALTH_REGEN_INTERVAL;
        
        // Health regen amount. Only validating if interval is set.
        new regen_amount = ClassData[classindex][class_health_regen_amount];
        if (!(regen_amount > 0 && regen_amount <= 65536))
        {
            flags += ZR_CLASS_ATTRIB_ERR_HEALTH_REGEN_AMOUNT;
        }
    }
    
    // Health infect gain.
    new infect_gain = ClassData[classindex][class_health_infect_gain];
    if (!(infect_gain >= 0 && infect_gain <= 65536))
    {
        flags += ZR_CLASS_ATTRIB_ERR_INFECT_GAIN;
    }
    
    // Kill bonus.
    new kill_bonus = ClassData[classindex][class_kill_bonus];
    if (!(kill_bonus >= 0 && kill_bonus <= 128))
    {
        flags += ZR_CLASS_ATTRIB_ERR_KILL_BONUS;
    }
    
    // Speed.
    new Float:speed = ClassData[classindex][class_speed];
    if (!(speed >= 0.0 && speed <= 1024.0))
    {
        flags += ZR_CLASS_ATTRIB_ERR_SPEED;
    }
    
    // Knockback.
    new Float:knockback = ClassData[classindex][class_knockback];
    if (!(knockback >= -10.0 && knockback <= 50.0))
    {
        flags += ZR_CLASS_ATTRIB_ERR_KNOCKBACK;
    }
    
    return flags;
}

/**
 * Checks if the specified class index points to a existing class in the
 * ClassData array.
 *
 * @param classindex    The class index to validate.
 * @return              True if the class exist, false otherwise.
 */
bool:ClassValidateIndex(classindex)
{
    if (classindex >= 0 && classindex < ClassCount)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * Compares the class team ID with a team ID.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param teamid    The team ID to compare with the class.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 *                  ZR_CLASS_CACHE_PLAYER - Player cache. If this one is used,
 *                  index will be used as a client index.
 * @return          True if equal, false otherwise.
 */
bool:ClassTeamCompare(index, teamid, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            if (ClassData[index][class_team] == teamid)
            {
                return true;
            }
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            if (ClassDataCache[index][class_team] == teamid)
            {
                return true;
            }
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            if (ClassPlayerCache[index][class_team] == teamid)
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * Gets the first class index of a class with the specified name (not a case
 * sensitive search).
 *
 * @param name      The name to search for.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 * @return          The class index if successful, -1 otherwise.
 */
ClassGetIndex(const String:name[], cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    decl String:current_name[64];

    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        // Get its name and compare it with the specified class name.
        ClassGetName(classindex, current_name, sizeof(current_name), cachetype);
        if (strcmp(name, current_name, false) == 0)
        {
            return classindex;
        }
    }
    
    // The class index wasn't found.
    return -1;
}

/**
 * Gets the currently active class index that the player is using.
 * Note: Does not check if the player is dead.
 *
 * @param client    The client index.
 * @return  The active class index. -1 on error or if a spectactor.
 */
ClassGetActiveIndex(client)
{
    new teamid = GetClientTeam(client);
    
    if (teamid == CS_TEAM_SPECTATOR || teamid == CS_TEAM_NONE)
    {
        // No active team.
        return -1;
    }
    
    if (IsPlayerHuman(client))
    {
        teamid = ZR_CLASS_TEAM_HUMANS;
    }
    else
    {
        teamid = ZR_CLASS_TEAM_ZOMBIES;
    }
    
    // TODO: How to detect that virtual admin team?
    
    // Return the active class for the current team.
    return ClassSelected[client][teamid];
}

/**
 * Gets all class indexes or from a specified team, and adds them to the
 * specified array.
 * 
 * @param array         The destination array to add class indexes.
 * @param teamfilter    Optional. The team ID to filter. A negative value for
 *                      no filter (default).
 * @param ignoreEnabled Optional. Ignore the class's enabled attribute. Default
 *                      is false.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                      class data.
 * @return  True on success. False on error or if no classes were added or
 *          found.
 */
bool:ClassAddToArray(Handle:array, teamfilter = -1, bool:ignoreEnabled = false, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Validate the array.
    if (array == INVALID_HANDLE)
    {
        return false;
    }
    
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    // Store a local boolean that says if the user specified a team filter or not.
    new bool:has_filter = bool:(teamfilter >= 0);
    new classes_added;
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        if (!ignoreEnabled && !ClassIsEnabled(classindex, cachetype))
        {
            // The class is disabled and the enabled attribute is NOT ignored.
            // Skip to the next class.
            continue;
        }
        
        // Check team filtering.
        if (has_filter)
        {
            // Only add classes with matching team ID.
            if (ClassGetTeamID(classindex, cachetype) == teamfilter)
            {
                // Team ID match. Add class index to array.
                PushArrayCell(array, classindex);
                classes_added++;
            }
        }
        else
        {
            // No filter. Add any class to the array.
            PushArrayCell(array, classindex);
            classes_added++;
        }
    }
    
    if (classes_added)
    {
        return true;
    }
    else
    {
        // No classes were found/added.
        return false;
    }
}

/**
 * Gets a random class index from a specified team or from all classes.
 *
 * @param teamfilter    Optional. The team ID to filter. A negative value for
 *                      no filter (default).
 * @param ignoreEnabled Optional. Ignore the class's enabled attribute. Default
 *                      is false.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                      class data.
 * @return  The class index if successful, or -1 on error.
 */
ClassGetRandomClass(teamfilter = -1, bool:ignoreEnabled = false, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    new Handle:classarray;
    new arraycount;
    new randnum;
    
    classarray = CreateArray();
    
    // Try to get a class list.
    if (ClassAddToArray(classarray, teamfilter, ignoreEnabled, cachetype))
    {
        // Get a random index from the new class array.
        arraycount = GetArraySize(classarray);
        randnum = GetRandomInt(0, arraycount - 1);
        
        // Return the value at the random index.
        return GetArrayCell(classarray, randnum);
    }
    else
    {
        // Failed to get a random class.
        return -1;
    }
}

/**
 * Gets the first class index, or the first class index with the specified team
 * ID.
 *
 * @param teamfilter    Optional. The team ID to filter. A negative value for
 *                      no filter (default).
 * @param ignoreEnabled Optional. Ignore the class's enabled attribute. Default
 *                      is false.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                      class data.
 * @return  The first class index, or the first class index with the specified
 *          team ID. -1 on error.
 */
ClassGetFirstClass(teamfilter = -1, bool:ignoreEnabled = false, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Check if there are no classes.
    if (ClassCount == 0)
    {
        return false;
    }
    
    new bool:has_filter = bool:(teamfilter >= 0);
    
    // Loop through all classes.
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        if (!ignoreEnabled && !ClassIsEnabled(classindex, cachetype))
        {
            continue;
        }
        
        if (has_filter)
        {
            if (teamfilter == ClassGetTeamID(classindex, cachetype))
            {
                // Team ID match. Return the class index.
                return classindex;
            }
        }
        else
        {
            // No team filter. Return the class index.
            return classindex;
        }
    }
    
    return -1;
}

/**
 * Gets the first class marked as default for the specified team.
 *
 * @param teamid        The team ID.
 * @param ignoreEnabled Optional. Ignore the class's enabled attribute. Default
 *                      is false.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options:
 *                      ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                      ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest
 *                      class data.
 * @return  The first default class index. -1 on error.
 */
ClassGetDefaultClass(teamid, bool:ignoreEnabled = false, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    new Handle:classarray;
    new arraycount;
    new classindex;
    
    classarray = CreateArray();
    
    // Get all classes from the specified team.
    if (!ClassAddToArray(classarray, teamid, ignoreEnabled, cachetype))
    {
        // Failed to get classes.
        return -1;
    }
    
    // Loop through all classes and return the first class marked as team default.
    arraycount = GetArraySize(classarray);
    for (new i = 0; i < arraycount; i++)
    {
        // Get class index from the array.
        classindex = GetArrayCell(classarray, i);
        
        // Check if the current class is marked as team default.
        if (ClassGetTeamDefault(classindex, cachetype))
        {
            // Default class found.
            return classindex;
        }
    }
    
    return -1;
}

/**
 * Gets the default class index for the specified team configured to be used
 * when players join the server.
 *
 * @param teamid    The team ID.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED (default) - Changed/newest class
 *                  data.
 * @return  The class index of the default class for the specified team if
 *          successful. -1 on critical errors. Otherwise it will try to fall
 *          back on the first class in the specified team.
 */
ClassGetDefaultSpawnClass(teamid, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    decl String:classname[64];
    new classindex;
    
    // Get the default class name from the correct CVAR depending on teamid.
    switch (teamid)
    {
        case ZR_CLASS_TEAM_ZOMBIES:
        {
            GetConVarString(gCvars[CVAR_CLASSES_DEFAULT_ZOMBIE], classname, sizeof(classname));
        }
        case ZR_CLASS_TEAM_HUMANS:
        {
            GetConVarString(gCvars[CVAR_CLASSES_DEFAULT_HUMAN], classname, sizeof(classname));
        }
        case ZR_CLASS_TEAM_ADMINS:
        {
            GetConVarString(gCvars[CVAR_CLASSES_DEFAULT_ADMIN], classname, sizeof(classname));
        }
        default:
        {
            // Invalid team ID.
            return -1;
        }
    }
    
    // Check if the class name isn't empty.
    if (strlen(classname) > 0)
    {
        // Check if the user set "random" as default class.
        if (strcmp(classname, "random", false) == 0)
        {
            // Get a list of all classes with the specified team ID.
            classindex = ClassGetRandomClass(teamid, _, cachetype);
            
            // Validate the result, in case there were errors.
            if (ClassValidateIndex(classindex))
            {
                return classindex;
            }
            else
            {
                // Invalid index. The ClassGetRandomClass function is pretty
                // failsafe. So if we can't get a class index here, it's a
                // critical error. No reason to fall back on other solutions.
                return -1;
            }
        }
        else
        {
            // The user set a spesific class.
            
            // Try to get the class index with the specified class name.
            classindex = ClassGetIndex(classname, cachetype);
            
            // Validate the class index and check if the team IDs match.
            if (ClassValidateIndex(classindex) && (teamid == ClassGetTeamID(classindex, cachetype)))
            {
                return classindex;
            }
            else
            {
                // The class index is invalid or the team IDs didn't match.
                // Because it's user input, we'll fall back to the first class
                // in the specified team, and log a warning.
                classindex = ClassGetFirstClass(teamid, _, cachetype);
                
                if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
                {
                    ZR_LogMessageFormatted(-1, "Classes", "DefaultSpawnClass", "Warning: Failed to set \"%s\" as default spawn class for team %d. The class doesn't exist or the team IDs doesn't match. Falling back to the first class in the team.", _, classname, teamid);
                }
                
                // Validate the new index.
                if (ClassValidateIndex(classindex))
                {
                    // Log a warning.
                    if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
                    {
                        ZR_LogMessageFormatted(-1, "Classes", "DefaultSpawnClass", "Warning: The default class name \"%s\" does not exist or matches the team ID.", _, classname);
                    }
                    
                    return classindex;
                }
                else
                {
                    // Something went wrong. This is a critical error.
                    return -1;
                }
            }
        }
    }
    else
    {
        // Blank class name, get the default class and return the index.
        return ClassGetDefaultClass(teamid, _, cachetype);
    }
}