
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
int getTwoTasks(char *fileName);

int main(int argc, char** argv) {
    //File name and amount of tasks m is taken here.
    printf("Scheduler started!\n");
   /*
    struct Task tasks[5];
    initialize(tasks, 3);

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
    
    printf("%d, %d, %d\n", a,b,c); */ 
    
    while(isEmpty()==0){
          struct Task *out = pop();
    printf("Popped task#= %d\n", out->task_number);
    }
    
    
    for(int i=0; i<2; i++){
        getTwoTasks("task_file");
    }
    
   
    
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

long fileReadHead;

//Reads the taskfile 2 tasks at a time.
int getTwoTasks(char *fileName){
    
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return -1;
    }
    
    fseek(pFile, 0, SEEK_END);  //moving file position indicator to the end.
   // printf("SIZE  %ld\n", ftell(pFile));    
    long endOfFile = ftell(pFile);  //current position (i.e. eof) of file position indicator.
    
    if(fileReadHead != endOfFile){      //if file position indicatior 'fileReadHead' is not at the end of file.

        fseek(pFile, fileReadHead, SEEK_SET);  //move file position indicator to last left off position.         

        int task_number, cpu_burst;

        for(int i = 0; i < 2; i++){
            fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);  //moves file position indicator by 2 lines, (for loop).
            printf("%d %d\n", task_number, cpu_burst);
        }
    }else{
        printf("END OF FILE REACHED\n");
    }

    fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.

    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    return 0;
    
}



