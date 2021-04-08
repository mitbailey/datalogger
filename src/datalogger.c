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


*/

// data: .data files
// save: Datalogger's personal variable repository.
int logData(int size, int* data){
    // Open the save file and update numbers.
    FILE *save = NULL;
    save = fopen("/sav/values.sav", "r+");

    if (save = NULL){
        return SAVE_OPEN;
    }

    // Index will get the data file number, can only accept up to 999.
    char sIndex[3]; // sIndex = c string index

    // Gets the index.
    if (fgets(sIndex, 4, (FILE*)save) == NULL){
        return INDEX_GET;
    }

    // Converts the retrieved string to an int, modulos it, and then
    // makes it a c-string again.
    int iIndex = atoi(sIndex) % 1000;
    itoa(iIndex, sIndex);

    // Data log file name: "/log/34.data"
    char fileName[] = "/log/";
    strcat(fileName, sIndex);
    strcat(fileName, ".data");

    // Remove the old file, if it exists.
    remove(fileName);

    FILE *data = NULL;
    data = fopen(fileName, "a");

    if (data = NULL){
        return DATA_OPEN;
    }

    // TODO: Here is where we will write the passed data to the now opened .data file.
    // size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
    // size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

    // fwrite(data, elementSize, sizeof(data), data);

    fclose(save);
    fclose(data);
}