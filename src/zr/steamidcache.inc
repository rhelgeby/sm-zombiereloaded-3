/*
 * ============================================================================
 *
 *  Zombie:Reloaded
 *
 *  File:          steamidcache.inc
 *  Type:          Core
 *  Description:   A SteamID caching API.
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
 * Creates a steamid cache.
 *
 * @return  Handle to SteamID cache.
 */
stock Handle SteamidCacheCreate()
{
    // Return steamid cache handle.
    return CreateArray(1);
}

/**
 * Add client serial number to the SteamID cache.
 *
 * @param steamidcache  The SteamID cache to add client to.
 * @param client        The client index.
 * @return              True if the client was added successfully, false if the client already exists.
 */
stock bool SteamidCacheAddClient(Handle steamidcache, int client)
{
    // Check if client is in the cache.
    if (SteamidCacheClientExists(steamidcache, client))
    {
        return false;
    }

    // Get client's SteamID.
    int steamid = GetSteamAccountID(client);
    if (!steamid)
        return false;

    // Push SteamID into the SteamID cache.
    PushArrayCell(steamidcache, steamid);

    // Client added successfully.
    return true;
}

/**
 * Remove client serial number from the SteamID cache.
 *
 * @param steamidcache  The SteamID cache to remove the client from.
 * @param client        The client index.
 * @return              True if the client was removed successfully, false if the client doesn't exist.
 */
stock bool SteamidCacheRemoveClient(Handle steamidcache, int client)
{
    // Get client's SteamID.
    int steamid = GetSteamAccountID(client);
    if (!steamid)
        return false;

    // Find the client in the cache.
    int idx = FindValueInArray(steamidcache, steamid);
    if (idx == -1)
        return false;

    // Remove index from the SteamID cache.
    RemoveFromArray(steamidcache, idx);

    // Client removed successfully.
    return true;
}

/**
 * Check if a client is in the SteamID cache.
 *
 * @param steamidcache  The SteamID cache to check in.
 * @param client        The client index.
 * @return              True if the client exists, false otherwise.
 */
stock bool SteamidCacheClientExists(Handle steamidcache, int client)
{
    // Get client's SteamID.
    int steamid = GetSteamAccountID(client);
    if (!steamid)
        return false;

    // Return true if client was found, false otherwise.
    return (FindValueInArray(steamidcache, steamid) != -1);
}

/**
 * Reset SteamID cache.
 *
 * @param steamidcache  The SteamID cache to reset.
 */
stock void SteamidCacheReset(Handle steamidcache)
{
    if (steamidcache == INVALID_HANDLE)
        return;

    // Clear array.
    ClearArray(steamidcache);
}
