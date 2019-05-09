
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

//The variables declared below allow blocking/resumption of a thread based on condition variable.
pthread_cond_t cpuCondition;    //condition variable associated with blocking/resumption of cpu threads.
pthread_cond_t taskCondition;   //condition variable associated with blocking/resumption of the task thread.

pthread_mutex_t fullSpacesMutex = PTHREAD_MUTEX_INITIALIZER;  //mutex that controls access to fullSpaces variable.
pthread_mutex_t emptySpacesMutex = PTHREAD_MUTEX_INITIALIZER;  //mutex that controls access to emptySpaces variable.

int fullSpaces = 0; //stores the amount of full spaces in the ready queue.
int emptySpaces = 0;    //stores the amount of empty spaces in the ready queue.

int num_tasks_executed = 0;   //shared variables, shared across the 3 cpus. stores number of tasks executed.
double total_waiting_time = 0.0, total_turnaround_time = 0.0;   //shared vars across 3 cpus.

int main(int argc, char *argv[]) {
    char *taskFileName;
    int queueSize = 10;  //default
    
    /*
     * Referred to the link below to understand how to read command-line parameters.
     * Link: https://www.tutorialspoint.com/cprogramming/c_command_line_arguments.htm
     * Accessed: 9th May 2019
     */
    //Getting the command-line parameters.
    if( argc == 3 ) {
        taskFileName = argv[1];//filename
        
        /*
         * Referred to reply on stackoverflow to see how to convert string containing numbers to integer type.
         * Link: https://stackoverflow.com/questions/628761/convert-a-character-digit-to-the-corresponding-integer-in-c
         * Author: Paul W Homer
         * Accessed: 9th May 2019
         */
        sscanf(argv[2], "%d", &queueSize);
        printf("Queue size input = %d\n", queueSize);
        printf("File name input = %s\n", taskFileName);
    }
    else if( argc > 3 ) {
        printf("Two arguments are expected.\nProgram will quit...\n");
        exit(0);
    }
    else {
        printf("Two arguments are expected, please provide task file name followed by queue size.\nProgram will quit...\n");
        exit(0);
    }

    printf("Scheduler started!\n\n");
   

    struct Task tasks[queueSize];
    initialize(tasks, queueSize);

    char choice;
    printf("Create new task file? Y/N?\n");
    scanf("%c", &choice);
    if(choice == 'Y'){
        generateTaskFile("task_file");
    }

    NUMBER_OF_TASKS_TASK_FILE = getMaxTaskNumber(taskFileName);

    emptySpaces = queueSize;
    fullSpaces = 0;

    /*
     * Initializing condition variables
     */
    pthread_cond_init(&cpuCondition, NULL); //null for default initialization.
    pthread_cond_init(&taskCondition, NULL); //null for default initialization.


    /*
     * task thread spawning
     */
    pthread_t tid;//thread id
    pthread_attr_t attr;    //attributes
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, task, taskFileName); //sending task file as param to the thread.
    
    pthread_t tid_cpu1; //thread id cpu 1
    pthread_t tid_cpu2; //thread id cpu 2
    pthread_t tid_cpu3; //thread id cpu 3

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

    pthread_join(tid, NULL);    //main thread wait till task thread is done.

    printf("Number of TASKS SERVICED %d, AVG wait Time %0.3f, AVG TAT %0.3f\n", num_tasks_executed,total_waiting_time / (double) num_tasks_executed, total_turnaround_time / (double) num_tasks_executed );
    addMainTerminationLog(num_tasks_executed, total_waiting_time, total_turnaround_time);
    destroy_queue();    //free the memory allocated for ready queue, see readyqueue.h.

    return 0;
}

//Generates task file
int generateTaskFile(char *fileName){

    FILE *pFile = fopen(fileName, "w");    //open/create file for writing.

    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
    }

    int cpu_burst = 0;

    //Writes formatted output to the task_file, [task_number cpu_burst]
    for(int task_number = 0; task_number < 100; task_number++){
        /*
         * Refered to below link to understand use of random number generators in c.
         * Link: https://www.programmingsimplified.com/c-program-generate-random-numbers
         * Accessed: 30th April 2019
        */
        cpu_burst = rand() % 50 + 1;
        int status = fprintf(pFile, "%d %d\n", task_number, cpu_burst);

        if(status < 0){
             fclose(pFile);  //closing opened file.
             pFile = NULL; //making sure ref is not there anymore.
            printf("writing to task file failed\n");
            return 0;
        }
    }

    fclose(pFile);  //closing opened file.
    pFile = NULL; //making sure ref is not there anymore.

    return 0;
}

long fileReadHead;
struct Task *taskArray = NULL;
/*
 * Returns pointer to next task from task file per every call, if not found or error the function returns NULL
 * Referred to the link below to get an idea on how to return array of struct.
 * Link: https://stackoverflow.com/questions/47028165/how-do-i-return-an-array-of-struct-from-a-function
 * Accessed: 1st May 2019
 */
