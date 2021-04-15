/**
 * @file datalogger_extern.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Externally accessible datalogger structures, functions, and variables.
 * @version 0.5
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <unistd.h>

#ifndef DATALOGGER_EXTERN_H
#define DATALOGGER_EXTERN_H

enum ERROR
{
    ERR_INIT = -20,
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
    ERR_MODU_OPEN,
    ERR_READ_NUM,
    
    ERR_MISC
};

enum SETTING
{
    MAX_FILE_SIZE = 0,
    MAX_DIR_SIZE
};

/**
 * @brief Logs passed data to a file.
 * 
 * Logs the data passed to it as binary in a .dat file, which is
 * located in /log/<MODULE>/. It follows the settings set in
 * /log/<MODULE>/settings.cfg, which means it will
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
int dlgr_logData(ssize_t size, void *data, char *moduleName);

/**
 * @brief Retrieves logged data.
 * 
 * Pulls information from binary .dat files and places it into output.
 * Use dlgr_getMemorySize() or dlgr_queryMemorySize() to determine the amount
 * of memory that needs to be allocated to store the number of logs that are
 * being requested.
 * 
 * @param output The location in memory where the data will be stored.
 * @param numRequestedLogs How many logs would you like?
 * @param moduleName The name of the caller module.
 * @return int Negative on error (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_retrieveData(char* output, int numRequestedLogs, char *moduleName);

/**
 * @brief Provides the memory size necessary to store some number of logs.
 * 
 * Use this if you know the size of a single log.
 * 
 * @param logSize The size of a single log structure.
 * @param numRequestedLogs The number of logs that will be requested.
 * @return ssize_t The size required to store n-logs.
 */
ssize_t dlgr_queryMemorySize (ssize_t logSize, int numRequestedLogs);

/**
 * @brief Provides the memory size necessary to store some number of logs.
 * 
 * Use this if you do not know the size of a log. This function will try to
 * figure it out itself.
 * 
 * @param numRequestedLogs The number of logs that will be requested.
 * @param moduleName The name of the calling module.
 * @return ssize_t Size of memory needed.
 */
ssize_t dlgr_getMemorySize (int numRequestedLogs, char* moduleName);

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
int dlgr_editSettings(int value, int setting, char *moduleName);

#endif // DATALOGGER_EXTERN_H