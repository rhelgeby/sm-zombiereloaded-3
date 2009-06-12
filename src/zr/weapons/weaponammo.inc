/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          weaponammo.inc
 *  Type:          Core 
 *  Description:   API for all weaponammo-related functions.
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
 * @section Variables to store ammo offset values.
 */
new g_iToolsClip1;
new g_iToolsClip2;
/**
 * @endsection
 */

/**
 * Find ammo-reserve-specific offsets here.
 */
WeaponAmmoOnOffsetsFound()
{
    // If offset "m_iClip1" can't be found, then stop the plugin.
    g_iToolsClip1 = FindSendPropInfo("CBaseCombatWeapon", "m_iClip1");
    if (g_iToolsClip1 == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CBaseCombatWeapon::m_iClip1\" was not found.");
    }
    
    // If offset "m_iClip2" can't be found, then stop the plugin.
    g_iToolsClip2 = FindSendPropInfo("CBaseCombatWeapon", "m_iClip2");
    if (g_iToolsClip2 == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Weapons, "Offsets", "Offset \"CBaseCombatWeapon::m_iClip2\" was not found.");
    }
}

/**
 * Set clip/reserve ammo on a weapon.
 * 
 * @param weapon    The weapon index.
 * @param clip      True sets clip ammo, false sets reserve.
 * @param value     The amount of ammo to set to.
 * @param add       (Optional) If true, the value is added to the weapon's current ammo count.
 */
stock WeaponAmmoSetAmmo(weapon, bool:clip, value, bool:add = false)
{
    // Set variable to offset we are changing.
    new ammooffset = clip ? g_iToolsClip1 : g_iToolsClip2;
    
    // Initialize variable (value is 0)
    new ammovalue;
    
    // If we are adding, then update variable with current ammo value.
    if (add)
    {
        ammovalue = WeaponAmmoGetAmmo(weapon, clip);
    }
    
    // Return ammo offset value.
    SetEntData(weapon, ammooffset, ammovalue + value, _, true);
}

/**
 * Get clip/reserve ammo on a weapon.
 * 
 * @param weapon    The weapon index.
 * @param clip      True gets clip ammo, false gets reserve.
 */
stock WeaponAmmoGetAmmo(weapon, bool:clip)
{
    // Set variable to offset we are changing.
    new ammooffset = clip ? g_iToolsClip1 : g_iToolsClip2;
    
    // Return ammo offset value.
    return GetEntData(weapon, ammooffset);
}
