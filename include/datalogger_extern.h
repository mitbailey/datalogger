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
    ERR_INVALID_INPUT,
    ERR_REREGISTER,
    ERR_MAXLOGSIZE_NOT_SET, // <- If you see this you need to call dlgr_RegisterMaxLogSize(...)!
    ERR_MAXLOGSIZE_EXCEEDED,

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
int dlgr_LogData(char *moduleName, ssize_t size, void *data);

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
int dlgr_RetrieveData(char *moduleName, char *output, int numRequestedLogs);

/**
 * @brief Provides the memory size necessary to store some number of logs.
 * 
 * Use this prior to allocating memory for a pointer in which you want
 * retrieved logs to be stored. For instance, prior to calling dlgr_retrieveData(...)
 * to retrieve 10 logs, you would first malloc whatever size dlgr_queryMemorySize(..., 10)
 * returns.
 * 
 * @param moduleName The name of the calling module.
 * @param numRequestedLogs The number of logs that will be requested for retrieval.
 * @return ssize_t The size necessary to store a number of logs.
 */
ssize_t dlgr_QueryMemorySize(char *moduleName, int numRequestedLogs);

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
int dlgr_EditSettings(char *moduleName, int value, int setting);

/**
 * @brief Defines the maximum log size able to be logged by this module.
 * 
 * Requests to log data must be composed of a data structure of this size. Sizes
 * smaller than max_size are allowable (datalogger provides padding). This
 * value, once set, can not be changed.
 * 
 * @param moduleName The name of the calling module.
 * @param max_size The maximum desired size for a log of data.
 * @return int Negative on failure (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_RegisterMaxLogSize(char *moduleName, ssize_t max_size);

#endif // DATALOGGER_EXTERN_H