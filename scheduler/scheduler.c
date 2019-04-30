
/* 
 * File:   main.c
 * Author: sasinda
 *
 * Created on 30 April 2019, 6:10 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "readyqueue.h"



int generateTaskFile(char *fileName);
int readTaskFile(char *fileName);




int main(int argc, char** argv) {
    //File name and amount of tasks m is taken here.
    printf("Scheduler started!\n");
    
    struct Task tasks[2];
    initialize(tasks, 2);

    struct Task ts1;
    ts1.task_number = 44;
    
    struct Task ts2;
    ts2.task_number = 77;
    
    struct Task ts3;
    ts3.task_number = 99;
    
    int a,b,c; 
    
    a = insert(ts1);
    b = insert(ts2);
    c = insert(ts3);
    
    printf("%d, %d, %d\n", a,b,c);
    
    struct Task *out = pop();
    printf("Popped task#= %d\n", out->task_number);
    
    struct Task *out1 = pop();
    printf("Popped task#= %d\n", out1->task_number);
    
    
   // int *a = (int *)malloc(sizeof(int)*3);
    //generateTaskFile("task_file");  //generates task file.
   // readTaskFile("task_file");

    return 0;
}

//Generates task file, returns 0 on success and less than 0 on failure.
int generateTaskFile(char *fileName){
       
    FILE *pFile = fopen(fileName, "w");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return -1;
    }
    
    int cpu_burst = 0;
    
    //Writes formatted output to the task_file, task_number cpu_burst
    for(int task_number = 0; task_number < 100; task_number++){
        /*
         * Refered to below link to understand use of random number generators.
         * Link: https://www.programmingsimplified.com/c-program-generate-random-numbers
         * Accessed: 30 April 2019
        */
        cpu_burst = rand() % 50 + 1;    
        int status = fprintf(pFile, "%d %d\n", task_number, cpu_burst);
       //printf("cpu_burst %d", cpu_burst);
        if(status < 0){
            printf("writing to file failed");
            return -1;
        }
    }
    
    fclose(pFile);  //closing opened file.
    pFile = NULL; //making sure ref is not there anymore.
    
    return 0;
}

//Reads the taskfile line by line and adds to two dimensional array.
int readTaskFile(char *fileName){
    
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return -1;
    }
    
    int task_number, cpu_burst;
    for(int i = 0; i < 100; i++){
        fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);
        printf("%d %d\n", task_number, cpu_burst);
    }
    
    
    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    return 0;
    
}



