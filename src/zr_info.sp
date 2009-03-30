
#include <sourcemod>

public Plugin:myinfo = {
	name = "Info Script for Zombie:Reloaded",
	author = "[SG-10]Cpt.Moore",
	description = "",
	version = "1.0",
	url = "http://zombie.swissquake.ch/"
};

public OnPluginStart()
{
    RegConsoleCmd("sm_show_cvar", Command_Show_CVar);
}

new String:g_sCVar[128];
new String:g_sCVarValue[128];
new Handle:g_hCVar;

public Action:Command_Show_CVar(client,args)
{
	GetCmdArgString(g_sCVar,sizeof(g_sCVar));

	g_hCVar = FindConVar(g_sCVar);
	if (g_hCVar != INVALID_HANDLE)
	{ //not found
		GetConVarString(g_hCVar, g_sCVarValue, sizeof(g_sCVarValue));
		if (client == 0)
		{
			PrintToServer("\"%s\" = \"%s\"", g_sCVar, g_sCVarValue);
		}
		else
		{
			PrintToConsole(client, "\"%s\" = \"%s\"", g_sCVar, g_sCVarValue);
		}
	}
	else
	{ //found
		if (client == 0)
		{
			PrintToServer("Couldn't find %s", g_sCVar);
		}
		else
		{
			PrintToConsole(client,"Couldn't find %s", g_sCVar);
		}
	}

	return Plugin_Handled;
}

