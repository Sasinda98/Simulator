/* 
 * File:   simulationLog.h
 * Author: W.A.A.D Gayal Sasinda Rupasinghe
 */

/*
 * The purpose of this header is to hold functions related to simulation log file operations.
 */

#include <unistd.h>
#include "timeTask.h"

//Adds record to simulation log about number of tasks serviced, average waiting time and average turnaround time.
void addMainTerminationLog(int num_tasks_serviced, double waitingTime, double turnaroundTime){
    double avgWaitingTime = waitingTime / (double) num_tasks_serviced;
    double avgTurnAroundTime = turnaroundTime / (double) num_tasks_serviced;

    FILE *pFile = fopen("simulation_log", "a");    //open for writing, appending.

    if(pFile == NULL){
        char temp[3];
        printf("ERROR: Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(-1);
    }

    int status = fprintf(pFile, "Number of tasks: %d\nAverage waiting time: %0.1f seconds\nAverage turnaround time: %0.1f seconds\n", num_tasks_serviced, avgWaitingTime, avgTurnAroundTime);

    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("ERROR: Writing/creating to simulation_log file failed, Application will quit...\n");
        exit(-1);
    }

    fclose(pFile);
    pFile = NULL;
}

//Adds record to simulation log containing task info, task number and arrival time. To be used in task().
void addSimulationLog_Task(struct Task task){

    FILE *pFile = fopen("simulation_log", "a");     //open for writing, appending.        .

    if(pFile == NULL){
        char temp[3];
        printf("ERROR: Failed to open file, press any key followed by enter key to exit.\n");
        scanf("%s", temp);
        exit(-1);
    }

    int status = fprintf(pFile, "task #: %d\nArrival time: %s\n\n", task.task_number, task.arrival_time);

    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("ERROR: Writing/creating to simulation_log file failed, Application will quit...\n");
        exit(-1);
    }

    fclose(pFile);
    pFile = NULL;
}

//Gets time of task thread termination, number of tasks executed by task thread and logs it to simulation_log file.
void addTaskTerminationLog(int num_tasks_inserted){

    char *currentTime = getCurrentTime(); //obtaining current time in full format.
    format_time(currentTime); //getting only the time

    FILE *pFile = fopen("simulation_log", "a");    //open for writing, appending.

    if(pFile == NULL){
        char temp[3];
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.\n");
        scanf("%s", temp);
        exit(-1);
    }

    int status = fprintf(pFile, "Number of asks put in to Ready-Queue: %d\nTerminate at time: %s\n\n", num_tasks_inserted, currentTime);

    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("ERROR: Writing/creating to simulation_log file failed, Application will quit...\n");
        exit(-1);
    }
    free(currentTime);
    fclose(pFile);
    pFile = NULL;
}


//Adds record to simulation log containing task info -: cpu#, task number, arrival time and service time. 
//To be used in cpu().
void addSimulationLog_Pre_Exec(struct Task task, char *service_time, int *cpuId){

    FILE *pFile = fopen("simulation_log", "a");     //open for writing, appending.

    if(pFile == NULL){
        char temp[3];
        printf("ERROR: Failed to open/create simulation_log file, press any key followed by enter key to exit.\n");
        scanf("%s", temp);
        exit(0);
    }

    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nService time: %s\n\n", *cpuId, task.task_number, task.arrival_time, service_time);
    //printf("cpu_burst %d", cpu_burst);
    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("ERROR: Writing/creating to simulation_log file failed, Application will quit...\n");
        exit(-1);
    }

    fclose(pFile);
    pFile = NULL;
}

//Adds record to simulation_log containing cpu execution info (i.e. cpu num, completion time) and task info (i.e. arrival times and task num).
//To be used in cpu().
void addSimulationLog_Post_Exec(struct Task task, char *completion_time, int *cpuId){

    FILE *pFile = fopen("simulation_log", "a");     //open for writing, appending.

    if(pFile == NULL){
        char temp[3];
        printf("ERROR: Failed to open/create simulation_log file, press any key followed by enter key to exit.\n");
        scanf("%s", temp);
        exit(0);
    }

    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nCompletion time: %s\n\n", *cpuId, task.task_number, task.arrival_time, completion_time);

    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("ERROR: Writing/creating to simulation_log file failed, Application will quit...\n");
        exit(-1);
    }

    fclose(pFile);
    pFile = NULL;

}

//Adds how many tasks each cpu thread executed in to the simulation_log file.
void addCpuTerminationLog(int num_tasks_inserted, int cpuId){

    FILE *pFile = fopen("simulation_log", "a");    //open for writing, appending.

    if(pFile == NULL){
        char temp[3];
        printf("ERROR: Failed to open/create simulation_log file, press any key followed by enter key to exit.\n");
        scanf("%s", temp);
        exit(0);
    }

    int status = fprintf(pFile, "CPU-%d terminates after servicing %d tasks.\n\n", cpuId, num_tasks_inserted);

    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("ERROR: Writing/creating to simulation_log file failed, Application will quit...\n");
        exit(-1);
    }

    fclose(pFile);
    pFile = NULL;
}