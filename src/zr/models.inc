/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          models.inc
 *   Type:          Core
 *   Description:   Model validation and API
 *
 * ============================================================================
 */

/**
 * Maximum folder depth a model file can be located.
 */
#define MODELS_PATH_MAX_DEPTH 8

/**
 * Maximum string length of a folder a model file is located under.
 */
#define MODELS_PATH_DIR_MAX_LENGTH 32

/**
 * Array that stores a list of validated models.
 * 
 * @redir   config.inc
 */

ModelsLoad()
{
    // Add models to downloads table and validate.
    ModelsPrepModels();
    
    // Add download entries to downloads table and validate.
    ModelsPrepDownloads();
}

/**
 * Validate model paths and add to downloads table.
 */
ModelsPrepModels()
{
    // Initialize log boolean.
    new bool:enablelog = LogCheckFlag(LOG_CORE_EVENTS, LOG_MODULE_CORE);
    
    // Get models file path.
    decl String:pathmodels[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetFilePath(CVAR_CONFIG_PATH_MODELS, pathmodels);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogMessageFormatted(-1, "Models", "Config Validation", "Fatal error: Missing models file: \"%s\"", LOG_FORMAT_TYPE_FATALERROR, pathmodels);
    }
    
    // If model array exists, then destroy it.
    if (arrayModelsList != INVALID_HANDLE)
    {
        CloseHandle(arrayModelsList);
    }
    
    arrayModelsList = ConfigLinesToArray(pathmodels);
    
    // If array couldn't be created, then fail.
    if (arrayModelsList == INVALID_HANDLE)
    {
        LogMessageFormatted(-1, "Models", "Config Validation", "Fatal error: Error parsing \"%s\"", LOG_FORMAT_TYPE_FATALERROR, pathmodels);
    }
    
    new modelcount;
    new modelvalidcount;
    new modelfilecount;
    
    decl String:modelbase[PLATFORM_MAX_PATH];
    decl String:modelpath[PLATFORM_MAX_PATH];
    decl String:modelname[MODELS_PATH_DIR_MAX_LENGTH];
    decl String:modelfile[MODELS_PATH_DIR_MAX_LENGTH];
    decl String:modeldiskname[MODELS_PATH_DIR_MAX_LENGTH];
    decl String:modelfullpath[PLATFORM_MAX_PATH];
    
    new String:baseexploded[MODELS_PATH_MAX_DEPTH][MODELS_PATH_DIR_MAX_LENGTH];
    
    new FileType:type;
    
    new models = modelcount = GetArraySize(arrayModelsList);
    
    // x = model array index.
    for (new x = 0; x < models; x++)
    {
        // Get base model path (rawline in models.txt)
        GetArrayString(arrayModelsList, x, modelbase, sizeof(modelbase));
        
        // Explode path into pieces. (separated by "/")
        new strings = ExplodeString(modelbase, "/", baseexploded, MODELS_PATH_MAX_DEPTH, MODELS_PATH_DIR_MAX_LENGTH);
        
        // Get model file name.
        strcopy(modelname, sizeof(modelname), baseexploded[strings - 1]);
        
        // Get the path to the file.
        // Works by truncating original path by the length of the file name.
        strcopy(modelpath, strlen(modelbase) - strlen(modelname), modelbase);
        
        // Open dir containing model files.
        new Handle:modeldir = OpenDirectory(modelpath);
        
        // Reset model file count.
        modelfilecount = 0;
        
        while (ReadDirEntry(modeldir, modelfile, sizeof(modelfile), type))
        {
            // If entry isn't a file, then stop.
            if (type != FileType_File)
            {
                continue;
            }
            
            // Find break point index in the string to get model name.
            // Add one because it seems to break on the character before.
            new breakpoint = FindCharInString(modelfile, '.') + 1;
            strcopy(modeldiskname, breakpoint, modelfile);
            
            // If this file doesn't match, then stop.
            if (!StrEqual(modelname, modeldiskname, false))
            {
                continue;
            }
            
            // Format a full path string.
            strcopy(modelfullpath, sizeof(modelfullpath), modelpath);
            Format(modelfullpath, sizeof(modelfullpath), "%s/%s", modelfullpath, modelfile);
            
            // Precache model file and add to downloads table.
            PrecacheModel(modelfullpath);
            AddFileToDownloadsTable(modelfullpath);
            
            // Increment modelfilecount
            modelfilecount++;
        }
        
        // Increment modelvalidcount if model files are valid.
        if (modelfilecount)
        {
            modelvalidcount++;
        }
        else
        {
            // Remove client from array.
            RemoveFromArray(arrayModelsList, x);
            
            // Subtract one from count.
            models--;
            
            // Backtrack one index, because we deleted it out from under the loop.
            x--;
            
            // Log missing model files.
            if (enablelog)
            {
                LogMessageFormatted(-1, "Models", "Config Validation", "Missing model files on server (%s)", LOG_FORMAT_TYPE_ERROR, modelbase);
            }
        }
    }
    
    // Log model validation info.
    if (enablelog)
    {
        LogMessageFormatted(-1, "Models", "Config Validation", "Total: %d | Successful: %d | Unsuccessful: %d", LOG_FORMAT_TYPE_FULL, modelcount, modelvalidcount, modelcount - modelvalidcount);
    }
    
    // If none of the model paths are valid, then log and fail.
    if (!modelvalidcount)
    {
        if (enablelog)
        {
            LogMessageFormatted(-1, "Models", "Config Validation", "No usable model paths in %s", LOG_FORMAT_TYPE_FATALERROR, pathmodels);
        }
    }
}

/**
 * Validate custom download paths and add to downloads table.
 */
ModelsPrepDownloads()
{
    // Initialize log boolean.
    new bool:enablelog = LogCheckFlag(LOG_CORE_EVENTS, LOG_MODULE_CORE);
    
    // Get downloads file path.
    decl String:pathdownloads[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetFilePath(CVAR_CONFIG_PATH_DOWNLOADS, pathdownloads);
    
    // If file doesn't exist, then log.
    if (!exists)
    {
        // Log error, then stop.
        if (enablelog)
        {
            LogMessageFormatted(-1, "Downloads", "Config Validation", "Missing downloads file: \"%s\"", LOG_FORMAT_TYPE_ERROR, pathdownloads);
        }
        
        return;
    }
    
    new Handle:arrayDownloadsList = ConfigLinesToArray(pathdownloads);
    
    // If array couldn't be created, then fail.
    if (arrayModelsList == INVALID_HANDLE)
    {
        if (enablelog)
        {
            LogMessageFormatted(-1, "Downloads", "Config Validation", "Error parsing \"%s\"", LOG_FORMAT_TYPE_ERROR, pathdownloads);
        }
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
            
            if (enablelog)
            {
                LogMessageFormatted(-1, "Downloads", "Config Validation", "Missing file \"%s\"", LOG_FORMAT_TYPE_ERROR, downloadpath);
            }
            
            continue;
        }
        
        // Increment downloadvalidcount
        downloadvalidcount++;
        
        // Precache model file and add to downloads table.
        AddFileToDownloadsTable(downloadpath);
    }
    
    // Log model validation info.
    if (enablelog)
    {
        LogMessageFormatted(-1, "Downloads", "Config Validation", "Total: %d | Successful: %d | Unsuccessful: %d", LOG_FORMAT_TYPE_FULL, downloadcount, downloadvalidcount, downloadcount - downloadvalidcount);
    }
}
