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
 * Number of weapon slots (For CS:S)
 */
#define WEAPONS_SLOTS_MAX 5

/**
 * Weapon config data indexes.
 */
enum WeaponsData
{
    WEAPONS_DATA_NAME = 0,
    WEAPONS_DATA_TYPE,
    WEAPONS_DATA_RESTRICTDEFAULT,
    WEAPONS_DATA_TOGGLEABLE,
    WEAPONS_DATA_AMMOTYPE,
    WEAPONS_DATA_AMMOPRICE,
    WEAPONS_DATA_KNOCKBACK,
    WEAPONS_DATA_ZMARKETPRICE,
    WEAPONS_DATA_RESTRICTED,
}

/**
 * @endsection
 */

/**
 * Variable to store active weapon offset value.
 */
new g_iToolsActiveWeapon;

/**
 * Weapon slots.
 */
enum WeaponsSlot
{
    Slot_Invalid        = -1,   /** Invalid weapon (slot). */
    Slot_Primary        = 0,    /** Primary weapon slot. */
    Slot_Secondary      = 1,    /** Secondary weapon slot. */
    Slot_Melee          = 2,    /** Melee (knife) weapon slot. */
    Slot_Projectile     = 3,    /** Projectile (grenades, flashbangs, etc) weapon slot. */
    Slot_Explosive      = 4,    /** Explosive (c4) weapon slot. */
}

/**
 * Array handle to store weapon config data.
 */
new Handle:arrayWeapons = INVALID_HANDLE;

#include "zr/weapons/restrict"
#include "zr/weapons/weaponammo"
#include "zr/weapons/weaponalpha"
#include "zr/weapons/zmarket"
#include "zr/weapons/menu_weapons"

/**
 * Weapons module init function.
 */ 
WeaponsInit()
{
    // Forward event to sub-modules.
    RestrictInit();
}

/**
 * Find active weapon-specific offsets here.
 */
WeaponsOnOffsetsFound()
{
    // If offset "m_hActiveWeapon" can't be found, then stop the plugin.
    g_iToolsActiveWeapon = FindSendPropInfo("CBasePlayer", "m_hActiveWeapon");
    if (g_iToolsActiveWeapon == -1)
    {
        LogPrintToLog(LOG_FORMAT_TYPE_FATALERROR, "Tools", "Offsets", "Offset \"CBasePlayer::m_hActiveWeapon\" was not found.");
    }
    
    // Forward event to sub-modules
    WeaponAmmoOnOffsetsFound();
}

/**
 * Create commands related to weapons here.
 */
WeaponsOnCommandsCreate()
{
    // Forward event to sub-modules.
    RestrictOnCommandsCreate();
}

/**
 * Loads weapon data from file.
 */
WeaponsLoad()
{
    // Register config file.
    ConfigRegisterConfig(File_Weapons, Structure_Keyvalue, CONFIG_FILE_ALIAS_WEAPONS);
    
    // If module is disabled, then stop.
    new bool:weapons = GetConVarBool(g_hCvarsList[CVAR_WEAPONS]);
    if (!weapons)
    {
        return;
    }
    
    // Get weapons config path.
    decl String:pathweapons[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_WEAPONS, pathweapons);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure.
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Weapons", "Config Validation", "Missing weapons config file: %s", pathweapons);
        
        return;
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_Weapons, pathweapons);
    
    // Load config from file and create array structure.
    new bool:success = ConfigLoadConfig(File_Weapons, arrayWeapons);
    
    // Unexpected error, stop plugin.
    if (!success)
    {
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Weapons", "Config Validation", "Unexpected error encountered loading: %s", pathweapons);
        
        return;
    }
    
    // Validate weapons config.
    new size = GetArraySize(arrayWeapons);
    if (!size)
    {
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Weapons", "Config Validation", "No usable data found in weapons config file: %s", pathweapons);
    }
    
    // Now copy data to array structure.
    WeaponsCacheData();
    
    // Set config data.
    ConfigSetConfigLoaded(File_Weapons, true);
    ConfigSetConfigReloadFunc(File_Weapons, GetFunctionByName(GetMyHandle(), "WeaponsOnConfigReload"));
    ConfigSetConfigHandle(File_Weapons, arrayWeapons);
    
    // Forward event to sub-modules
    RestrictLoad();
}

