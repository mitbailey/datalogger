/**
 * @file datalogger.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Declarations of datalogger functions and structures.
 * @version 0.4
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef DATALOGGER_H
#define DATALOGGER_H

// File and directories cannot exceed these limits.
#define SIZE_FILE_HARDLIMIT 1048576 // 1MB
#define SIZE_DIR_HARDLIMIT 16777216  // 16MB

typedef struct SETTINGS
{
    int maxFileSize;
    int maxDirSize;
} settings_t;

// Initialization
int dlgr_init();

// Helper function for dlgr_retrieveData()
int dlgr_retrieve(char* output, int numRequestedLogs, char* moduleName, int indexOffset);

// Destruction
void dlgr_destroy();

#endif // DATALOGGER_H