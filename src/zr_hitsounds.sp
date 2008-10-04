
#include <sourcemod>
#include <sdktools>

public Plugin:myinfo = {
	name = "Hit Sounds for Zombie:Reloaded",
	author = "[SG-10]Cpt.Moore",
	description = "",
	version = "1.0",
	url = "http://zombie.swissquake.ch/"
};

static const String:hit_head[] = {"hit_head.wav"};
static const String:hit_body[] = {"hit_body.wav"};

// native hooks
public OnPluginStart()
{
	HookEvent("player_hurt", Event_PlayerHurt);
	LoadSound(hit_head);
	LoadSound(hit_body);
}

public OnMapStart()
{
	LoadSound(hit_head);
	LoadSound(hit_body);
}

public Event_PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{	
	new attackerId = GetEventInt(event, "attacker");
	new attacker = GetClientOfUserId(attackerId);
	//new damage = GetEventInt(event, "dmg_health");
	new hitgroup = GetEventInt(event,"hitgroup");
	
	if( attacker > 0 && !IsFakeClient(attacker) )
	{
		if ( hitgroup == 1 )
		{
			EmitSoundToClient(attacker, hit_head);
		}
		else
		{
			EmitSoundToClient(attacker, hit_body);
		}
	}
}

// utility functions

public LoadSound(const String:sound_file[])
{
    new String:sound_path[PLATFORM_MAX_PATH];
    Format(sound_path, sizeof(sound_path), "sound/%s", sound_file);
    PrecacheSound(sound_file, true);
    AddFileToDownloadsTable(sound_path);
}

