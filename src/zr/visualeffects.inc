/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          visualeffects.inc
 *   Type:          Module
 *   Description:   Visual effects such as map darkening, fog, etc..
 *
 * ============================================================================
 */

/**
 * Default sky of current map.
 */
new String:g_VEffectsDefaultSky[PLATFORM_MAX_PATH];

/**
 * Get cvar data and downloadable content to add to download table.
 */
VEffectsLoad()
{
    // Apply all visual effects now
    VEffectsApplyAll();
    
    // Find map's default sky.
    new Handle:hSkyname = FindConVar("sv_skyname");
    if (hSkyname != INVALID_HANDLE)
    {
        // Store map's default sky before applying new one.
        GetConVarString(hSkyname, g_VEffectsDefaultSky, sizeof(g_VEffectsDefaultSky));
    }
    
    // If sky is disabled, then stop.
    new bool:sky = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SKY]);
    if (!sky)
    {
        return;
    }
    
    decl String:downloadpath[PLATFORM_MAX_PATH];
    decl String:skypath[PLATFORM_MAX_PATH];
    
    // Get sky path.
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], skypath, sizeof(skypath));
    
    // Prepend materials/skybox to the path.
    Format(downloadpath, sizeof(downloadpath), "materials/skybox/%s", skypath);
    
    // Add skybox file to downloads table.
    AddFileToDownloadsTable(downloadpath);
}

/**
 * Plugin has just finished creating/hooking cvars.
 */
VEffectsOnCvarInit()
{
    // Hook zr_veffects_* cvars.
    VEffectsCvarsHook();
}

/**
 * Hook zr_veffects_* cvar changes.
 * 
 * @param unhook    If true, cvars will be unhooked, false to hook cvars.
 */
VEffectsCvarsHook(bool:unhook = false)
{
    // If unhook is true, then continue.
    if (unhook)
    {
        // Unhook lightstyle cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE], VEffectsCvarsHookLightStyle);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE], VEffectsCvarsHookLightStyle);
        
        // Unhook sky cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY], VEffectsCvarsHookSky);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], VEffectsCvarsHookSky);
        
        // Unhook sun cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE], VEffectsCvarsHookSunDisable);
        
        // Unhook fog cvars.
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG], VEffectsCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_OVERRIDE], VEffectsCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR], VEffectsCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR], VEffectsCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_DENSITY], VEffectsCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST], VEffectsCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST], VEffectsCvarsHookFog);
        UnhookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_FARZ], VEffectsCvarsHookFog);
        
        // Stop after unhooking cvars.
        return;
    }
    
    // Hook lightstyle cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE], VEffectsCvarsHookLightStyle);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE], VEffectsCvarsHookLightStyle);
    
    // Hook sky cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY], VEffectsCvarsHookSky);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], VEffectsCvarsHookSky);
    
    // Hook sun cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE], VEffectsCvarsHookSunDisable);
    
    // Hook fog cvars.
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG], VEffectsCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_OVERRIDE], VEffectsCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR], VEffectsCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR], VEffectsCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_DENSITY], VEffectsCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST], VEffectsCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST], VEffectsCvarsHookFog);
    HookConVarChange(g_hCvarsList[CVAR_VEFFECTS_FOG_FARZ], VEffectsCvarsHookFog);
}

/**
 * Cvar hook callback (zr_veffects_lightstyle, zr_veffects_lightstyle_value)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VEffectsCvarsHookLightStyle(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If lightstyle is disabled, then disable.
    new bool:lightstyle = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE]);
    
    // Apply light style.
    VEffectsApplyLightStyle(!lightstyle);
}

/**
 * Cvar hook callback (zr_veffects_sky, zr_veffects_sky_path)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VEffectsCvarsHookSky(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If sky is disabled, then disable.
    new bool:sky = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SKY]);
    
    // Apply new sky.
    VEffectsApplySky(!sky);
}

/**
 * Cvar hook callback (zr_veffects_sun_disable)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VEffectsCvarsHookSunDisable(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If fog is disabled, then disable.
    new bool:sun = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SUN_DISABLE]);
    
    // Apply fog.
    VEffectsApplySunDisable(!sun);
}

/**
 * Cvar hook callback (zr_veffects_fog_*)
 * Updated server to cvar values.
 * 
 * @param convar    The cvar handle.
 * @param oldvalue  The value before change.
 * @param newvalue  The new value.
 */
public VEffectsCvarsHookFog(Handle:cvar, const String:oldvalue[], const String:newvalue[])
{
    // If fog is disabled, then disable.
    new bool:fogoverride = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_FOG_OVERRIDE]);
    
    // Apply fog.
    VEffectsApplyFog(fogoverride);
}

