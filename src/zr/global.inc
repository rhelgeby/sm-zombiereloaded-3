/**
 * ====================
 *   Zombie:Reloaded
 *   File: global.inc
 *   Author: Greyscale
 * ==================== 
 */

new Handle:hZombify = INVALID_HANDLE;
new Handle:hOnZClassChanged = INVALID_HANDLE;

CreateGlobals()
{
    CreateNative("ZR_IsClientZombie", Native_IsClientZombie);
    CreateNative("ZR_GetClientZClass", Native_GetClientZClass);
    CreateNative("ZR_HasZombieSpawned", Native_HasZombieSpawned);
    
    hZombify = CreateGlobalForward("ZR_Zombify", ET_Ignore, Param_Cell, Param_Cell);
    hOnZClassChanged = CreateGlobalForward("ZR_OnZClassChanged", ET_Ignore, Param_Cell, Param_Cell, Param_Cell);
}

public Native_IsClientZombie(Handle:plugin,  argc)
{
    return gZombie[GetNativeCell(1)];
}

public Native_GetClientZClass(Handle:plugin, argc)
{
    new class = GetNativeCell(1);
    return pClass[class];
}

public Native_HasZombieSpawned(Handle:plugin, argc)
{
    return zombieSpawned;
}