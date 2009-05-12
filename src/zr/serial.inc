/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          serial.inc
 *   Type:          Core
 *   Description:   Client serial number tracking API.
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