/**
 * ====================
 *   Zombie:Reloaded
 *   File: classes.inc
 *   Author: Greyscale
 * ==================== 
 */

enum ZR_ClassOptions
{
    String:data_name[64],
    String:data_model[256],
    String:data_menu_description[256],
    String:data_zvision[256],
    data_health,
    Float:data_speed,
    Float:data_jump_distance,
    Float:data_jump_height,
    Float:data_knockback,
    data_nvgs,
    data_fov,
    bool:data_regen,
    data_regen_health,
    Float:data_regen_interval,
    bool:data_napalm,
    Float:data_napalm_time,
    bool:data_nofalldamage,
    data_kill_bonus,
    data_infect_health,
    data_alpha_spawn,
    data_alpha_damaged,
    data_alpha_damage
}

#define MAXCLASSES 20

new Handle:kvClasses = INVALID_HANDLE;

new arrayClasses[MAXCLASSES][ZR_ClassOptions];
new classCount;

LoadClassData()
{
    if (kvClasses != INVALID_HANDLE)
    {
        CloseHandle(kvClasses);
    }
    
    kvClasses = CreateKeyValues("classes");
    
    decl String:path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), "configs/zr/classes.txt");
    
    if (!FileToKeyValues(kvClasses, path))
    {
        SetFailState("\"%s\" missing from server", path);
    }
    
    KvRewind(kvClasses);
    if (!KvGotoFirstSubKey(kvClasses))
    {
        new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
        if (classes)
        {
            SetConVarBool(gCvars[CVAR_CLASSES], false);
            
            ZR_LogMessage("Class auto-disable", path);
        }
    }
    
    decl String:name[64];
    decl String:model[256];
    decl String:menu_description[256];
    decl String:zvision[256];
    
    classCount = 0;
    
    do
    {
        KvGetString(kvClasses, "name", name, sizeof(name));
        strcopy(arrayClasses[classCount][data_name], 64, name);
        
        KvGetString(kvClasses, "model", model, sizeof(model));
        strcopy(arrayClasses[classCount][data_model], 256, model);
        
        KvGetString(kvClasses, "menu_description", menu_description, sizeof(menu_description));
        strcopy(arrayClasses[classCount][data_menu_description], 256, menu_description);
        
        decl String:cvar_zvision[256];
        GetConVarString(gCvars[CVAR_ZOMBIE_ZVISION], cvar_zvision, sizeof(cvar_zvision));
    
        KvGetString(kvClasses, "zvision", zvision, sizeof(zvision), cvar_zvision);
        strcopy(arrayClasses[classCount][data_zvision], 256, zvision);
        
        arrayClasses[classCount][data_health] = KvGetNum(kvClasses, "health"), GetConVarInt(gCvars[CVAR_ZOMBIE_HEALTH]);
        arrayClasses[classCount][data_speed] = KvGetFloat(kvClasses, "speed"), GetConVarFloat(gCvars[CVAR_ZOMBIE_SPEED]);
        arrayClasses[classCount][data_jump_distance] = KvGetFloat(kvClasses, "jump_distance"), GetConVarFloat(gCvars[CVAR_ZOMBIE_JUMP_DISTANCE]);
        arrayClasses[classCount][data_jump_height] = KvGetFloat(kvClasses, "jump_height"), GetConVarFloat(gCvars[CVAR_ZOMBIE_JUMP_HEIGHT]);
        arrayClasses[classCount][data_knockback] = KvGetFloat(kvClasses, "knockback"), GetConVarFloat(gCvars[CVAR_ZOMBIE_KNOCKBACK]);
        arrayClasses[classCount][data_nvgs] = KvGetNum(kvClasses, "nvgs"), GetConVarInt(gCvars[CVAR_ZOMBIE_NVGS]);
        arrayClasses[classCount][data_fov] = KvGetNum(kvClasses, "fov"), GetConVarInt(gCvars[CVAR_ZOMBIE_FOV]);
        arrayClasses[classCount][data_regen] = bool:KvGetNum(kvClasses, "regen"), GetConVarBool(gCvars[CVAR_ZOMBIE_REGEN]);
        arrayClasses[classCount][data_regen_health] = KvGetNum(kvClasses, "regen_health"), GetConVarInt(gCvars[CVAR_ZOMBIE_REGEN_HEALTH]);
        arrayClasses[classCount][data_regen_interval] = KvGetFloat(kvClasses, "regen_interval"), GetConVarFloat(gCvars[CVAR_ZOMBIE_REGEN_INTERVAL]);
        arrayClasses[classCount][data_napalm] = bool:KvGetNum(kvClasses, "napalm"), GetConVarBool(gCvars[CVAR_ZOMBIE_NAPALM]);
        arrayClasses[classCount][data_napalm_time] = KvGetFloat(kvClasses, "napalm_time"), GetConVarFloat(gCvars[CVAR_ZOMBIE_NAPALM_TIME]);
        arrayClasses[classCount][data_nofalldamage] = bool:KvGetNum(kvClasses, "nofalldamage"), GetConVarBool(gCvars[CVAR_ZOMBIE_NOFALLDAMAGE]);
        arrayClasses[classCount][data_kill_bonus] = KvGetNum(kvClasses, "kill_bonus"), GetConVarInt(gCvars[CVAR_ZOMBIE_KILL_BONUS]);
        arrayClasses[classCount][data_infect_health] = KvGetNum(kvClasses, "infect_health"), GetConVarInt(gCvars[CVAR_ZOMBIE_INFECT_HEALTH]);
        arrayClasses[classCount][data_alpha_spawn] = KvGetNum(kvClasses, "alpha_spawn");
        arrayClasses[classCount][data_alpha_damaged] = KvGetNum(kvClasses, "alpha_damaged");
        arrayClasses[classCount][data_alpha_damage] = KvGetNum(kvClasses, "alpha_damage");

        classCount++;
    } while (KvGotoNextKey(kvClasses));
}

