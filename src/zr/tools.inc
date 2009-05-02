/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        tools.inc
 *   Description: Find offsets and signatures.
 *
 * ============================================================================
 */

/**
 * Initialize global offset variables.
 */
new g_iToolsVelocity;
new g_iToolsBaseVelocity;
new g_iToolsLMV;
new g_iToolsHasNightVision;
new g_iToolsNightVisionOn;
new g_iToolsCollisionGroup;
new g_iToolsAccount;
new g_iToolsDefaultFOV;
new g_iToolsInBuyZone;
new g_iToolsRender;
new g_iToolsRenderMode;
new g_iToolsActiveWeapon;
/**
 * @endsection
 */

/**
 * Initialize global SDKTools handles.
 */
new Handle:g_hToolsGameConfig = INVALID_HANDLE;
new Handle:g_hToolsEyeAngles = INVALID_HANDLE;
new Handle:g_hToolsTerminateRound = INVALID_HANDLE;
new Handle:g_hToolsCSWeaponDrop = INVALID_HANDLE;
/**
 * @endsection
 */

// Tools Functions (core)
#include "zr/tools_functions"

/**
 * Tools module init function.
 */
ToolsInit()
{
    // Find offsets.
    ToolsFindOffsets();
    
    // Setup SDKTools
    ToolsSetupGameData();
}

/**
 * Finds all offset values for the plugin.
 */
ToolsFindOffsets()
{
    // If offset "m_vecVelocity[0]" can't be found, then stop the plugin.
    g_iToolsVelocity = FindSendPropInfo("CBasePlayer", "m_vecVelocity[0]");
    if (g_iToolsVelocity == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CBasePlayer::m_vecVelocity[0]\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_vecBaseVelocity" can't be found, then stop the plugin.
    g_iToolsBaseVelocity = FindSendPropInfo("CBasePlayer", "m_vecBaseVelocity");
    if (g_iToolsBaseVelocity == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CBasePlayer::m_vecBaseVelocity\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_flLaggedMovementValue" can't be found, then stop the plugin.
    g_iToolsLMV = FindSendPropInfo("CCSPlayer", "m_flLaggedMovementValue");
    if (g_iToolsLMV == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CCSPlayer::m_flLaggedMovementValue\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_bHasNightVision" can't be found, then stop the plugin.
    g_iToolsHasNightVision = FindSendPropInfo("CCSPlayer", "m_bHasNightVision");
    if (g_iToolsHasNightVision == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CCSPlayer::m_bHasNightVision\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_bNightVisionOn" can't be found, then stop the plugin.
    g_iToolsNightVisionOn = FindSendPropInfo("CCSPlayer", "m_bNightVisionOn");
    if (g_iToolsNightVisionOn == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CCSPlayer::m_bNightVisionOn\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_CollisionGroup" can't be found, then stop the plugin.
    g_iToolsCollisionGroup = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");
    if (g_iToolsCollisionGroup == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CBaseEntity::m_CollisionGroup\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_iAccount" can't be found, then stop the plugin.
    g_iToolsAccount = FindSendPropInfo("CCSPlayer", "m_iAccount");
    if (g_iToolsAccount == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CCSPlayer::m_iAccount\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_iDefaultFOV" can't be found, then stop the plugin.
    g_iToolsDefaultFOV = FindSendPropInfo("CBasePlayer", "m_iDefaultFOV");
    if (g_iToolsDefaultFOV == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CBasePlayer::m_iDefaultFOV\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_bInBuyZone" can't be found, then stop the plugin.
    g_iToolsInBuyZone = FindSendPropInfo("CCSPlayer", "m_bInBuyZone");
    if (g_iToolsInBuyZone == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CCSPlayer::m_bInBuyZone\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_clrRender" can't be found, then stop the plugin.
    g_iToolsRender = FindSendPropInfo("CAI_BaseNPC", "m_clrRender");
    if (g_iToolsRender == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CAI_BaseNPC::m_clrRender\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_nRenderMode" can't be found, then stop the plugin.
    g_iToolsRenderMode = FindSendPropInfo("CBaseAnimating", "m_nRenderMode");
    if (g_iToolsRenderMode == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CBaseAnimating::m_nRenderMode\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // If offset "m_hActiveWeapon" can't be found, then stop the plugin.
    g_iToolsActiveWeapon = FindSendPropInfo("CBasePlayer", "m_hActiveWeapon");
    if (g_iToolsActiveWeapon == -1)
    {
        LogMessageFormatted(-1, "Tools", "Offsets", "Offset \"CBasePlayer::m_hActiveWeapon\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
}

/**
 * Sets up gamedata for the plugin.
 */
ToolsSetupGameData()
{
    // Load game config file.
    g_hToolsGameConfig = LoadGameConfigFile("plugin.zombiereloaded");
    
    // If gamedata file can't be loaded, then stop the plugin.
    if (g_hToolsGameConfig == INVALID_HANDLE)
    {
        LogMessageFormatted(-1, "Tools", "GameData", "Can't load game config file (plugin.zombiereloaded.txt) from the gamedata directory.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // Prep the SDKCall for "EyeAngles."
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(g_hToolsGameConfig, SDKConf_Virtual, "EyeAngles");
    PrepSDKCall_SetReturnInfo(SDKType_QAngle, SDKPass_ByValue);
    g_hToolsEyeAngles = EndPrepSDKCall();
    
    // If offset "EyeAngles" can't be found, then stop the plugin.
    if(g_hToolsEyeAngles == INVALID_HANDLE)
    {
        LogMessageFormatted(-1, "Tools", "GameData", "Offset \"EyeAngles\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // Prep the SDKCall for "TerminateRound."
    StartPrepSDKCall(SDKCall_GameRules);
    PrepSDKCall_SetFromConf(g_hToolsGameConfig, SDKConf_Signature, "TerminateRound");
    PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    g_hToolsTerminateRound = EndPrepSDKCall();
    
    // If offset "TerminateRound" can't be found, then stop the plugin.
    if(g_hToolsTerminateRound == INVALID_HANDLE)
    {
        LogMessageFormatted(-1, "Tools", "GameData", "Signature \"CGameRules::TerminateRound\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
    
    // Prep the SDKCall for "CSWeaponDrop."
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(g_hToolsGameConfig, SDKConf_Signature, "CSWeaponDrop");
    PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
    PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
    PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
    g_hToolsCSWeaponDrop = EndPrepSDKCall();
    
    // If offset "CSWeaponDrop" can't be found, then stop the plugin.
    if(g_hToolsCSWeaponDrop == INVALID_HANDLE)
    {
        LogMessageFormatted(-1, "Tools", "GameData", "Signature \"CBasePlaye::CSWeaponDrop\" was not found.", LOG_FORMAT_TYPE_FATALERROR);
    }
}