/**
 * Caches weapon data from file into arrays.
 * Make sure the file is loaded before (ConfigLoadConfig) to prep array structure.
 */
WeaponsCacheData()
{
    // Get config's file path.
    decl String:pathweapons[PLATFORM_MAX_PATH];
    ConfigGetConfigPath(File_Weapons, pathweapons, sizeof(pathweapons));
    
    new Handle:kvWeapons;
    new bool:success = ConfigOpenConfigFile(File_Weapons, kvWeapons);
    
    if (!success)
    {
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Weapons", "Config Validation", "Unexpected error caching data from weapons config file: %s", pathweapons);
    }
    
    decl String:weaponname[WEAPONS_MAX_LENGTH];
    
    // x = array index
    new size = GetArraySize(arrayWeapons);
    for (new x = 0; x < size; x++)
    {
        WeaponsGetName(x, weaponname, sizeof(weaponname));
        KvRewind(kvWeapons);
        if (!KvJumpToKey(kvWeapons, weaponname))
        {
            LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Weapons", "Config Validation", "Couldn't cache weapon data for: %s (check weapons config)", weaponname);
            continue;
        }
        
        // Get config data.
        
        decl String:weapontype[CONFIG_MAX_LENGTH];
        decl String:ammotype[CONFIG_MAX_LENGTH];
        
        // General
        KvGetString(kvWeapons, "weapontype", weapontype, sizeof(weapontype));
        
        // Restrict (core)
        new bool:restrictdefault = ConfigKvGetStringBool(kvWeapons, "restrictdefault", "no");
        new bool:toggleable = ConfigKvGetStringBool(kvWeapons, "toggleable", "yes");
        
        // Weapon Ammo (core)
    	KvGetString(kvWeapons, "ammotype", ammotype, sizeof(ammotype));
    	new ammoprice = KvGetNum(kvWeapons, "ammoprice");
    	
        // Knockback (module)
        new Float:knockback = KvGetFloat(kvWeapons, "knockback", 1.0);
        
        // ZMarket (module)
        new zmarketprice = KvGetNum(kvWeapons, "zmarketprice", -1);
        
        new Handle:arrayWeapon = GetArrayCell(arrayWeapons, x);
        
        // Push data into array.
        PushArrayString(arrayWeapon, weapontype);       // Index: 1
        PushArrayCell(arrayWeapon, restrictdefault);    // Index: 2
        PushArrayCell(arrayWeapon, toggleable);         // Index: 3
        PushArrayString(arrayWeapon, ammotype);         // Index: 4
        PushArrayCell(arrayWeapon, ammoprice);          // Index: 5
        PushArrayCell(arrayWeapon, knockback);          // Index: 6
        PushArrayCell(arrayWeapon, zmarketprice);       // Index: 7
        
        // Initialize other stored weapon info here.
        PushArrayCell(arrayWeapon, restrictdefault);    // Index: 8
    }
    
    // We're done with this file now, so we can close it.
    CloseHandle(kvWeapons);
}

/**
 * Called when config is being reloaded.
 */
public WeaponsOnConfigReload()
{
    // Reload weapons config.
    WeaponsLoad();
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
    WeaponAlphaClientInit(client);
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
    WeaponAlphaOnClientDisconnect(client);
}

/**
 * The round is starting.
 */
WeaponsOnRoundStart()
{
    // Forward event to sub-modules
    WeaponAlphaOnRoundStart();
}

/**
 * The round is ending.
 * 
 * @param reason    Reason the round has ended.
 */
WeaponsOnRoundEnd()
{
    // Forward event to sub-modules
    WeaponAlphaOnRoundEnd();
}

/**
 * Weapon data reading API.
 */

/**
 * Clear cache for a given weapon.
 * 
 * @param index     The weapon index.
 */
WeaponsClearCache(index)
{
    // Get array handle of weapon at given index.
    new Handle:hWeapon = GetArrayCell(arrayWeapons, index);
    
    // Clear array.
    ClearArray(hWeapon);
}

/**
 * Find the index at which the weapon's name is at.
 * 
 * @param weapon    The weapon name.
 * @return          The array index containing the given weapon name.
 */