GetClassName(classindex, String:name[], maxlen)
{
    strcopy(name, maxlen, arrayClasses[classindex][data_name]);
}

GetClassIndex(String:name[])
{
    new i;
    decl String:current_name[64];

    for (i = 0; i < classCount; i++)
    {
        GetClassName(i, current_name, sizeof(current_name));
        if (strcmp(name, current_name, false) == 0)
        {
            return i;
        }
    }

    return -1;
}

GetDefaultClassIndex()
{
    new String:classname[64];
    new classindex;
    GetConVarString(gCvars[CVAR_CLASSES_DEFAULT], classname, sizeof(classname));
    
    if (strlen(classname) > 0)
    {
        if (classCount > 1 && (strcmp(classname, "random", false) == 0))
        {
            classindex = GetRandomInt(0, classCount - 1);
            return classindex;
        }
        else
        {
            classindex = GetClassIndex(classname);
            if (classindex == -1)
            {
                return 0;
            }
            else
            {
                return classindex;
            }
        }
    }
    else
    {
        return 0;
    }
}

/*GetClassModel(classindex, String:model[], maxlen)
{
    strcopy(model, maxlen, arrayClasses[classindex][data_model]);
}*/

GetClassMenuDescription(classindex, String:menudescription[], maxlen)
{
    strcopy(menudescription, maxlen, arrayClasses[classindex][data_menu_description]);
}

/*GetClassZVision(classindex, String:zvision[], maxlen)
{
    strcopy(zvision, maxlen, arrayClasses[classindex][data_zvision]);
}*/

/*GetClassHealth(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_health];
    }
    
    return GetConVarInt(gCvars[CVAR_ZOMBIE_HEALTH]);
}*/

/*Float:GetClassSpeed(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_speed];
    }
    
    return GetConVarFloat(gCvars[CVAR_ZOMBIE_SPEED]);
}*/

/*Float:GetClassJumpDistance(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_jump_distance];
    }
    
    return GetConVarFloat(gCvars[CVAR_ZOMBIE_JUMP_DISTANCE]);
}*/

/*Float:GetClassJumpHeight(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_jump_height];
    }
    
    return GetConVarFloat(gCvars[CVAR_ZOMBIE_JUMP_HEIGHT]);
}*/

/*Float:GetClassKnockback(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_knockback] * GetConVarFloat(gCvars[CVAR_ZOMBIE_KNOCKBACK]);
    }
    
    return GetConVarFloat(gCvars[CVAR_ZOMBIE_KNOCKBACK]);
}*/

/*bool:GetClassNVGs(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);

    if (classes)
    {
        new nvgs_override = GetConVarInt(gCvars[CVAR_ZOMBIE_NVGS]);
        if (nvgs_override > -1)
        {
            return IntToBool(nvgs_override);
        }
        else
        {
            return IntToBool(arrayClasses[classindex][data_nvgs]);
        }
    }
    
    return IntToBool(GetConVarInt(gCvars[CVAR_ZOMBIE_NVGS]));
}*/

/*GetClassFOV(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_fov];
    }
    
    return GetConVarInt(gCvars[CVAR_ZOMBIE_FOV]);
}*/

/*bool:GetClassRegen(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_regen];
    }
    
    return GetConVarBool(gCvars[CVAR_ZOMBIE_REGEN]);
}*/

/*GetClassRegenHealth(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_regen_health];
    }
    
    return GetConVarInt(gCvars[CVAR_ZOMBIE_REGEN_HEALTH]);
}*/

/*Float:GetClassRegenInterval(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_regen_interval];
    }
    
    return GetConVarFloat(gCvars[CVAR_ZOMBIE_REGEN_INTERVAL]);
}*/

/*bool:GetClassNapalm(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_napalm];
    }
    
    return GetConVarBool(gCvars[CVAR_ZOMBIE_NAPALM]);
}*/

/*Float:GetClassNapalmTime(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_napalm_time];
    }
    
    return GetConVarFloat(gCvars[CVAR_ZOMBIE_NAPALM_TIME]);
}*/

/*bool:GetClassNoFallDamage(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_nofalldamage];
    }
    
    return GetConVarBool(gCvars[CVAR_ZOMBIE_NOFALLDAMAGE]);
}*/

/*GetClassKillBonus(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_kill_bonus];
    }
    
    return GetConVarInt(gCvars[CVAR_ZOMBIE_KILL_BONUS]);
}*/

/*GetClassInfectHealth(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_infect_health];
    }
    
    return GetConVarInt(gCvars[CVAR_ZOMBIE_INFECT_HEALTH]);
}*/

/*GetClassAlphaSpawn(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_alpha_spawn];
    }

    return GetConVarInt(gCvars[CVAR_ZOMBIE_ALPHA_SPAWN]);
}*/

/*GetClassAlphaDamaged(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_alpha_damaged];
    }

    return GetConVarInt(gCvars[CVAR_ZOMBIE_ALPHA_DAMAGED]);
}*/

/*GetClassAlphaDamage(classindex)
{
    new bool:classes = GetConVarBool(gCvars[CVAR_CLASSES]);
    if (classes)
    {
        return arrayClasses[classindex][data_alpha_damage];
    }

    return GetConVarInt(gCvars[CVAR_ZOMBIE_ALPHA_DAMAGE]);
}*/
