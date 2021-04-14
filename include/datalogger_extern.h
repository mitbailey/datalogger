/**
 * @file datalogger_extern.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Externally accessible datalogger structures, functions, and variables.
 * @version 0.3
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef DATALOGGER_EXTERN_H
#define DATALOGGER_EXTERN_H

enum ERROR
{
    ERR_INIT = -12,
    ERR_SETTINGS_OPEN,
    ERR_SETTINGS_ACCESS,
    ERR_SETTINGS_SET,
    ERR_DATA_OPEN,
    ERR_DATA_REMOVE,
    ERR_DATA_READ,
    ERR_DEFAULT_CASE,
    ERR_FILE_DNE,
    ERR_MALLOC,
    ERR_LOG_SIZE,
    
    ERR_MISC
};

enum SETTING
{
    MAX_FILE_SIZE = 0,
    MAX_DIR_SIZE
};

// TODO: May need a lock in implementation
// Writes the passed data to a binary log file.
int dlgr_logData(ssize_t size, void *data, char *moduleName);

// TODO: WIP
int dlgr_retrieveData(char* output, int numRequestedLogs, char *moduleName);

ssize_t dlgr_queryMemorySize (ssize_t logSize, int numRequestedLogs);

ssize_t dlgr_getMemorySize (int numRequestedLogs, char* moduleName);

// Replacement for dlgr_setMaxFileSize, dlgr_setMaxDirSize, and dlgr_doOverwrite.
// Use the setting_t to determine which setting gets set.
int dlgr_editSettings(int value, int setting, char *moduleName);

#endif // DATALOGGER_EXTERN_H