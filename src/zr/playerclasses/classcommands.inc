/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:        classcommands.inc
 *  Type:        Core 
 *  Description: Console commands for working with classes.
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

ClassOnCommandsCreate()
{
    // Create base class commands.
    RegConsoleCmd("zr_class_dump", ClassDumpCommand, "Dumps class data at a specified index in the specified cache. Usage: zr_class_dump <cachetype> <index|targetname>");
    RegAdminCmd("zr_class_modify", ClassModifyCommand, ADMFLAG_GENERIC, "Modify class data on one or more classes. Usage: zr_class_modify <classname|\"zombies\"|\"humans\"|\"admins\"> <attribute> <value> [is_multiplier]");
}

/**
 * Hook commands related to classes here.
 */
ClassOnCommandsHook()
{
    // Forward event to sub-modules.
    ClassOverlayOnCommandsHook();
}

/**
 * Command callback. (zr_class_dump)
 * Dumps class data at a specified index in the specified cache.
 *   
 * @param client    The client index.
 * @param argc      Argument count.
 */
public Action:ClassDumpCommand(client, argc)
{
    decl String:syntax[1024];
    syntax[0] = 0;
    
    if (argc < 2)
    {
        // Write syntax info.
        StrCat(syntax, sizeof(syntax), "Dumps class data at a specified index in the specified cache. Usage: zr_class_dump <cachetype> <index|targetname>\n\n");
        StrCat(syntax, sizeof(syntax), "Cache types:\n");
        StrCat(syntax, sizeof(syntax), "original - Unmodified class data\n");
        StrCat(syntax, sizeof(syntax), "modified - Newest class data\n");
        StrCat(syntax, sizeof(syntax), "player   - Players class data\n");
        ReplyToCommand(client, syntax);
        
        return Plugin_Handled;
    }
    
    new cachetype = -1;
    new index = -1;
    
    decl String:type[64];
    decl String:target[64];
    decl String:buffer[2048];
    
    // Quick initialize buffer.
    buffer[0] = 0;
    
    // Get cache type.
    GetCmdArg(1, type, sizeof(type));
    
    // Set cache type depending on parameter setting.
    if (StrEqual(type, "original", false))
    {
        cachetype = ZR_CLASS_CACHE_ORIGINAL;
    }
    else if (StrEqual(type, "modified", false))
    {
        cachetype = ZR_CLASS_CACHE_MODIFIED;
    }
    else if (StrEqual(type, "player", false))
    {
        cachetype = ZR_CLASS_CACHE_PLAYER;
        
        // Get client index.
        GetCmdArg(2, target, sizeof(target));
        index = FindTarget(client, target, _, false);
        
        // Check if failed.
        if (index < 0)
        {
            ReplyToCommand(client, "Invalid target name.");
            return Plugin_Handled;
        }
    }
    
    // Check if cachetype is valid.
    if (cachetype < 0)
    {
        ReplyToCommand(client, "Invalid cache type.");
        return Plugin_Handled;
    }
    
    // Validate class index.
    if (cachetype != ZR_CLASS_CACHE_PLAYER)
    {
        // Get class index.
        GetCmdArg(2, target, sizeof(target));
        index = StringToInt(target);
        
        if (!ClassValidateIndex(index))
        {
            ReplyToCommand(client, "Invalid class index.");
            return Plugin_Handled;
        }
    }
    
    // Dump the specified cache.
    ReplyToCommand(client, "DUMPING CACHE: \"%s\" (%d classes total)\n========================================\n", type, ClassCount);
    ClassDumpData(index, cachetype, buffer, sizeof(buffer));
    
    // Print all data to client.
    decl String:partbuffer[1024];
    new pos;
    new cellswritten = 1;   // Initialize for the loop.

    while (cellswritten)
    {
        cellswritten = strcopy(partbuffer, sizeof(partbuffer), buffer[pos]);
        ReplyToCommand(client, partbuffer);
        pos += cellswritten;
    }
    
    return Plugin_Handled;
}

/**
 * Modifies class data on one or more classes.
 *
 * Syntax: zr_class_modify <class> <attribute> <value> [is_multiplier]
 *
 * class:           The class to modify. Can be any class name, or one of the
 *                  following team names; "all", "humans", "zombies" or
 *                  "admins".
 * attribute:       The name of the class attribute.
 * value:           Value to set. Use quotes if value is a string.
 * is_multiplier:   Optional. specifies wether the original value should be
 *                  multiplied by the specified value. Defaults to false.
 *
 * Note: Original values are retrieved from the original class cache, not the
 *       modified class cache.
 */
