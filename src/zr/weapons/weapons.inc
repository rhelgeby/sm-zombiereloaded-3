/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          weapons.inc
 *  Type:          Core 
 *  Description:   API for all weapon-related functions.
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
    WEAPONS_DATA_SLOT,
    WEAPONS_DATA_RESTRICTDEFAULT,
    WEAPONS_DATA_TOGGLEABLE,
    WEAPONS_DATA_AMMOTYPE,
    WEAPONS_DATA_AMMOPRICE,
    WEAPONS_DATA_KNOCKBACK,
    WEAPONS_DATA_ZMARKETPRICE,
    WEAPONS_DATA_ZMARKETPURCHASEMAX,
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
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CBasePlayer::m_hActiveWeapon\" was not found.");
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
    ZMarketOnCommandsCreate();
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
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Missing weapons config file: %s", pathweapons);
        
        return;
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_Weapons, pathweapons);
    
    // Load config from file and create array structure.
    new bool:success = ConfigLoadConfig(File_Weapons, arrayWeapons);
    
    // Unexpected error, stop plugin.
    if (!success)
    {
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Unexpected error encountered loading: %s", pathweapons);
        
        return;
    }
    
    // Validate weapons config.
    new size = GetArraySize(arrayWeapons);
    if (!size)
    {
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "No usable data found in weapons config file: %s", pathweapons);
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
        LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Unexpected error caching data from weapons config file: %s", pathweapons);
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
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Weapons, "Config Validation", "Couldn't cache weapon data for: %s (check weapons config)", weaponname);
            continue;
        }
        
        // Get config data.
        
        decl String:weapontype[CONFIG_MAX_LENGTH];
        decl String:ammotype[CONFIG_MAX_LENGTH];
        
        // General
        KvGetString(kvWeapons, "weapontype", weapontype, sizeof(weapontype));
        new WeaponsSlot:weaponslot = WeaponsSlot:KvGetNum(kvWeapons, "weaponslot", -1);
        
        // Restrict (core)
        new bool:restrictdefault = ConfigKvGetStringBool(kvWeapons, "restrictdefault", "no");
        new bool:toggleable = ConfigKvGetStringBool(kvWeapons, "toggleable", "yes");
        
        // Weapon Ammo (core)
    	KvGetString(kvWeapons, "ammotype", ammotype, sizeof(ammotype));
    	new ammoprice = KvGetNum(kvWeapons, "ammoprice", -1);
    	
        // Knockback (module)
        new Float:knockback = KvGetFloat(kvWeapons, "knockback", 1.0);
        
        // ZMarket (module)
        new zmarketprice = KvGetNum(kvWeapons, "zmarketprice", -1);
        new zmarketpurchasemax = KvGetNum(kvWeapons, "zmarketpurchasemax", 0);
        
        new Handle:arrayWeapon = GetArrayCell(arrayWeapons, x);
        
        // Push data into array.
        PushArrayString(arrayWeapon, weapontype);           // Index: 1
        PushArrayCell(arrayWeapon, weaponslot);             // Index: 2
        PushArrayCell(arrayWeapon, restrictdefault);        // Index: 3
        PushArrayCell(arrayWeapon, toggleable);             // Index: 4
        PushArrayString(arrayWeapon, ammotype);             // Index: 5
        PushArrayCell(arrayWeapon, ammoprice);              // Index: 6
        PushArrayCell(arrayWeapon, knockback);              // Index: 7
        PushArrayCell(arrayWeapon, zmarketprice);           // Index: 8
        PushArrayCell(arrayWeapon, zmarketpurchasemax);    // Index: 9
        
        // Initialize other stored weapon info here.
        PushArrayCell(arrayWeapon, restrictdefault);        // Index: 10
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
    // Forward event to sub-modules.
    RestrictClientInit(client);
    WeaponAlphaClientInit(client);
    ZMarketClientInit(client);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
WeaponsOnClientDisconnect(client)
{
    // Forward event to sub-modules.
    RestrictOnClientDisconnect(client);
    WeaponAlphaOnClientDisconnect(client);
    ZMarketOnClientDisconnect(client);
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */
WeaponsOnClientSpawn(client)
{
    // Forward event to sub-modules.
    RestrictOnClientSpawn(client);
    ZMarketOnClientSpawn(client);
}

/**
 * The round is starting.
 */
WeaponsOnRoundStartPost()
{
    // Forward event to sub-modules
    WeaponAlphaOnRoundStartPost();
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
stock WeaponsClearCache(index)
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
 * Takes a weapon's classname and returns the display name in weapons config file.
 * 
 * @param
 */
stock WeaponsClassnameToDisplay(String:classname[], classnamemaxlen, String:display[], displaymaxlen)
{
    // Strip off classnames' weapon prefix.
    ReplaceString(classname, classnamemaxlen, "weapon_", "");
    ReplaceString(classname, classnamemaxlen, "item_", "");
    
    // Get the index of the weapon.
    new weaponindex = WeaponsNameToIndex(classname);
    
    // If weapon index is invalid, then return an empty string.
    if (weaponindex == -1)
    {
        // Return an empty string.
        strcopy(display, displaymaxlen, "");
        return;
    }
    
    // Return the display name.
    WeaponsGetName(weaponindex, display, displaymaxlen);
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
 * Gets the slot index of a weapon at a given index.
 * @param index     The weapon index.
 * @return          The slot index of the weapon.
 */
stock WeaponsSlot:WeaponsGetSlot(index)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Return default restriction status.
    return WeaponsSlot:GetArrayCell(arrayWeapon, _:WEAPONS_DATA_SLOT);
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
    GetArrayString(arrayWeapon, _:WEAPONS_DATA_AMMOTYPE, ammotype, maxlen);
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
 * Gets the max purchases from ZMarket per round per client of a weapon.
 * @param index     The weapon index.
 * @return          The max purchases of the weapon.
 */
stock WeaponsGetZMarketPurchaseMax(index)
{
    // Get array handle of weapon at given index.
    new Handle:arrayWeapon = GetArrayCell(arrayWeapons, index);
    
    // Return the ZMarket price of the weapon.
    return GetArrayCell(arrayWeapon, _:WEAPONS_DATA_ZMARKETPURCHASEMAX);
}

/**
 * General weapon API.
 */

/**
 * Checks if a client has a specific weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon classname.
 */
stock bool:WeaponsClientHasWeapon(client, const String:weapon[])
{
    // Get all of client's current weapons.
    new weapons[WeaponsSlot];
    WeaponsGetClientWeapons(client, weapons);
    
    decl String:classname[64];
    
    // x = slot index
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        // If slot is empty, then stop.
        if (weapons[x] == -1)
        {
            continue;
        }
        
        // If the weapon's classname matches, then return true.
        GetEdictClassname(weapons[x], classname, sizeof(classname));
        ReplaceString(classname, sizeof(classname), "weapon_", "");
        if (StrEqual(weapon, classname, false))
        {
            return true;
        }
    }
    
    return false;
}

/**
 * Return an array that contains all client's weapon indexes.
 * 
 * @param client    The client index.
 * @param weapons   The weapon index array.
 *                  -1 if no weapon in slot. 
 */
stock WeaponsGetClientWeapons(client, weapons[WeaponsSlot])
{
    // x = Weapon slot.
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
