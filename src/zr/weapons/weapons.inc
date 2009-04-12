/**
 * ====================
 *   Zombie:Reloaded
 *   File: weapons.inc
 *   Author: Greyscale
 * ==================== 
 */

/**
 * Handle to store trie weapon data
 */
new Handle:trieWeapons = INVALID_HANDLE;

/**
 * Maximum length of a weapon name string
 */
#define WEAPONS_MAX_LENGTH 32

/**
 * @section Config array indexes.
 */
#define WEAPON_RESTRICT 0
#define WEAPON_MENU 1
/**
 * @endsection
 */
  
/**
 * Array to store keyvalue data.
 */
new Handle:kvWeapons = INVALID_HANDLE;

#include "zr/weapons/restrict"
#include "zr/weapons/markethandler"
#include "zr/weapons/menu_weapons"

/**
 * Loads weapon data from file.
 */
WeaponsOnMapStart()
{
    // Destroy trie data if populated
    if (trieWeapons != INVALID_HANDLE)
    {
        CloseHandle(trieWeapons);
    }
    
    // Create trie to store weapon data
    trieWeapons = CreateTrie();
    
    // Load weapon data
    if (kvWeapons != INVALID_HANDLE)
    {
        CloseHandle(kvWeapons);
    }
    
    kvWeapons = CreateKeyValues("weapons");
    
    decl String:path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), "configs/zr/weapons/weapons.txt");
    
    // If file isn't found, stop plugin
    if (!FileToKeyValues(kvWeapons, path))
    {
        SetFailState("\"%s\" missing from server", path);
    }
    
    KvRewind(kvWeapons);
    if (KvGotoFirstSubKey(kvWeapons))
    {
        decl String:weapon[WEAPONS_MAX_LENGTH];
        decl String:restrict[8];
        decl String:menu[8];
        
        do
        {
            KvGetSectionName(kvWeapons, weapon, sizeof(weapon));
            KvGetString(kvWeapons, "restrict", restrict, sizeof(restrict), "no");
            KvGetString(kvWeapons, "menu", menu, sizeof(menu), "yes");
            
            new bool:config[2];
            config[WEAPON_RESTRICT] = ConfigOptionToBool(restrict);
            config[WEAPON_MENU] = ConfigOptionToBool(menu);
            
            SetTrieArray(trieWeapons, weapon, config, sizeof(config), false);
            
        } while (KvGotoNextKey(kvWeapons));
    }
    
    // Weapon restrict
    WeaponRestrictOnMapStart();
}

/**
 * Creates an array of all listed weapons in weapons.txt.
 * @param arrayWeapons  The handle of the array, don't forget to call CloseHandle
 *                      on it when finished!
 * @return              The size of the array.
 */
WeaponsCreateWeaponArray(&Handle:arrayWeapons, maxlen = WEAPONS_MAX_LENGTH)
{
    arrayWeapons = CreateArray(maxlen);
    new count = 0;
    
    KvRewind(kvWeapons);
    if (KvGotoFirstSubKey(kvWeapons))
    {
        decl String:weapon[maxlen];
        
        do
        {
            KvGetSectionName(kvWeapons, weapon, maxlen);
            
            PushArrayString(arrayWeapons, weapon);
            count++;
        } while (KvGotoNextKey(kvWeapons));
    }
    
    return count;
}

/**
 * Creates an array of all listed weapon groups in weapongroups.txt.
 * @param arrayWeaponGroups  The handle of the array, don't forget to call CloseHandle
 *                           on it when finished!
 * @return                   The size of the array.
 */
WeaponsCreateWeaponGroupArray(&Handle:arrayWeaponGroups, maxlen = WEAPONS_MAX_LENGTH)
{
    arrayWeaponGroups = CreateArray(maxlen);
    new count = 0;
    
    KvRewind(kvWeaponGroups);
    if (KvGotoFirstSubKey(kvWeaponGroups))
    {
        decl String:weapongroup[maxlen];
        
        do
        {
            KvGetSectionName(kvWeaponGroups, weapongroup, maxlen);
            
            PushArrayString(arrayWeaponGroups, weapongroup);
            count++;
        } while (KvGotoNextKey(kvWeaponGroups));
    }
    
    return count;
}

/**
 * Checks if a weapon is valid (aka listed in weapons.txt)
 * @param weapon    The weapon name.
 * @return          Returns true if valid, false it not.
 */
bool:WeaponsIsValidWeapon(const String:weapon[])
{
    new bool:config[2];
    
    return GetTrieArray(trieWeapons, weapon, config, sizeof(config));
}
    
/**
 * Checks if a weapon is restricted by default.
 * @param weapon    The weapon name.
 * @return          Returns true if restricted, false it not.
 */
bool:WeaponsIsWeaponRestrict(const String:weapon[])
{
    new bool:config[2];
    GetTrieArray(trieWeapons, weapon, config, sizeof(config));
    
    return config[WEAPON_RESTRICT];
}

/**
 * Checks if a weapon restriction can be toggled by the admin menu.
 * @param weapon    The weapon name.
 * @return          Returns true if restricted, false it not.
 */
bool:WeaponsIsWeaponMenu(const String:weapon[])
{
    new bool:config[2];
    GetTrieArray(trieWeapons, weapon, config, sizeof(config));
    
    return config[WEAPON_MENU];
}