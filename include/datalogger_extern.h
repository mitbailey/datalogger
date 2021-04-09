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
    ERR_INIT = -1,
    ERR_SETTINGS_OPEN = -2,
    ERR_DATA_OPEN = -3,
    ERR_SETTINGS_ACCESS = -4,
    ERR_SET_SETTING = -5,
    ERR_DATA_REMOVE = -6,
    ERR_DEFAULT
};

enum SETTING
{
    MAX_FILE_SIZE = 0,
    MAX_DIR_SIZE,
    DO_OVERWRITE
};

// TODO: May need a lock in implementation
// Writes the passed data to a binary log file.
int dlgr_logData(int size, void *data, char *moduleName);

// TODO: WIP
int *dlgr_retrieveData();

// Replacement for dlgr_setMaxFileSize, dlgr_setMaxDirSize, and dlgr_doOverwrite.
// Use the setting_t to determine which setting gets set.
int dlgr_editSettings(int value, int setting, char *moduleName);

#endif // DATALOGGER_EXTERN_H