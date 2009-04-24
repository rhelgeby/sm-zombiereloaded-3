/**
 * ====================
 *   Zombie:Reloaded
 *   File: offsets.inc
 *   Author: Greyscale
 * ==================== 
 */

new offsGetVelocity0;
new offsGetVelocity1;
new offsGetVelocity2;
new offsSpeed;
new offsNVG;
new offsNVGOn;
new offsCollision;
new offsAccount;
new offsFOV;
new offsBuyZone;
new offsColor;
new offsRender;
new offsActiveWeapon;

new Handle:g_hGameConf = INVALID_HANDLE;
new Handle:g_hEyeAngles = INVALID_HANDLE;
new Handle:g_hTerminateRound = INVALID_HANDLE;
new Handle:g_hCSWeaponDrop = INVALID_HANDLE;

FindOffsets()
{
    offsGetVelocity0 = FindSendPropInfo("CBasePlayer", "m_vecVelocity[0]");
    if (offsGetVelocity0 == -1)
    {
        SetFailState("Couldn't find \"m_vecVelocity[0]\"!");
    }
    
    offsGetVelocity1 = FindSendPropInfo("CBasePlayer", "m_vecVelocity[1]");
    if (offsGetVelocity1 == -1)
    {
        SetFailState("Couldn't find \"m_vecVelocity[1]\"!");
    }
    
    offsGetVelocity2 = FindSendPropInfo("CBasePlayer", "m_vecVelocity[2]");
    if (offsGetVelocity2 == -1)
    {
        SetFailState("Couldn't find \"m_vecVelocity[2]\"!");
    }
    
    offsSpeed = FindSendPropInfo("CCSPlayer", "m_flLaggedMovementValue");
    if (offsSpeed == -1)
    {
        SetFailState("Couldn't find \"m_flLaggedMovementValue\"!");
    }
    
    offsNVG = FindSendPropInfo("CCSPlayer", "m_bHasNightVision");
    if (offsNVG == -1)
    {
        SetFailState("Couldn't find \"m_bHasNightVision\"!");
    }
    
    offsNVGOn = FindSendPropInfo("CCSPlayer", "m_bNightVisionOn");
    if (offsNVGOn == -1)
    {
        SetFailState("Couldn't find \"m_bNightVisionOn\"!");
    }
    
    offsCollision = FindSendPropInfo("CBaseEntity", "m_CollisionGroup");
    if (offsCollision == -1)
    {
        SetFailState("Couldn't find \"m_CollisionGroup\"!");
    }
    
    offsAccount = FindSendPropInfo("CCSPlayer", "m_iAccount");
    if (offsAccount == -1)
    {
        SetFailState("Couldn't find \"m_iAccount\"!");
    }
    
    offsFOV = FindSendPropInfo("CBasePlayer", "m_iDefaultFOV");
    if (offsFOV == -1)
    {
        SetFailState("Couldn't find \"m_iDefaultFOV\"!");
    }
    
    offsBuyZone = FindSendPropInfo("CCSPlayer", "m_bInBuyZone");
    if (offsBuyZone == -1)
    {
        SetFailState("Couldn't find \"m_bInBuyZone\"!");
    }

    offsColor = FindSendPropInfo("CAI_BaseNPC", "m_clrRender");
    if(offsColor == -1)
    {
        SetFailState("Couldn't find \"m_clrRender\"!");
    }

    offsRender = FindSendPropInfo("CBaseAnimating", "m_nRenderMode");
    if(offsRender == -1)
    {
        SetFailState("Couldn't find \"m_nRenderMode\"!");
    }
    
    offsActiveWeapon = FindSendPropInfo("CBasePlayer", "m_hActiveWeapon");
    if(offsActiveWeapon == -1)
    {
        SetFailState("Couldn't find \"m_hActiveWeapon\"!");
    }
}

SetupGameData()
{
    // Load game config file.
    g_hGameConf = LoadGameConfigFile("plugin.zombiereloaded");
	
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(g_hGameConf, SDKConf_Virtual, "EyeAngles");
    PrepSDKCall_SetReturnInfo(SDKType_QAngle, SDKPass_ByValue);
    g_hEyeAngles = EndPrepSDKCall();
    
    if(g_hEyeAngles == INVALID_HANDLE)
	{
		SetFailState("Couldn't find offset \"EyeAngles\"!");
	}
	
    StartPrepSDKCall(SDKCall_GameRules);
    PrepSDKCall_SetFromConf(g_hGameConf, SDKConf_Signature, "TerminateRound");
    PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    g_hTerminateRound = EndPrepSDKCall();
    
    if(g_hTerminateRound == INVALID_HANDLE)
	{
		SetFailState("Couldn't find signature \"CGameRules::TerminateRound\"!");
	}
	
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(g_hGameConf, SDKConf_Signature, "CSWeaponDrop");
    PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
    PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
    PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
    g_hCSWeaponDrop = EndPrepSDKCall();
    
    if(g_hCSWeaponDrop == INVALID_HANDLE)
    {
        SetFailState("Couldn't find signature \"CBasePlayer::CSWeaponDrop\"!");
    }
}

SetPlayerVelocity(client, const Float:vec[3], bool:reset)
{
    if (reset)
    {
        TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vec);
    }
    else
    {
        new Float:fVelocity[3];
        GetPlayerVelocity(client, fVelocity);
        
        AddVectors(vec, fVelocity, fVelocity);
        
        TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, fVelocity);
    }
}

GetPlayerVelocity(client, Float:vel[3])
{
    vel[0] = GetEntDataFloat(client, offsGetVelocity0);
    vel[1] = GetEntDataFloat(client, offsGetVelocity1);
    vel[2] = GetEntDataFloat(client, offsGetVelocity2);
}

SetPlayerSpeed(client, Float:speed)
{
    new Float:newspeed = speed / 300.0;
    SetEntDataFloat(client, offsSpeed, newspeed, true);
}

NightVision(client, bool:enable)
{
    SetEntData(client, offsNVG, enable, 1, true);
}

NightVisionOn(client, bool:enable)
{
    SetEntData(client, offsNVGOn, enable, 1, true);
}

NoCollide(client, bool:nocollide)
{
    if (nocollide)
    {
        SetEntData(client, offsCollision, 2, 1, true);
    }
    else
    {
        SetEntData(client, offsCollision, 5, 1, true);
    }
}

bool:CanCollide(client)
{
    new collisionstate = GetEntData(client, offsCollision, 1);
    
    if (collisionstate == 2)
    {
        return false;
    }
    
    return true;
}

SetPlayerFOV(client, fov)
{
    SetEntData(client, offsFOV, fov, 1, true);
}

bool:IsClientInBuyZone(client)
{
    return bool:GetEntData(client, offsBuyZone);
}

AddPlayerScore(client, amount)
{
    new frags = GetEntProp(client, Prop_Data, "m_iFrags");
    SetEntProp(client, Prop_Data, "m_iFrags", frags + amount);
}

AddPlayerDeath(client, amount)
{
    new deaths = GetEntProp(client, Prop_Data, "m_iDeaths");
    SetEntProp(client, Prop_Data, "m_iDeaths", deaths + amount);
}

GetPlayerEyeAngles(client, Float:ang[3])
{
    SDKCall(g_hEyeAngles, client, ang);
}

TerminateRound(Float:delay, reason)
{
    SDKCall(g_hTerminateRound, delay, reason);
}

SetPlayerAlpha(client, alpha)
{
    SetEntData(client, offsRender, 3, 1, true);
    SetEntData(client, offsColor + 3, alpha, 1, true);
}