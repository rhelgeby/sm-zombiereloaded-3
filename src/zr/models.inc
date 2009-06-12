/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *  File:          models.inc
 *  Type:          Core
 *  Description:   Model validation.
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
 * Maximum folder depth a model file can be located.
 */
#define MODELS_PATH_MAX_DEPTH 8

/**
 * Maximum string length of a folder a model file is located under.
 */
#define MODELS_PATH_DIR_MAX_LENGTH 32

/**
 * Array that stores a list of validated models.
 */
new Handle:arrayModels = INVALID_HANDLE;

/**
 * Prepare all model/download data.
 */
ModelsLoad()
{
    // Register config file.
    ConfigRegisterConfig(File_Models, Structure_List, CONFIG_FILE_ALIAS_MODELS);
    
    // Get models file path.
    decl String:pathmodels[PLATFORM_MAX_PATH];
    new bool:exists = ConfigGetCvarFilePath(CVAR_CONFIG_PATH_MODELS, pathmodels);
    
    // If file doesn't exist, then log and stop.
    if (!exists)
    {
        // Log failure and stop plugin.
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Missing models file: \"%s\"", pathmodels);
    }
    
    // Set the path to the config file.
    ConfigSetConfigPath(File_Models, pathmodels);
    
    // Load config from file and create array structure.
    new bool:success = ConfigLoadConfig(File_Models, arrayModels);
    
    // Unexpected error, stop plugin.
    if (!success)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Unexpected error encountered loading: %s", pathmodels);
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
    
    new models = modelcount = GetArraySize(arrayModels);
    
    // x = model array index.
    for (new x = 0; x < models; x++)
    {
        // Get base model path (rawline in models.txt)
        GetArrayString(arrayModels, x, modelbase, sizeof(modelbase));
        
        // Explode path into pieces. (separated by "/")
        new strings = ExplodeString(modelbase, "/", baseexploded, MODELS_PATH_MAX_DEPTH, MODELS_PATH_DIR_MAX_LENGTH);
        
        // Get model file name.
        strcopy(modelname, sizeof(modelname), baseexploded[strings - 1]);
        
        // Get the path to the file.
        // Works by truncating original path by the length of the file name.
        strcopy(modelpath, strlen(modelbase) - strlen(modelname), modelbase);
        
        // Open dir containing model files.
        new Handle:modeldir = OpenDirectory(modelpath);
        
        if (modeldir == INVALID_HANDLE)
        {
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Error opening model path directory: %s", modelpath);
            continue;
        }
        
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
            RemoveFromArray(arrayModels, x);
            
            // Subtract one from count.
            models--;
            
            // Backtrack one index, because we deleted it out from under the loop.
            x--;
            
            // Log missing model files.
            LogEvent(false, LogType_Error, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Missing model files on server (%s)", modelbase);
        }
    }
    
    // Log model validation info.
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Total: %d | Successful: %d | Unsuccessful: %d", modelcount, modelvalidcount, modelcount - modelvalidcount);
    
    // If none of the model paths are valid, then log and fail.
    if (!modelvalidcount)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "No usable model paths in %s", pathmodels);
    }
    
    // Set config data.
    ConfigSetConfigLoaded(File_Models, true);
    ConfigSetConfigReloadFunc(File_Models, GetFunctionByName(GetMyHandle(), "ModelsOnConfigReload"));
    ConfigSetConfigHandle(File_Models, arrayModels);
}

/**
 * Called when config is being reloaded.
 */
public ModelsOnConfigReload(ConfigFile:config)
{
    // Reload models config.
    ModelsLoad();
}
