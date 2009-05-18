/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:          models.inc
 *   Type:          Core
 *   Description:   Model validation.
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

/**
 * Prepare all model/download data.
 */
ModelsLoad()
{
    // Get models file path.
    decl String:pathmodels[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_MODELS, pathmodels);
    
    // Register config info.
    ConfigRegisterConfig(ConfigModels, false, GetFunctionByName(GetMyHandle(), "ModelsOnConfigReload"), _, pathmodels, CONFIG_FILE_ALIAS_MODELS);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogPrintToLog(LOG_FORMAT_TYPE_FATALERROR, "Models", "Config Validation", "Fatal Error: Missing models file: \"%s\"", pathmodels);
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
        LogPrintToLog(LOG_FORMAT_TYPE_FATALERROR, "Models", "Config Validation", "Fatal Error: Error parsing \"%s\"", pathmodels);
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
            LogPrintToLog(LOG_FORMAT_TYPE_ERROR, "Models", "Config Validation", "Missing model files on server (%s)", modelbase);
        }
    }
    
    // Log model validation info.
    LogPrintToLog(LOG_FORMAT_TYPE_NORMAL, "Models", "Config Validation", "Total: %d | Successful: %d | Unsuccessful: %d", modelcount, modelvalidcount, modelcount - modelvalidcount);
    
    // If none of the model paths are valid, then log and fail.
    if (!modelvalidcount)
    {
        LogPrintToLog(LOG_FORMAT_TYPE_FATALERROR, "Models", "Config Validation", "Fatal Error: No usable model paths in %s", pathmodels);
    }
    
    // Set config data.
    ConfigSetConfigLoaded(ConfigModels, true);
    ConfigSetConfigHandle(ConfigModels, arrayModelsList);
}

/**
 * Called when configs are being reloaded.
 * 
 * @param config    The config being reloaded. (only if 'all' is false)
 */
public ModelsOnConfigReload(ConfigFile:config)
{
    // Reload model config.
    if (config == ConfigModels)
    {
        ModelsLoad();
    }
}