stock WeaponsNameToIndex(const String:weapon[])
{
    decl String:weaponname[WEAPONS_MAX_LENGTH];
    
    // x = Array index.
    new size = GetArraySize(arrayWeapons);
    for (new x = 0; x < size; x++)
    {
        WeaponsGetName(x, weaponname, sizeof(weaponname));
        
        // If names match, then return index.
        if (StrEqual(weapon, weaponname, false))
        {
            return x;
        }
    }
    
    // Name doesn't exist.
    return -1;
}

/**
 * Checks if a weapon is valid. (E.G. listed in weapons.txt)
 * @param weapon    The weapon name.
 * @return          Returns true if valid, false it not.
 */
stock bool:WeaponsIsWeaponValid(const String:weapon[])
{
    return (WeaponsNameToIndex(weapon) != -1);
}

/**
 * Gets the name of a weapon at a given index.
 * @param index     The weapon index.
 * @param weapon    The string to return name in.
 * @param maxlen    The max length of the string.
 */
stock WeaponsGetName(index, String:weapon[], maxlen)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Get weapon name.
    GetArrayString(arrayWeapon, _:WEAPONS_DATA_NAME, weapon, maxlen);
}

/**
 * Gets the type of a weapon at a given index.
 * @param index     The weapon index.
 * @param type      The string to return type in.
 * @param maxlen    The max length of the string.
 */
stock WeaponsGetType(index, String:type[], maxlen)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Get weapon type.
    GetArrayString(arrayWeapon, _:WEAPONS_DATA_TYPE, type, maxlen);
}

/**
 * Gets if a weapon is restricted by default.
 * @param index     The weapon index.
 * @return          True if the weapon is restricted by default, false if not.
 */
stock bool:WeaponsGetRestrictDefault(index)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Return default restriction status.
    return bool:GetArrayCell(arrayWeapon, _:WEAPONS_DATA_RESTRICTDEFAULT);
}

/**
 * Gets if a weapon's restriction status is toggleable.
 * @param index     The weapon index.
 * @return          True if the weapon restriction can be toggled, false if not.
 */
stock bool:WeaponsGetToggleable(index)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Return if weapon is toggleable.
    return bool:GetArrayCell(arrayWeapon, _:WEAPONS_DATA_TOGGLEABLE);
}

/**
 * Gets the ammo type of a weapon at a given index.
 * @param index     The weapon index.
 * @param ammotype  The string to return ammotype in.
 * @param maxlen    The max length of the string.
 */
stock WeaponsGetAmmoType(index, String:ammotype[], maxlen)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Get ammo type of the weapon.
    GetArrayString(arrayWeapon, _:WEAPONS_DATA_AMMOTYPE, type, maxlen);
}

/**
 * Gets the price of ammo for the weapon.
 * @param index     The weapon index.
 * @return          The ammo price.
 */
stock WeaponsGetAmmoPrice(index)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Return ammo price of the weapon.
    return GetArrayCell(arrayWeapon, _:WEAPONS_DATA_AMMOPRICE);
}

/**
 * Gets the knockback multiplier for the weapon.
 * @param index     The weapon index.
 * @return          The weapon knockback multiplier.
 */
stock Float:WeaponsGetKnockback(index)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Return knockback multiplier of the weapon.
    return Float:GetArrayCell(arrayWeapon, _:WEAPONS_DATA_KNOCKBACK);
}

/**
 * Gets the ZMarket price for the weapon.
 * @param index     The weapon index.
 * @return          The ZMarket price.
 */
stock WeaponsGetZMarketPrice(index)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Return the ZMarket price of the weapon.
    return GetArrayCell(arrayWeapon, _:WEAPONS_DATA_ZMARKETPRICE);
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
stock WeaponsGetClientWeapons(client, weapons[WeaponsSlot])
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
stock WeaponsGetDeployedWeaponIndex(client)
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
stock WeaponsSlot:WeaponsGetDeployedWeaponSlot(client)
{
    // Get all client's weapon indexes.
    new weapons[WeaponsSlot];
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
            return WeaponsSlot:x;
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
stock WeaponsForceClientDrop(client, weapon)
{
    // Force client to drop weapon.
    SDKCall(g_hToolsCSWeaponDrop, client, weapon, true, false);
}
