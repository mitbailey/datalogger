/**
 * @file datalogger_tester.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief For testing datalogger
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datalogger.h"
#include "datalogger_extern.h"

int main(){
    printf("Datalogger test start.\n");
    fflush(stdout);

    int testmod_testvar = 0;

    printf("Registering testmod_testvar: %d\n", testmod_testvar);
    fflush(stdout);
    if(DLGR_REGISTER(testmod_testvar, sizeof(testmod_testvar)) < 0){
        return -1;
    }

    printf("Writing testmod_testvar: %d\n", testmod_testvar);
    fflush(stdout);
    while(testmod_testvar < 128){
        if(DLGR_WRITE(testmod_testvar) < 0){
            eprintf("Error!");
            return -1;
        }
        testmod_testvar++;
    }

    printf("Priming read of some testmod_testvars.\n");
    fflush(stdout);
    int bytes = DLGR_PRIME_READ(testmod_testvar, 128);
    unsigned char* read_data = malloc(bytes);
    memset(read_data, 0x0, bytes);

    printf("Reading some testmod_testvars.\n");
    fflush(stdout);
    if(DLGR_PERFORM_READ(read_data, bytes) < 0){
        printf("dlgr read error\n");
    }

    printf("Printing read data:\n");
    fflush(stdout);

    // for(int i = 0; i < bytes; i++){
    //     printf("%c", read_data[i]);
    // }
    // printf("\n");
    // fflush(stdout);

    for(int i = 0; i < bytes; i++){
        printf("%02x ", read_data[i]);
    }
    printf("\n");
    fflush(stdout);

    printf("Number of log files: %d.\n", DLGR_COUNT_LOGS(testmod_testvar));

    printf("Datalogger test end.\n");
    fflush(stdout);
    return 1;
}