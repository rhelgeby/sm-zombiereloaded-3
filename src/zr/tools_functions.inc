/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          tools_functions.inc
 *   Type:          Core
 *   Description:   API for offsets/signatures exposed in tools.inc
 *
 * ============================================================================
 */

/**
 * Get or set a client's velocity.
 * @param client        The client index.
 * @param vecVelocity   Array to store vector in, or velocity to set on client.
 * @param retrieve      True to get client's velocity, false to set it.
 * @param stack         If modifying velocity, then true will stack new velocity onto the client's
 *                      current velocity, false will reset it.
 */
ToolsClientVelocity(client, Float:vecVelocity[3], bool:apply = true, bool:stack = true)
{
    // If retrieve if true, then get client's velocity.
    if (!apply)
    {
        // x = vector component.
        for (new x = 0; x < 3; x++)
        {
            vecVelocity[x] = GetEntDataFloat(client, g_iToolsVelocity + (x*4));
        }
        
        // Stop here.
        return;
    }
    
    // If stack is true, then add client's velocity.
    if (stack)
    {
        // Get client's velocity.
        new Float:vecClientVelocity[3];
        
        // x = vector component.
        for (new x = 0; x < 3; x++)
        {
            vecClientVelocity[x] = GetEntDataFloat(client, g_iToolsVelocity + (x*4));
        }
        
        AddVectors(vecClientVelocity, vecVelocity, vecVelocity);
    }
    
    // Apply velocity on client.
    TeleportEntity(client, NULL_VECTOR, NULL_VECTOR, vecVelocity);
}

/**
 * Set a client's lagged movement value.
 * @param client    The client index.
 * @param value     LMV value. (1.0 = default, 2.0 = double)
 */
ToolsSetClientLMV(client, Float:fLMV)
{
    // Set lagged movement value of client.
    SetEntDataFloat(client, g_iToolsLMV, fLMV / 300.0, true);
}

/**
 * Control nightvision values on a client.
 * @param client    The client index.
 * @param enable    Enable or disable an aspect of nightvision. (see ownership parameter)
 * @param ownership If true, enable will toggle the client's ownership of nightvision.
 *                  If false, enable will toggle the client's on/off state of the nightvision.
 */
ToolsClientNightVision(client, bool:enable, bool:ownership = true)
{
    // If ownership is true, then toggle the ownership of nightvision on client.
    if (ownership)
    {
        SetEntData(client, g_iToolsHasNightVision, enable, 1, true);
        
        // Stop here.
        return;
    }
    
    SetEntData(client, g_iToolsNightVisionOn, enable, 1, true);
}

/**
 * Set a client's default field of vision.
 * @param client    The client index.
 * @param FOV       The field of vision of the client.
 */
ToolsSetClientDefaultFOV(client, FOV)
{
    SetEntData(client, g_iToolsDefaultFOV, FOV, 1, true);
}

/**
 * Get or set a client's score or deaths.
 * 
 * @param client    The client index.
 * @param score     True to look at score, false to look at deaths.  
 * @param apply     True to set scores or death, false to get. 
 * @param value     The value of the client's score or deaths.
 * @return          The score or death count of the client, -1 if setting.
 */
ToolsClientScore(client, bool:score = true, bool:apply = true, value = 0)
{
    if (!apply)
    {
        if (score)
        {
            // If score is true, then return client's score.
            return GetEntProp(client, Prop_Data, "m_iFrags");
        }
        // Return client's deaths.
        else
        {
            return GetEntProp(client, Prop_Data, "m_iDeaths");
        }
    }
    
    // If score is true, then set client's score.
    if (score)
    {
        SetEntProp(client, Prop_Data, "m_iFrags", value);
    }
    // Set client's deaths.
    else
    {
        SetEntProp(client, Prop_Data, "m_iDeaths", value);
    }
    
    // We set the client's score or deaths.
    return -1;
}

/**
 * Set a client's alpha value.
 * 
 * @param client    The client index.
 * @param alpha     The alpha value to set client's alpha to. (0-255)
 * @param weapons   Apply alpha to all client's weapons.
 */
ToolsSetClientAlpha(client, alpha, bool:bWeapons = true)
{
    // Turn rendermode on, on the client.
    SetEntityRenderMode(client, RENDER_TRANSALPHA);
    
    // Set alpha value on the client.
    SetEntityRenderColor(client, _, _, _, alpha);
    
    // If bWeapons is false, then stop.
    if (!bWeapons)
    {
        return;
    }
    
    // Set client's weapons' alpha.
    WeaponSetWeaponAlpha(client, alpha);
}
