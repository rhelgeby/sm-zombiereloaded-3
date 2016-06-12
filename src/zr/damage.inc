/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          damage.inc
 *  Type:          Core
 *  Description:   Modify damage stuff here.
 *
 *  Copyright (C) 2009-2013  Greyscale, Richard Helgeby
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
 * @section Suicide intercept defines.
 */
#define DAMAGE_SUICIDE_MAX_CMDS 5
#define DAMAGE_SUICIDE_MAX_LENGTH 16
/**
 * @endsection
 */

/**
 * Array to keep track of normal/mother zombies.
 */
new bool:g_bDamageMotherZombie[MAXPLAYERS + 1];

/**
 * Hook commands related to damage here.
 * Note: This isn't OnCommandsHook because this depends on cvars.
 */
new bool:g_bSuicideCmdsHooked = false;
DamageLoad()
{
    if (g_bSuicideCmdsHooked)
    {
        return;
    }

    // Create command callbacks (intercepts) for listed suicide commands.
    decl String:suicidecmds[DAMAGE_SUICIDE_MAX_CMDS * DAMAGE_SUICIDE_MAX_LENGTH];
    GetConVarString(g_hCvarsList[CVAR_DAMAGE_SUICIDE_CMDS], suicidecmds, sizeof(suicidecmds));

    // Create array to store cmds
    new String:arrayCmds[DAMAGE_SUICIDE_MAX_CMDS][DAMAGE_SUICIDE_MAX_LENGTH];

    // Explode string into array indexes.
    new cmdcount = ExplodeString(suicidecmds, ",", arrayCmds, sizeof(arrayCmds), sizeof(arrayCmds[]));

    // x = Array index.
    // arrayCmds[x] = suicide command.
    for (new x = 0; x <= cmdcount - 1; x++)
    {
        // Trim whitespace.
        TrimString(arrayCmds[x]);

        // Prepare intercept for this command.
        RegConsoleCmd(arrayCmds[x], DamageSuicideIntercept);
    }

    // Important: If ZR can be unloaded some day, make sure to remove the listeners and set this to false.
    g_bSuicideCmdsHooked = true;
}

/**
 * Client is joining the server.
 *
 * @param client    The client index.
 */
DamageClientInit(client)
{
    // Hook damage callbacks.
    SDKHook(client, SDKHook_TraceAttack, DamageTraceAttack);
    SDKHook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
}

/**
 * Client is leaving the server.
 *
 * @param client    The client index.
 */
DamageOnClientDisconnect(client)
{
    SDKUnhook(client, SDKHook_TraceAttack, DamageTraceAttack);
    SDKUnhook(client, SDKHook_OnTakeDamage, DamageOnTakeDamage);
}

/**
 * A client was infected.
 *
 * @param client        The client index.
 * @param motherinfect  True if the zombie is mother, false if not.
 */
DamageOnClientInfected(client, bool:motherinfect)
{
    // Update if client is a mother zombie or not.
    g_bDamageMotherZombie[client] = motherinfect;
}

/**
 * Hook: TraceAttack
 * Called right before the bullet enters a client.
 *
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param hitbox        The hitbox index.
 * @param hitgroup      The hitgroup index.
 * @return              Return Plugin_Handled to stop bullet from hitting client.
 *                      Plugin_Continue to allow bullet to hit client.
 */
