/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          downloads.inc
 *   Type:          Core
 *   Description:   Download validation.
 *
 * ============================================================================
 */

/**
 * Array that stores a list of downloadable files.
 */
new Handle:arrayDownloads = INVALID_HANDLE;

/**
 * Prepare all model/download data.
 */
DownloadsLoad()
{
    // Register config file.
    ConfigRegisterConfig(File_Downloads, Structure_List, CONFIG_FILE_ALIAS_DOWNLOADS);
    
    // Get downloads file path.
    decl String:pathdownloads[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_DOWNLOADS, pathdownloads);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogPrintToLog(LOG_FORMAT_TYPE_FATALERROR, "Downloads", "Config Validation", "Fatal Error: Missing downloads file: \"%s\"", pathdownloads);
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_Downloads, pathdownloads);
    
    // Load config from file and create array structure.
    new bool:success = ConfigLoadConfig(File_Downloads, arrayDownloads);
    
    // Unexpected error, stop plugin.
    if (!success)
    {
        LogPrintToLog(LOG_FORMAT_TYPE_FATALERROR, "Downloads", "Config Validation", "Fatal Error: Unexpected error encountered loading: %s", pathdownloads);
    }
    
    new downloadcount;
    new downloadvalidcount;
    
    decl String:downloadpath[PLATFORM_MAX_PATH];
    
    new downloads = downloadcount = GetArraySize(arrayDownloads);
    
    // x = download array index.
    for (new x = 0; x < downloads; x++)
    {
        // Get download path
        GetArrayString(arrayDownloads, x, downloadpath, sizeof(downloadpath));
        
        // If file doesn't exist, then remove, log, and stop.
        if (!FileExists(downloadpath))
        {
            // Remove client from array.
            RemoveFromArray(arrayDownloads, x);
            
            // Subtract one from count.
            downloads--;
            
            // Backtrack one index, because we deleted it out from under the loop.
            x--;
            
            LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Downloads", "Config Validation", "Missing file \"%s\"", downloadpath);
            continue;
        }
        
        // Increment downloadvalidcount
        downloadvalidcount++;
        
        // Precache model file and add to downloads table.
        AddFileToDownloadsTable(downloadpath);
    }
    
    // Log model validation info.
    LogPrintToLog(LOG_FORMAT_TYPE_NORMAL, "Downloads", "Config Validation", "Total: %d | Successful: %d | Unsuccessful: %d", downloadcount, downloadvalidcount, downloadcount - downloadvalidcount);
    
    // Set config data.
    ConfigSetConfigLoaded(File_Downloads, true);
    ConfigSetConfigReloadFunc(File_Downloads, GetFunctionByName(GetMyHandle(), "DownloadsOnConfigReload"));
    ConfigSetConfigHandle(File_Downloads, arrayDownloads);
}

/**
 * Called when configs are being reloaded.
 * 
 * @param config    The config being reloaded. (only if 'all' is false)
 */
public DownloadsOnConfigReload(ConfigFile:config)
{
    // Reload download config.
    DownloadsLoad();
}