//Returns next task from task file, if not NULL is returned.
struct Task *getNextTask(char *fileName){
    FILE *pFile = fopen(fileName, "r");    //open file (task file) for reading.

    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);

        exit(-1);   //quit entire application.
        return NULL;
    }

    fseek(pFile, 0, SEEK_END);  //moving file position indicator to the end of the file.

    long endOfFile = ftell(pFile);  //current position (i.e. eof) of file position indicator.

    if(fileReadHead != endOfFile){      //if file position indicatior 'fileReadHead' is not at the end of file.

        fseek(pFile, fileReadHead, SEEK_SET);  //move file position indicator to last left off position.

        int task_number, cpu_burst;
        struct Task task;
        struct Task *ptask = malloc(sizeof(struct Task));


        int status = fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);  //write the line to file.
        task.task_number = task_number;
        task.cpu_burst = cpu_burst;

        if(status == EOF){
            fclose(pFile);  //closing opened file.
            pFile = NULL; //making sure ref is not there anymore.
            return NULL;
        }

        *ptask = task;

        fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.
        fclose(pFile);  //closing opened file
        pFile = NULL; //making sure reference is not there anymore.
        return ptask;
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
        exit(0);    //quit entire app
        return 0;
    }

    fseek(pFile, 0, SEEK_END);  //moving file position indicator to the end.

    long endOfFile = ftell(pFile);  //current position (i.e. eof) of file position indicator.

    int task_number, cpu_burst;
    while(readHead != endOfFile){      //while file position indicator 'fileReadHead' is not at the end of file.

        fseek(pFile, readHead, SEEK_SET);  //move file position indicator to last left off position.

        int status = fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);  //write the line to file.

        if(status != EOF){
            numberOfTasks++;
        }
        else{
            fclose(pFile);  //closing opened file.
            pFile = NULL; //making sure ref is not there anymore.
            return 0;
        }
        readHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.
    }
    printf("TASK FILE HAS %d tasks.\n", numberOfTasks);

    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    return numberOfTasks;
}


int continueInsertionNew = 1;      //Determines whether new task insertion is possible or not, NOT possible to insert new ones when there are tasks leftover needing insertion.

int isT1_Inserted = 0, isT2_Inserted = 0; //indicates whether task 1, task 2 were successfully inserted or not. 1 = success, 0 = fail.
struct Task *pTask_1 = NULL;    //pointer to task 1
struct Task *pTask_2 = NULL;    //pointer to task 2
struct Task task_1, task_2; //variable that can store tasks 1,2

