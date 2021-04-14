/**
 * @file datalogger.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Implementation of datalogger functions.
 * @version 0.4
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

#include "include/datalogger.h"
#include "include/datalogger_extern.h"

#define FBEGIN_SIZE 6
#define FEND_SIZE 4

char *FBEGIN = {'F', 'B', 'E', 'G', 'I', 'N'};
char *FEND = {'F', 'E', 'N', 'D'};
uint64_t logIndex;
settings_t settingsLoc[1];

// Example Directory
/* datalogger
 * - log
 * - - eps
 * - - - settings.cfg
 * - - - index.inf
 * - - - 0.dat
 * - - - 1.dat
 * - - - 2.dat
 * - - - 3.dat
 * - - - 4.dat
 * - - - 5.dat
 * - - acs
 * - - - settings.cfg
 * - - - index.inf
 * - - - 0.dat
 * - - - 1.dat
 */

// settings.cfg file format (index moved to index.inf)
/* 1. max file size (Bytes)
 * 2. max dir size (Bytes)
 */

// char* moduleName is just a placeholder. Later, we will get the
// module names from somewhere else.

/**
 * @brief Initializes the datalogger and its files.
 * 
 * If files such as index.inf exist, init will set dlgr's variables
 * to match. If they do not exist, init will create them.
 * 
 * @param moduleName Temp. variable, assumption is this is a module name such as "eps".
 * @return int Negative on failure (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_init(char *moduleName)
{
    if (settingsLoc == NULL)
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

    // Creates an initial settings file, or syncs settings
    // if one already exists.
    char settingsFile[20] = "log/";
    strcat(settingsFile, moduleName);
    strcat(settingsFile, "/0.log");
    FILE *settings = NULL;

    // If exists, sync settings, else create a new one.
    if (access(settingsFile, F_OK | R_OK) == 0)
    {
        settings = fopen(settingsFile, "r");

        if (settings = NULL)
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
        settingsLoc->maxFileSize = atoi(sMaxFileSize);
        settingsLoc->maxDirSize = atoi(sMaxDirSize);

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
}

/**
 * @brief Logs passed data to a file.
 * 
 * Logs the data passed to it as binary in a .dat file, which is
 * located in /log/<MODULE>/. It follows the settings set in
 * /log/<MODULE>/settings.cfg, which typically means it will
 * create a new .dat file when the file size exceeds maxFileSize
 * (line 2) and will begin deleting old .dat files when the 
 * directory size exceeds maxDirSize (line 3). It also stores
 * the .dat file's index for naming (ie: 42.dat). Encapsulates
 * each section of written data between FBEGIN and FEND.
 * 
 * @param size The size of the data to be logged.
 * @param dataIn The data to be logged.
 * @param moduleName The calling module's name, a unique directory.
 * @return int Negative on failure (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_logData(int size, void *dataIn, char *moduleName)
{
    // Note: Directory will probably be accessed some other way eventually.

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

    if (data = NULL)
    {
        return ERR_DATA_OPEN;
    }

    // Fetch the current file size.
    long int fileSize = ftell(dataFile);

    // For the adjacent if statement.
    FILE *index = NULL;
    // This file has reached its maximum size.
    // Make a new one and iterate the index.
    if (fileSize >= settingsLoc->maxFileSize)
    { // 16KB
        index = fopen(indexFile, "w");

        if (index = NULL)
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
        strcat(dataFileOld, logIndex - (settingsLoc->maxDirSize / settingsLoc->maxFileSize));
        strcat(dataFileOld, ".dat");

        if (remove(dataFileOld) != 0)
        {
            return ERR_DATA_REMOVE;
        }
    }

    // TODO: Here is where we will write the passed data to the now opened .data file.
    // size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
    // size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

    fwrite(FBEGIN, 6, 1, data);
    fwrite(dataIn, sizeof(dataIn), 1, data);
    fwrite(FEND, 4, 1, data);

    fclose(data);
    sync();
    return 1;
}

// TODO: WIP
int dlgr_retrieveData(int numRequestedStructs, char *moduleName)
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

    // First, construct the directories.
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

    // We will receive a request for some amount of data.
    // ASSUMING this is passed as the number of module-specific structures desired...
    // 1. Find the size of a structure.
    // 2. Go to EOF - (sizeof(structure) + FEND_SIZE)
    // 3. Read sizeof(structure) worth of data
    // 4. Move up the file (2*sizeof(structure) + FBEGIN_SIZE + FEND_SIZE)
    // 5. Go to step 3, repeat until start of file OR read number of structures
    // 6. If we still have more to read, get the previous file and repeat.

    // fseek(data, -8, SEEK_CUR); <-- Brings us to 8 bytes before where we are now.

    // Read the entire file into memory buffer. One byte a time for sizeof(buffer) bytes.
    if (fread(buffer, 0x1, fileSize, data) != 1)
    {
        return ERR_DATA_READ;
    }

    // Determine what the size of one structure is.
    // 1. Find the address of the first occurrence of "FEND".
    // 2. Subtract the address of the c-string from the address of FEND to get the
    //    size of a structure.
    // 3. Use memcopy to extract structSize bytes at every FBEGIN+FBEGIN_SIZE
    int iFEND = dlgr_indexOf(buffer, fileSize, FEND, (ssize_t)4);
    char *pFEND = buffer + iFEND;

    // Set the structSize
    ssize_t structSize = pFEND - (buffer + FBEGIN_SIZE);

    // Create and allocate memory for an output c-string.
    char *output = NULL;
    output = malloc((FBEGIN_SIZE + FEND_SIZE + structSize) * numRequestedStructs);

    if (output == NULL)
    {
        return ERR_MALLOC;
    }

    // Create a cursor to mark our current position in the buffer. Pull it one structure from the back.
    char *bufferCursor = buffer + fileSize;
    bufferCursor -= structSize - FBEGIN_SIZE + FEND_SIZE;

    // To keep track of the number of structures we've added to the output.
    int numReadStructs = 0;

    // If we havent reached the beginning of the buffer, keep getting structures.
    while (buffer - bufferCursor != 0 && numReadStructs < numRequestedStructs)
    {
        // Copies one structure to the output. Contains delimiters and trace amounts of tree nuts.
        memcpy(output, buffer, structSize + FBEGIN_SIZE + FEND_SIZE);
        numReadStructs++;
    } // TODO: Currently we get here if the requested amount > amount in the file...need to search next file.

    // TODO: Frees may need to move.
    fclose(data);
    free(buffer);
    free(output);

    // TODO: Should return a character array, specifically the output buffer.
    return 1;
}

/**
 * @brief Used to edit settings.cfg.
 * 
 * This function sets a setting to a value within a module's own datalogger directory.
 * 
 * @param value The value to be written.
 * @param setting The setting to edit (see: datalogger_extern.h's SETTING enum).
 * @param directory The calling module's name, a unique directory.
 * @return int Negative on failure (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_editSettings(int value, int setting, char *directory)
{
    // Change settingsLoc values.
    // Overwrite settings.cfg file and write new values in.

    // Change settingsLoc values.
    switch (setting)
    {
    case MAX_FILE_SIZE:
        if (value > SIZE_FILE_HARDLIMIT || value < 1)
        {
            return ERR_SETTINGS_SET;
        }

        settingsLoc->maxFileSize = value;

        break;
    case MAX_DIR_SIZE:
        // Memory amount out-of-bounds.
        if (value > SIZE_DIR_HARDLIMIT || value < 1)
        {
            return ERR_SETTINGS_SET;
        }

        settingsLoc->maxDirSize = value;

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

    if (settings = NULL)
    {
        return ERR_SETTINGS_OPEN;
    }

    // Write everything back into the file.
    fprintf(settings, "%d\n", settingsLoc->maxFileSize);
    fprintf(settings, "%d\n", settingsLoc->maxDirSize);

    fclose(settings);
    sync();

    return 1;
}

void dlgr_destroy()
{
    // TODO: Add frees
}

// Helper functions

// Returns the index of the beginning of token in buffer is present, -1 if it fails.
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