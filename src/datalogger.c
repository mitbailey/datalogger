/**
 * @file datalogger.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Implementation of datalogger functions.
 * @version 0.6
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "datalogger.h"
#include "datalogger_extern.h"

#define FBEGIN_SIZE 6
#define FEND_SIZE 4

char FBEGIN[6] = {'F', 'B', 'E', 'G', 'I', 'N'};
char FEND[4] = {'F', 'E', 'N', 'D'};
uint64_t logIndex;
settings_t localSettings[1];
ssize_t moduleLogSize;

// Example Directory
/* datalogger
 * - log
 * - - eps
 * - - - settings.cfg
 * - - - index.inf
 * - - - module.inf
 * - - - 0.dat
 * - - - 1.dat
 * - - - 2.dat
 * - - - 3.dat
 * - - - 4.dat
 * - - - 5.dat
 * - - acs
 * - - - settings.cfg
 * - - - index.inf
 * - - - module.inf
 * - - - 0.dat
 * - - - 1.dat
 */

// settings.cfg file format (index moved to index.inf)
/* 1. max file size (Bytes)
 * 2. max dir size (Bytes)
 */

// char* moduleName is just a placeholder. Later, we will get the
// module names from somewhere else.

int dlgr_init(char *moduleName)
{
    // Set the module log size to -1 until we know how large one is.
    moduleLogSize = -1;

    // Get the size of this module's log if one was already written.
    char moduleFile[20] = "log/";
    strcat(moduleFile, moduleName);
    strcat(moduleFile, "/module.inf");

    FILE *modu = NULL;

    char* sLogSize;
    // If a module.inf file does not exist, create one and put in this module's log's size.
    if (access(moduleFile, F_OK | R_OK) == 0){
        modu = fopen(moduleFile, "r");
        fgets(sLogSize, 20, (FILE *)index);
        moduleLogSize = atoi(sLogSize);
    }

    fclose(modu);

    if (localSettings == NULL)
    {
        return ERR_INIT;
    }

    // Create index.inf, with a single 0, for each module.
    // Create an initial log file, 0.dat, for each module.
    // TODO: No list of module names yet.

    // Creates index.inf
    char indexFile[20] = "log/";
    strcat(indexFile, moduleName);
    strcat(indexFile, "/index.inf");
    FILE *index = NULL;

    // Note: access() returns 0 on success.
    // If an index file exists, update our index to match.
    // Otherwise, make an index file with index = 0.
    if (access(indexFile, F_OK | R_OK) == 0)
    {
        // An index file already exists.
        // Set our logIndex to the value in index.inf
        index = fopen(indexFile, "r");
        char sIndex[20];
        fgets(sIndex, 20, (FILE *)index);
        logIndex = atoi(sIndex);
        fclose(index);
    }
    else
    {
        index = fopen(indexFile, "w");
        fprintf(index, "0\n");
        fclose(index);
        sync();
    }

    // Creates an initial log file, 0.dat
    char dataFile[20] = "log/";
    strcat(dataFile, moduleName);
    strcat(dataFile, "/0.log");
    FILE *data = NULL;

    if (access(dataFile, F_OK | R_OK) != 0)
    {
        // An initial data file does not already exist.
        data = fopen(dataFile, "wb");
        fclose(data);
    }

    // Creates an initial settings file, or syncs localSettings if one already exists.
    char settingsFile[20] = "log/";
    strcat(settingsFile, moduleName);
    strcat(settingsFile, "/0.log");
    FILE *settings = NULL;

    // If exists, sync settings, else create a new one.
    if (access(settingsFile, F_OK | R_OK) == 0)
    {
        settings = fopen(settingsFile, "r");

        if (settings == NULL)
        {
            return ERR_SETTINGS_OPEN;
        }

        char sMaxFileSize[10];
        char sMaxDirSize[10];

        // Gets the index, maxSizes
        if (fgets(sMaxFileSize, 10, (FILE *)settings) == NULL || fgets(sMaxDirSize, 10, (FILE *)settings) == NULL)
        {
            return ERR_SETTINGS_ACCESS;
        }

        // Converts the retrieved string to an int.
        localSettings->maxFileSize = atoi(sMaxFileSize);
        localSettings->maxDirSize = atoi(sMaxDirSize);

        fclose(settings);
    }
    else
    {
        // No settings exists, create one and fill it with defaults.
        settings = fopen(settingsFile, "w");
        fprintf(settings, "8192");    // 8KB
        fprintf(settings, "4194304"); // 4 MB
        fprintf(settings, "1");
        fclose(settings);
        sync();
    }

    return 1;
}

