
/* 
 * File:   main.c
 * Author: W.A.A.D Gayal Sasinda Rupasinghe
 *
 * Created on 30 April 2019, 6:10 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "readyqueue.h"
#include <string.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>


//FUNCTION PROTOTYPES...........................................................
int generateTaskFile(char *fileName);
int readTaskFile(char *fileName);

void *task(void *fileName);
void format_time(char *output);
char *getCurrentTime();
double  getTimeElapsed();
int addSimulationLog_Task(struct Task task);
void *cpu( void *arg);
struct Task *getNextTask(char *fileName);
int getMaxTaskNumber(char *fileName);
void setArrivalTimeTask(struct Task *task);
void addTaskTerminationLog(int num_tasks_inserted);
void addSimulationLog_Pre_Exec(struct Task task, char *service_time, int *cpuId);
void addSimulationLog_Post_Exec(struct Task task, char *service_time, int *cpuId);
void addCpuTerminationLog(int num_tasks_inserted, int cpuId);
void addMainTerminationLog(int num_tasks_serviced, double waitingTime, double turnaroundTime);


//GLOBAL VARIABLES..............................................................
int NUMBER_OF_TASKS_TASK_FILE = 0;  //number of tasks in task file.

//The variables declared below allow blocking of cpu threads until task has inserted tasks for cpu to take. 
pthread_cond_t cpuCondition;    //condition variable associated with blocking the cpu threads.
pthread_cond_t taskCondition;   //condition variable associated with blocking the task thread.

pthread_mutex_t fullSpacesMutex = PTHREAD_MUTEX_INITIALIZER;  //mutex that controls access to isInserted variable.
pthread_mutex_t emptySpacesMutex = PTHREAD_MUTEX_INITIALIZER;  //mutex that controls access to isFull variable.

int fullSpaces = 0; 
int emptySpaces = 0;

//The variables declared below are for thread id.
//Declared global so it is possible to cancel the thread by using these vars.
pthread_t tid_cpu1; //thread id cpu 1
pthread_t tid_cpu2; //thread id cpu 2
pthread_t tid_cpu3; //thread id cpu 3

int queueSize = 0;

int num_tasks_executed = 0;   //shared variables, shared across the 3 cpus. stores number of tasks executed.
double total_waiting_time = 0.0, total_turnaround_time = 0.0;   //shared vars across 3 cpus.

int main(int argc, char** argv) {
    //File name and amount of tasks m is taken here.
    printf("Scheduler started!\n\n");
    queueSize = 10;
    
    struct Task tasks[queueSize];
    initialize(tasks, queueSize);
    
    NUMBER_OF_TASKS_TASK_FILE = getMaxTaskNumber("task_file");
    
    char choice;
    printf("Create new task file? Y/N?\n");
    if(choice == 'Y'){
        generateTaskFile("task_file");
    }
   

    emptySpaces = queueSize;
    fullSpaces = 0;
    
    /*
     * Initializing condition vars
     */
    pthread_cond_init(&cpuCondition, NULL); //null for default initialization.
    pthread_cond_init(&taskCondition, NULL); //null for default initialization.
    

    /*
     * task thread spawning
     */
    pthread_t tid;//thread id 
    pthread_attr_t attr;    //attributes
    pthread_attr_init(&attr);
    
    pthread_create(&tid, &attr, task, "task_file"); //sending task file as param to the thread.
   
    //Preventing pointless creation of cpu threads by only creating required amount depending on number of tasks.
    if(NUMBER_OF_TASKS_TASK_FILE >= 3){
        /*
         * cpu-1 thread spawning
         */
        pthread_attr_t attr_cpu1;    //attributes
        pthread_attr_init(&attr_cpu1);

        int cpuid1 = 1;
        pthread_create(&tid_cpu1, &attr_cpu1, cpu, &cpuid1); //sending cpu id as param

        /*
         * cpu-2 thread spawning
         */
        pthread_attr_t attr_cpu2;    //attributes
        pthread_attr_init(&attr_cpu2);

        int cpuid2 = 2;
        pthread_create(&tid_cpu2, &attr_cpu2, cpu, &cpuid2); //sending cpu id as param

        /*
         * cpu-3 thread spawning
         */

        pthread_attr_t attr_cpu3;    //attributes
        pthread_attr_init(&attr_cpu3);

        int cpuid3 = 3;
        pthread_create(&tid_cpu3, &attr_cpu3, cpu, &cpuid3); //sending cpu id as param
        
        pthread_join(tid_cpu1, NULL);    //main thread wait till cpu1 is done.    
        pthread_join(tid_cpu2, NULL);    //main thread wait till cpu2 is done.   
        pthread_join(tid_cpu3, NULL);    //main thread wait till cpu3 is done. 

    }else if(NUMBER_OF_TASKS_TASK_FILE == 2){
         /*
         * cpu-1 thread spawning
         */
        pthread_attr_t attr_cpu1;    //attributes
        pthread_attr_init(&attr_cpu1);

        int cpuid1 = 1;
        pthread_create(&tid_cpu1, &attr_cpu1, cpu, &cpuid1); //sending cpu id as param

        /*
         * cpu-2 thread spawning
         */
        pthread_attr_t attr_cpu2;    //attributes
        pthread_attr_init(&attr_cpu2);

        int cpuid2 = 2;
        pthread_create(&tid_cpu2, &attr_cpu2, cpu, &cpuid2); //sending cpu id as param
        
        pthread_join(tid_cpu1, NULL);    //main thread wait till cpu1 is done.    
        pthread_join(tid_cpu2, NULL);    //main thread wait till cpu2 is done.   
        
    }else if(NUMBER_OF_TASKS_TASK_FILE == 1){
                 /*
         * cpu-1 thread spawning
         */
        pthread_attr_t attr_cpu1;    //attributes
        pthread_attr_init(&attr_cpu1);

        int cpuid1 = 1;
        pthread_create(&tid_cpu1, &attr_cpu1, cpu, &cpuid1); //sending cpu id as param

        pthread_join(tid_cpu1, NULL);    //main thread wait till cpu1 is done.    
    }

    pthread_join(tid, NULL);    //main thread wait till task is done.
    
    printf("Number of TASKS SERVICED %d, AVG wait Time %f, AVG TAT %f\n", num_tasks_executed,total_waiting_time / (double) num_tasks_executed, total_turnaround_time / (double) num_tasks_executed );
    addMainTerminationLog(num_tasks_executed, total_waiting_time, total_turnaround_time);
    destroy_queue();
    
    return 0;
}

