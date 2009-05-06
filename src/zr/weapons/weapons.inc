/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          weapons.inc
 *   Type:          Core 
 *   Description:   API for all weapon-related functions.
 *
 * ============================================================================
 */

/**
 * Maximum length of a weapon name string
 */
#define WEAPONS_MAX_LENGTH 32
/**
 * @endsection
 */

/**
 * Number of weapon slots (For CS:S)
 */
#define WEAPONS_SLOTS_MAX 5

/**
 * Weapon types.
 */
enum WeaponsType
{
    Type_Invalid = -1, /** Invalid weapon (slot). */
    Type_Primary = 0, /** Primary weapon slot. */
    Type_Secondary = 1, /** Secondary weapon slot. */
    Type_Melee = 2, /** Melee (knife) weapon slot. */
    Type_Projectile = 3, /** Projectile (grenades, flashbangs, etc) weapon slot. */
    Type_Explosive = 4, /** Explosive (c4) weapon slot. */
}

/**
 * Keyvalue handle to store weapon data.
 * 
 * @redir   config.inc
 */

#include "zr/weapons/restrict"
#include "zr/weapons/markethandler"
#include "zr/weapons/menu_weapons"

/**
 * Weapons module init function.
 */ 
WeaponsInit()
{
    // Forward event to sub-module
    RestrictInit();
}

/**
 * Clears weapon data.
 */
WeaponsClearData()
{
    // Load weapon data
    if (kvWeapons != INVALID_HANDLE)
    {
        CloseHandle(kvWeapons);
    }
    
    kvWeapons = CreateKeyValues("weapons");
}

/**
 * Loads weapon data from file.
 */
WeaponsLoad()
{
    // Clear weapon data.
    WeaponsClearData();
    
    // If module is disabled, then stop.
    new bool:weapons = GetConVarBool(g_hCvarsList[CVAR_WEAPONS]);
    if (!weapons)
    {
        return;
    }
    
    // Get weapons config path.
    decl String:pathweapons[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetFilePath(CVAR_CONFIG_PATH_WEAPONS, pathweapons);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure.
        if (LogCheckFlag(LOG_CORE_EVENTS, LOG_MODULE_WEAPONS))
        {
            LogMessageFormatted(-1, "Weapons", "Config Validation", "Missing weapons config file: %s", LOG_FORMAT_TYPE_ERROR, pathweapons);
        }
        
        return;
    }
    
    // Put file data into memory.
    FileToKeyValues(kvWeapons, pathweapons);
    
    // Validate weapons config.
    WeaponsValidateConfig();
    
    // Forward event to sub-module.
    RestrictOnMapStart();
}

/**
 * Validate weapon config file and settings.
 */
WeaponsValidateConfig()
{
    // If log flag check fails, then don't log.
    if (!LogCheckFlag(LOG_CORE_EVENTS, LOG_MODULE_WEAPONS))
    {
        return;
    }
    
    KvRewind(kvWeapons);
    if (!KvGotoFirstSubKey(kvWeapons))
    {
        LogMessageFormatted(-1, "Weapons", "Config Validation", "No weapons listed in weapons.txt.", LOG_FORMAT_TYPE_ERROR);
    }
}

/**
 * Client is joining the server.
 * 
 * @param client    The client index.  
 */
