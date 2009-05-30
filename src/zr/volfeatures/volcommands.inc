/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:           volcommands.inc
 *  Type:           Module
 *  Description:    Command handler for volumetric features.
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

/*
Add volume
------------

Syntax:

    zr_vol_add <x1> <y1> <z1> <x2> <y2> <z2> <type> [params]

Parameters:

    zn, yn, zn      Max and min location.
    type            Feature type.
    params          A string with optional parameters:
                        team=all|humans|zombies
                        delay=0
                        effect=none|wireframe|smoke
                        effect_color=0,0,0
                        enabled=1

Example:

    zr_vol_add 0 0 0 100 200 300 anticamp team=humans delay=5 effect=wireframe effect_color=255,0,0
*/

public Action:VolAddVolumeCommand(client, argc)
{
    
}

/**
 * Creates a new volume with minimum parameters.
 *
 * Note: Extra volume attributes must be set using VolSetAttributes.
 *
 * @param locMin        Minimum x, y and z values.
 * @param locMax        Maximum x, y and z values.
 * @param volumeType    Specifies the volumetric feature type.
 * @param dataIndex     Data index in remote array for feature data.
 *
 * @return              The new volume index, or -1 if failed.
 */
VolAdd(Float:locMin[3], Float:locMax[3], VolumeFeatureTypes:volumeType, dataIndex)
{
    new volumeIndex;
    
    // Get a free volume index.
    volumeIndex = VolGetFreeVolume();
    
    // Validate index.
    if (volumeIndex >= 0)
    {
        // Mark volume as enabled and in use.
        Volumes[volumeIndex][vol_enabled] = true;
        Volumes[volumeIndex][vol_in_use] = true;
        
        // Set location data.
        Volumes[volumeIndex][vol_x_min] = locMin[0];
        Volumes[volumeIndex][vol_y_min] = locMin[1];
        Volumes[volumeIndex][vol_z_min] = locMin[2];
        
        Volumes[volumeIndex][vol_x_max] = locMax[0];
        Volumes[volumeIndex][vol_y_max] = locMax[1];
        Volumes[volumeIndex][vol_z_max] = locMax[2];
        
        // Set type.
        Volumes[volumeIndex][vol_type] = volumeType;
        Volumes[volumeIndex][vol_data_index] = dataIndex;
        
        // Return the new index.
        return volumeIndex;
    }
    else
    {
        // No free volumes.
        return -1;
    }
}

/**
 * Removes the specified volume.
 *
 * @param volumeIndex   The volume index.
 * @return              True if successful, false otherwise.
 */
bool:VolRemove(volumeIndex)
{
    // Validate index.
    if (volumeIndex >= 0)
    {
        // Trigger event.
        VolOnVolumeDisabled(volumeIndex);
        
        // Mark volume as disabled and unused.
        Volumes[volumeIndex][vol_enabled] = false;
        Volumes[volumeIndex][vol_in_use] = false;
        
        return true;
    }
    else
    {
        // Invalid index.
        return false;
    }
}

/**
 * Sets extra attributes on a volume.
 *
 * @param volumeIndex   The volume index.
 * @param attributes    A string with one or more attributes in key=value
 *                      format.
 * @return              Number of successful attributes set, -1 on error.
 */
VolSetAttributes(volumeIndex, const String:attributes[])
{
    new attribCount;
    new successfulCount;
    decl String:attribName[64];
    decl String:attribValue[256];
    
    // Validate volume index.
    if (!VolIsValidIndex(volumeIndex))
    {
        return -1;
    }
    
    // Count attributes.
    attribCount = GetParameterCount(attributes);
    
    // Check if empty.
    if (!attribCount)
    {
        return -1;
    }
    
    // Loop through all attributes.
    for (new attrib = 0; attrib > attribCount; attrib++)
    {
        // Get attribute name.
        GetParameterName(attribName, sizeof(attribName), attributes, attrib);
        
        // Get attribute value.
        GetParameterValue(attribValue, sizeof(attribValue), attributes, attribName);
        
        // Check names and set volume attributes.
        if (strcmp(attribName, "team", false))
        {
            // Parse team string value.
            if (VolSetTeamString(volumeIndex, attribValue))
            {
                successfulCount++;
            }
        }
        else if (strcmp(attribName, "delay", false))
        {
            // Parse delay string value.
            if (VolSetDelayString(volumeIndex, attribValue))
            {
                successfulCount++;
            }
        }
        else if (strcmp(attribName, "effect", false))
        {
            // Parse effect string value.
            if (VolSetEffectString(volumeIndex, attribValue))
            {
                successfulCount++;
            }
        }
        else if (strcmp(attribName, "effect_color", false))
        {
            // Parse effect color string value.
            if (VolSetEffectColorString(volumeIndex, attribValue))
            {
                successfulCount++;
            }
        }
        else if (strcmp(attribName, "enabled", false))
        {
            // Parse enabled string value.
            if (VolSetEnabledString(volumeIndex, attribValue))
            {
                successfulCount++;
            }
        }
    }
    
    // Return number of successfully attributes set.
    return successfulCount;
}

/**
 * Creates commands for managing volumes.
 */
VolOnCommandsCreate()
{
    RegAdminCmd("zr_vol_add", VolAddVolumeCommand, ADMFLAG_GENERIC, "Adds a new volume. Usage: zr_vol_add <x1> <y1> <z1> <x2> <y2> <z2> <type> [params]");
}