//Generates task file, returns 0 on success and less than 0 on failure.
int generateTaskFile(char *fileName){
       
    FILE *pFile = fopen(fileName, "w");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
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
  
        if(status < 0){
            printf("writing to task file failed\n");
            return 0;
        }
    }
    
    fclose(pFile);  //closing opened file.
    pFile = NULL; //making sure ref is not there anymore.
    
    return 0;
}

//Reads the taskfile line by line.
int readTaskFile(char *fileName){
    
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
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
struct Task *taskArray = NULL;

/*
 * Returns pointer to two tasks from task file per call, if not found or error returns NULL
 * Referred to the link below to get an idea on how to return array of struct.
 * Link: https://stackoverflow.com/questions/47028165/how-do-i-return-an-array-of-struct-from-a-function
 * Accessed: 1 May 2019
 */
//Returns tasks from task file, if not found NULL is returned.
struct Task *getNextTask(char *fileName){
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);

        exit(-1);   //quit entire application.
        return NULL; 
    }
    
    fseek(pFile, 0, SEEK_END);  //moving file position indicator to the end.

    long endOfFile = ftell(pFile);  //current position (i.e. eof) of file position indicator.
    
    if(fileReadHead != endOfFile){      //if file position indicatior 'fileReadHead' is not at the end of file.

        fseek(pFile, fileReadHead, SEEK_SET);  //move file position indicator to last left off position.         

        int task_number, cpu_burst;
        struct Task task;
        struct Task *ptask = malloc(sizeof(struct Task));
       
      
        int status = fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);  //moves file position indicator by 2 lines, (for loop).
        task.task_number = task_number;
        task.cpu_burst = cpu_burst;
        
        if(status == EOF){
  
            
            return NULL;
        }
        
        *ptask = task;

        fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.
        fclose(pFile);  //closing opened file
        pFile = NULL; //making sure reference is not there anymore.
        return ptask;
    }else{
       // printf("END OF FILE REACHED\n");
    }

    fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.

    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    
    return NULL; 
}


