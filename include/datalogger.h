/**
 * @file datalogger.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Declarations of datalogger functions and structures.
 * @version 0.5
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

/**
 * @brief Initializes the datalogger and its files.
 * 
 * If files such as index.inf exist, init will set dlgr's variables
 * to match. If they do not exist, init will create them.
 * 
 * @param moduleName Temp. variable, assumption is this is a module name such as "eps".
 * @return int Negative on failure (see: datalogger_extern.h's ERROR enum), 1 on success.
 */
int dlgr_init();

/**
 * @brief A helper function for dlgr_retrieveData
 * 
 * @param output A char* to store the output.
 * @param numRequestedLogs The number of logs to be fetched.
 * @param moduleName The name of the calling module.
 * @param indexOffset Essentially, the number of files we've had to go through already.
 * @return int The number of logs added to output.
 */
int dlgr_retrieve(char* output, int numRequestedLogs, char* moduleName, int indexOffset);

/**
 * @brief Returns the index of the token within buffer.
 * 
 * Searches for the first instance of token within buffer and returns the index of
 * the first character.
 * 
 * @param buffer The character array to be searched.
 * @param bufferSize The size of the buffer.
 * @param token The token to search for within buffer.
 * @param tokenSize The size of the token.
 * @return int Index of the token within buffer on success, ERR_MISC on failure.
 */
int dlgr_indexOf(char *buffer, ssize_t bufferSize, char *token, ssize_t tokenSize);

/**
 * @brief WIP
 * 
 */
void dlgr_destroy();

#endif // DATALOGGER_H