/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          tools.inc
 *  Type:          Core
 *  Description:   Find offsets and signatures.
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
 * Initialize global offset variables.
 */
new g_iToolsVelocity;
new g_iToolsLMV;
new g_iToolsHasNightVision;
new g_iToolsNightVisionOn;
new g_iToolsFOV;

/**
 * @endsection
 */

/**
 * Initialize global SDKTools handles.
 */
new Handle:g_hToolsGameConfig = INVALID_HANDLE;
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
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CBasePlayer::m_vecVelocity[0]\" was not found.");
    }
    
    // If offset "m_flLaggedMovementValue" can't be found, then stop the plugin.
    g_iToolsLMV = FindSendPropInfo("CCSPlayer", "m_flLaggedMovementValue");
    if (g_iToolsLMV == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CCSPlayer::m_flLaggedMovementValue\" was not found.");
    }
    
    // If offset "m_bHasNightVision" can't be found, then stop the plugin.
    g_iToolsHasNightVision = FindSendPropInfo("CCSPlayer", "m_bHasNightVision");
    if (g_iToolsHasNightVision == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CCSPlayer::m_bHasNightVision\" was not found.");
    }
    
    // If offset "m_bNightVisionOn" can't be found, then stop the plugin.
    g_iToolsNightVisionOn = FindSendPropInfo("CCSPlayer", "m_bNightVisionOn");
    if (g_iToolsNightVisionOn == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CCSPlayer::m_bNightVisionOn\" was not found.");
    }
    
    // If offset "m_iFOV" can't be found, then stop the plugin.
    g_iToolsFOV = FindSendPropInfo("CBasePlayer", "m_iFOV");
    if (g_iToolsFOV == -1)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "Offsets", "Offset \"CBasePlayer::m_iFOV\" was not found.");
    }
    
    // Forward event to modules.
    WeaponsOnOffsetsFound();
    AccountOnOffsetsFound();
    VEffectsOnOffsetsFound();
    NapalmOnOffsetsFound();
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
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "GameData", "Can't load game config file (plugin.zombiereloaded.txt) from the gamedata directory.");
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
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "GameData", "Signature \"CGameRules::TerminateRound\" was not found.");
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
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Tools, "GameData", "Signature \"CBasePlayer::CSWeaponDrop\" was not found.");
    }
}
