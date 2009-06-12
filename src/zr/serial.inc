/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          serial.inc
 *  Type:          Core
 *  Description:   Client serial number tracking API.
 *
 *  Copyright (C) 2009  Greyscale, Richard Helgeby
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
 * Maximum length of a client's serial number.
 */
#define SERIAL_MAX_LENGTH 8

/**
 * Array to store client serial numbers.
 */
new Handle:arraySerial = INVALID_HANDLE;

/**
 * Map is starting.
*/
SerialOnMapStart()
{
    // If array exists, destroy before recreating.
    if (arraySerial != INVALID_HANDLE)
    {
        CloseHandle(arraySerial);
    }
    
    // Create array.
    arraySerial = CreateArray();
}

/**
 * Add client serial number to global array.
 * 
 * @param client    The client index.
 * @return          True if the client was added successfully, false if the client already exists.
 */
bool:SerialAddClient(client)
{
    // Check if client is already added.
    new bool:exists = SerialClientExists(client);
    if (exists)
    {
        return false;
    }
    
    // Get client's serial number.
    new serial = GetClientSerial(client);
    
    // Push serial number into the global array.
    PushArrayCell(arraySerial, serial);
    
    // Client added successfully.
    return true;
}

/**
 * Check if a client has been added to the global array.
 * 
 * @param client    The client index.
 * @return          True if the client exists, false otherwise.
 */
bool:SerialClientExists(client)
{
    // Get client's serial number.
    new serial = GetClientSerial(client);
    
    // Return true if value was found, false otherwise.
    return (FindValueInArray(arraySerial, serial) != -1);
}

/**
 * Reset serial number array.
 */
SerialReset()
{
    // Clear array.
    ClearArray(arraySerial);
}
