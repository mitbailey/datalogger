/**
 * @file datalogger.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Implementation of datalogger functions.
 * @version 0.3
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "include/datalogger.h"
#include "include/datalogger_extern.h"

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
 * 3. do overwrite
 */

// char* directory is just a placeholder. Later, we will get the
// module names from somewhere else.

/**
 * @brief Initializes the datalogger and its files.
 * 
 * If files such as index.inf exist, init will set dlgr's variables
 * to match. If they do not exist, init will create them.
 * 
 * @param directory Temp. variable, assumption is this is a module name such as "eps".
 * @return int Negative on failure (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_init(char *directory)
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
    strcat(indexFile, directory);
    strcat(indexFile, "/index.inf");
    FILE *index = NULL;

    if (access(indexFile, F_OK | R_OK))
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
    strcat(dataFile, directory);
    strcat(dataFile, "/0.log");
    FILE *data = NULL;

    if (access(dataFile, F_OK | R_OK))
    {
        // An initial data file already exists.
    }
    else
    {
        data = fopen(dataFile, "wb");
        fclose(data);
    }

    // Creates an initial settings file, or syncs settings
    // if one already exists.
    char settingsFile[20] = "log/";
    strcat(settingsFile, directory);
    strcat(settingsFile, "/0.log");
    FILE *settings = NULL;

    // If exists, sync settings, else create a new one.
    if (access(settingsFile, F_OK | R_OK))
    {
        settings = fopen(settingsFile, "r");

        if (settings = NULL)
        {
            return ERR_SETTINGS_OPEN;
        }

        char sMaxFileSize[10];
        char sMaxDirSize[10];
        char sDoOverwrite[1];

        // Gets the index, maxSizes, and doOverwrite..
        if (fgets(sMaxFileSize, 10, (FILE *)settings) == NULL || fgets(sMaxDirSize, 10, (FILE *)settings) == NULL || fgets(sDoOverwrite, 1, (FILE *)settings) == NULL)
        {
            return ERR_SETTINGS_ACCESS;
        }

        // Converts the retrieved string to an int.
        settingsLoc->maxFileSize = atoi(sMaxFileSize);
        settingsLoc->maxDirSize = atoi(sMaxDirSize);
        settingsLoc->doOverwrite = atoi(sDoOverwrite);

        fclose(settings);
    }
    else
    {
        // No settings exists, create one and fill it with defaults.
        settings = fopen(settingsFile, "w");
        fprintf(settings, "8192"); // 8KB
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
 * (line 2) and will begin overwriting old .dat files when the 
 * directory size exceeds maxDirSize (line 3). It also stores
 * the .dat file's index for naming (ie: 42.dat).
 * 
 * @param size The size of the data to be logged.
 * @param dataIn The data to be logged.
 * @param directory The calling module's name, a unique directory.
 * @return int Negative on failure (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_logData(int size, int *dataIn, char *directory)
{
    // Note: Directory will probably be accessed some other way eventually.

    // Constructs the index.inf directory => indexFile.
    char indexFile[20] = "log/";
    strcat(indexFile, directory);   
    strcat(indexFile, "/index.inf");

    // Constructs the n.dat directory.
    char dataFile[20] = "log/";
    strcat(dataFile, directory); // "/log/eps"
    strcat(dataFile, "/");       // "/log/eps/"
    strcat(dataFile, logIndex);  // "/log/eps/42"
    strcat(dataFile, ".dat");    // "/log/eps/42.dat"

    // Constructs the n+1.dat directory.
    char dataFileOld[20] = "log/";
    strcat(dataFileOld, directory);    // "/log/eps"
    strcat(dataFileOld, "/");          // "/log/eps/"
    strcat(dataFileOld, logIndex + 1); // "/log/eps/43"
    strcat(dataFileOld, ".dat");       // "/log/eps/43.dat"

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

        // Rewrite the entire file.
        fprintf(index, "%d\n", logIndex);

        fclose(data);
        fclose(index);
        sync();
        data = fopen(dataFileOld, "wb"); // This will overwrite the old file.
    }

    // TODO: Here is where we will write the passed data to the now opened .data file.
    // size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
    // size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

    fwrite(dataIn, sizeof(data), 1, data);

    fclose(data);
    sync();
    return 1;
}

// TODO: WIP
int *dlgr_retrieveData()
{
}

// TODO: set max etc can probably be condensed into a edit settings function... ?

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
            return ERR_SET_SETTING;
        }

        settingsLoc->maxFileSize = value;

        break;
    case MAX_DIR_SIZE:
        // Memory amount out-of-bounds.
        if (value > SIZE_DIR_HARDLIMIT || value < 1)
        {
            return ERR_SET_SETTING;
        }

        settingsLoc->maxDirSize = value;

        break;
    case DO_OVERWRITE:
        // Psuedo-boolean value not binary.
        if (value != 0 && value != 1)
        {
            return ERR_SET_SETTING;
        }

        settingsLoc->doOverwrite = value;

        break;
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
    fprintf(settings, "%d\n", settingsLoc->doOverwrite);

    fclose(settings);
    sync();

    return 1;
}

void dlgr_destroy()
{
    // TODO: Add frees
}