//Returns number of tasks available in task file.
long readHead;
int getMaxTaskNumber(char *fileName){
    int numberOfTasks = 0;
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
      
        return 0; 
    }
    
    fseek(pFile, 0, SEEK_END);  //moving file position indicator to the end.

    long endOfFile = ftell(pFile);  //current position (i.e. eof) of file position indicator.
    
    int task_number, cpu_burst;
    while(readHead != endOfFile){      //if file position indicatior 'fileReadHead' is not at the end of file.

        fseek(pFile, readHead, SEEK_SET);  //move file position indicator to last left off position.         

        int status = fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);  //moves file position indicator by 2 lines, (for loop).
        
        if(status != EOF){
            numberOfTasks++;
        }
        else{
            return 0;
        }
        readHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.
    }
    printf("TASK FILE HAS %d tasks.\n", numberOfTasks);
   
    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    return numberOfTasks;
}


int continueInsertionNew = 1;      //determines whether new task insertion is possible or not, NOT possible when there are tasks left over needing insertion.

int isT1_Inserted = 0, isT2_Inserted = 0;
struct Task *pTask_1 = NULL;
struct Task *pTask_2 = NULL; 
struct Task task_1, task_2;

/*
 * Populate the queue with tasks.
 */
//Task function.
void *task(void *fileName){
    char *pFileName = (char *)fileName; //casting void * to char *
    int total_num_tasks_inserted = 0;
    
    while(1){
        
        pthread_mutex_lock(&emptySpacesMutex);
        
        while(emptySpaces == 0){     //queue is full
            printf("TASK THREAD going to BLOCKING state.\n");
            pthread_cond_wait(&taskCondition, &emptySpacesMutex);  //releases mutex waits on condition (signal).
            printf("TASK THREAD going to UNBLOCKED state.\n");
        }
        
        pthread_mutex_unlock(&emptySpacesMutex);

        if(continueInsertionNew == 1){
            continueInsertionNew = 0;

            if(pTask_1 != NULL)
              //  free(pTask_1);
            
            if(pTask_2 != NULL)
              //  free(pTask_2);
            
            pTask_1 = NULL;
            pTask_2 = NULL;

            pTask_1 = getNextTask(pFileName); //read from task_file
            pTask_2 = getNextTask(pFileName); //read from task_file

            if(pTask_1 != NULL)
                task_1 = *pTask_1;  //deref to get rid of a bug...

            if(pTask_2 != NULL)
                task_2 = *pTask_2;

            isT1_Inserted = 0;
            isT2_Inserted = 0;
        }

        
        if(getRemainingSpaces() >= 2){  //two spaces avail for insertion in the queue

            if(pTask_1 != NULL){    //task available
                if(isT1_Inserted == 0){  
                    setArrivalTimeTask(&task_1);
                    isT1_Inserted = insert(task_1); //returns 1 on successful insertion, ready queue not full.
                    
                    if(isT1_Inserted == 1){ //successfully inserted
                        total_num_tasks_inserted++;
                        addSimulationLog_Task(task_1);  
                    
                        pthread_mutex_lock(&emptySpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        emptySpaces--;
                        pthread_mutex_unlock(&emptySpacesMutex);    //release lock

                        pthread_mutex_lock(&fullSpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        fullSpaces++; //task inserted.
                        pthread_mutex_unlock(&fullSpacesMutex); //release the lock

                        pthread_cond_signal(&cpuCondition); //signal cpu thread to wake up.
                    }
                }
            }

            if(pTask_2 != NULL){    //task available.
                if(isT2_Inserted == 0){
                    setArrivalTimeTask(&task_2);
                    isT2_Inserted = insert(task_2);
                    
                    if(isT2_Inserted == 1){
                        total_num_tasks_inserted++;
                        addSimulationLog_Task(task_2);
         
                        pthread_mutex_lock(&emptySpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        emptySpaces--;
                        pthread_mutex_unlock(&emptySpacesMutex);    //release lock

                        pthread_mutex_lock(&fullSpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        fullSpaces++; //task inserted.
                        pthread_mutex_unlock(&fullSpacesMutex); //release the lock

                        pthread_cond_signal(&cpuCondition); //signal cpu thread to wake up.
                    }
                }
            }

            int res = isT1_Inserted - isT2_Inserted;
            if(res == 0){
                continueInsertionNew = 1;
            }
        }
        else if(getRemainingSpaces() == 1){

            if(pTask_1 != NULL){    //task available
                if(isT1_Inserted == 0){
                    setArrivalTimeTask(&task_1);
                    isT1_Inserted = insert(task_1);

                    if(isT1_Inserted == 1){
                        total_num_tasks_inserted++;
                        addSimulationLog_Task(task_1);
                        
                        pthread_mutex_lock(&emptySpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        emptySpaces--;
                        pthread_mutex_unlock(&emptySpacesMutex);    //release lock

                        pthread_mutex_lock(&fullSpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        fullSpaces++; //task inserted.
                        pthread_mutex_unlock(&fullSpacesMutex); //release the lock

                        pthread_cond_signal(&cpuCondition); //signal cpu thread to wake up.
                    }
                }
            }

            if(pTask_2 != NULL){    //task available.
                if(isT2_Inserted == 0){
                    setArrivalTimeTask(&task_2);
                    isT2_Inserted = insert(task_2);

                    if(isT2_Inserted == 1){
                        total_num_tasks_inserted++;
                        addSimulationLog_Task(task_2);
                        
                        pthread_mutex_lock(&emptySpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        emptySpaces--;
                        pthread_mutex_unlock(&emptySpacesMutex);    //release lock

                        pthread_mutex_lock(&fullSpacesMutex); //aquire lock to modify the var. [not really required since 1 thread runs this, but added anyway]
                        fullSpaces++; //task inserted.
                        pthread_mutex_unlock(&fullSpacesMutex); //release the lock

                        pthread_cond_signal(&cpuCondition); //signal cpu thread to wake up.
                    }
                }

            }

            int res = isT1_Inserted - isT2_Inserted;
            if(res == 0){
                continueInsertionNew = 1;
            }


        }
        else{
           // printf("READY QUEUE IS FULL CANNOT INSERT\n");
            continueInsertionNew = 0; //insertion cannot happen, so wait till space is avail.
        }

        //Kill switch
        if(getSuccessfulInsertions() == NUMBER_OF_TASKS_TASK_FILE ){
            printf("TASK THREAD QUITTING : ALL ITEMS IN TASK FILE WAS ADDED TO QUEUE. %d tasks were added to ready queue\n",   total_num_tasks_inserted);
            addTaskTerminationLog(total_num_tasks_inserted);
            
            if(pTask_1 != NULL)
                free(pTask_1);
            
            if(pTask_2 != NULL)
                free(pTask_2);
            
            pthread_exit(0);    //terminate the thread.
        }
    }
    
    return 0;
}

//mutexes for the shared variables. mutex per variable to increase performance .
pthread_mutex_t num_tasks_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_waiting_time_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_turnaround_time_mutex = PTHREAD_MUTEX_INITIALIZER;

//int cpu1_task_count = 0, cpu2_task_count = 0, cpu3_task_count = 0;  //holds the number of tasks executed by each.

//This function is the function that gets executed by each cpu thread.
void* cpu( void *arg){
    int *pcpuId = (int *)arg;
    int cpuId = *pcpuId;
    
    int task_exec_count_individual = 0;

    printf("CPU ID: %d\n", cpuId);

    while(1){
        
        pthread_mutex_lock(&fullSpacesMutex);
        
        while(fullSpaces == 0){     //no new tasks in ready queue to execute so go block the thread.
            printf("CPU-%d going to blocking state.\n", cpuId);
            
            pthread_cond_wait(&cpuCondition, &fullSpacesMutex);  //releases mutex waits on condition (signal).
            printf("CPU-%d going to UNBLOCKED state.\n", cpuId);
        }

        pthread_mutex_unlock(&fullSpacesMutex);
        
        struct Task *task = pop();  //get a task from ready queue. pthread_cleanup_push() an

        if(task != NULL){   //task available from ready queue.
            
            pthread_mutex_lock(&emptySpacesMutex);
            emptySpaces++;
            pthread_mutex_unlock(&emptySpacesMutex);
            
            pthread_mutex_lock(&fullSpacesMutex);
            fullSpaces--;
            pthread_mutex_unlock(&fullSpacesMutex);
            
            pthread_cond_signal(&taskCondition); //signal TASK thread to wake up (if blocked)as item from queue was taken in for execution.
            
            printf("CPU-%d executing task# = %d burst = %d\n", cpuId, task->task_number, task->cpu_burst);
            //Obtaining service time, waiting time.........................................................................
            time_t arrival_t = task->arrival_t;    //getting arrival time from the task.
            time_t service_t;   //var that stores service time. Time at which task entered the cpu.
            time(&service_t);  //sets the service_t to its value i.e. time now.

            char *service_time = getCurrentTime(); //obtaining current time in full format.
            format_time(service_time); //formatting it down to only contain the time.

            double waiting_time = getTimeElapsed(arrival_t, service_t); //compute waiting time for this task by getting the difference.
            printf("Waiting TIME ELAPSED: %f\n", waiting_time);
            //END of obtaining service time, waiting time...................................................................         
            
            pthread_mutex_lock(&total_waiting_time_mutex);  //obtaining lock to modify total_waiting_time [shared resource]
            
            total_waiting_time = total_waiting_time + waiting_time; //compute total waiting time.
            
            pthread_mutex_unlock(&total_waiting_time_mutex);    //release the lock so another thread can have its go at it.
            
            addSimulationLog_Pre_Exec(*task, service_time, pcpuId); //adds record to simulation log with service time & other related fields.
                
            free(service_time);
            
            sleep(task->cpu_burst); //sleep for burst time, simulate cpu EXECUTING the task.

            //Obtaining completion time.....................................................................................
            time_t completion_t;  //var that stores completion time.
            time(&completion_t); //sets the completion_t to its value i.e. time now, which is essentially the completion time.

            char *completion_time = getCurrentTime(); //obtaining current time in full format.
            format_time(completion_time); //formatting it down to only contain the time.

            addSimulationLog_Post_Exec(*task, completion_time, pcpuId); //adds record to simulation log with completion time.
            task_exec_count_individual++;
            
            free(completion_time);
            
            double turn_around_time = getTimeElapsed(arrival_t, completion_t);      //computes turn around time by getting the difference & other related fields.
            //End of obtaining completion time.....................................................................................
            printf("Turn Around Time: %f\n", turn_around_time);

            pthread_mutex_lock(&total_turnaround_time_mutex);   ////getting the lock for modification of total_turnaround_time var. [shared resource].
            
            total_turnaround_time = total_turnaround_time + turn_around_time;   //computes total turn around time.
            
            pthread_mutex_unlock(&total_turnaround_time_mutex); //release the lock so another thread can have its go at it.
            
            pthread_mutex_lock(&num_tasks_mutex);    //getting the lock for modification of num_tasks_executed var [shared resource].
            
            num_tasks_executed++;    //increment num of tasks executed by one. [shared resource]
            
            pthread_mutex_unlock(&num_tasks_mutex); //release the lock so another thread can have its go at it.
            
            printf("number of tasks executed all together %d\n", num_tasks_executed);
         
        }
        else{   //task not available, ready queue empty.
            printf("Empty/no tasks available for cpu - %d execution. GOING TO EXIT PHASE\n", cpuId);
        }
       
        int remainingTasks = -9;
        pthread_mutex_lock(&num_tasks_mutex);
        remainingTasks = NUMBER_OF_TASKS_TASK_FILE - num_tasks_executed;
        pthread_mutex_unlock(&num_tasks_mutex);
        printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU PRE = %d\n", NUMBER_OF_TASKS_TASK_FILE);
        if(remainingTasks == 0){   //add logs!!!
            printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU TERM = %d\n", NUMBER_OF_TASKS_TASK_FILE);
            printf("CPU-%d THREAD TERMINATES AFTER EXEC %d tasks : ALL TASKS IN TASK FILE EXECUTED.\n.", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);  
        }
        else if(remainingTasks == 1){
            printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU TERM = %d\n", NUMBER_OF_TASKS_TASK_FILE);
            printf("CPU-%d THREAD TERMINATES AFTER EXEC %d tasks : ALL TASKS IN TASK FILE EXECUTED.\n.", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);
        }
        else if(remainingTasks == 2){
            printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU TERM = %d\n", NUMBER_OF_TASKS_TASK_FILE);
            printf("CPU-%d THREAD TERMINATES AFTER EXEC %d tasks : ALL TASKS IN TASK FILE EXECUTED.\n.", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);    
        }
    }
}

/*
 * Solution to obtain current time taken from the given link. This was modified to suit my needs.
 * Link: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
 * Author: mingos
 * Accessed: 2 May 2019
 */
//Gets the current time in full date time format, refer to format_time() to see how the output could be used to extract time.
char *getCurrentTime(){
    time_t rawtime;
    struct tm * timeinfo;
    char *ptime = NULL;
    ptime = malloc(sizeof(char)*50);

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
 
    strcpy (ptime, asctime (timeinfo));

    return ptime;

}

/*
 * Solution to format time was taken from the given link. Modifications were done to suit the needs.
 * Link: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
 * Author: hexinpeter
 * Accessed: 2 May 2019
 */
//Extracts out the time from the full date time format outputted by the getCurrentTime() function.
void format_time(char *output){
    time_t rawtime;
    struct tm * timeinfo;
    
    char str[50];
    strcpy(str, output);
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(output, "%d:%d:%d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    //printf("format_time %s\n", output);
}


/*
 * Refered to the given link to understand the use of difftime() function in c.
 * Link: https://www.tutorialspoint.com/c_standard_library/c_function_difftime.htm
 * Accessed: 2 May 2019
 */
//This function is to get the time elapsed when two start and end times of time_t type are given. 
double getTimeElapsed( time_t start_t, time_t end_t ){
   double diff_t;

   diff_t = difftime(end_t, start_t);   //getting the difference.
   
   return diff_t;
}

//Adds record to simulation log containing task info, number and arrival time. To be used in task().
int addSimulationLog_Task(struct Task task){
    
    FILE *pFile = fopen("simulation_log", "a");     //open for writing, appending.        .        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
    }
    
    int status = fprintf(pFile, "task #: %d\nArrival time: %s\n", task.task_number, task.arrival_time);

    if(status < 0){
        printf("writing to simulation_log file failed\n");
        return 0;
    }
    
    fclose(pFile);
    pFile = NULL;
    return 1;
}

void addSimulationLog_Pre_Exec(struct Task task, char *service_time, int *cpuId){

    FILE *pFile = fopen("simulation_log", "a");     //open for writing, appending.          
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(0);
    }
    
    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nService time: %s\n", *cpuId, task.task_number, task.arrival_time, service_time);
    //printf("cpu_burst %d", cpu_burst);
    if(status < 0){
        printf("writing to simulation_log file failed\n");
    }
    
    fclose(pFile);
    pFile = NULL;
}

//Adds record to simulation_log containing cpu execution info (i.e. cpu num, completion time) and task info (i.e. arrival times and task num). 
//To be used in cpu().
void addSimulationLog_Post_Exec(struct Task task, char *completion_time, int *cpuId){
    //int cpuId = 1;
    
    FILE *pFile = fopen("simulation_log", "a");     //open for writing, appending.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(0);
    }
    
    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nCompletion time: %s\n", *cpuId, task.task_number, task.arrival_time, completion_time);
    
    if(status < 0){
        printf("writing to simulation_log file failed\n");
        
    }
    
    fclose(pFile);
    pFile = NULL;

} 

//gets time of task thread termination, number of tasks executed by task thread and logs it to simulation_log file.
void addTaskTerminationLog(int num_tasks_inserted){

    char *currentTime = getCurrentTime(); //obtaining current time in full format.
    format_time(currentTime); //getting only the time
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing, appending.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(0);
    }
    
    int status = fprintf(pFile, "Number of asks put in to Ready-Queue: %d\nTerminate at time: %s\n", num_tasks_inserted, currentTime);
  
    if(status < 0){
        printf("writing to simulation_log file failed\n");
    }
    
    fclose(pFile);
    pFile = NULL;
}

//adds how many tasks each cpu thread executed in to the simulation_log file.
void addCpuTerminationLog(int num_tasks_inserted, int cpuId){
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing, appending.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(0);
    }
    
    int status = fprintf(pFile, "CPU-%d terminates after servicing %d tasks\n", cpuId, num_tasks_inserted);

    if(status < 0){
        printf("writing to simulation_log file failed\n");
    }
    
    fclose(pFile);
    pFile = NULL;
}

void addMainTerminationLog(int num_tasks_serviced, double waitingTime, double turnaroundTime){
    double avgWaitingTime = waitingTime / (double) num_tasks_serviced;
    double avgTurnAroundTime = turnaroundTime / (double) num_tasks_serviced;
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing, appending.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(0);
    }
    
    int status = fprintf(pFile, "Number of tasks: %d\nAverage waiting time: %0.3f\nAverage turn around time: %0.3f\n", num_tasks_serviced, avgWaitingTime, avgTurnAroundTime);
  
    if(status < 0){
        printf("writing to simulation_log file failed\n");
    }
    
    fclose(pFile);
    pFile = NULL;
}


//sets the arrival time of task, to be used just before insertion.
void setArrivalTimeTask(struct Task *task){
    time_t task1_start;
    time(&task1_start);

    task->arrival_t = task1_start;    //setting times that are operator friendly. for calculations.

    char *time1 = getCurrentTime(); //obtaining current time in full format.
    format_time(time1); //getting only the time

    strcpy(task->arrival_time, time1);    //for sim logs.
    
    free(time1);

    printf("Task number = %d cpu_burst = %d arrival_time = %s\n", task->task_number, task->cpu_burst, task->arrival_time);
}


