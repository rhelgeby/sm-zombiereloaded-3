/**
 * ====================
 *   Zombie:Reloaded
 *   File: zombiereloaded.inc
 *   Author: Greyscale
 * ==================== 
 */

enum ZTeam
{
    Neither, /** Round is not over */
    Zombie, /** Round is over because zombies win */
    Human, /** Round is over because humans wins */
}

#define Target_Bombed							1		// Target Successfully Bombed!
#define VIP_Escaped								2		// The VIP has escaped!
#define VIP_Assassinated						3		// VIP has been assassinated!
#define Terrorists_Escaped						4		// The terrorists have escaped!
#define CTs_PreventEscape						5		// The CT's have prevented most of the terrorists from escaping!
#define Escaping_Terrorists_Neutralized			6		// Escaping terrorists have all been neutralized!
#define Bomb_Defused							7		// The bomb has been defused!
#define CTs_Win									8		// Counter-Terrorists Win!
#define Terrorists_Win							9		// Terrorists Win!
#define Round_Draw								10		// Round Draw!
#define All_Hostages_Rescued					11		// All Hostages have been rescued!
#define Target_Saved							12		// Target has been saved!
#define Hostages_Not_Rescued					13		// Hostages have not been rescued!
#define Terrorists_Not_Escaped					14		// Terrorists have not escaped!
#define VIP_Not_Escaped							15		// VIP has not escaped!
#define Game_Commencing							16		// Game Commencing!

#define DXLEVEL_MIN 90

#define DEFAULT_FOV 90

new bool:market;

new dxLevel[MAXPLAYERS+1];

new bool:zombieSpawned;
new bool:motherZombie[MAXPLAYERS+1];
new bool:gZombie[MAXPLAYERS+1];
new bool:gBlockMotherInfect[MAXPLAYERS+1];
new bool:bZVision[MAXPLAYERS+1];
new bool:dispHP[MAXPLAYERS+1];
new bool:pProtect[MAXPLAYERS+1];

new pClass[MAXPLAYERS+1];
new pNextClass[MAXPLAYERS+1];

new teleCount[MAXPLAYERS+1];
new protCount[MAXPLAYERS+1];
new Float:spawnLoc[MAXPLAYERS+1][3];

new maxclients;

new Handle:tRound = INVALID_HANDLE;
new Handle:tInfect = INVALID_HANDLE;

new Handle:pList = INVALID_HANDLE;

#define MAXTIMERS 7

#define TMOAN 0
#define TREGEN 1
#define TTELE 2
#define TZHP 3
#define TPROTECT 4
#define TRESPAWN 5
#define TZVISION 6

new Handle:tHandles[MAXPLAYERS+1][MAXTIMERS];

new QueryCookie:mat_dxlevel;

FindClientDXLevel(client)
{
    if (IsFakeClient(client))
    {
        return;
    }
    
    mat_dxlevel = QueryClientConVar(client, "mat_dxlevel", DXLevelClientQuery);
}

public DXLevelClientQuery(QueryCookie:cookie, client, ConVarQueryResult:result, const String:cvarName[], const String:cvarValue[])
{
    if (cookie != mat_dxlevel)
    {
        return;
    }
    
    dxLevel[client] = 0;
    
    if (result != ConVarQuery_Okay)
    {   
        return;
    }
    
    dxLevel[client] = StringToInt(cvarValue);
}

DisplayClientOverlay(client, const String:overlay[])
{
    if (!dxLevel[client])
    {
        FindClientDXLevel(client);
        
        return;
    }
    
    if (dxLevel[client] >= DXLEVEL_MIN)
    {
        ClientCommand(client, "r_screenoverlay \"%s\"", overlay);
    }
    else
    {
        ZR_PrintCenterText(client, "DX90 not supported", dxLevel[client], DXLEVEL_MIN);
    }
}

RefreshList()
{
    ClearList();
    
    pList = CreateArray();
    
    new maxplayers = GetMaxClients();
    for (new x = 1; x <= maxplayers; x++)
    {
        if (IsClientInGame(x))
        {  
            new team = GetClientTeam(x);
            if (team == CS_TEAM_T || team == CS_TEAM_CT)
            {
                PushArrayCell(pList, x);
            }
        }
    }
}

AddPlayerToList(client)
{
    if (pList != INVALID_HANDLE)
    {
        PushArrayCell(pList, client);
    }
}

ClearList()
{
    if (pList != INVALID_HANDLE)
    {
        ClearArray(pList);
    }
}

RandomPlayerFromList()
{
    if (pList != INVALID_HANDLE)
    {
        new size = GetArraySize(pList);
        new index = GetRandomInt(0, size - 1);
        
        return GetArrayCell(pList, index);
    }
    
    return -1;
}

bool:IsPlayerInList(client)
{
    if (pList != INVALID_HANDLE)
    {
        return (FindValueInArray(pList, client) != -1);
    }
    
    return false;
}

bool:IntToBool(intval)
{
    if (intval == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool:IsClientPlayer(client)
{
    if (client > 0 && client <= maxclients)
    {
        return true;
    }
    else
    {
        return false;
    }
}

ZR_DebugPrintToConsole(client, String:message[])
{
    if (client)
    { /* Client console */
        PrintToConsole(client, message);
        LogMessage("Debug log (client %i) -- %s", client, message);
    }
    else
    { /* Server console */
        PrintToServer(message);
        LogMessage("Debug log (client %i) -- %s", client, message);
    }
}