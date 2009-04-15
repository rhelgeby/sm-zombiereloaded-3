/**
 * ====================
 *   Zombie:Reloaded
 *   File: models.inc
 *   Author: Greyscale
 * ==================== 
 */

new String:modelSuffix[8][16] = {".dx80.vtx", ".dx90.vtx", ".mdl", ".phy", ".sw.vtx", ".vvd", ".xbox", ".xbox.vtx"};

new Handle:arrayModels = INVALID_HANDLE;

FileLinesToArray(Handle:array, const Handle:file)
{
    ClearArray(array);
    
    decl String:line[128];
    
    while(!IsEndOfFile(file) && ReadFileLine(file, line, sizeof(line)))
    {
        if (StrContains(line, ";") == -1)
        {
            if (StrContains(line, "//") > -1)
            {
                SplitString(line, "//", line, sizeof(line));
            }
            TrimString(line);
            
            if (!StrEqual(line, "", false))
            {
                PushArrayString(array, line);
            }
        }
    }
}

LoadModelData()
{
    decl String:path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), "configs/zr/models.txt");
    
    if (arrayModels != INVALID_HANDLE)
    {
        CloseHandle(arrayModels);
    }
    
    arrayModels = CreateArray(256, 0);
    
    new Handle:fileModels = OpenFile(path, "r");
    if (fileModels == INVALID_HANDLE)
    {
        SetFailState("\"%s\" missing from server", path);
    }
    
    FileLinesToArray(arrayModels, fileModels);
    
    if (!GetArraySize(arrayModels))
    {
        SetFailState("No models listed in models.txt, please add some models then restart");
    }
    
    decl String:model[256];
    decl String:modelpath[256];
    
    new modelsize = GetArraySize(arrayModels);
    for (new x = 0; x < modelsize; x++)
    {
        for (new y = 0; y < 8; y++)
        {
            GetArrayString(arrayModels, x, model, sizeof(model));
            Format(modelpath, sizeof(modelpath), "%s%s", model, modelSuffix[y]);
            if (FileExists(modelpath))
            {
                PrecacheModel(modelpath);
                AddFileToDownloadsTable(modelpath);
            }
        }
    }
  
    CloseHandle(fileModels);
}

LoadDownloadData()
{
    decl String:path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), "configs/zr/downloads.txt");
    
    new Handle:fileDownloads = OpenFile(path, "r");
    
    if (fileDownloads == INVALID_HANDLE)
    {
        SetFailState("\"%s\" missing from server", path);
    }
    
    new Handle:arrayDownloads = CreateArray(256, 0);
    
    FileLinesToArray(arrayDownloads, fileDownloads);
    
    decl String:file[256];
    
    new downloadsize = GetArraySize(arrayDownloads);
    for (new x = 0; x < downloadsize; x++)
    {
        GetArrayString(arrayDownloads, x, file, sizeof(file));
        if (FileExists(file))
        {
            AddFileToDownloadsTable(file);
        }
        else
        {
            ZR_LogMessage("File load failed", file);
        }
    }
  
    CloseHandle(fileDownloads);
    CloseHandle(arrayDownloads);
}
