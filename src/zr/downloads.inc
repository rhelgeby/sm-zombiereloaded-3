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
 * 
 * @redir   config.inc
 */

/**
 * Prepare all model/download data.
 */
DownloadsLoad()
{
    // Get downloads file path.
    decl String:pathdownloads[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_DOWNLOADS, pathdownloads);
    
    // Register config info.
    ConfigRegisterConfig(ConfigDownloads, false, GetFunctionByName(GetMyHandle(), "DownloadsOnConfigReload"), _, pathdownloads, CONFIG_FILE_ALIAS_DOWNLOADS);
    
    // If file doesn't exist, then log.
    if (!exists)
    {
        // Log error, then stop.
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Downloads", "Config Validation", "Missing downloads file: \"%s\"", pathdownloads);
        
        return;
    }
    
    // If download array exists, then destroy it.
    if (arrayDownloadsList != INVALID_HANDLE)
    {
        CloseHandle(arrayDownloadsList);
    }
    
    arrayDownloadsList = ConfigLinesToArray(pathdownloads);
    
    // If array couldn't be created, then fail.
    if (arrayDownloadsList == INVALID_HANDLE)
    {
        LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Downloads", "Config Validation", "Error parsing \"%s\"", pathdownloads);
    }
    
    new downloadcount;
    new downloadvalidcount;
    
    decl String:downloadpath[PLATFORM_MAX_PATH];
    
    new downloads = downloadcount = GetArraySize(arrayDownloadsList);
    
    // x = download array index.
    for (new x = 0; x < downloads; x++)
    {
        // Get base model path (rawline in models.txt)
        GetArrayString(arrayDownloadsList, x, downloadpath, sizeof(downloadpath));
        
        // If file doesn't exist, then remove, log, and stop.
        if (!FileExists(downloadpath))
        {
            // Remove client from array.
            RemoveFromArray(arrayDownloadsList, x);
            
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
    ConfigSetConfigLoaded(ConfigDownloads, true);
    ConfigSetConfigHandle(ConfigDownloads, arrayDownloadsList);
}

/**
 * Called when configs are being reloaded.
 * 
 * @param config    The config being reloaded. (only if 'all' is false)
 */
public DownloadsOnConfigReload(ConfigFile:config)
{
    // Reload download config.
    if (config == ConfigDownloads)
    {
        DownloadsLoad();
    }
}