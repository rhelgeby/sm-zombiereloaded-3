/**
 * ====================
 *   Zombie:Reloaded
 *   File: offsets.inc
 *   Author: Greyscale
 * ==================== 
 */

new offsBaseVelocity;
new offsGetVelocity0;
new offsGetVelocity1;
new offsGetVelocity2;
new offsSpeed;
new offsNVG;
new offsNVGOn;
new offsCollision;
new offsMoney;
new offsFOV;
new offsBuyZone;
new offsColor;
new offsRender;

new Handle:hGameConf = INVALID_HANDLE;
new Handle:hRemoveAllItems = INVALID_HANDLE;
new Handle:hTerminateRound = INVALID_HANDLE;

FindOffsets()
{
    offsBaseVelocity = FindSendPropInfo("CBasePlayer", "m_vecBaseVelocity");
    if (offsBaseVelocity == -1)
    {
        SetFailState("Couldn't find \"m_vecBaseVelocity\"!");
    }
    
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
    
    offsMoney = FindSendPropInfo("CCSPlayer", "m_iAccount");
    if (offsMoney == -1)
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
    if(offsColor == -1) {
        SetFailState("Couldn't find \"m_clrRender\"!");
    }

    offsRender = FindSendPropInfo("CBaseAnimating", "m_nRenderMode");
    if(offsRender == -1) {
        SetFailState("Couldn't find \"m_nRenderMode\"!");
    }
}

SetupGameData()
{
    hGameConf = LoadGameConfigFile("plugin.zombiereloaded");
    
    StartPrepSDKCall(SDKCall_Player);
    PrepSDKCall_SetFromConf(hGameConf, SDKConf_Virtual, "RemoveAllItems");
    hRemoveAllItems = EndPrepSDKCall();
    
    StartPrepSDKCall(SDKCall_GameRules);
    PrepSDKCall_SetFromConf(hGameConf, SDKConf_Signature, "TerminateRound");
    PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
    PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
    hTerminateRound = EndPrepSDKCall();
}

SetPlayerVelocity(client, const Float:vec[3])
{
    SetEntDataVector(client, offsBaseVelocity, vec, true);
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

SetPlayerMoney(client, amount)
{
    SetEntData(client, offsMoney, amount, 4, true);
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

RemoveAllPlayersWeapons(client)
{
    SDKCall(hRemoveAllItems, client);
}

TerminateRound(Float:delay, reason)
{
    SDKCall(hTerminateRound, delay, reason);
}

SetPlayerModel(client, const String:model[])
{
    PrecacheModel(model);
    SetEntityModel(client, model);
}

SetPlayerAlpha(client, alpha)
{
    SetEntData(client, offsColor + 3, alpha, 1, true);
    SetEntData(client, offsRender, 3, 1, true);
}