/*
 * Populate the ready queue with tasks, this function runs on task thread.
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
                free(pTask_1);  

            if(pTask_2 != NULL)
                free(pTask_2);

            pTask_1 = NULL;
            pTask_2 = NULL;

            pTask_1 = getNextTask(pFileName); //read from task_file, gets task 1
            pTask_2 = getNextTask(pFileName); //read from task_file, gets task 2

            if(pTask_1 != NULL)
                task_1 = *pTask_1; //deref to get rid of a bug

            if(pTask_2 != NULL)
                task_2 = *pTask_2;

            isT1_Inserted = 0;
            isT2_Inserted = 0;
        }


        if(getRemainingSpaces() >= 2){  //two spaces available for insertion in the ready queue.

            if(pTask_1 != NULL){    //task available for insertion
                if(isT1_Inserted == 0){
                    setArrivalTimeTask(&task_1);    //set arrival time for the task.
                    isT1_Inserted = insert(task_1); //returns 1 on successful insertion, ready queue not full. 0 = fail, ready queue full.

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

            if(pTask_2 != NULL){    //task available for insertion.
                if(isT2_Inserted == 0){
                    setArrivalTimeTask(&task_2);     //set arrival time for the task.
                    isT2_Inserted = insert(task_2);    //returns 1 on successful insertion, ready queue not full. 0 = fail, ready queue full.

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

            int res = isT1_Inserted - isT2_Inserted; //if zero, the tasks intended to be inserted got inserted (success), else there are tasks leftover needing insertion.
            if(res == 0){   //if insertions successful, allow insertion of new tasks.
                continueInsertionNew = 1;
            }
        }
        else if(getRemainingSpaces() == 1){     //one space available in ready queue for insertion.

            if(pTask_1 != NULL){    //task available
                if(isT1_Inserted == 0){
                    setArrivalTimeTask(&task_1);    //set arrival time for the task.
                    isT1_Inserted = insert(task_1);  //returns 1 on successful insertion, ready queue not full. 0 = fail, ready queue full.

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
                    setArrivalTimeTask(&task_2);    //set arrival time for the task.
                    isT2_Inserted = insert(task_2);  //returns 1 on successful insertion, ready queue not full. 0 = fail, ready queue full.

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

            int res = isT1_Inserted - isT2_Inserted;    //if zero, the tasks intended to be inserted got inserted(success), else there are tasks leftover needing insertion.
            if(res == 0){    //if insertions successful, allow insertion of new tasks.
                continueInsertionNew = 1;
            }


        }
        else{
            continueInsertionNew = 0; //insertion cannot happen, so wait till space is available in the ready queue.
        }

        //Kill switch
        if(getSuccessfulInsertions() == NUMBER_OF_TASKS_TASK_FILE ){
            printf("TASK THREAD QUITTING : ALL ITEMS IN TASK FILE WAS ADDED TO QUEUE. %d tasks were added to ready queue.\n",   total_num_tasks_inserted);
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

//mutexes for the shared variables. mutex per variable to increase performance by not blocking massive chunks of code at once.
pthread_mutex_t num_tasks_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_waiting_time_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_turnaround_time_mutex = PTHREAD_MUTEX_INITIALIZER;

//This function is the function that gets executed by each cpu thread, gets task from ready queue, executes and logs.
void* cpu( void *arg){
    int *pcpuId = (int *)arg;
    int cpuId = *pcpuId;

    int task_exec_count_individual = 0;

    printf("CPU ID: %d thread created successfully.\n", cpuId);

    while(1){

        pthread_mutex_lock(&fullSpacesMutex);

        while(fullSpaces == 0){     //no new tasks in ready queue to execute so block the thread.
            printf("CPU-%d going to blocking state.\n", cpuId);

            pthread_cond_wait(&cpuCondition, &fullSpacesMutex);  //releases mutex waits on condition var (signal). Waits on task thread to wake it up.
            printf("CPU-%d going to UNBLOCKED state.\n", cpuId);
        }

        pthread_mutex_unlock(&fullSpacesMutex);

        struct Task *task = pop();  //get a task from ready queue.

        if(task != NULL){   //task available from ready queue.

            pthread_mutex_lock(&emptySpacesMutex);
            emptySpaces++;
            pthread_mutex_unlock(&emptySpacesMutex);

            pthread_mutex_lock(&fullSpacesMutex);
            fullSpaces--;
            pthread_mutex_unlock(&fullSpacesMutex);

            pthread_cond_signal(&taskCondition); //signal TASK thread to wake up (if blocked)as task from queue was taken in for execution.

            printf("CPU-%d executing task# = %d burst = %d\n", cpuId, task->task_number, task->cpu_burst);
            
            //Obtaining service time, waiting time.........................................................................
            time_t arrival_t = task->arrival_t;    //getting arrival time from the task.
            time_t service_t;   //var that stores service time. Time at which task entered the cpu.
            time(&service_t);  //sets the service_t to its value i.e. time now.

            char *service_time = getCurrentTime(); //obtaining current time in full format. Human readable format.
            format_time(service_time); //formatting it down to only contain the time. Human readable format.

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

            char *completion_time = getCurrentTime(); //obtaining current time in full format. Human readable format.
            format_time(completion_time); //formatting it down to only contain the time. Human readable format.

            addSimulationLog_Post_Exec(*task, completion_time, pcpuId); //adds record to simulation log with completion time.
            task_exec_count_individual++;

            free(completion_time);

            double turn_around_time = getTimeElapsed(arrival_t, completion_t);      //computes turn around time by getting the difference.
            //End of obtaining completion time.....................................................................................
            
            printf("Turn Around Time: %f\n", turn_around_time);

            pthread_mutex_lock(&total_turnaround_time_mutex);   //getting the lock for modification of total_turnaround_time var. [shared resource].

            total_turnaround_time = total_turnaround_time + turn_around_time;   //computes total turn around time.

            pthread_mutex_unlock(&total_turnaround_time_mutex); //release the lock so another thread can have its go at it.

            pthread_mutex_lock(&num_tasks_mutex);    //getting the lock for modification of num_tasks_executed var [shared resource].

            num_tasks_executed++;    //increment num of tasks executed by one. [shared resource]

            pthread_mutex_unlock(&num_tasks_mutex); //release the lock so another thread can have its go at it.

            printf("Number of tasks executed all together %d\n", num_tasks_executed);

        }
        else{   //task not available, ready queue empty.
            printf("Empty/no tasks available for cpu - %d execution. GOING TO EXIT PHASE\n", cpuId);
        }

        int remainingTasks = -1;
        pthread_mutex_lock(&num_tasks_mutex);
        remainingTasks = NUMBER_OF_TASKS_TASK_FILE - num_tasks_executed;
        pthread_mutex_unlock(&num_tasks_mutex);
        printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU PRE = %d\n", NUMBER_OF_TASKS_TASK_FILE);
        
        //CPU thread termination is handled here.
        if(remainingTasks == 0){   //add logs!!!
            printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU TERM = %d\n", NUMBER_OF_TASKS_TASK_FILE);
            printf("CPU-%d THREAD TERMINATES AFTER EXECUTING %d TASKS.\n", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);
        }
        else if(remainingTasks == 1){
            printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU TERM = %d\n", NUMBER_OF_TASKS_TASK_FILE);
            printf("CPU-%d THREAD TERMINATES AFTER EXECUTING %d TASKS.\n", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);
        }
        else if(remainingTasks == 2){
            printf("\nNUMBER_OF_TASKS_TASKS_FILE CPU TERM = %d\n", NUMBER_OF_TASKS_TASK_FILE);
            printf("CPU-%d THREAD TERMINATES AFTER EXECUTING %d TASKS.\n", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);
        }
    }
}

/*
 * Solution to obtain current time taken from the given link. This was modified to suit the needs.
 * Link: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
 * Author: mingos
 * Accessed: 2nd May 2019
 */