/**
 * Apply all cvar values on server.
 */
VEffectsApplyAll()
{
    // If lightstyle is disabled, then disable.
    new bool:lightstyle = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE]);
    
    // Apply light style.
    VEffectsApplyLightStyle(!lightstyle);
    
    // If sky is disabled, then disable.
    new bool:sky = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SKY]);
    
    // Apply new sky.
    VEffectsApplySky(!sky);
    
    // If fog is disabled, then disable.
    new bool:fogoverride = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_FOG_OVERRIDE]);
    
    // Apply fog.
    VEffectsApplyFog(fogoverride);
}

VEffectsApplyLightStyle(bool:disable = false)
{
    // If default, then set to normal light style.
    if (disable)
    {
        // Set light style.
        SetLightStyle(0, "n");
        
        return;
    }
    
    // Get light value.
    decl String:lightstylevalue[4];
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE_VALUE], lightstylevalue, sizeof(lightstylevalue));
    
    // Set light style.
    SetLightStyle(0, lightstylevalue);
}

VEffectsApplySky(bool:disable = false)
{
    // if we can't find the sv_skyname cvar, then stop.
    new Handle:hSkyname = FindConVar("sv_skyname");
    if (hSkyname == INVALID_HANDLE)
    {
        return;
    }
    
    // If default, then set to default sky.
    if (disable)
    {
        if (g_VEffectsDefaultSky[0])
        {
            SetConVarString(hSkyname, g_VEffectsDefaultSky, true);
        }
        
        return;
    }
    
    // Get sky path.
    decl String:skypath[PLATFORM_MAX_PATH];
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], skypath, sizeof(skypath));
    
    // Set new sky on all clients.
    SetConVarString(hSkyname, skypath, true);
}

VEffectsApplySunDisable(bool:disable = false)
{
    // Find sun entity.
    new sun = FindEntityByClassname(-1, "env_sun");
    
    // If sun is invalid, then stop.
    if (sun == -1)
    {
        return;
    }
    
    // If default, then re-enable sun rendering.
    if (disable)
    {
        // Turn on sun rendering.
        AcceptEntityInput(sun, "TurnOn");
        
        return;
    }
    
    // Turn off sun rendering.
    AcceptEntityInput(sun, "TurnOff");
}

VEffectsApplyFog(bool:override = false)
{
    // If fog is disabled, then stop.
    new bool:fog = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_FOG]);
    
    // DUE TO SOURCEMOD NOT SUPPORT FOG, DISABLE.
    fog = false;
    
    if (!fog)
    {
        return;
    }
    
    // Find current fog index
    new fogindex = FindEntityByClassname(-1, "env_fog_controller");
    
    // If override is enabled, then continue.
    if (override)
    {
        // If there is fog, then continue.
        if (fogindex != -1)
        {
            // Delete fog.
            RemoveEdict(fogindex);
        }
    }
    
    // If there is no fog on the map, create new fog.
    if (fogindex == -1)
    {
        // Create and spawn fog.
        fogindex = CreateEntityByName("env_fog_controller");
        DispatchSpawn(fogindex);
    }
    
    decl String:fogcolor[16];
    
    // Set primary fog color.
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_FOG_PCOLOR], fogcolor, sizeof(fogcolor));
    VEffectsSetFogColor(fogindex, fogcolor, true);
    
    // Set secondary fog color.
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_FOG_SCOLOR], fogcolor, sizeof(fogcolor));
    VEffectsSetFogColor(fogindex, fogcolor, false);
    
    // Set fog's density.
    new Float:fogdensity = GetConVarFloat(g_hCvarsList[CVAR_VEFFECTS_FOG_DENSITY]);
    VEffectsSetFogDensity(fogindex, fogdensity);
    
    // Set fog's start distance.
    new fogstart = GetConVarInt(g_hCvarsList[CVAR_VEFFECTS_FOG_STARTDIST]);
    VEffectsSetFogStartDist(fogindex, fogstart);
    
    // Set fog's end distance.
    new fogend = GetConVarInt(g_hCvarsList[CVAR_VEFFECTS_FOG_ENDDIST]);
    VEffectsSetFogEndDist(fogindex, fogend);
    
    // Set fog's far z distance.
    new fogfarz = GetConVarInt(g_hCvarsList[CVAR_VEFFECTS_FOG_FARZ]);
    VEffectsSetFogFarZ(fogindex, fogfarz);
}