public Action:ClassModifyCommand(client, argc)
{
    decl String:syntax[1024];
    syntax[0] = 0;
    
    if (argc < 3)
    {
        // Write syntax info.
        StrCat(syntax, sizeof(syntax), "Modifies class data on one or more classes. Usage: zr_class_modify <class> <attribute> <value> [is_multiplier]\n\n");
        StrCat(syntax, sizeof(syntax), "class:           The class to modify. Can be any class name, or one of the following team names; all, humans, zombies or admins.\n");
        StrCat(syntax, sizeof(syntax), "attribute:       The name of the class attribute.\n");
        StrCat(syntax, sizeof(syntax), "value:           Value to set. Use quotes if value is a string.\n");
        StrCat(syntax, sizeof(syntax), "is_multiplier:   Optional. specifies wether the original value should be multiplied by the specified value. Not all attributes support multiplying. Defaults to false.\n\n");
        StrCat(syntax, sizeof(syntax), "Note: Original values are retrieved from the original class cache, not the modified class cache.");
        ReplyToCommand(client, syntax);
        
        return Plugin_Handled;
    }
    
    decl String:classname[64];
    decl String:attributename[128];
    decl String:value[256];
    decl String:ismultiplier[4];
    
    new attributeflag;
    new ClassDataTypes:attributetype;
    new bool:isgroup;
    new bool:hasmultiplier;
    new Handle:classlist;
    
    new classindex;
    new bool:listresult;
    classlist = CreateArray();
    
    // Get command arguments.
    GetCmdArg(1, classname, sizeof(classname));
    GetCmdArg(2, attributename, sizeof(attributename));
    GetCmdArg(3, value, sizeof(value));
    
    // Get last command argument if specified.
    if (argc == 4)
    {
        GetCmdArg(4, ismultiplier, sizeof(ismultiplier));
        if (StringToInt(ismultiplier))
        {
            hasmultiplier = true;
        }
    }
    
    // Get attribute flag.
    attributeflag = ClassAttributeNameToFlag(attributename);
    
    // Validate attribute flag.
    if (attributeflag < 0)
    {
        ReplyToCommand(client, "Invalid class attribute specified.");
        return Plugin_Handled;
    }
    
    // Get attribute data type.
    attributetype = ClassGetAttributeType(attributeflag);
    
    // Check if classname is a group. Add classes to the class list
    // and use the specified team filter.
    if (StrEqual(classname, "all", false))
    {
        listresult = ClassAddToArray(classlist);
        isgroup = true;
    }
    else if (StrEqual(classname, "humans", false))
    {
        listresult = ClassAddToArray(classlist, ZR_CLASS_TEAM_HUMANS);
        isgroup = true;
    }
    else if (StrEqual(classname, "zombies", false))
    {
        listresult = ClassAddToArray(classlist, ZR_CLASS_TEAM_ZOMBIES);
        isgroup = true;
    }
    else if (StrEqual(classname, "admins", false))
    {
        listresult = ClassAddToArray(classlist, ZR_CLASS_TEAM_ADMINS);
        isgroup = true;
    }
    
    // Check if classname is a group.
    if (isgroup)
    {
        // Check if the list is valid.
        if (!listresult)
        {
            ReplyToCommand(client, "Failed to get classes in the specified team: \"%s\".", classname);
            return Plugin_Handled;
        }
        
        // Loop through all classes in the list.
        new listsize = GetArraySize(classlist);
        
        for (new i = 0; i < listsize; i++)
        {
            classindex = GetArrayCell(classlist, i);
            
            switch (attributetype)
            {
                case ClassDataType_Boolean:
                {
                    if (!ClassModifyBoolean(classindex, attributeflag, bool:StringToInt(value)))
                    {
                        ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                    }
                }
                case ClassDataType_Integer:
                {
                    if (hasmultiplier)
                    {
                        if (!ClassModifyInteger(classindex, attributeflag, StringToInt(value), StringToFloat(value)))
                        {
                            ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                        }
                    }
                    else
                    {
                        if (!ClassModifyInteger(classindex, attributeflag, StringToInt(value)))
                        {
                            ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                        }
                    }
                }
                case ClassDataType_Float:
                {
                    if (!ClassModifyFloat(classindex, attributeflag, StringToFloat(value), hasmultiplier))
                    {
                        ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                    }
                }
                case ClassDataType_String:
                {
                    if (!ClassModifyString(classindex, attributeflag, value))
                    {
                        ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                    }
                }
            }
        }
    }
    else
    {
        // It's a single class.
        classindex = ClassGetIndex(classname);
        
        // Validate classindex.
        if (!ClassValidateIndex(classindex))
        {
            ReplyToCommand(client, "Invalid class name specified.");
            return Plugin_Handled;
        }
        
        switch (attributetype)
        {
            case ClassDataType_Boolean:
            {
                if (!ClassModifyBoolean(classindex, attributeflag, bool:StringToInt(value)))
                {
                    ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                }
            }
            case ClassDataType_Integer:
            {
                if (hasmultiplier)
                {
                    if (!ClassModifyInteger(classindex, attributeflag, StringToInt(value), StringToFloat(value)))
                    {
                        ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                    }
                }
                else
                {
                    if (!ClassModifyInteger(classindex, attributeflag, StringToInt(value)))
                    {
                        ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                    }
                }
            }
            case ClassDataType_Float:
            {
                if (!ClassModifyFloat(classindex, attributeflag, StringToFloat(value)), hasmultiplier)
                {
                    ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                }
            }
            case ClassDataType_String:
            {
                if (!ClassModifyString(classindex, attributeflag, value))
                {
                    ReplyToCommand(client, "Failed to set \"%s\" to \"%s\" in class \"%d\".", attributename, value, classindex);
                }
            }
        }
    }
    
    return Plugin_Handled;
}

