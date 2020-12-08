#if defined _AFKManager_Included
	#endinput
#endif
#define _AFKManager_Included

native int GetClientIdleTime(int client);

public SharedPlugin __pl_AFKManager =
{
	name = "AFKManager",
	file = "AFKManager.smx",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_PLUGIN
public __pl_AFKManager_SetNTVOptional()
{
	MarkNativeAsOptional("GetClientIdleTime");
}
#endif
