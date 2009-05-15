/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          hitgroup.inc
 *   Type:          Core
 *   Description:   API for loading hitgroup specific settings.
 *
 * ============================================================================
 */

/**
 * Keyvalue handle to store hitgroups data.
 * 
 * @redir   config.inc
 */

/**
 * @section Player hitgroup values.
 */
#define HITGROUP_GENERIC    0
#define HITGROUP_HEAD       1
#define HITGROUP_CHEST      2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4
#define HITGROUP_RIGHTARM   5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG   7
#define HITGROUP_GEAR       10
/**
 * @endsection
 */

/**
 * Clears hitgroup data.
 */ 
HitgroupsClearData()
{
    // Load hitgroup data.
    if (kvHitgroups != INVALID_HANDLE)
    {
        CloseHandle(kvHitgroups);
    }
    
    kvHitgroups = CreateKeyValues("hitgroups");
}

/**
 * Loads hitgroup data from file.
 */ 
HitgroupsLoad()
{
    // Clear hitgroup data
    HitgroupsClearData();
    
    // If module is disabled, then stop.
    new bool:hitgroups = GetConVarBool(g_hCvarsList[CVAR_HITGROUPS]);
    if (!hitgroups)
    {
        return;
    }
    
    // Get hitgroups config path.
    decl String:pathhitgroups[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetFilePath(CVAR_CONFIG_PATH_HITGROUPS, pathhitgroups);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure.
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Hitgroups", "Config Validation", "Missing hitgroups config file: %s", pathhitgroups);
        return;
    }
    
    // Put file data into memory.
    FileToKeyValues(kvHitgroups, pathhitgroups);
    
    // Validate hitgroups config.
    HitgroupsValidateConfig();
}

/**
 * Validate hitgroup config file and settings.
 */ 
HitgroupsValidateConfig()
{
    KvRewind(kvHitgroups);
    if (!KvGotoFirstSubKey(kvHitgroups))
    {
        // Log that no data was loaded from hitgroup file.
        LogPrintToLog(LOG_FORMAT_TYPE_NORMAL, "Hitgroups", "Config Validation", "No hitgroups listed in hitgroups.txt, disabling hitgroup-based modules.");
    }
}

/**
 * Retrieve hitgroup knockback value.
 * 
 * @param hitgroup  The hitgroup index.
 * @return          The knockback multiplier of the hitgroup. 
 */
Float:HitgroupsGetHitgroupKnockback(hitgroup)
{
    // Reset keyvalue's traversal stack.
    KvRewind(kvHitgroups);
    if (KvGotoFirstSubKey(kvHitgroups))
    {
        decl String:sHitgroup[4];
        
        do
        {
            KvGetSectionName(kvHitgroups, sHitgroup, sizeof(sHitgroup));
            
            // If this is the right hitgroup, then return knockback for it.
            if (hitgroup == StringToInt(sHitgroup))
            {
                return KvGetFloat(kvHitgroups, "knockback", 1.0);
            }
        } while (KvGotoNextKey(kvHitgroups));
    }
    
    return 1.0;
}

/**
 * Retrieve hitgroup damage value.
 * 
 * @param hitgroup  The hitgroup index.
 * @return          True if hitgroup can be damaged, false if not.
 */
bool:HitgroupsCanDamageHitgroup(hitgroup)
{
    // Reset keyvalue's traversal stack.
    KvRewind(kvHitgroups);
    if (KvGotoFirstSubKey(kvHitgroups))
    {
        decl String:sHitgroup[4];
        decl String:damage[8];
        
        do
        {
            KvGetSectionName(kvHitgroups, sHitgroup, sizeof(sHitgroup));
            
            // If this is the right hitgroup, then return knockback for it.
            if (hitgroup == StringToInt(sHitgroup))
            {
                // Get config setting string.
                KvGetString(kvHitgroups, "damage", damage, sizeof(damage), "yes");
                
                // Return hitgroup's damage setting.
                return ConfigSettingToBool(damage);
            }
        } while (KvGotoNextKey(kvHitgroups));
    }
    
    // If hitgroup is missing, then default to "yes."
    return true;
}