/**
 * Modify class boolean attribute on a class.
 *
 * @param classindex        The class index.
 * @param attributeflag     Attribute to modify (a single attribute flag).
 * @param value             New value to set.
 * @return                  True on success, false otherwise.
 */
stock bool:ClassModifyBoolean(classindex, attributeflag, bool:value)
{
    // Validate class index.
    if (!ClassValidateIndex(classindex))
    {
        return false;
    }
    
    switch (attributeflag)
    {
        case ZR_CLASS_FLAG_ENABLED:
        {
            ClassDataCache[classindex][class_enabled] = bool:value;
            return true;
        }
        case ZR_CLASS_FLAG_NVGS:
        {
            ClassDataCache[classindex][class_nvgs] = bool:value;
            return true;
        }
        case ZR_CLASS_FLAG_NO_FALL_DAMAGE:
        {
            ClassDataCache[classindex][class_no_fall_damage] = bool:value;
            return true;
        }
    }
    
    // Invalid flag or multiple flags combined.
    return false;
}

/**
 * Modify class integer attribute on a class.
 *
 * @param classindex        The class index.
 * @param attributeflag     Attribute to modify (a single attribute flag).
 * @param value             New value to set, or multiply with.
 * @param multiplier        Optional. Use a multiplier instead of the value,
 *                          that multiplies with the original class value.
 *                          Not all attributes support multipliers. 0.0 to
 *                          disable. Value is ignored if this is non-zero.
 * @return                  True on success, false otherwise.
 */
stock ClassModifyInteger(classindex, attributeflag, value, Float:multiplier = 0.0)
{
    // Validate class index.
    if (!ClassValidateIndex(classindex))
    {
        return false;
    }
    
    // Check if multiplier is specified.
    new bool:ismultiplier = (multiplier != 0.0) ? true : false;
    
    switch (attributeflag)
    {
        case ZR_CLASS_FLAG_ALPHA_INITIAL:
        {
            if (ismultiplier)
            {
                value = RoundToNearest(float(ClassData[classindex][class_alpha_initial]) * multiplier);
            }
            ClassDataCache[classindex][class_alpha_initial] = value;
            return true;
        }
        case ZR_CLASS_FLAG_ALPHA_DAMAGED:
        {
            if (ismultiplier)
            {
                value = RoundToNearest(float(ClassData[classindex][class_alpha_damaged]) * multiplier);
            }
            ClassDataCache[classindex][class_alpha_damaged] = value;
            return true;
        }
        case ZR_CLASS_FLAG_ALPHA_DAMAGE:
        {
            if (ismultiplier)
            {
                value = RoundToNearest(float(ClassData[classindex][class_alpha_damage]) * multiplier);
            }
            ClassDataCache[classindex][class_alpha_damage] = value;
            return true;
        }
        case ZR_CLASS_FLAG_FOV:
        {
            ClassDataCache[classindex][class_fov] = value;
            return true;
        }
        case ZR_CLASS_FLAG_IMMUNITY_MODE:
        {
            ClassDataCache[classindex][class_fov] = value;
            return true;
        }
        case ZR_CLASS_FLAG_HEALTH:
        {
            if (ismultiplier)
            {
                value = RoundToNearest(float(ClassData[classindex][class_health]) * multiplier);
            }
            ClassDataCache[classindex][class_health] = value;
            return true;
        }
        case ZR_CLASS_FLAG_HEALTH_REGEN_AMOUNT:
        {
            if (ismultiplier)
            {
                value = RoundToNearest(float(ClassData[classindex][class_health_regen_amount]) * multiplier);
            }
            ClassDataCache[classindex][class_health_regen_amount] = value;
            return true;
        }
        case ZR_CLASS_FLAG_HEALTH_INFECT_GAIN:
        {
            if (ismultiplier)
            {
                value = RoundToNearest(float(ClassData[classindex][class_health_infect_gain]) * multiplier);
            }
            ClassDataCache[classindex][class_health_infect_gain] = value;
            return true;
        }
        case ZR_CLASS_FLAG_KILL_BONUS:
        {
            if (ismultiplier)
            {
                value = RoundToNearest(float(ClassData[classindex][class_kill_bonus]) * multiplier);
            }
            ClassDataCache[classindex][class_kill_bonus] = value;
            return true;
        }
    }
    
    // Invalid flag or multiple flags combined.
    return false;
}

