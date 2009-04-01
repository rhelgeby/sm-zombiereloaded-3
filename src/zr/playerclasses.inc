/**
 * ====================
 *   Zombie:Reloaded
 *   File: playerclasses.inc
 *   Author: Richard Helgeby, Greyscale
 * ==================== 
 */

/*
    Ideas for immunity modes for humans:
    - Zombies have to stab x times to infect.
    - Zombies have to hurt humans so they loose hp. When the hp reach zero (or
      below) they turn into zombies.
    - Fully imune to all damage. Can't take or give damage. Sould only be used
      on admin classes.

    TODO: Admin command for overriding class values;
          zr_class_ovverride <classname> <key> <value> [value_is_multiplier]
          Either set absolute values or use it as a multiplier, if possible.
          "classname" should support "all" to set a spesific value on all
          classes.
          
          How overrides should work:
          When a player select a class, its attributes are cached for each
          player. On infection or spawn, the plugin will use the cached
          values.
          
          When we override, the real class attributes are changed and then
          copied to the player caches (with matching team IDs).
          
          There is one problem, the in-game knockback multiplier menu. It
          depends on the original class knockback values. A solution might be
          to have another array of class data, which is never canged.
    
    TODO: Make a solution with default valueas for each class team, without
          using CVARs.
          
          SOLVED: Added boolean team_default class attribute. Default values
          will be used from the first class with this value set.
    
    TODO: Make class attributes for for changing model render mode and colors.
    
    TODO: Make class attributes for fancy effects, like a glow (if possible).
    
    TODO: Make alpha settings also apply to weapons (we could use code from
          zombie riot in this case).
    
    TODO: Make immunity settings suitable for both teams, or certain zombie-
          only/human-only settings.
*/

/**
 * Total number of classes that can be stored in each cache. A total of 32
 * classes should be enough. Too many classes will comfuse players.
 */
#define ZR_CLASS_MAX 31

/**
 * @section Class cache types. Specifies what data array to use.
 */
#define ZR_CLASS_CACHE_ORIGINAL 0   /** Points ClassData array. A cache that is never changed after loading. */
#define ZR_CLASS_CACHE_MODIFIED 1   /** Points to ClassDataCache array. Modified by admins or overrides. */
#define ZR_CLASS_CACHE_PLAYER   2   /** Points to ClassPlayerCache array. Current player attributes. */
/**
 * @endsection
 */

/**
 * Number of available class teams.
 */
#define ZR_CLASS_TEAMCOUNT      3

/**
 * @section Available class teams. The admin team is optional and not required
 * in class configs.
 */
#define ZR_CLASS_TEAM_ZOMBIES   0
#define ZR_CLASS_TEAM_HUMANS    1
#define ZR_CLASS_TEAM_ADMINS    2   /** Note: Will set you in a special mode where you don't really participates in the game, but just walk around. */
/**
 * @endsection
 */

/**
 * @section Damage immunity modes. The mode effects will vary depending on the
 * class team.
 */
#define ZR_CLASS_IMMUNITY_DISABLED  0   /** No immunity. */
#define ZR_CLASS_IMMUNITY_CONSTANT  1   /** Always imune. Should only be used in special cases like on admin classes. */
#define ZR_CLASS_IMMUNITY_TIMED     2   /** Imune to damage for n seconds. The time is specified in a class as immunity amount. */
/**
 * @endsection
 */

/**
 * @section Overall default class settings. Since this is a zombie plugin the
 * default values represent a zombie.
 */
