/**
 * ====================
 *        Market
 *   File: market.inc
 *   Version: 1.2
 *   Author: Greyscale
 * ==================== 
 */

/**
 * Used to send the market to a client
 * @param client   Client index
 * @param title   Title of the market menu
 * @param rebuy   Text to display for the "Rebuy" option
 * @noreturn
 */   
native Market_Send(client, const String:title[], const String:rebuy[]);

/**
 * Used to send the market to a client
 * @param weapondid   ID of the weapon whose info will be returned
 * @param display   The display name of the weapon
 * @param weapon   The entity name of the weapon
 * @param price   The price of the weapon
 * @return   True if weaponid exists, false if not
 */
native bool:Market_GetWeaponIDInfo(const String:weaponid[], String:display[], String:weapon[], &price);

/**
 * Called when a player makes a selection from the menu
 * @param client   Client index
 * @param weaponid   Returns the ID of the weapon selected ("rebuy" is returned when player selected rebuy)
 * @return   True to allow purchase, false to block
 */
forward bool:Market_OnWeaponSelected(client, String:weaponid[]);

/**
 * Called after a player makes a selection from the menu
 * @param client   Client index
 * @param allowed   True if selection was allowed and false if it was blocked
 * noreturn
 */
forward Market_PostOnWeaponSelected(client, &bool:allowed);



public SharedPlugin:__pl_market = 
{
	name = "market",
	file = "market.smx",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};
 
public __pl_market_SetNTVOptional()
{
	MarkNativeAsOptional("Market_Send");
	MarkNativeAsOptional("Market_GetWeaponIDInfo");
}