public Action:DamageTraceAttack(client, &attacker, &inflictor, &Float:damage, &damagetype, &ammotype, hitbox, hitgroup)
{
    // If attacker isn't valid, then stop.
    if (!ZRIsClientValid(attacker))
    {
        return Plugin_Continue;
    }

    // If client is attacking himself, then stop.
    if(attacker == client)
    {
        return Plugin_Continue;
    }

    // Get zombie flag for each client.
    new bool:clientzombie = InfectIsClientInfected(client);
    new bool:attackerzombie = InfectIsClientInfected(attacker);

    // If the flags are the same on both clients, then stop.
    if (clientzombie == attackerzombie)
    {
        // If friendly fire is blocked, then allow damage.
        new bool:damageblockff = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_FF]);
        if (!damageblockff)
        {
            return Plugin_Continue;
        }

        // Stop bullet from hurting client.
        return Plugin_Handled;
    }

    // Here we know that attacker and client are different teams.

    // Check if immunity module requires damage to be blocked.
    if (ImmunityOnClientTraceAttack(client, attacker, damage, hitgroup, damagetype))
    {
        // Block damage.
        return Plugin_Handled;
    }

    // If client is a human, then allow damage.
    if (InfectIsClientHuman(client))
    {
        // Allow damage.
        return Plugin_Continue;
    }

    // If damage hitgroups cvar is disabled, then allow damage.
    // TODO: There are two cvars: zr_hitgroups and zr_damage_hitgroups. Maybe
    //       just use zr_hitgroups?
    new bool:damagehitgroups = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_HITGROUPS]);
    if (!damagehitgroups)
    {
        // Allow damage.
        return Plugin_Continue;
    }

    // If damage is disabled for this hitgroup, then stop.
    new index = HitgroupToIndex(hitgroup);

    // If index can't be found, then allow damage.
    if (index == -1)
    {
        // Allow damage.
        return Plugin_Continue;
    }

    new bool:candamage = HitgroupsCanDamage(index);
    if (!candamage)
    {
        // Stop bullet from hurting client.
        return Plugin_Handled;
    }

    // Allow damage.
    return Plugin_Continue;
}

/**
 * Hook: OnTakeDamage
 * Called right before damage is done.
 *
 * @param client        The client index.
 * @param inflictor     The entity index of the inflictor.
 * @param attacker      The client index of the attacker.
 * @param damage        The amount of damage inflicted.
 * @param damagetype    The type of damage inflicted.
 * @param ammotype      The ammo type of the attacker's weapon.
 * @return              Return Plugin_Handled to stop the damage to client.
 *                      Plugin_Continue to allow damage to client.
 */
public Action:DamageOnTakeDamage(client, &attacker, &inflictor, &Float:damage, &damagetype)
{
    // Get classname of the inflictor.
    decl String:classname[64];
    GetEdictClassname(inflictor, classname, sizeof(classname));

    // If entity is a trigger, then allow damage. (Map is damaging client)
    if (StrContains(classname, "trigger") > -1)
    {
        return Plugin_Continue;
    }

    new Action:action;

    // Forward this hook to another module an return (or not) what it wants.
    action = Action:NapalmOnTakeDamage(client, damagetype);

    // If the napalm module wants to return here, then return the int casted into the action type.
    if (action > Action:-1)
    {
        return action;
    }

    // Client was shot or knifed.
    if (damagetype & DMG_BULLET || damagetype & DMG_NEVERGIB)
    {
        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Plugin_Continue;
        }

        // Get zombie flag for each client.
        new bool:clientzombie = InfectIsClientInfected(client);
        new bool:attackerzombie = InfectIsClientInfected(attacker);

        // If client and attacker are on the same team, then let CS:S handle the rest.
        if (clientzombie == attackerzombie)
        {
            return Plugin_Continue;
        }

        // We know that clientzombie is the opposite of attacker zombie.

        // If the client is a zombie, then allow damage.
        if (clientzombie)
        {
            return Plugin_Continue;
        }

        // Check if immunity module blocked or modified the damage.
        new Action:immunityAction = ImmunityOnClientDamage(client, attacker, damage);
        if (immunityAction != Plugin_Continue)
        {
            // Damage was blocked or modified.
            return immunityAction;
        }

        // Check if distance between clients is too high. (Anti lagcompensation / longknife)
        new Float:maxDistance = GetConVarFloat(g_hCvarsList[CVAR_INFECT_MAX_DISTANCE]);
        if (maxDistance != 0.0)
        {
            new Float:clientPosition[3];
            new Float:attackerPosition[3];

            GetClientAbsOrigin(client, clientPosition);
            GetClientAbsOrigin(attacker, attackerPosition);

            new Float:distance = GetVectorDistance(clientPosition, attackerPosition);

            // Block infection if distance is higher than allowed.
            if (distance > maxDistance)
            {
                return Plugin_Handled;
            }
        }

        // Client is about to be infected, re-add HP so they aren't killed by
        // knife. But only do this when immunity mode is disabled.
        if (ClassGetImmunityMode(client) == Immunity_None)
        {
            new health = GetClientHealth(client);
            SetEntityHealth(client, health + RoundToNearest(damage));

            // Allow damage.
            return Plugin_Continue;
        }
    }
    // Client was damaged by explosion.
    else if (damagetype & DMG_BLAST)
    {
        // If blast damage is blocked, then stop.
        new bool:damageblockblast = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_BLOCK_BLAST]);
        if (!damageblockblast)
        {
            return Plugin_Continue;
        }

        // If attacker isn't valid, then allow damage.
        if (!ZRIsClientValid(attacker))
        {
            return Plugin_Continue;
        }

        // If client is a zombie, then allow damage.
        if (InfectIsClientInfected(client))
        {
            return Plugin_Continue;
        }

        // Stop damage.
        return Plugin_Handled;
    }
    // Client was damaged by falling.
    else if (damagetype & DMG_FALL)
    {
        // If class has "nofalldamage" disabled, then allow damage.
        new bool:blockfalldamage = ClassGetNoFallDamage(client);
        if (!blockfalldamage)
        {
            return Plugin_Continue;
        }

        // Stop damage.
        return Plugin_Handled;
    }

    // Allow damage.
    return Plugin_Continue;
}

