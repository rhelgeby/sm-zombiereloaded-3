/**
 * ====================
 *   Zombie:Reloaded
 *   File: zr.inc
 *   Author: Greyscale
 * ==================== 
 */

/**
 * Used to check if a player is a zombie.
 * @param client   Client index.
 * @return   True if the player is a zombie, and false if human.
 */   
native bool:ZR_IsClientZombie(client);

/**
 * Returns the client's zombie class.
 * @param client   Client index.
 * @return   The client's zombie class.
 */   
native ZR_GetClientZClass(client);

/**
 * Returns if the zombie has spawned or not.
 * @return   Returns true, if the first zombie has spawned during the round, false if not.
 */   
native bool:ZR_HasZombieSpawned();

/**
 * Called when a player turns into a zombie.
 * @param client   Client index.
 * @param mother   True if zombie is a mother, false if zombified by another zombie.
 */
forward ZR_Zombify(client, bool:mother);

/**
 * Called when a player changes their zombie class.
 * @param client   Client index.
 * @param oldclass   The client's old class.
 * @param newclass   The client's new class.
 */
forward ZR_OnZClassChanged(client, oldclass, newclass);