#define ZR_CLASS_DEFAULT_ENABLED                true
#define ZR_CLASS_DEFAULT_TEAM                   ZR_CLASS_TEAM_ZOMBIES
#define ZR_CLASS_DEFAULT_TEAM_DEFAULT           true
#define ZR_CLASS_DEFAULT_NAME                   "classic"
#define ZR_CLASS_DEFAULT_DESCRIPTION            "Need brains!!! Arrrrggghh!"
#define ZR_CLASS_DEFAULT_MODEL_PATH             "models/player/zh/zh_zombie003.mdl"
#define ZR_CLASS_DEFAULT_ALPHA_INITIAL          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGED          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGE           0
#define ZR_CLASS_DEFAULT_OVERLAY_PATH           "overlays/zr/zvision"
#define ZR_CLASS_DEFAULT_NVGS                   1
#define ZR_CLASS_DEFAULT_FOV                    90
#define ZR_CLASS_DEFAULT_NAPALM_TIME            10.0
#define ZR_CLASS_DEFAULT_IMMUNITY_MODE          ZR_CLASS_IMMUNITY_DISABLED
#define ZR_CLASS_DEFAULT_IMMUNITY_AMOUNT        0.0
#define ZR_CLASS_DEFAULT_NO_FALL_DAMAGE         true
#define ZR_CLASS_DEFAULT_HEALTH                 6000
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_INTERVAL  0
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_AMOUNT    2
#define ZR_CLASS_DEFAULT_HEALTH_INFECT_GAIN     800
#define ZR_CLASS_DEFAULT_KILL_BONUS             2
#define ZR_CLASS_DEFAULT_SPEED                  360
#define ZR_CLASS_DEFAULT_KNOCKBACK              2.0
#define ZR_CLASS_DEFAULT_JUMP_HEIGHT            10.0
#define ZR_CLASS_DEFAULT_JUMP_DISTANCE          0.2
/**
 * @endsection
 */

/**
 * Generic player attributes.
 */
enum ClassAttributes
{
    /* General */
    bool:class_enabled,
    class_team,
    bool:class_team_default,
    
    String:class_name[64],
    String:class_description[256],
    
    /* Model */
    String:class_model_path[256],
    class_alpha_initial,
    class_alpha_damaged,
    class_alpha_damage,
    
    /* Hud */
    String:class_overlay_path[256],
    bool:class_nvgs,
    class_fov,
    
    /* Effects */
    Float:class_napalm_time,
    
    /* Player behaviour */
    bool:class_immunity_mode,
    Float:class_immunity_amount,
    bool:class_no_fall_damage,
    
    class_health,
    Float:class_health_regen_interval,
    class_health_regen_amount,
    class_health_infect_gain,
    class_kill_bonus,
    
    Float:class_speed,
    Float:class_knockback,
    Float:class_jump_height,
    Float:class_jump_distance
}

new Handle:kvClassData;

/**
 * The original class data. This array only changed when class data is loaded.
 * ZR_CLASS_CACHE_ORIGINAL is the cache type to this array.
 */
new ClassData[ZR_CLASS_MAX][ClassAttributes];

/**
 * The class thata cache that can be modified. ZR_CLASS_CACHE_MODIFIED is the
 * cache type to this array.
 */
new ClassDataCache[ZR_CLASS_MAX][ClassAttributes];

/**
 * Cache for player attributes. Makes it possible for one or more players to
 * have custom attributes. ZR_CLASS_CACHE_PLAYER is the cache type to this
 * array.
 */
new ClassPlayerCache[MAXPLAYERS + 1][ClassAttributes];

/**
 * Number of classes successfully loaded.
 */
new ClassCount;

/**
 * Stores what class that the player have selected, for each team.
 */
new ClassActive[MAXPLAYERS + 1][ZR_CLASS_TEAMCOUNT - 1];


/**
 * Loads class attributes from playerclasses.txt into the ClassData array. If
 * any error occour, the plugin load will fail.
 *
 * @param classfile     Optional. Specifies what file to read from. Valves key/
 *                      values format. The path is relative to the sourcemod
 *                      folder.
 */
