/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          weaponalpha.inc
 *   Type:          Core 
 *   Description:   Weapon alpha functions, and alpha updating on drop/pickup.
 *
 * ============================================================================
 */

/**
 * Default alpha on any CS:S weapon.
 */
#define WEAPONALPHA_DEFAULT_VALUE 255

/**
 * Array that stores the "HookID" to be later unhooked on player disconnect.
 */
new g_iWeaponDropHookID[MAXPLAYERS + 1] = {-1, ...};

/**
 * Global variable that stops render mode modifying
 */
new bool:g_bWeaponAlpha;

/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
WeaponAlphaClientInit(client)
{
    // Hook "Weapon_Drop" on client.
    g_iWeaponDropHookID[client] = ZRTools_HookWeapon_Drop(client, WeaponAlphaDrop);
}

/**
 * Client is leaving the server.
 * 
 * @param client    The client index.
 */
WeaponAlphaOnClientDisconnect(client)
{
    // Unhook "Weapon_Drop" callback, and reset variable.
    if (g_iWeaponDropHookID[client] != -1)
    {
        ZRTools_UnhookWeapon_Drop(g_iWeaponDropHookID[client]);
        g_iWeaponDropHookID[client] = -1;
    }
}

/**
 * The round is starting.
 */
WeaponAlphaOnRoundStart()
{
    // Allow weapon render mode to be modified.
    g_bWeaponAlpha = true;
}

/**
 * The round is ending.
 * 
 * @param reason    Reason the round has ended.
 */
WeaponAlphaOnRoundEnd()
{
    // Disallow weapon render mode to be modified.
    g_bWeaponAlpha = false;
}

/**
 * Client has just picked up a weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index.   
 */
WeaponAlphaOnItemPickup(client, weapon)
{
    // If weapon alpha updating is disabled, then stop.
    if (!g_bWeaponAlpha)
    {
        return;
    }
    
    // Get client's current alpha.
    new alpha = ToolsGetEntityAlpha(client);
    
    // Set new alpha on weapons.
    WeaponAlphaApplyWeaponAlpha(weapon, alpha);
}

/**
 * Callback function for Weapon_Drop.
 * Called when a client drops their weapon.
 * 
 * @param client    The client index.
 * @param weapon    The weapon index.
 */
public ZRTools_Action:WeaponAlphaDrop(client, weapon)
{
    // If weapon alpha updating is disabled, then stop.
    if (!g_bWeaponAlpha)
    {
        return;
    }
    
    // If weapon isn't a valid entity, then stop.
    if (weapon < MaxClients)
    {
        return;
    }
    
    // Set new alpha on weapons.
    WeaponAlphaApplyWeaponAlpha(weapon, WEAPONALPHA_DEFAULT_VALUE);
}

/**
 * A client's alpha has been changed by the plugin.
 * 
 * @param client    The client index.
 */
WeaponAlphaOnClientAlphaChanged(client, alpha)
{
    // Set new alpha on weapons.
    WeaponAlphaApplyWeaponAlpha(client, alpha);
}

/**
 * Set's the alpha on a client's weapons.
 * 
 * @param entity    If client index is given, alpha will be set on all their weapons.
 *                  If a non-client index is given, alpha will be set on given entity.
 * @param alpha     The alpha to set the weapons to.
 */
WeaponAlphaApplyWeaponAlpha(entity, alpha)
{
    if (entity > MaxClients)
    {
        // Turn rendermode on, on the weapon.
        SetEntityRenderMode(entity, RENDER_TRANSALPHA);
        
        // Set alpha value on the weapon.
        SetEntityRenderColor(entity, _, _, _, alpha);
        
        // Entity alpha has been set, so stop.
        return;
    }
    
    // Get client's list of weapons.
    new weapons[WeaponsType];
    WeaponsGetClientWeapons(entity, weapons);
    
    // Loop through array slots and set alpha.
    // x = weapon slot.
    for (new x = 0; x < WEAPONS_SLOTS_MAX; x++)
    {
        // If weapon is invalid, then stop.
        if (weapons[x] == -1)
        {
            continue;
        }
        
        // Turn rendermode on, on the weapon.
        SetEntityRenderMode(weapons[x], RENDER_TRANSALPHA);
        
        // Set alpha value on the weapon.
        SetEntityRenderColor(weapons[x], _, _, _, alpha);
    }
}