/*
 * ============================================================================
 *
 *  Zombie:Reloaded
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
    new bool:success = ConfigLoadConfig(File_Models, arrayModels, PLATFORM_MAX_PATH);
    
    // Unexpected error, stop plugin.
    if (!success)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Unexpected error encountered loading: %s", pathmodels);
    }
    
    new modelcount;
    new modelpublicvalidcount;
    new modelnonpublicvalidcount;
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
        // Get base model path, excluding the public/non-public setting.
        ModelReturnPath(x, modelbase, sizeof(modelbase));
        
        // Explode path into pieces. (separated by "/")
        new strings = ExplodeString(modelbase, "/", baseexploded, sizeof(baseexploded), sizeof(baseexploded[]));
        
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
        
        // Increment variable if model files are valid.
        if (modelfilecount)
        {
            // Increment proper variable.
            if (ModelIsPublic(x))
            {
                modelpublicvalidcount++;
            }
            else
            {
                modelnonpublicvalidcount++;
            }
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
    LogEvent(false, LogType_Normal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "Total: %d | Successful Public: %d | Successful Non-Public: %d | Unsuccessful: %d", modelcount, modelpublicvalidcount, modelnonpublicvalidcount, modelcount - (modelpublicvalidcount + modelnonpublicvalidcount));
    
    // If none of the model paths are valid, then log and fail.
    if (!modelpublicvalidcount)
    {
        LogEvent(false, LogType_Fatal, LOG_CORE_EVENTS, LogModule_Models, "Config Validation", "No usable (public) model paths in %s", pathmodels);
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

/**
 * Checks if a model is public.
 * 
 * @param modelindex    The array index of the model to check.
 * @return              True if public, false if not.
 */
stock bool:ModelIsPublic(modelindex)
{
    // Get the entire model string to parse for what we need.
    decl String:modelsetting[PLATFORM_MAX_PATH + 16];
    GetArrayString(arrayModels, modelindex, modelsetting, sizeof(modelsetting));
    
    // We define this to use as little memory as possible, because the value won't be used.
    decl String:modelpath[1];
    decl String:strpublic[32];
    
    if (StrContains(modelsetting, ";") > -1)
    {
        // Get string index of where the public setting starts.
        new strindex = SplitString(modelsetting, ";", modelpath, sizeof(modelpath));
        
        // Copy setting to new string
        strcopy(strpublic, sizeof(strpublic), modelsetting[strindex]);
        
        // Trim the whitespace.
        TrimString(strpublic);
        
        // If public, return true, non-public returns false.
        return StrEqual(strpublic, "public", false);
    }
    
    // If nothing is specified, assume public.
    return true;
}

/**
 * Returns the path of a given model index.
 * 
 * @param modelindex    The array index of the model.
 * @param modelpath     The output string of the model path.
 * @param maxlen        The maximum length of the output string.
 */
stock ModelReturnPath(modelindex, String:modelpath[], maxlen)
{
    // Get the entire model string to parse for what we need.
    decl String:modelsetting[PLATFORM_MAX_PATH + 16];
    GetArrayString(arrayModels, modelindex, modelsetting, sizeof(modelsetting));
    
    // Copy to path before split just in case the string has no ";"
    strcopy(modelpath, maxlen, modelsetting);
    if (StrContains(modelsetting, ";") > -1)
    {
        SplitString(modelsetting, ";", modelpath, maxlen);
    }
    
    // Trim whitespace.
    TrimString(modelpath);
}

/**
 * Get a random model index in arrayModels, allows you to specify a filter.
 * 
 * @param modelpath     The output string of the model path.
 * @param maxlen        The maximum length of the output string.  
 * @param all           True to choose any of the models in the file, false to use 'publicmodels' param.
 * @param publicmodels  True to find a random public model, false to find non-public.
 */
stock ModelsGetRandomModelIndex(String:modelpath[], maxlen, bool:all = true, bool:publicmodels = true)
{
    new modelindex = -1;
    
    // Return any random model.
    if (all)
    {
        // Get random model index and return the string in it.
        modelindex = GetRandomInt(0, GetArraySize(arrayModels) - 1);
    }
    else
    {
        new Handle:modelsarray = CreateArray(PLATFORM_MAX_PATH);
        decl String:modelsetting[PLATFORM_MAX_PATH];
        
        // x = Array index.
        new size = GetArraySize(arrayModels);
        for (new x = 0; x < size; x++)
        {
            if (publicmodels == ModelIsPublic(x))
            {
                // Transfer model to temp array.
                GetArrayString(arrayModels, x, modelsetting, sizeof(modelsetting));
                PushArrayString(modelsarray, modelsetting);
            }
        }
        
        // y = Array index.
        size = GetArraySize(modelsarray);
        
        // If there are no models then copy a blank string to the output.
        if (size == 0)
        {
            strcopy(modelpath, maxlen, "");
            return;
        }
        
        // Get random model index from the temp list, and return the string in it.
        modelindex = GetRandomInt(0, GetArraySize(modelsarray) - 1);
        
        // Destroy the handle.
        CloseHandle(modelsarray);
    }
    
    // Get the path to the selected model.
    ModelReturnPath(modelindex, modelpath, maxlen);
}