ClassLoad(const String:classfile[256] = "configs/zr/playerclasses.txt")
{
    // Make sure kvClassData is ready to use.
    if (kvClassData != INVALID_HANDLE)
    {
        CloseHandle(kvClassData);
    }
    kvClassData = CreateKeyValues("classes");
    
    // Try to load the class configuration file.
    decl String:path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), classfile);
    
    if (!FileToKeyValues(kvClassData, path))
    {
        SetFailState("Class data (\"%s\") missing from server.", path);
    }
    
    // Try to find the first class.
    KvRewind(kvClassData);
    if (!KvGotoFirstSubKey(kvClassData))
    {
        SetFailState("Cannot find any classes in \"%s\".", path);
    }
    
    decl String:name[64];
    decl String:description[256];
    decl String:model_path[256];
    decl String:overlay_path[256];
    
    ClassCount = 0;
    
    // Loop through all classes and store attributes in the ClassData array.
    do
    {
        if (ClassCount > ZR_CLASS_MAX)
        {
            // Maximum classes reached. Write a warning and exit the loop.
            if (LogHasFlag(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
            {
                ZR_LogMessageFormatted(-1, "classes", "loading classes", "Warning: Maximum classes reached (%d). The rest is skipped.", _, ZR_CLASSES_MAX + 1);
            }
            
            break;
        }
        
        /* General */
        ClassData[ClassCount][class_enabled] = bool:KvGetNum(kvClassData, "enabled", ZR_CLASS_DEFAULT_ENABLED);
        ClassData[ClassCount][class_team] = KvGetNum(kvClassData, "team", ZR_CLASS_DEFAULT_TEAM);
        
        KvGetString(kvClassData, "name", name, sizeof(name), ZR_CLASS_DEFAULT_NAME);
        strcopy(ClassData[ClassCount][class_name], 64, name);
        
        KvGetString(kvClassData, "description", description, sizeof(description), ZR_CLASS_DEFAULT_DESCRIPTION);
        strcopy(ClassData[ClassCount][class_description], 256, description);
        
        
        /* Model */
        KvGetString(kvClassData, "model_path", model_path, sizeof(model_path), ZR_CLASS_DEFAULT_MODEL_PATH);
        strcopy(ClassData[ClassCount][class_model_path], 256, model_path);
        
        ClassData[ClassCount][class_alpha_initial] = KvGetNum(kvClassData, "alpha_initial", ZR_CLASS_DEFAULT_ALPHA_INITIAL);
        ClassData[ClassCount][class_alpha_damaged] = KvGetNum(kvClassData, "alpha_damaged", ZR_CLASS_DEFAULT_ALPHA_DAMAGED);
        ClassData[ClassCount][class_alpha_damage] = KvGetNum(kvClassData, "alpha_damage", ZR_CLASS_DEFAULT_ALPHA_DAMAGE);
        
        
        /* Hud */
        KvGetString(kvClassData, "overlay_path", overlay_path, sizeof(overlay_path), ZR_CLASS_DEFAULT_OVERLAY_PATH);
        strcopy(ClassData[ClassCount][class_overlay_path], 256, overlay_path);
        
        ClassData[ClassCount][class_nvgs] = KvGetNum(kvClassData, "nvgs", ZR_CLASS_DEFAULT_NVGS);
        ClassData[ClassCount][class_fov] = KvGetNum(kvClassData, "fov", ZR_CLASS_DEFAULT_FOV);
        
        
        /* Effects */
        ClassData[ClassCount][class_napalm_time] = KvGetFloat(kvClassData, "napalm_time", ZR_CLASS_DEFAULT_NAPALM_TIME);
        
        
        /* Player behaviour */
        ClassData[ClassCount][class_immunity_mode] = KvGetNum(kvClassData, "immunity_mode", ZR_CLASS_DEFAULT_IMMUNITY_MODE);
        ClassData[ClassCount][class_immunity_amount] = KvGetFloat(kvClassData, "immunity_amount", ZR_CLASS_DEFAULT_IMMUNITY_AMOUNT);
        ClassData[ClassCount][class_no_fall_damage] = bool:KvGetNum(kvClassData, "no_fall_damage", ZR_CLASS_DEFAULT_NO_FALL_DAMAGE);
        
        ClassData[ClassCount][class_health] = KvGetNum(kvClassData, "health", ZR_CLASS_DEFAULT_HEALTH);
        ClassData[ClassCount][class_health_regen_interval] = KvGetFloat(kvClassData, "health_regen_interval", ZR_CLASS_DEFAULT_HEALTH_REGEN_INTERVAL);
        ClassData[ClassCount][class_health_regen_amount] = KvGetNum(kvClassData, "health_regen_amount", ZR_CLASS_DEFAULT_HEALTH_REGEN_AMOUNT);
        ClassData[ClassCount][class_health_infect_bonus] = KvGetNum(kvClassData, "health_infect_bonus", ZR_CLASS_DEFAULT_HEALTH_INFECT_BONUS);
        ClassData[ClassCount][class_kill_bonus] = KvGetNum(kvClassData, "kill_bonus", ZR_CLASS_DEFAULT_KILL_BONUS);
        
        ClassData[ClassCount][class_speed] = KvGetFloat(kvClassData, "speed", ZR_CLASS_DEFAULT_SPEED);
        ClassData[ClassCount][class_knockback] = KvGetFloat(kvClassData, "knockback", ZR_CLASS_DEFAULT_KNOCKBACK);
        ClassData[ClassCount][class_jump_height] = KvGetFloat(kvClassData, "jump_height", ZR_CLASS_DEFAULT_JUMP_HEIGHT);
        ClassData[ClassCount][class_jump_distance] = KvGetFloat(kvClassData, "jump_distance", ZR_CLASS_DEFAULT_JUMP_DISTANCE);
        
        // Validate the class attributes.
        if (!ClassValidateAttributes(ClassCount))
        {
            // TODO: There's an invalid class, what do we do?
            // Skip it (clearing data too), disable it and give a log warning,
            // or set the plugin in a failed state?
        }
        else
        {
            // The class is valid. Update the counter.
            ClassCount++;
        }
    } while (KvGotoNextKey(kvClassData));
    
    // Validate team requirements.
    if (!ClassValidateTeamRequirements())
    {
        SetFailState("The class configuration doesn't match the team requirements.");
    }
}

/**
 * Clears the cached class data and copies data from the ClassData array.
 */
ClassReloadDataCache()
{
    /*
     * TODO: This must be done in a safe way, because the plugin may read from
     *       the cache at any time. While looping through attributes, overwrite
     *       if possible, or clear the attribute right before it's set again.
     */
}

/**
 * Clears the players class data and copies data from the specified class data
 * cache.
 *
 * @param client        The client index.
 * @param classindex    The index of the class to read from.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options: ZR_CLASS_CACHE_ORIGINAL (unchanged class
 *                      data), ZR_CLASS_CACHE_MODIFIED (default, modified class
 *                      data).
 */
ClassReloadPlayerCache(client, classindex, cachetype = ZR_CLASS_CACHE_MODIFIED)
{

}

/**
 * Applies class attributes on a player. Changing model, hp, speed, etc.
 *
 * @param client        The player to apply attributes on.
 * @param classindex    Index of the class in the ClassData array.
 */
ClassApplyAttributes(client, classindex)
{
    // TODO: Move class related from zombie infection events to this procedure.
}

/* ============================================================================
 * 
 *                        FILTER AND SEARCH FUNCTIONS
 *                   Validating, getting indexes or lists
 *
 * ============================================================================
 */

/**
 * Validates the team requirements in a class cache and check that theres at
 * least one class for each team. Minium team requirements are zombies and
 * humans. The admin team is optinal.
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
    zombieindex = ClassGetFirstIndex(zr_CLASS_TEAM_ZOMBIES, _, cachetype);
    humanindex = ClassGetFirstIndex(zr_CLASS_TEAM_HUMANS, _, cachetype);
    
    if (zombieindex > -1 && humanindex > -1)
    {
        return true;
    }
    
    return false;
}

/**
 * Validates all the class attributes in the original class data array, to
 * check if they have invalid values.
 *
 * @param classindex    The index of the class to validate.
 * @return              True if validation was successful, false otherwise.
 */
bool:ClassValidateAttributes(classindex)
{
    return true;
    // TODO: Manually check each attribute (hard coded). Better solutions?
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
    if (ClassCount == 0)
    {
        return false;
    }
    
    if (classindex >= 0 && classid < ClassCount)
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
        ClassesGetName(classindex, current_name, sizeof(current_name), cachetype);
        if (strcmp(name, current_name, false) == 0)
        {
            return classindex;
        }
    }
    
    // The class index wasn't found.
    return -1;
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
    new Handle:classarray;
    new arraycount;
    
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
    if (strlen(class_name) > 0)
    {
        // Check if the user set "random" as default class.
        if (strcmp(classname, "random", false) == 0)
        {
            // Get a list of all classes with the specified team ID.
            classindex = ClassGetRandomClass(teamid, cachetype);
            
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
            classindex = ClassGetClassIndex(classname, cachetype);
            
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
                classindex = ClassGetFirstClass(teamid, cachetype);
                
                if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
                {
                    ZR_LogMessageFormatted(-1, "classes", "default spawn class", "Warning: Failed to set \"%s\" as default spawn class for team %d. The class doesn't exist or the team IDs doesn't match. Falling back to the first class in the team.", _, classname, teamid);
                }
                
                // Validate the new index.
                if (ClassValidateIndex(classindex))
                {
                    // Log a warning.
                    if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
                    {
                        ZR_LogMessageFormatted(-1, "classes", "default spawn class", "Warning: The default class name \"%s\" does not exist or matches the team ID.", _, classname);
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
        // Blank class name, return a error code.
        return -1;
    }
}

/* ============================================================================
 * 
 *                         CLASS ATTRIBUTE FUNCTIONS
 *              Retrieving class attributes from a certain cache
 *
 * ============================================================================
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
 * @return          True if it's valid and enabled, false otherwise.
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
            return ClassDataCached[index][class_enabled];
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
            return ClassDataCached[index][class_team];
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return ClassPlayerCache[index][class_team];
        }
    }
    return -1;
}

/**
 * Gets the class name to be displayed in the class menu.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param name      The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return          Number of cells written. -1 on error.
 */
ClassGetName(index, String:name[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(name, maxlen, ClassData[index][class_name]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(name, maxlen, ClassDataCache[index][class_name]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(name, maxlen, ClassPlayerCache[index][class_name]);
        }
    }
    
    return -1;
}

/**
 * Gets the model path string from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param model     The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
ClassGetModel(index, String:model[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(model, maxlen, ClassData[index][class_model_path]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(model, maxlen, ClassDataCache[index][class_model_path]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(model, maxlen, ClassPlayerCache[index][class_model_path]);
        }
    }
    return -1;
}

/**
 * Gets the menu description string from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param menudescr The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
ClassGetMenuDescription(index, String:menudescr[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(menudescr, maxlen, ClassData[index][class_menu_description]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(menudescr, maxlen, ClassDataCache[index][class_menu_description]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(menudescr, maxlen, ClassPlayerCache[index][class_menu_description]);
        }
    }
    return -1;
}

/**
 * Gets the overlay path from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param overlay   The destination string buffer.
 * @param maxlen    The length of the destination string buffer.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  Number of cells written. -1 on error.
 */
ClassGetOverlayPath(index, String:overlay[], maxlen, cachetype = ZR_CLASS_CACHE_PLAYER)
{
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            return strcopy(overlay, maxlen, ClassData[index][class_overlay_path]);
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            return strcopy(overlay, maxlen, ClassDataCache[index][class_overlay_path]);
        }
        case ZR_CLASS_CACHE_PLAYER:
        {
            return strcopy(overlay, maxlen, ClassPlayerCache[index][class_overlay_path]);
        }
    }
    return -1;    
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
bool:ClassGetNVGs(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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

/**
 * Gets the health regen interval value from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health regen interval value from the specified class. -1.0 on
 *          error.
 */
Float:ClassGetHealthRegenInterv(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
 * Gets the kill bonus value from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The kill bonus value from the specified class.
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
 * Gets the health infect bonus value from the specified class.
 *
 * @param index     Index of the class in a class cache or a client index,
 *                  depending on the cache type specified.
 * @param cachetype Optional. Specifies what class cache to read from. Options:
 *                  ZR_CLASS_CACHE_ORIGINAL - Unchanced class data.
 *                  ZR_CLASS_CACHE_MODIFIED - Changed/newest class data.
 *                  ZR_CLASS_CACHE_PLAYER (default) - Player cache. If this one
 *                  is used, index will be used as a client index.
 * @return  The health infect bonus value from the specified class. -1 on
 *          error.
 */
ClassGetHealthInfectBonus(index, cachetype = ZR_CLASS_CACHE_PLAYER)
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
