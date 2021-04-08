/**
 * @file datalogger_extern.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Externally accessible datalogger structures, functions, and variables.
 * @version 0.1
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef DATALOGGER_EXTERN_H
#define DATALOGGER_EXTERN_H

enum ERROR{
    SAVE_OPEN = -1,
    DATA_OPEN = -2,
    SAVE_ACCESS = -3,
    SET_SETTING = -4
};

enum SETTING{
    MAX_FILE_SIZE = 0,
    MAX_DIR_SIZE,
    DO_OVERWRITE
};

// TODO: Will need a lock in implementation
// Writes the passed data to a binary log file.
int dlgr_logData(int size, int* data);

// TODO: WIP
int* dlgr_retrieveData();

// Replacement for dlgr_setMaxFileSize, dlgr_setMaxDirSize, and dlgr_doOverwrite.
// Use the setting_t to determine which setting gets set.
int dlgr_editSettings(int value, int setting, char* directory);

/* Deprecated by dlgr_editSettings(...)
// Sets the maximum log size, in bytes, will create a new log file when this is hit. 
int dlgr_setMaxFileSize(int size_B, char* directory);

// Sets the maximum log directory size, will overwrite logs at dirSize/logSize logs. 
int dlgr_setMaxDirSize(int size_B, char* directory);

// 0 to ignore max directory size and never overwrite, 1 to behave
int dlgr_doOverwrite(int overwrite, char* directory);
*/

#endif // DATALOGGER_EXTERN_H