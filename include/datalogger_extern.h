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

#ifndef DATALOGGER_EXTERN_H
#define DATALOGGER_EXTERN_H

// Note: varname should be formatted as modname_varname. dlgr_register(const char* var_name, void* var_data, int var_size);
/**
 * @brief 
 * 
 */
#define DLGR_REGISTER(varname, varsize) dlgr_register(#varname, varsize)

// #varname = "passed_varname", passed_varname_DATASIZE_LOGNUM == file to save log to 
/**
 * @brief 
 * 
 */
#define DLGR_WRITE(varname) dlgr_write(#varname, &varname)

// Returns the number of bytes necessary to perform a read.
// Stores the varname and number for later.
/**
 * @brief 
 * 
 */
#define DLGR_PRIME_READ(varname, number) dlgr_prime_read(#varname, number)

// Will return if DLGR_PRIME_READ wasnt called prior, because
// it unsets the varname and number that was set using _PRIME_.
// Returns if the passed number*varname_size doesnt equal what was calculated / is expected due to the previously registered varname and number using DLGR_PRIME_READ().
// STORAGEPTR MUST BE A POINTER TO VALID MEMORY
// BYTES MUST BE THE NUMBER OF BYTES ALLOCATED FOR THIS READ
/**
 * @brief 
 * 
 */
#define DLGR_PERFORM_READ(storageptr, bytes) dlgr_perform_read(storageptr, bytes)

/**
 * @brief Gets current log index for varname (not the number of logs, old ones may have been deleted).
 * 
 */
#define DLGR_GET_MAX_INDEX(varname) dlgr_get_log_index(#varname)

/**
 * @brief 
 * 
 */
#define DLGR_COUNT_LOGS(varname) dlgr_count_logs(#varname)

#endif // DATALOGGER_EXTERN_H