/**
 * Modify class float attribute on a class.
 *
 * @param classindex        The class index.
 * @param attributeflag     Attribute to modify (a single attribute flag).
 * @param value             New value to set, or multiply with.
 * @param ismultiplier      Optional. Specifies wether to value as a multiplier
 *                          that multiplies with the original class value.
 *                          Not all attributes support multipliers.
 * @return                  True on success, false otherwise.
 */
stock ClassModifyFloat(classindex, attributeflag, Float:value, bool:ismultiplier = false)
{
    // Validate class index.
    if (!ClassValidateIndex(classindex))
    {
        return false;
    }
    
    switch (attributeflag)
    {
        case ZR_CLASS_FLAG_NAPALM_TIME:
        {
            if (ismultiplier)
            {
                value = ClassData[classindex][class_napalm_time] * value;
            }
            ClassDataCache[classindex][class_napalm_time] = value;
            return true;
        }
        case ZR_CLASS_FLAG_IMMUNITY_AMOUNT:
        {
            if (ismultiplier)
            {
                value = ClassData[classindex][class_immunity_amount] * value;
            }
            ClassDataCache[classindex][class_immunity_amount] = value;
            return true;
        }
        case ZR_CLASS_FLAG_HEALTH_REGEN_INTERVAL:
        {
            if (ismultiplier)
            {
                value = ClassData[classindex][class_health_regen_interval] * value;
            }
            ClassDataCache[classindex][class_health_regen_interval] = value;
            return true;
        }
        case ZR_CLASS_FLAG_SPEED:
        {
            if (ismultiplier)
            {
                value = ClassData[classindex][class_speed] * value;
            }
            ClassDataCache[classindex][class_speed] = value;
            return true;
        }
        case ZR_CLASS_FLAG_KNOCKBACK:
        {
            if (ismultiplier)
            {
                value = ClassData[classindex][class_knockback] * value;
            }
            ClassDataCache[classindex][class_knockback] = value;
            return true;
        }
        case ZR_CLASS_FLAG_JUMP_HEIGHT:
        {
            if (ismultiplier)
            {
                value = ClassData[classindex][class_jump_height] * value;
            }
            ClassDataCache[classindex][class_jump_height] = value;
            return true;
        }
        case ZR_CLASS_FLAG_JUMP_DISTANCE:
        {
            if (ismultiplier)
            {
                value = ClassData[classindex][class_jump_distance] * value;
            }
            ClassDataCache[classindex][class_jump_distance] = value;
            return true;
        }
    }
    
    // Invalid flag or multiple flags combined.
    return false;
}

/**
 * Modify class string attribute on a class.
 *
 * @param classindex        The class index.
 * @param attributeflag     Attribute to modify (a single attribute flag).
 * @param value             New value to set.
 * @return                  True on success, false otherwise.
 */
stock ClassModifyString(classindex, attributeflag, const String:value[])
{
    // Validate class index.
    if (!ClassValidateIndex(classindex))
    {
        return false;
    }
    
    switch (attributeflag)
    {
        case ZR_CLASS_FLAG_NAME:
        {
            strcopy(ClassDataCache[classindex][class_name], 64, value);
            return true;
        }
        case ZR_CLASS_FLAG_DESCRIPTION:
        {
            strcopy(ClassDataCache[classindex][class_description], 256, value);
            return true;
        }
        case ZR_CLASS_FLAG_MODEL_PATH:
        {
            strcopy(ClassDataCache[classindex][class_model_path], PLATFORM_MAX_PATH, value);
            return true;
        }
        case ZR_CLASS_FLAG_OVERLAY_PATH:
        {
            strcopy(ClassDataCache[classindex][class_overlay_path], PLATFORM_MAX_PATH, value);
            return true;
        }
    }
    
    // Invalid flag or multiple flags combined.
    return false;
}