int dlgr_logData(ssize_t size, void *dataIn, char *moduleName)
{
    // Note: Directory will probably be accessed some other way eventually.

    // Set the size of this module's log.
    char moduleFile[20] = "log/";
    strcat(moduleFile, moduleName);
    strcat(moduleFile, "/module.inf");

    FILE *modu = NULL;

    // If a module.inf file does not exist, create one and put in this module's log's size.
    if (access(moduleFile, F_OK | R_OK) != 0){
        modu = fopen(moduleFile, "w");

        if (modu == NULL)
        {
            return ERR_MODU_OPEN;
        }

        fprintf(modu, "%d\n", size);
        moduleLogSize = size;
    }

    fclose(modu);

    // Constructs the index.inf directory => indexFile.
    char indexFile[20] = "log/";
    strcat(indexFile, moduleName);
    strcat(indexFile, "/index.inf");

    // Constructs the n.dat directory.
    char dataFile[20] = "log/";
    strcat(dataFile, moduleName); // "/log/eps"
    strcat(dataFile, "/");        // "/log/eps/"
    strcat(dataFile, logIndex);   // "/log/eps/42"
    strcat(dataFile, ".dat");     // "/log/eps/42.dat"

    // Constructs the n+1.dat directory.
    char dataFileNew[20] = "log/";
    strcat(dataFileNew, moduleName);   // "/log/eps"
    strcat(dataFileNew, "/");          // "/log/eps/"
    strcat(dataFileNew, logIndex + 1); // "/log/eps/43"
    strcat(dataFileNew, ".dat");       // "/log/eps/43.dat"

    // Open the current data (.dat) file in append-mode.
    FILE *data = NULL;
    data = fopen(dataFile, "ab");

    if (data == NULL)
    {
        return ERR_DATA_OPEN;
    }

    // Fetch the current file size.
    long int fileSize = ftell(dataFile);

    // For the adjacent if statement.
    FILE *index = NULL;
    // This file has reached its maximum size.
    // Make a new one and iterate the index.
    if (fileSize >= localSettings->maxFileSize)
    { // 16KB
        index = fopen(indexFile, "w");

        if (index == NULL)
        {
            return ERR_SETTINGS_OPEN;
        }

        // Iterate the index.
        logIndex++;

        // Rewrite the index file.
        fprintf(index, "%d\n", logIndex);

        fclose(data);
        fclose(index);
        sync();

        // Replace data pointer to the new data file.
        data = fopen(dataFileNew, "wb");

        // Delete an old file.
        char dataFileOld[20] = "log/";
        strcat(dataFileOld, moduleName);
        strcat(dataFileOld, "/");
        strcat(dataFileOld, logIndex - (localSettings->maxDirSize / localSettings->maxFileSize));
        strcat(dataFileOld, ".dat");

        if (remove(dataFileOld) != 0)
        {
            return ERR_DATA_REMOVE;
        }
    }

    // Write FBEGIN, the data, and then FEND to the binary .dat file.
    fwrite(FBEGIN, 6, 1, data);
    fwrite(dataIn, sizeof(dataIn), 1, data);
    fwrite(FEND, 4, 1, data);

    fclose(data);
    sync();
    return 1;
}

int dlgr_retrieveData(char* output, int numRequestedLogs, char *moduleName)
{
    /*
     * This should return sets of data from the binary .dat files irregardless of what file its in,
     * for as much as is requested.
     *
     */

    // Assume:      The size of every item within a .dat file is the same size. Note that,
    //              for instance, eps and acs will not have equally sized data.
    // Consider:    A header at the start of the file containing the size of each item.
    // Remember:    .dat binary files look like...

    /* FBEGIN
     * < Some module-specific data of size n >
     * FEND
     */

    // Check if the module log size is already defined. 
    // If not, nothing has been logged yet, so we cannot retrieve.
    if (moduleLogSize < 0){
        return ERR_LOG_SIZE;
    }

    // To keep track of the number of logged structures we've added to the output.
    int numReadLogs = 0;
    
    // How many files we have to look 'back'.
    int indexOffset = 0;

    int errorCheck = 0;

    // Where the magic happens.
    while (numReadLogs < numRequestedLogs){
        if(errorCheck == dlgr_retrieve(output, numRequestedLogs-numReadLogs, moduleName, indexOffset) < 0){
            return errorCheck;
        }
        numReadLogs += errorCheck;
        indexOffset++;
    }
    
    // Check if we got the right amount of logs.
    if (numReadLogs != numRequestedLogs){
        return ERR_READ_NUM;
    }

    return 1;
}

