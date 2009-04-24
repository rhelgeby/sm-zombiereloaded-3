/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        visualeffects.inc
 *   Description: Visual effects such as map darkening, fog, etc..
 *
 * ============================================================================
 */

/**
 * Handle of cvar "sv_skyname."
 */
new Handle:g_hSkyname = INVALID_HANDLE;

/**
 * Default sky of current map.
 */
new String:g_VEffectsDefaultSky[PLATFORM_MAX_PATH];

/**
 * Get cvar data and downloadable content to add to download table.
 */
VEffectsLoad()
{
    // Get sv_skyname's convar handle, if invalid, log error, then stop.
    g_hSkyname = FindConVar("sv_skyname");
    if (g_hSkyname == INVALID_HANDLE)
    {
        // TODO LOG.
        
        return;
    }
    
    // Store map's default sky before applying new one.
    GetConVarString(g_hSkyname, g_VEffectsDefaultSky, sizeof(g_VEffectsDefaultSky));
    
    // TODO add sky to downloads table.
}

/**
 * The round is starting.
 */
VEffectsOnRoundStart()
{
    // If lightstyle is disabled, then disable.
    new bool:lightstyle = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_LIGHTSTYLE]);
    
    // Apply light style.
    VEffectsApplyLightStyle(!lightstyle);
    
    // If sky is disabled, then disable.
    new bool:sky = GetConVarBool(g_hCvarsList[CVAR_VEFFECTS_SKY]);
    
    // Apply new sky.
    VEffectsApplySky(!sky);
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
    // If default, then set to default sky.
    if (disable)
    {
        // Set new sky on all clients.
        SetConVarString(g_hSkyname, g_VEffectsDefaultSky, true);
        
        return;
    }
    
    // Get sky path.
    decl String:skypath[PLATFORM_MAX_PATH];
    GetConVarString(g_hCvarsList[CVAR_VEFFECTS_SKY_PATH], skypath, sizeof(skypath));
    
    // Set new sky on all clients.
    SetConVarString(g_hSkyname, skypath, true);
}