//Gets the current time in full date time format, refer to format_time() to see how the output could be used to extract time.
char *getCurrentTime(){
    time_t rawtime;
    struct tm * timeinfo = malloc(sizeof(struct tm));
    char *ptime = NULL;
    ptime = malloc(sizeof(char)*50);

    time (&rawtime);
    localtime_r (&rawtime, timeinfo);   //thread safe, localtime_r is thread safe.

    strcpy (ptime, asctime (timeinfo));
    free(timeinfo);
    return ptime;

}

/*
 * Solution to format time was taken from the given link. Modifications were done to suit the needs.
 * Link: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
 * Author: hexinpeter
 * Accessed: 2nd May 2019
 */
//Extracts out the time from the full date time format outputted by the getCurrentTime() function.
void format_time(char *output){
    time_t rawtime;
    struct tm * timeinfo = malloc(sizeof(struct tm));

    char str[50];
    strcpy(str, output);

    time ( &rawtime );
    localtime_r(&rawtime, timeinfo);    //localtime_r is thread safe.

    sprintf(output, "%d:%d:%d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    free(timeinfo);
}


/*
 * Refered to the given link to understand the use of difftime() function in c.
 * Link: https://www.tutorialspoint.com/c_standard_library/c_function_difftime.htm
 * Accessed: 2nd May 2019
 */
//This function is to get the time elapsed when start and end times of time_t type are given.
double getTimeElapsed( time_t start_t, time_t end_t ){
   double diff_t;

   diff_t = difftime(end_t, start_t);   //getting the difference.

   return diff_t;
}

//Adds record to simulation log containing task info, task number and arrival time. To be used in task().
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
        fclose(pFile);
        pFile = NULL;
        printf("writing to simulation_log file failed\n");
        return 0;
    }

    fclose(pFile);
    pFile = NULL;
    return 1;
}

//Adds record to simulation log containing task info -: cpu#, task number, arrival time and service time. 
//To be used in cpu().
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
        fclose(pFile);
        pFile = NULL;
        printf("writing to simulation_log file failed\n");
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
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(0);
    }

    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nCompletion time: %s\n", *cpuId, task.task_number, task.arrival_time, completion_time);

    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("writing to simulation_log file failed\n");

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
        printf("Failed to open/create simulation_log file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        exit(0);
    }

    int status = fprintf(pFile, "Number of asks put in to Ready-Queue: %d\nTerminate at time: %s\n", num_tasks_inserted, currentTime);

    if(status < 0){
        fclose(pFile);
        pFile = NULL;
        printf("writing to simulation_log file failed\n");
    }
    free(currentTime);
    fclose(pFile);
    pFile = NULL;
}

//Adds how many tasks each cpu thread executed in to the simulation_log file.
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
        fclose(pFile);
        pFile = NULL;
        printf("writing to simulation_log file failed\n");
    }

    fclose(pFile);
    pFile = NULL;
}

//Adds record to simulation log about number of tasks serviced, average waiting time and average turnaround time.
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
        fclose(pFile);
        pFile = NULL;
        printf("writing to simulation_log file failed\n");
    }

    fclose(pFile);
    pFile = NULL;
}


//Sets the arrival time of task, to be used just before insertion to the ready queue.
void setArrivalTimeTask(struct Task *task){
    time_t task1_start;
    time(&task1_start); //init with time stamp for this point.

    task->arrival_t = task1_start;    //setting times that are operator friendly. for calculations. Not human readable.

    char *time1 = getCurrentTime(); //obtaining current time in full format.
    format_time(time1); //getting only the time

    strcpy(task->arrival_time, time1);    //setting the arival time of the task.

    free(time1);

    printf("Task number = %d cpu_burst = %d arrival_time = %s\n", task->task_number, task->cpu_burst, task->arrival_time);
}


