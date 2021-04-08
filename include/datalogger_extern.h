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
    INDEX_GET = -3
};

// Will need a lock in implementation
int logData(int size, int* data);

#endif // DATALOGGER_EXTERN_H