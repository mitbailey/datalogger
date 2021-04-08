/**
 * @file datalogger.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Implementation of datalogger functions.
 * @version 0.1
 * @date 2021-04-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>

#include "include/datalogger.h"
#include "include/datalogger_extern.h"

/*
.sav file format
BEGIN
log index
max file size (KB)
max dir size (KB)
do overwrite

*/

// data: .data files
// save: Datalogger's personal variable repository.
int logData(int size, int* dataIn){
    // Open the save file and update numbers.
    FILE *save = NULL;
    save = fopen("/sav/values.sav", "r+");

    if (save = NULL){
        return SAVE_OPEN;
    }

    // Index will get the data file number, can only accept up to 999.
    char sIndex[10]; // sIndex = c string index
    char sIndexP1[10]; // sIndex.toInt() + 1
    char sMaxFileSize[10];
    char sMaxDirSize[10];
    char sDoOverwrite[1];

    // Gets the index, maxSizes, and doOverwrite..
    if (fgets(sIndex, 10, (FILE*)save) == NULL
    || fgets(sMaxFileSize, 10, (FILE*)save) == NULL
    || fgets(sMaxDirSize, 10, (FILE*)save) == NULL
    || fgets(sDoOverwrite, 1, (FILE*)save == NULL)){
        return SAVE_ACCESS;
    }

    // Converts the retrieved string to an int, modulos it, and then
    // makes it a c-string again.
    int maxFileSize = atoi(sMaxFileSize);
    int maxDirSize = atoi(sMaxDirSize);
    int doOverwrite = atoi(sDoOverwrite);
    int index = atoi(sIndex) % (maxDirSize/maxFileSize); // 64 * 16KB == 1MB
    itoa(index, sIndex);
    itoa(index+1, sIndexP1);

    // Example Data log file name: "/log/34.data"
    char dataFile[] = "/log/";
    strcat(dataFile, sIndex);
    strcat(dataFile, ".data");

    char dataFileOld[] = "/log/";
    strcat(dataFileOld, sIndexP1);
    strcat(dataFileOld, ".data");

    // ! This is no longer needed because we overwrite 
    // Remove the old file, if it exists.
    // remove(dataFileOld);

    FILE *data = NULL;
    data = fopen(dataFile, "a");

    if (data = NULL){
        return DATA_OPEN;
    }

    long int fileSize = ftell(dataFile);

    // This file has reached its maximum size.
    // Make a new one and iterate the index.
    if (fileSize >= maxFileSize){ // 16KB
        fclose(save);
        save = fopen("/sav/values.sav", "w");
        fprintf(save, "%d", index+1);

        fclose(data);
        data = fopen(dataFileOld, "w"); // This will overwrite the old file.
    }

    // TODO: Here is where we will write the passed data to the now opened .data file.
    // size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
    // size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

    // fwrite(dataIn, elementSize, sizeof(data), data);

    fclose(save);
    fclose(data);
}

// Sets the maximum log size, in bytes, will create a new log file when this is hit. 
int dlgr_setMaxLogSize(int size_KB){
    // Overwrite the value of line 2.
    FILE* save = NULL;
    save = fopen("/sav/values.sav", "r+");
    if (save = NULL){
        return SAVE_OPEN;
    }

    // TODO: Overwrite value.

    fclose(save);
}

// Sets the maximum log directory size, will overwrite logs at dirSize/logSize logs. 
int dlgr_setMaxDirSize(int size_KB){
    // Overwrite the value of line 3.
    FILE* save = NULL;
    save = fopen("/sav/values.sav", "r+");
    if (save = NULL){
        return SAVE_OPEN;
    }

    // TODO: Overwrite value.

    fclose(save);
}

// 0 to ignore max directory size and never overwrite, 1 to behave
int dlgr_doOverwrite(int overwrite){
    // Overwrite the value of line 4.
    FILE* save = NULL;
    save = fopen("/sav/values.sav", "r+");
    if (save = NULL){
        return SAVE_OPEN;
    }

    // TODO: Overwrite value.

    fclose(save);
}