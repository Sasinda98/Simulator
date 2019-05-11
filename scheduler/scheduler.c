
/*
 * File:   main.c
 * Author: W.A.A.D Gayal Sasinda Rupasinghe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "readyqueue.h"
#include "taskFile.h"
#include "simulationLog.h" 

//FUNCTION PROTOTYPES...........................................................
void *task(void *fileName);
void *cpu( void *arg);

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

/*
 * Populate the ready queue with tasks, this function runs on task thread.
 */
//Task function.
void *task(void *fileName){
    char *pFileName = (char *)fileName; //casting void * to char *
    int total_num_tasks_inserted = 0;
    
    int continueInsertionNew = 1;      //Determines whether new task insertion is possible or not, NOT possible to insert new ones when there are tasks leftover needing insertion.
    int isT1_Inserted = 0, isT2_Inserted = 0; //indicates whether task 1, task 2 were successfully inserted or not. 1 = success, 0 = fail.
    struct Task *pTask_1 = NULL;    //pointer to task 1
    struct Task *pTask_2 = NULL;    //pointer to task 2
    struct Task task_1, task_2; //variable that can store tasks 1,2
    

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
            printf("TASK THREAD QUITTING: All tasks added to the queue. %d tasks were added to ready queue.\n",   total_num_tasks_inserted);
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
    struct Task *pTask = NULL;

    printf("THREAD CREATION SUCCESSFUL: CPU-%d thread was created.\n", cpuId);

    while(1){

        pthread_mutex_lock(&fullSpacesMutex);

        while(fullSpaces == 0){     //no new tasks in ready queue to execute so block the thread.
            printf("CPU-%d going to BLOCKING state.\n", cpuId);
            pthread_cond_wait(&cpuCondition, &fullSpacesMutex);  //releases mutex waits on condition var (signal). Waits on task thread to wake it up.
            printf("CPU-%d going to UNBLOCKED state.\n", cpuId);
        }

        pthread_mutex_unlock(&fullSpacesMutex);

        pTask = pop();  //get a task from ready queue.
        struct Task task = *pTask;  //dereferencing.
        
        if(pTask != NULL){   //task available from ready queue.

            pthread_mutex_lock(&emptySpacesMutex);
            emptySpaces++;
            pthread_mutex_unlock(&emptySpacesMutex);

            pthread_mutex_lock(&fullSpacesMutex);
            fullSpaces--;
            pthread_mutex_unlock(&fullSpacesMutex);

            pthread_cond_signal(&taskCondition); //signal TASK thread to wake up (if blocked)as task from queue was taken in for execution.

            printf("CPU-%d EXECUTING: task# = %d burst = %d\n", cpuId, task.task_number, task.cpu_burst);
            
            //Obtaining service time, waiting time.........................................................................
            time_t arrival_t = task.arrival_t;    //getting arrival time from the task.
            time_t service_t;   //var that stores service time. Time at which task entered the cpu.
            time(&service_t);  //sets the service_t to its value i.e. time now.

            char *service_time = getCurrentTime(); //obtaining current time in full format. Human readable format.
            format_time(service_time); //formatting it down to only contain the time. Human readable format.

            double waiting_time = getTimeElapsed(arrival_t, service_t); //compute waiting time for this task by getting the difference.
            printf("WAITING TIME: Task# = %d, Waiting time = %0.3f\n", task.task_number, waiting_time);
            //END of obtaining service time, waiting time...................................................................

            pthread_mutex_lock(&total_waiting_time_mutex);  //obtaining lock to modify total_waiting_time [shared resource]

            total_waiting_time = total_waiting_time + waiting_time; //compute total waiting time.

            pthread_mutex_unlock(&total_waiting_time_mutex);    //release the lock so another thread can have its go at it.

            addSimulationLog_Pre_Exec(task, service_time, cpuId); //adds record to simulation log with service time & other related fields.

            free(service_time);

            sleep(task.cpu_burst); //sleep for burst time, simulate cpu EXECUTING the task.

            //Obtaining completion time.....................................................................................
            time_t completion_t;  //var that stores completion time.
            time(&completion_t); //sets the completion_t to its value i.e. time now, which is essentially the completion time.

            char *completion_time = getCurrentTime(); //obtaining current time in full format. Human readable format.
            format_time(completion_time); //formatting it down to only contain the time. Human readable format.

            addSimulationLog_Post_Exec(task, completion_time, cpuId); //adds record to simulation log with completion time.
            task_exec_count_individual++;

            free(completion_time);

            double turn_around_time = getTimeElapsed(arrival_t, completion_t);      //computes turn around time by getting the difference.
            //End of obtaining completion time.....................................................................................
            
            printf("TURNAROUND TIME: Task# = %d, Turnaround time = %0.3f\n\n", task.task_number,turn_around_time);

            pthread_mutex_lock(&total_turnaround_time_mutex);   //getting the lock for modification of total_turnaround_time var. [shared resource].

            total_turnaround_time = total_turnaround_time + turn_around_time;   //computes total turn around time.

            pthread_mutex_unlock(&total_turnaround_time_mutex); //release the lock so another thread can have its go at it.

            pthread_mutex_lock(&num_tasks_mutex);    //getting the lock for modification of num_tasks_executed var [shared resource].

            num_tasks_executed++;    //increment num of tasks executed by one. [shared resource]

            pthread_mutex_unlock(&num_tasks_mutex); //release the lock so another thread can have its go at it.
        }
        else{   //task not available, ready queue empty.
            printf("Empty/no tasks available for cpu - %d execution. GOING TO EXIT PHASE\n", cpuId);
        }

        int remainingTasks = -1;
        pthread_mutex_lock(&num_tasks_mutex);
        remainingTasks = NUMBER_OF_TASKS_TASK_FILE - num_tasks_executed;
        pthread_mutex_unlock(&num_tasks_mutex);
        
        //CPU thread termination is handled here.
        if(remainingTasks == 0){   //add logs!!!
            printf("CPU-%d THREAD TERMINATES AFTER EXECUTING %d TASKS.\n", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);
        }
        else if(remainingTasks == 1){
            printf("CPU-%d THREAD TERMINATES AFTER EXECUTING %d TASKS.\n", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);
        }
        else if(remainingTasks == 2){
            printf("CPU-%d THREAD TERMINATES AFTER EXECUTING %d TASKS.\n", cpuId, task_exec_count_individual);
            addCpuTerminationLog(task_exec_count_individual, cpuId);
            pthread_exit(0);
        }
    }
}