int dlgr_retrieve(char* output, int numRequestedLogs, char* moduleName, int indexOffset){

    int numReadLogs = 0;

    // First, construct the directories.
    char dataFile[20] = "log/";
    strcat(dataFile, moduleName);
    strcat(dataFile, "/");
    strcat(dataFile, logIndex - indexOffset);
    strcat(dataFile, ".dat");

    FILE *data = NULL;
    data = fopen(dataFile, "rb");

    if (data == NULL)
    {
        return ERR_DATA_OPEN;
    }

    // Find the total size of the .dat file.
    fseek(data, 0, SEEK_END);
    ssize_t fileSize = ftell(data);
    fseek(data, 0, SEEK_SET);

    // Create a memory buffer of size fileSize
    char *buffer = NULL;
    buffer = malloc(fileSize + 1);

    if (buffer == NULL)
    {
        return ERR_MALLOC;
    }

    // Read the entire file into memory buffer. One byte a time for sizeof(buffer) bytes.
    if (fread(buffer, 0x1, fileSize, data) != 1)
    {
        return ERR_DATA_READ;
    }

    // Create a cursor to mark our current position in the buffer. Pull it one structure from the back.
    char *bufferCursor = buffer + fileSize;
    bufferCursor -= moduleLogSize - FBEGIN_SIZE + FEND_SIZE;

    // If we havent reached the beginning of the buffer, keep getting structures.
    while (buffer - bufferCursor != 0 && numReadLogs < numRequestedLogs)
    {
        // Copies one structure to the output. Contains delimiters and trace amounts of tree nuts.
        memcpy(output, buffer, moduleLogSize + FBEGIN_SIZE + FEND_SIZE);
        numReadLogs++;
    }

    fclose(data);
    free(buffer);

    return numReadLogs;
}

ssize_t dlgr_queryMemorySize (ssize_t logSize, int numRequestedLogs){
    return numRequestedLogs * (logSize + FBEGIN_SIZE + FEND_SIZE);
}

ssize_t dlgr_getMemorySize (int numRequestedLogs, char* moduleName){
    
    if (moduleLogSize > 0) {
        return numRequestedLogs * (moduleLogSize + FBEGIN_SIZE + FEND_SIZE);
    }    

    // Should never get here...leaving it for now.
    char dataFile[20] = "log/";
    strcat(dataFile, moduleName);
    strcat(dataFile, "/");
    strcat(dataFile, logIndex);
    strcat(dataFile, ".dat");

    FILE *data = NULL;
    data = fopen(dataFile, "rb");

    if (data == NULL)
    {
        return ERR_DATA_OPEN;
    }

    // Find the total size of the .dat file.
    fseek(data, 0, SEEK_END);
    ssize_t fileSize = ftell(data);
    fseek(data, 0, SEEK_SET);

    // Create a memory buffer of size fileSize
    char *buffer = NULL;
    buffer = malloc(fileSize + 1);

    if (buffer == NULL)
    {
        return ERR_MALLOC;
    }

    // Read the entire file into memory buffer. One byte a time for sizeof(buffer) bytes.
    if (fread(buffer, 0x1, fileSize, data) != 1)
    {
        return ERR_DATA_READ;
    }

    // Determine what the size of one structure is.
    // 1. Find the address of the first occurrence of "FEND".
    // 2. Subtract the address of the c-string from the address of FEND to get the
    //    size of a structure.
    // 3. Use memcopy to extract logSize bytes at every FBEGIN+FBEGIN_SIZE
    int iFEND = dlgr_indexOf(buffer, fileSize, FEND, (ssize_t)4);
    char *pFEND = buffer + iFEND;

    // Set the logSize
    ssize_t logSize = pFEND - (buffer + FBEGIN_SIZE);

    fclose(data);
    free(buffer);

    return numRequestedLogs * (logSize + FBEGIN_SIZE + FEND_SIZE);    
}

int dlgr_editSettings(int value, int setting, char *directory)
{
    // Change localSettings values.
    // Overwrite settings.cfg file and write new values in.

    // Change localSettings values.
    switch (setting)
    {
    case MAX_FILE_SIZE:
        if (value > SIZE_FILE_HARDLIMIT || value < 1)
        {
            return ERR_SETTINGS_SET;
        }

        localSettings->maxFileSize = value;

        break;
    case MAX_DIR_SIZE:
        // Memory amount out-of-bounds.
        if (value > SIZE_DIR_HARDLIMIT || value < 1)
        {
            return ERR_SETTINGS_SET;
        }

        localSettings->maxDirSize = value;

        break;
    default:
        return ERR_DEFAULT_CASE;
    }

    // Constructs the settings.cfg directory into settingsFile.
    char settingsFile[20] = "log/";
    strcat(settingsFile, directory);       // "/log/eps"
    strcat(settingsFile, "/settings.cfg"); // "/log/eps/settings/cfg"

    // Open the settings file and update numbers.
    FILE *settings = NULL;
    settings = fopen(settingsFile, "w");

    if (settings == NULL)
    {
        return ERR_SETTINGS_OPEN;
    }

    // Write everything back into the file.
    fprintf(settings, "%d\n", localSettings->maxFileSize);
    fprintf(settings, "%d\n", localSettings->maxDirSize);

    fclose(settings);
    sync();

    return 1;
}

void dlgr_destroy()
{
    // TODO: Add frees
}

// Helper functions

int dlgr_indexOf(char *buffer, ssize_t bufferSize, char *token, ssize_t tokenSize)
{
    int i = 0;
    while (i + tokenSize < bufferSize)
    {
        int score = 0;
        for (int ii = 0; ii < tokenSize; ii++)
        {
            if (buffer[i + ii] == token[ii])
            {
                score++;
            }
        }
        if (score == tokenSize)
        {
            return i;
        }
    }
    return ERR_MISC;
}