/**
 * Command callback (kill, spectate, jointeam, joinclass)
 * Block command if plugin thinks they are trying to commit suicide.
 *
 * @param client    The client index.
 * @param argc      The number of arguments in command string.
 */
public Action:DamageSuicideIntercept(client, argc)
{
    // Get suicide interception settings.
    new bool:suicideAfterInfect = GetConVarBool(g_hCvarsList[CVAR_SUICIDE_AFTER_INFECT]);
    new bool:suicideZombie = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_ZOMBIE]);
    new bool:suicideZombieMother = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_MZOMBIE]);
    new bool:suicideHuman = GetConVarBool(g_hCvarsList[CVAR_DAMAGE_SUICIDE_HUMAN]);

    // Check various criterias that will _allow_ the command. If no criterias
    // match, block it.

    // Check general criterias.
    if ((suicideAfterInfect && !InfectHasZombieSpawned()) || // Check if it should block suicides before mother zombie.
        !ZRIsClientValid(client) ||                     // Validate client (to stop console).
        !IsPlayerAlive(client))                         // Check if dead.
    {
        // Allow command.
        return Plugin_Continue;
    }

    // Check zombie criterias.
    if (InfectIsClientInfected(client))
    {
        if (g_bDamageMotherZombie[client] && !suicideZombieMother ||    // Check if suicide is allowed for mother zombies.
            (!g_bDamageMotherZombie[client] && !suicideZombie))         // Check if suicide is allowed for regular zombies.
        {
            // Allow command.
            return Plugin_Continue;
        }
    }

    // Check human criterias.
    // Allow suicide if player is a human and humans can suicide.
    if (InfectIsClientHuman(client) && !suicideHuman)
    {
        // Allow command.
        return Plugin_Continue;
    }

    // Tell client their command has been intercepted, and log.
    TranslationPrintToChat(client, "Damage suicide intercept");
    LogEvent(false, LogType_Normal, LOG_GAME_EVENTS, LogModule_Damage, "Suicide Intercept", "\"%L\" attempted suicide.", client);

    // Block command.
    return Plugin_Handled;
}
