/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volgenericattributes.inc
 *  Type:           Module 
 *  Description:    Functions for getting or setting general volume attributes.
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
 * Parses a team string value and applies it to the specified volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param team          A team string value. Valid options: "all", "humans"
 *                      and "zombies".
 * @return              True if successfully set, false otherwise.
 */
stock bool:VolSetTeamString(volumeIndex, const String:team[])
{
    new VolumeTeamFilters:teamfilter;
    
    // Check if string value is empty.
    if (strlen(team) == 0)
    {
        return false;
    }
    
    // Convert value.
    if (strcmp(team, "all", false))
    {
        teamfilter = VolTeam_All;
    }
    else if (strcmp(team, "humans", false))
    {
        teamfilter = VolTeam_Humans;
    }
    else if (strcmp(team, "zombies", false))
    {
        teamfilter = VolTeam_Zombies;
    }
    
    // Apply value.
    Volumes[volumeIndex][vol_team_filter] = teamfilter;
    return true;
    
}

/**
 * Sets the feam filter attribute on a volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param team          The team filter value.
 */
stock VolSetTeam(volumeIndex, VolumeTeamFilters:team[])
{
    Volumes[volumeIndex][vol_team_filter] = team;
}

/**
 * Parses a delay string value and applies it to the specified volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param delay         The delay to apply. A floating point number formatted
 *                      as a string. 
 * @return              True if successfully set, false otherwise.
 */
stock bool:VolSetDelayString(volumeIndex, const String:delay[])
{
    new Float:triggerdelay;
    
    // Check if string value is empty.
    if (strlen(delay) == 0)
    {
        return false;
    }
    
    // Convert value.
    triggerdelay = StringToFloat(delay);
    
    // Apply value.
    Volumes[volumeIndex][vol_trigger_delay] = triggerdelay;
    return true;
}

/**
 * Sets the trigger delay attribute on a volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param delay         The trigger delay, in seconds.
 */
stock VolSetDelay(volumeIndex, Float:delay)
{
    Volumes[volumeIndex][vol_trigger_delay] = delay;
}

/**
 * Parses a effect string value and applies it to the specified volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param effect        A effect string value. Valid options: see VolumeEffects
 *                      enumeration, exclude prefix.
 * @return              True if successfully set, false otherwise.
 */
stock bool:VolSetEffectString(volumeIndex, const String:effect[])
{
    // Check if string value is empty.
    if (strlen(effect) == 0)
    {
        return false;
    }
    
    // Check effect string values and apply them to the volume.
    if (strcmp(effect, "none", false))
    {
        Volumes[volumeIndex][vol_effect] = VolEffect_None;
        return true;
    }
    else if (strcmp(effect, "wireframe", false))
    {
        Volumes[volumeIndex][vol_effect] = VolEffect_Wireframe;
        return true;
    }
    else if (strcmp(effect, "smoke", false))
    {
        Volumes[volumeIndex][vol_effect] = VolEffect_Smoke;
        return true;
    }
    
    // The string value didn't match any valid effects.
    return false;
}

/**
 * Sets the effect attribute on a volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param effect        Specifies what effect to apply on the volume.
 */
stock VolSetEffect(volumeIndex, VolumeEffects:effect)
{
    Volumes[volumeIndex][vol_effect] = effect;
}

/**
 * Parses a effect color string value and applies it to the specified volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param effect_color  A effect color string value. No whitespace! Format
 *                      (0-255): "red,green,blue".
 * @return              True if successfully set, false otherwise.
 */
stock bool:VolSetEffectColorString(volumeIndex, const String:effect_color[])
{
    new String:colors[3][3];
    new red, green, blue;
    
    // Check if string value is empty.
    if (strlen(effect_color) == 0)
    {
        return false;
    }
    
    // Split values into a string array.
    ExplodeString(effect_color, ",", colors, 3, 3);
    
    // Convert values.
    red = StringToInt(colors[0]);
    green = StringToInt(colors[1]);
    blue = StringToInt(colors[2]);
    
    // Apply values.
    Volumes[volumeIndex][vol_effect_color][0] = red;
    Volumes[volumeIndex][vol_effect_color][1] = green;
    Volumes[volumeIndex][vol_effect_color][2] = blue;
    return true;
}

/**
 * Sets the effect color attribute on a volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param red           Amount of red color. 0 to 255.
 * @param green         Amount of green color. 0 to 255.
 * @param blue          Amount of blue color. 0 to 255.
 */
stock VolSetEffectColor(volumeIndex, red, green, blue)
{
    Volumes[volumeIndex][vol_effect_color][0] = red;
    Volumes[volumeIndex][vol_effect_color][1] = green;
    Volumes[volumeIndex][vol_effect_color][2] = blue;
}

/**
 * Parses a enabled string value and applies it to the specified volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param enabled       A enabled string value. Valid options: A number that
 *                      evaluates to true or false, "yes" or "no".
 * @return              True if successfully set, false otherwise.
 */
stock bool:VolSetEnabledString(volumeIndex, const String:enabled[])
{
    // Check if string value is empty.
    if (strlen(enabled) == 0)
    {
        return false;
    }
    
    new bool:val = bool:StringToInt(enabled);
    
    // Check yes or no values first.
    if (strcmp(enabled, "yes", false) == 0)
    {
        val = true;
    }
    else if (strcmp(enabled, "no", false) == 0)
    {
        val = false;
    }
    
    // Check if the new value is different from the current.
    if (Volumes[volumeIndex][vol_enabled] != val)
    {
        // Forward event.
        if (val)
        {
            VolOnVolumeEnabled(volumeIndex);
        }
        else
        {
            VolOnVolumeDisabled(volumeIndex);
        }
    }
    
    // Apply converted value.
    Volumes[volumeIndex][vol_enabled] = val;
    return true;
}

/**
 * Sets the enabled attribute on a volume.
 *
 * @param volumeIndex   The volume index to apply to.
 * @param enabled       Specifies if the volume should be enabled or not.
 */
stock VolSetEnabled(volumeIndex, bool:enabled)
{
    // Check if the new value is different from the current.
    if (Volumes[volumeIndex][vol_enabled] != enabled)
    {
        // Forward event.
        if (enabled)
        {
            VolOnVolumeEnabled(volumeIndex);
        }
        else
        {
            VolOnVolumeDisabled(volumeIndex);
        }
    }

    Volumes[volumeIndex][vol_enabled] = enabled;
}