/**
 * Set fog's primary or secondary color.
 * 
 * @param fogindex  Edict index of the fog to modify.
 * @param color     The rgb color of the fog. 
 * @param primary   (Optional) True to set primary, false otherwise.
 */
VEffectsSetFogColor(fogindex, const String:color[], bool:primary = true)
{
    // Set primary color.
    if (primary)
    {
        // Set new color.
        SetVariantString(color);
        AcceptEntityInput(fogindex, "SetColor");
    }
    // Set secondary color.
    else
    {
        // Set new color.
        SetVariantString(color);
        AcceptEntityInput(fogindex, "SetColorSecondary");
    }
}

/**
 * Set fog's density.
 * 
 * @param fogindex  Edict index of the fog to modify.
 * @param density   The density of the fog. 
 */
VEffectsSetFogDensity(fogindex, Float:density)
{
    // Set density.
    DispatchKeyValueFloat(fogindex, "fogmaxdensity", density);
}

/**
 * Set fog's start distance.
 * 
 * @param fogindex      Edict index of the fog to modify.
 * @param startdist     The start distance of the fog. 
 */
VEffectsSetFogStartDist(fogindex, startdist)
{
    // Set start distance.
    SetVariantInt(startdist);
    AcceptEntityInput(fogindex, "SetStartDist");
}

/**
 * Set fog's end distance.
 * 
 * @param fogindex      Edict index of the fog to modify.
 * @param enddist       The end distance of the fog. 
 */
VEffectsSetFogEndDist(fogindex, enddist)
{
    // Set end distance.
    SetVariantInt(enddist);
    AcceptEntityInput(fogindex, "SetEndDist");
}

/**
 * Set fog's far z distance.
 * 
 * @param fogindex      Edict index of the fog to modify.
 * @param farz          The far z distance of the fog. 
 */
VEffectsSetFogFarZ(fogindex, farz)
{
    // Set far z distance.
    SetVariantInt(farz);
    AcceptEntityInput(fogindex, "SetFarZ");
}

/**
 * Create an energy splash effect.
 * 
 * @param client        The client index.
 * @param origin        The origin of the effect.
 * @param direction     The direction of the effect.
 */
VEffectsCreateEnergySplash(const Float:origin[3], const Float:direction[3], bool:explosive)
{
    TE_SetupEnergySplash(origin, direction, explosive);
    TE_SendToAll();
}

/**
 * Create an explosion effect with strict flags.
 * 
 * @param origin    The (x, y, z) coordinate of the explosion.
 * @param flags     The flags to set on the explosion.
 */
VEffectsCreateExplosion(const Float:origin[3], flags)
{
    // Create an explosion entity.
    new explosion = CreateEntityByName("env_explosion");
    
    // If explosion entity isn't valid, then stop.
    if (explosion == -1)
    {
        return;
    }
    
    // Get and modify flags on explosion.
    new spawnflags = GetEntProp(explosion, Prop_Data, "m_spawnflags");
    spawnflags = spawnflags | EXP_NODAMAGE | EXP_NODECAL | flags;
    
    // Set modified flags on entity.
    SetEntProp(explosion, Prop_Data, "m_spawnflags", spawnflags);
    
    // Spawn the entity into the world.
    DispatchSpawn(explosion);
    
    // Set the origin of the explosion.
    DispatchKeyValueVector(explosion, "origin", origin);
    
    // Set fireball material.
    PrecacheModel("materials/sprites/xfireball3.vmt");
    DispatchKeyValue(explosion, "fireballsprite", "materials/sprites/xfireball3.vmt");
    
    // Tell the entity to explode.
    AcceptEntityInput(explosion, "Explode");
    
    // Remove entity from world.
    RemoveEdict(explosion);
}

/**
 * Shake a client's screen with specific parameters.
 * 
 * @param client        The client index.
 * @param amplitude     The amplitude (intensity) of the shaking.
 * @param frequency     The frequency (speed) of the shaking.
 * @param duration      The duration (time) of the shaking.
 */
VEffectsShakeClientScreen(client, Float:amplitude, Float:frequency, Float:duration)
{
    // If shake usermsg isn't invalid, then stop.
    new Handle:hShake = StartMessageOne("Shake", client);
    if (hShake == INVALID_HANDLE)
    {
        return;
    }
    
    // Write shake information to usermsg handle.
    BfWriteByte(hShake, 0);
    BfWriteFloat(hShake, amplitude);
    BfWriteFloat(hShake, frequency);
    BfWriteFloat(hShake, duration);
    
    // End usermsg and send to client.
    EndMessage();
}
