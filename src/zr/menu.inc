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
    new itemdraw = (ZRIsClientAdmin(client)) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED;
    
    SetGlobalTransTarget(client);
    
    SetMenuTitle(menu_main, "%t\n ", "!zmenu title");
    
    decl String:zadmin[128];
    decl String:zclass[128];
    decl String:zmarket[128];
    decl String:zspawn[128];
    decl String:ztele[128];
    decl String:zhp[128];
    
    Format(zadmin, sizeof(zadmin), "%t", "!zmenu admin");
    Format(zclass, sizeof(zclass), "%t", "!zmenu class", ITEMDRAW_DISABLED);
    Format(zmarket, sizeof(zmarket), "%t", "!zmenu market");
    Format(zspawn, sizeof(zspawn), "%t", "!zmenu spawn");
    Format(ztele, sizeof(ztele), "%t", "!zmenu tele");
    Format(zhp, sizeof(zhp), "%t (%d HP)", "!zmenu hp", GetClientHealth(client));
    
    AddMenuItem(menu_main, "zadmin", zadmin, itemdraw);
    AddMenuItem(menu_main, "zclass", zclass);
    
    if (g_bMarket)
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
            case 0:
            {
                if (!ZRAdminMenu(client))
                {
                    MainMenu(client);
                }
            }
            case 1:
            {
                ClassMenuMain(client);
            }
            case 2:
            {
                if (!ZMarketSend(client))
                {
                    MainMenu(client);
                }
            }
            case 3:
            {
                ZSpawn(client);
            }
            case 4:
            {
                if (!ZTele(client))
                {
                    MainMenu(client);
                }
            }
            case 5:
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