WeaponsClientInit(client)
{
    // Forward event to sub-module.
    RestrictClientInit(client);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */ 
WeaponsOnClientDisconnect(client)
{
    // Forward event to sub-module.
    RestrictOnClientDisconnect(client);
}

/**
 * Creates an array of all listed weapons in weapons.txt.
 * @param arrayWeapons  The handle of the array, don't forget to call CloseHandle
 *                      on it when finished!
 * @return              The size of the array.
 */
WeaponsCreateWeaponArray(&Handle:arrayWeapons, maxlen = WEAPONS_MAX_LENGTH)
{
    // Initialize array handle.
    arrayWeapons = CreateArray(maxlen);
    new count = 0;
    
    // Reset keyvalue's traveral stack.
    KvRewind(kvWeapons);
    if (KvGotoFirstSubKey(kvWeapons))
    {
        decl String:weapon[maxlen];
        
        do
        {
            KvGetSectionName(kvWeapons, weapon, maxlen);
            
            // Push weapon name into the array.
            PushArrayString(arrayWeapons, weapon);
            
            // Increment count.
            count++;
        } while (KvGotoNextKey(kvWeapons));
    }
    
    // Return the count
    return count;
}

/**
 * Checks if a weapon is valid. (aka listed in weapons.txt)
 * @param weapon    The weapon name.
 * @return          Returns true if valid, false it not.
 */
bool:WeaponsIsValidWeapon(const String:weapon[])
{
    // Reset keyvalue's traversal stack.
    KvRewind(kvWeapons);
    if (KvGotoFirstSubKey(kvWeapons))
    {
        decl String:validweapon[WEAPONS_MAX_LENGTH];
        
        do
        {
            KvGetSectionName(kvWeapons, validweapon, sizeof(validweapon));
            
            // If weaponname matches a valid weapon, then return true.
            if (StrEqual(validweapon, weapon, false))
            {
                return true;
            }
            
        } while (KvGotoNextKey(kvWeapons));
    }
    
    // Weapon is invalid.
    return false;
}

/**
 * Looks up a weapon in weapons.txt and returns exact display name.
 * @param weapon    The weapon name.
 * @param display   Returns with the display name, is not changed if weapon is invalid. 
 */
WeaponGetDisplayName(const String:weapon[], String:display[])
{
    // Reset keyvalue's traversal stack.
    KvRewind(kvWeapons);
    if (KvGotoFirstSubKey(kvWeapons))
    {
        decl String:validweapon[WEAPONS_MAX_LENGTH];
        
        do
        {
            KvGetSectionName(kvWeapons, validweapon, sizeof(validweapon));
            
            // If weapon matches a valid weapon (case-insensitive), then return display name.
            if (StrEqual(validweapon, weapon, false))
            {
                strcopy(display, WEAPONS_MAX_LENGTH, validweapon);
            }
            
        } while (KvGotoNextKey(kvWeapons));
    }
}

/**
 * Checks if a weapon restriction can be toggled by the admin menu.
 * @param weapon    The weapon name.
 * @return          Returns true if restricted, false it not.
 */
bool:WeaponsIsWeaponMenu(const String:weapon[])
{
    // Reset keyvalue's traversal stack.
    KvRewind(kvWeapons);
    if (KvGotoFirstSubKey(kvWeapons))
    {
        decl String:validweapon[WEAPONS_MAX_LENGTH];
        decl String:menu[8];
        
        do
        {
            KvGetSectionName(kvWeapons, validweapon, sizeof(validweapon));
            
            // If this is the right weapon, then return setting for it.
            if (StrEqual(validweapon, weapon, false))
            {
                KvGetString(kvWeapons, "menu", menu, sizeof(menu), "yes");
                
                // Return weapon's setting.
                return ConfigSettingToBool(menu);
            }
        } while (KvGotoNextKey(kvWeapons));
    }
    
    return false;
}

/**
 * Returns knockback multiplier of the weapon.
 * @param weapon    The weapon name.
 * @return          The float value of the knockback multiplier, 1.0 if not found.
 */
Float:WeaponGetWeaponKnockback(const String:weapon[])
{
    // Reset keyvalue's traversal stack.
    KvRewind(kvWeapons);
    if (KvGotoFirstSubKey(kvWeapons))
    {
        decl String:validweapon[WEAPONS_MAX_LENGTH];
        
        do
        {
            KvGetSectionName(kvWeapons, validweapon, sizeof(validweapon));
            
            // If this is the right weapon, then return setting for it.
            if (StrEqual(validweapon, weapon, false))
            {
                return KvGetFloat(kvWeapons, "knockback", 1.0);
            }
        } while (KvGotoNextKey(kvWeapons));
    }
    
    return 1.0;
}

/**
 * General weapon API.
 */

/**
 * Return an array that contains all client's weapon indexes.
 * 
 * @param client    The client index.
 * @param weapons   The weapon index array.
 *                  -1 if no weapon in slot. 
 */
WeaponsGetClientWeapons(client, weapons[WeaponsType])
{
    // x = weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        weapons[x] = GetPlayerWeaponSlot(client, x);
    }
}

/**
 * Returns weapon index of the client's deployed weapon.
 * 
 * @param client    The client index.
 * @return          The weapon index of the deployed weapon.
 *                  -1 if no weapon is deployed. 
 */
WeaponsGetDeployedWeaponIndex(client)
{
    // Return the client's active weapon.
    return GetEntDataEnt2(client, offsActiveWeapon);
}

/**
 * Returns slot of client's deployed weapon.
 *
 * @param client    The client index.
 * @return          The slot number of deployed weapon.
 */
WeaponsType:WeaponsGetDeployedWeaponSlot(client)
{
    // Get all client's weapon indexes.
    new weapons[WeaponsType];
    WeaponsGetClientWeapons(client, weapons);
    
    // Get client's deployed weapon.
    new deployedweapon = WeaponsGetDeployedWeaponIndex(client);
    
    // If client has no deployed weapon, then stop.
    if (deployedweapon == -1)
    {
        return Type_Invalid;
    }
    
    // x = weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        if (weapons[x] == deployedweapon)
        {
            return x;
        }
    }
    
    return Type_Invalid;
}

/**
 * Forces player to drop weapon index.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index to force client to drop.
 */
WeaponForceClientDrop(client, weapon)
{
    // Force client to drop weapon.
    SDKCall(g_hToolsCSWeaponDrop, client, weapon, true, false);
}
