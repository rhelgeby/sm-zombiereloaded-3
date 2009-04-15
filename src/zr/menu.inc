/**
 * ====================
 *   Zombie:Reloaded
 *   File: menu.sp
 *   Author: Greyscale
 * ==================== 
 */

MainMenu(client)
{
    new Handle:menu_main = CreateMenu(MainMenuHandle);
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_main, "%t\n ", "!zmenu title");
    
    decl String:zmenu[128];
    decl String:zadmin[128];
    decl String:zclass[128];
    decl String:zmarket[128];
    decl String:zspawn[128];
    decl String:ztele[128];
    decl String:zhp[128];
    
    Format(zmenu, sizeof(zmenu), "%t", "!zmenu menu");
    Format(zadmin, sizeof(zadmin), "%t", "!zmenu admin");
    Format(zclass, sizeof(zclass), "%t", "!zmenu class", ITEMDRAW_DISABLED);
    Format(zmarket, sizeof(zmarket), "%t", "!zmenu market");
    Format(zspawn, sizeof(zspawn), "%t", "!zmenu spawn");
    Format(ztele, sizeof(ztele), "%t", "!zmenu tele");
    Format(zhp, sizeof(zhp), "%t (%d HP)", "!zmenu hp", GetClientHealth(client));
    
    AddMenuItem(menu_main, "zmenu", zmenu, ITEMDRAW_DISABLED);
    
    if (GetAdminFlag(GetUserAdmin(client), Admin_Generic))
    {
        AddMenuItem(menu_main, "zadmin", zadmin);
    }
    else
    {
        AddMenuItem(menu_main, "zadmin", zadmin, ITEMDRAW_DISABLED);
    }
    
    AddMenuItem(menu_main, "zclass", zclass);
    
    if (market)
    {
        AddMenuItem(menu_main, "zmarket", zmarket);
    }
    else
    {
        AddMenuItem(menu_main, "zmarket", zmarket, ITEMDRAW_DISABLED);
    }
    
    AddMenuItem(menu_main, "zspawn", zspawn);
    AddMenuItem(menu_main, "ztele", ztele);
    AddMenuItem(menu_main, "zhp", zhp);
    
    DisplayMenu(menu_main, client, MENU_TIME_FOREVER);
}

public MainMenuHandle(Handle:menu_main, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        switch(slot)
        {
            case 1:
            {
                if (!ZRAdminMenu(client))
                {
                    MainMenu(client);
                }
            }
            case 2:
            {
                /*if (!ZClass(client))
                {
                    MainMenu(client);
                }*/
            }
            case 3:
            {
                if (!ZMarket(client))
                {
                    MainMenu(client);
                }
            }
            case 4:
            {
                ZSpawn(client);
            }
            case 5:
            {
                if (!ZTele(client))
                {
                    MainMenu(client);
                }
            }
            case 6:
            {
                // Toggle ZHP.
                ZHPToggle(client);
                
                // Resend menu.
                MainMenu(client);
            }
        }
    }
    if (action == MenuAction_End)
    {
        CloseHandle(menu_main);
    }
}

/**ClassMenu(client)
{
    new Handle:menu_classes = CreateMenu(ClassMenuHandle);
    
    decl String:menu_description[256];
    decl String:display[288]; // 32 + 256
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_classes, "%t\n ", "!zclass title");
    
    for (new x = 0; x < classCount; x++)
    {
        GetClassName(x, display, sizeof(display));
        GetClassMenuDescription(x, menu_description, sizeof(menu_description));
        
        if (pNextClass[client] == -1)
        {
            if (x == pClass[client])
            {
                Format(display, sizeof(display), "%s (current)", display);
            }
        }
        else if (x == pNextClass[client])
        {
            Format(display, sizeof(display), "%s (current)", display);
        }
        
        Format(display, sizeof(display), "%s\n %s", display, menu_description);
        
        AddMenuItem(menu_classes, "", display);
    }
    
    SetMenuExitBackButton(menu_classes, true);
    
    DisplayMenu(menu_classes, client, MENU_TIME_FOREVER);
}

public ClassMenuHandle(Handle:menu_classes, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        if (IsPlayerHuman(client) || !IsPlayerAlive(client))
        {
            Call_StartForward(hOnZClassChanged);
            Call_PushCell(client);
            Call_PushCell(pClass[client]);
            Call_PushCell(slot);
            Call_Finish();
            
            pClass[client] = slot;
        }
        else
        {
            pNextClass[client] = slot;
        }
            
        decl String:name[64];
        GetClassName(slot, name, sizeof(name));
        
        ZR_PrintToChat(client, "Class select", name);
    }
    
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            MainMenu(client);
        }
    }
    else if (action == MenuAction_End)
    {
        CloseHandle(menu_classes);
    }
}*/