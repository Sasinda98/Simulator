
/* 
 * File:   main.c
 * Author: sasinda
 *
 * Created on 30 April 2019, 6:10 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "readyqueue.h"
#include <string.h>

#include <pthread.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>


//FUNCTION PROTOTYPES...........................................................
int generateTaskFile(char *fileName);
int readTaskFile(char *fileName);
struct Task *getNextTwoTasks(char *fileName);
void *task(void *fileName);
long long timeInMilliseconds(void);
void format_time(char *output);
char *getCurrentTime();
double  getTimeElapsed();
int addSimulationLog_Task(struct Task task);
void *cpu( void *arg);
int addSimulationLog_Pre_Exec(struct Task task, char *service_time, int *cpuId);
int addSimulationLog_Post_Exec(struct Task task, char *service_time, int *cpuId);
struct Task *getNextTask(char *fileName);
int getMaxTaskNumber(char *fileName);
void setArrivalTimeTask(struct Task *task);

//GLOBAL VARIABLES..............................................................
int INVALID_TASK_NUM_CODE = -99;    //deprecated
int NUMBER_OF_TASKS_TASK_FILE = 0;  //number of tasks in task file.
int num = 0;

//The variables declared below allow blocking of cpu threads until task has inserted tasks for cpu to take. 
pthread_cond_t taskCpuCondition;    //condition variable associated with task() and cpu().
pthread_mutex_t isTaskInsertedMutex = PTHREAD_MUTEX_INITIALIZER;  //mutex that controls access to isInserted variable.
int isTaskInserted = 0; //condition variable that requires the mutex to be managed.

//The variables declared below are for thread id.
//Declared global so it is possible to cancel the thread by using these vars.
pthread_t tid_cpu1; //thread id cpu 1
pthread_t tid_cpu2; //thread id cpu 2
pthread_t tid_cpu3; //thread id cpu 3

int main(int argc, char** argv) {
    //File name and amount of tasks m is taken here.
    printf("Scheduler started!\n\n");

    struct Task tasks[3];
    initialize(tasks, 3);
    NUMBER_OF_TASKS_TASK_FILE = getMaxTaskNumber("task_file");
   // generateTaskFile("task_file");
    
    struct Task ts1;
    ts1.task_number = 44;
    ts1.cpu_burst = 1;
    
    struct Task ts2;
    ts2.task_number = 77;
    ts2.cpu_burst = 2;
    
    /*
     * Initializing condition and mutex variables
     */
    pthread_cond_init(&taskCpuCondition, NULL); //null for default initialization.
    
    

    /*
     * task thread spawning
     */
    pthread_t tid;//thread id 
    pthread_attr_t attr;    //attributes
    pthread_attr_init(&attr);
    
    pthread_create(&tid, &attr, task, "task_file"); //sending task file as param to the thread.
    
   
    
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

    pthread_join(tid, NULL);    //main thread wait till task is done.
    
   // sleep(5);
   // struct Task *ts44 = pop();
    //printf("Popping the first task %d %d\n", ts44->task_number, ts44->cpu_burst);
    
    //setArrivalTimeTask(&ts1);
    
    //printf("TS1 arrival time %s\n", ts1.arrival_time);
     /*
      struct Task *temp = NULL;
    int eof = 0;
    int *peof = &eof;
    
        
    for(int i = 0; i < 3; i++){
      task("task_file"); 
    }
    
    printf("counting %d", num);
    
    for(int i = 0; i < 0; i++){
        temp = pop();
        if(temp != NULL){
            printf("%d %d\n", temp->task_number, temp->cpu_burst );
        }else{
            printf("NULL DETECTED\n");
        }
    }
      */     
    
    

    
          
    
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
       //printf("cpu_burst %d", cpu_burst);
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



/*
 * Returns pointer to two tasks from task file per call, if not found or error returns NULL
 * Referred to the link below to get an idea on how to return array of struct.
 * Link: https://stackoverflow.com/questions/47028165/how-do-i-return-an-array-of-struct-from-a-function
 * Accessed: 1 May 2019
 */
long fileReadHead;
struct Task *taskArray = NULL;

struct Task *getNextTwoTasks1(char *fileName){
    
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return NULL;
    }
    
    fseek(pFile, 0, SEEK_END);  //moving file position indicator to the end.

    long endOfFile = ftell(pFile);  //current position (i.e. eof) of file position indicator.
    
    if(fileReadHead != endOfFile){      //if file position indicatior 'fileReadHead' is not at the end of file.

        fseek(pFile, fileReadHead, SEEK_SET);  //move file position indicator to last left off position.         

        int task_number, cpu_burst;
        struct Task task[2];
        taskArray = &task[0];
        
        struct Task *taskArray = malloc(sizeof(struct Task) * 2);
      
        for(int i = 0; i < 2; i++){
            
            fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);  //moves file position indicator by 2 lines, (for loop).
    
            task[i].task_number = task_number;
            task[i].cpu_burst = cpu_burst;
         
        }
        
        *(taskArray) = task[0]; //setting up the pointers.
        *(taskArray + 1) = task[1]; //setting up the pointers.
          
        fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.
        fclose(pFile);  //closing opened file
        pFile = NULL; //making sure reference is not there anymore.
        return taskArray;
    }else{
        printf("END OF FILE REACHED\n");
    }

   fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.

    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    return NULL;
    
}

struct Task *getNextTwoTasks(char *fileName){
    
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return NULL;
    }
    
    fseek(pFile, 0, SEEK_END);  //moving file position indicator to the end.

    long endOfFile = ftell(pFile);  //current position (i.e. eof) of file position indicator.
    
    if(fileReadHead != endOfFile){      //if file position indicatior 'fileReadHead' is not at the end of file.

        fseek(pFile, fileReadHead, SEEK_SET);  //move file position indicator to last left off position.         

        int task_number, cpu_burst;
        struct Task task[2];
        taskArray = &task[0];
        
        struct Task *taskArray = malloc(sizeof(struct Task) * 2);
      
        for(int i = 0; i < 2; i++){
            
            int status = fscanf(pFile, "%d %d\n", &task_number, &cpu_burst);  //moves file position indicator by 2 lines, (for loop).
            task[i].task_number = task_number;
            task[i].cpu_burst = cpu_burst;
           
            if(status == EOF){
                task[i].task_number = -99;  //flag to indicate that this is not a valid task.
                task[i].cpu_burst = -99;
                if(i==0){
                     *(taskArray) = task[0]; //setting up the pointers.
                }
                else{
                     *(taskArray + 1) = task[1]; //setting up the pointers.
                }
            }
            else{
                if(i==0){
                     *(taskArray) = task[0]; //setting up the pointers.
                }
                else{
                     *(taskArray + 1) = task[1]; //setting up the pointers.
                }
            }
               
        }
        
       // *(taskArray) = task[0]; //setting up the pointers.
       // *(taskArray + 1) = task[1]; //setting up the pointers.
          
        fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.
        fclose(pFile);  //closing opened file
        pFile = NULL; //making sure reference is not there anymore.
        return taskArray;
    }else{
        printf("END OF FILE REACHED\n");
    }

   fileReadHead = ftell(pFile);    //store the current position of file position indicator so the next time, it start read from there.

    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    return NULL;
    
}

//Returns tasks from task file, if not found NULL is returned.
struct Task *getNextTask(char *fileName){
    FILE *pFile = fopen(fileName, "r");    //open for writing.
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        
        struct Task invalidTask;
        invalidTask.task_number = INVALID_TASK_NUM_CODE;
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
          //  printf("EOF REACHED\n");
            
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
            //  printf("EOF REACHED\n");
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
int outcome = 0;    //holds outcome of the insertion.
struct Task *ptask_array = NULL;   //pointer to the array that holds the 2 tasks.
struct Task tasks[2];

int isT1_Inserted = 0, isT2_Inserted = 0;
struct Task *pTask_1 = NULL;
struct Task *pTask_2 = NULL; 
struct Task task_1, task_2;
int eofFlag=0;
/*
 * Gets two tasks from file and adds it to ready queue if available 
 * and sets appropriate fields in the task to their respective values.
 */
/*int task1(char *fileName){
   // char *pFileName = fileName;
    
    if(continueInsertionNew == 1){ //previous add was successful continue with inserting new tasks.
         ptask_array = getNextTwoTasks(fileName);   //getting the next two tasks from task file.
         printf("Getting next task successful --top\n");
        
        if(ptask_array != NULL){    //two tasks available
            struct Task twoTasks[2];    //holds the tasks to be added to the queue.
            twoTasks[0] = *(ptask_array);
            twoTasks[1] = *(ptask_array + 1);
            
            //Setting the arrival times....................................................
            time_t task1_start, task2_start;
            time(&task1_start);
            time(&task2_start);
            
            twoTasks[0].arrival_t = task1_start;    //setting times that are operator friendly. for calculations.
            twoTasks[1].arrival_t = task2_start;     //setting times that are operator friendly. for calculations
            
            char *time1 = getCurrentTime(); //obtaining current time in full format.
            format_time(time1);
            
            char *time2 = getCurrentTime(); //obtaining current time in full format.
            format_time(time2);
            
            strcpy(twoTasks[0].arrival_time, time1);    //for sim logs.
            strcpy(twoTasks[1].arrival_time, time2);    //for sim logs.
            
            printf("Task number = %d cpu_burst = %d arrival_time = %s\n", twoTasks[0].task_number, twoTasks[0].cpu_burst, twoTasks[0].arrival_time);
            printf("Task number = %d cpu_burst = %d arrival_time = %s\n", twoTasks[1].task_number, twoTasks[1].cpu_burst, twoTasks[1].arrival_time);
            //End setting arrival times.....................................................
            
            outcome = insertTwo(twoTasks);    //returns 2 if both tasks were inserted, returns 1 if only first task was inserted & returns 0 if failure.
            
            if(outcome == 1){ //only first task was added see insertTwo() at readyqueue.h
                addSimulationLog_Task(twoTasks[0]);
                continueInsertionNew = 0;  //next call direct to else code block
            }
            else if(outcome == 2){
                addSimulationLog_Task(twoTasks[0]);
                addSimulationLog_Task(twoTasks[1]);
                continueInsertionNew = 1;  //next call to direct to if code block.(this)
            }
            else{
                //failure to add any, no spaces.
                continueInsertionNew = 0; //next call direct to else code block
                //outcome = 0;
            }
            
        }else{
            printf("No tasks available, thread exit -- top \n");
            pthread_exit(0);    //nothing available to add.
        }
    }
    else{   //previous add was failed, so try again.
        if(ptask_array != NULL){
            printf("Adding previous task that failed to add --bottom\n");
            struct Task twoTasks[2];    //holds the tasks to be added to the queue.
            twoTasks[0] = *(ptask_array);
            twoTasks[1] = *(ptask_array + 1);

            //Setting the arrival times....................................................
            time_t task1_start, task2_start;
            time(&task1_start);
            time(&task2_start);
            
            twoTasks[0].arrival_t = task1_start;    //setting times that are operator friendly.
            twoTasks[1].arrival_t = task2_start;     //setting times that are operator friendly.
            
            char *time1 = getCurrentTime(); //obtaining current time in full format.
            format_time(time1);
            
            char *time2 = getCurrentTime(); //obtaining current time in full format.
            format_time(time2);
        
            strcpy(twoTasks[0].arrival_time, time1);
            strcpy(twoTasks[1].arrival_time, time2);
            
            printf("Task number = %d cpu_burst = %d arrival_time = %s\n", twoTasks[0].task_number, twoTasks[0].cpu_burst, twoTasks[0].arrival_time);
            printf("Task number = %d cpu_burst = %d arrival_time = %s\n", twoTasks[1].task_number, twoTasks[1].cpu_burst, twoTasks[1].arrival_time);
            
            //End setting arrival times.....................................................
            
         //   outcome = insertTwo(twoTasks);    //returns 2 if both tasks were inserted, returns 1 if only first task was inserted & returns 0 if failure.
            
            if((outcome == 1) && (continueInsertionNew == 0)){ //only first task was added see insertTwo() at readyqueue.h
                int res = insert(twoTasks[1]);    //add second task.
                
                if(res == 1){   //successful adding of second task.
                    addSimulationLog_Task(twoTasks[1]);
                    continueInsertionNew = 1;  //next call direct to if code block to allow for new task insertion, since current insertion is complete.
                }
                else{
                    continueInsertionNew = 0;   //prevent continuation of insertion of new tasks since current insertion is not complete.
                }
              
            }
            else if((outcome == 0) && (continueInsertionNew == 0)){ //no task were added first time
                
                outcome = insertTwo(twoTasks);  //returns 2 if both tasks were inserted, returns 1 if only first task was inserted & returns 0 if failure.
                
                if(outcome == 2){   //both tasks were added
                    addSimulationLog_Task(twoTasks[0]);
                    addSimulationLog_Task(twoTasks[1]);
                    continueInsertionNew = 1;  //next call to direct to if code block. To retry insertion.
                }
                else if(outcome == 1){
                     addSimulationLog_Task(twoTasks[0]);
                }
                else{
                     continueInsertionNew = 0;  //prevent continuation of insertion of new tasks since current insertion is not complete.
                }
                

            }
        }
    }
    return  continueInsertionNew;  //no and op before.. debug purpose
    pthread_exit(0);    //exit the thread.
}
*/

//Task function.
void *task(void *fileName){
    char *pFileName = (char *)fileName; //casting void * to char *
    
    while(1){
        
       // pthread_mutex_lock(&isTaskInsertedMutex); //aquire lock to modify the isTaskInserted variable.
            
      //  isTaskInserted = 1; //task inserted.
       // pthread_mutex_unlock(&isTaskInsertedMutex); //aquire lock to modify the 
       
       // pthread_cond_signal(&taskCpuCondition); //to the end
        
        if(continueInsertionNew == 1){
            continueInsertionNew = 0;
            num++;
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

           // printf("Greater than or 2 spaces avail\n");

            if(pTask_1 != NULL){    //task available
                if(isT1_Inserted == 0){  
                    setArrivalTimeTask(&task_1);
                    isT1_Inserted = insert(task_1);
                    if(isT1_Inserted == 1){
                        addSimulationLog_Task(task_1);
                        
                        pthread_mutex_lock(&isTaskInsertedMutex); //aquire lock to modify the isTaskInserted variable.

                        isTaskInserted++; //task inserted.
                        pthread_mutex_unlock(&isTaskInsertedMutex); //aquire lock to modify the 

                        pthread_cond_signal(&taskCpuCondition); //to the end
                    }
                }
            }

            if(pTask_2 != NULL){    //task available.
                if(isT2_Inserted == 0){
                    setArrivalTimeTask(&task_2);
                    isT2_Inserted = insert(task_2);
                    if(isT2_Inserted == 1){
                        addSimulationLog_Task(task_2);
                        
                        pthread_mutex_lock(&isTaskInsertedMutex); //aquire lock to modify the isTaskInserted variable.

                        isTaskInserted++; //task inserted.
                        pthread_mutex_unlock(&isTaskInsertedMutex); //aquire lock to modify the 

                        pthread_cond_signal(&taskCpuCondition); //to the end
                    }
                }
            }

            int res = isT1_Inserted - isT2_Inserted;
            if(res == 0){
                continueInsertionNew = 1;
            }
        }
        else if(getRemainingSpaces() == 1){
          //  printf("\n1 space available\n");

            if(pTask_1 != NULL){    //task available
                if(isT1_Inserted == 0){
                    setArrivalTimeTask(&task_1);
                    isT1_Inserted = insert(task_1);

                    if(isT1_Inserted == 1){
                        addSimulationLog_Task(task_1);
                        
                        pthread_mutex_lock(&isTaskInsertedMutex); //aquire lock to modify the isTaskInserted variable.

                        isTaskInserted++; //task inserted.
                        pthread_mutex_unlock(&isTaskInsertedMutex); //aquire lock to modify the 

                        pthread_cond_signal(&taskCpuCondition); //to the end
                    }
                }
            }

            if(pTask_2 != NULL){    //task available.
                if(isT2_Inserted == 0){
                    setArrivalTimeTask(&task_2);
                    isT2_Inserted = insert(task_2);

                    if(isT2_Inserted == 1){
                        addSimulationLog_Task(task_2);
                        
                        pthread_mutex_lock(&isTaskInsertedMutex); //aquire lock to modify the isTaskInserted variable.

                        isTaskInserted++; //task inserted.
                        pthread_mutex_unlock(&isTaskInsertedMutex); //aquire lock to modify the 

                        pthread_cond_signal(&taskCpuCondition); //to the end
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
            printf("TASK THREAD QUITTING : ALL ITEMS IN TASK FILE WAS ADDED TO QUEUE.\n");
            pthread_exit(0);    //terminate the thread.
          //  return 0;
        }
        //sleep(1); disabling sleep to make it fast
       
      // pthread_mutex_lock(&isTaskInsertedMutex); //aquire lock to modify the 
        
      // pthread_cond_signal(&taskCpuCondition); //to the end
    }
    
    return 0;
}

/*
 * Solution to obtain time in milliseconds taken from the given link.
 * Link: https://stackoverflow.com/questions/10192903/time-in-milliseconds-in-c
 * Author: zebo zhuang
 * Accessed: 2 May 2019
 */
long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
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
   // printf ( "Current local time and date: %s", asctime (timeinfo) );
    strcpy (ptime, asctime (timeinfo));
    
   // printf("The time---------- %s", ptime);
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
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
    }
    
    int status = fprintf(pFile, "task #: %d\nArrival time: %s\n", task.task_number, task.arrival_time);
    //printf("cpu_burst %d", cpu_burst);
    if(status < 0){
        printf("writing to simulation_log file failed\n");
        return 0;
    }
    
    fclose(pFile);
    pFile = NULL;
    return 1;
}
    
int num_tasks = 0;   //shared variables, shared across the 3 cpus.
double total_waiting_time = 0.0, total_turnaround_time = 0.0;   //shared vars across 3 cpus.

//mutexes for the shared variables. mutex per variable to increase performance .
pthread_mutex_t num_tasks_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_waiting_time_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t total_turnaround_time_mutex = PTHREAD_MUTEX_INITIALIZER;

int cpu1_task_count = 0, cpu2_task_count = 0, cpu3_task_count = 0;  //holds the number of tasks executed by each.

//This function is the function that gets executed by each cpu thread.
void* cpu( void *arg){
    int *pcpuId = (int *)arg;
    int cpuId = *pcpuId;

    printf("CPU ID: %d\n", cpuId);
    
    while(1){
        
        pthread_mutex_lock(&isTaskInsertedMutex);
        
        while(isTaskInserted == 0){     //no new tasks in ready queue to execute.
            printf("CPU-%d going to blocking state.\n", cpuId);
            pthread_cond_wait(&taskCpuCondition, &isTaskInsertedMutex);  //releases mutex waits on condition (signal).
            printf("CPU-%d going to UNBLOCKED state.\n", cpuId);
        }
        
        isTaskInserted--; //is task taken.
        pthread_mutex_unlock(&isTaskInsertedMutex);
        
        struct Task *task = pop();  //get a task from ready queue.

        if(task != NULL){   //task available from ready queue.
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

            //strcpy(twoTasks[0].arrival_time, time1);    //for sim logs.
            
            pthread_mutex_lock(&total_waiting_time_mutex);  //obtaining lock to modify total_waiting_time [shared resource]
            
            total_waiting_time = total_waiting_time + waiting_time; //compute total waiting time.
            
            pthread_mutex_unlock(&total_waiting_time_mutex);    //release the lock so another thread can have its go at it.
            
            addSimulationLog_Pre_Exec(*task, service_time, pcpuId); //adds record to simulation log with service time & other related fields.

            sleep(task->cpu_burst); //sleep for burst time, simulate cpu EXECUTING the task.

            //Obtaining completion time.....................................................................................
            time_t completion_t;  //var that stores completion time.
            time(&completion_t); //sets the completion_t to its value i.e. time now, which is essentially the completion time.

            char *completion_time = getCurrentTime(); //obtaining current time in full format.
            format_time(completion_time); //formatting it down to only contain the time.

            double turn_around_time = getTimeElapsed(arrival_t, completion_t);      //computes turn around time by getting the difference & other related fields.
            //End of obtaining completion time.....................................................................................
            printf("Turn Around Time: %f\n", turn_around_time);

            pthread_mutex_lock(&total_turnaround_time_mutex);   ////getting the lock for modification of total_turnaround_time var. [shared resource].
            
            total_turnaround_time = total_turnaround_time + turn_around_time;   //computes total turn around time.
            
            pthread_mutex_unlock(&total_turnaround_time_mutex); //release the lock so another thread can have its go at it.
            
            pthread_mutex_lock(&num_tasks_mutex);    //getting the lock for modification of num_tasks var [shared resource].
            
            num_tasks++;    //increment num of tasks executed by one. [shared resource]
            
            pthread_mutex_unlock(&num_tasks_mutex); //release the lock so another thread can have its go at it.
            
            printf("number of tasks executed all together %d\n", num_tasks);
            
            addSimulationLog_Post_Exec(*task, completion_time, pcpuId); //adds record to simulation log with completion time.
            
            switch(cpuId){  //record how many tasks each thread executed.
                case 1: cpu1_task_count++; break;
                case 2: cpu2_task_count++; break;
                case 3: cpu3_task_count++; break;
            }
        }
        else{   //task not available, ready queue empty.
            printf("Empty/no tasks available for cpu - %d execution. GOING TO EXIT PHASE\n", cpuId);
        }
       // sleep(1); disabling sleep here as not needed
                    
        if(num_tasks == NUMBER_OF_TASKS_TASK_FILE){ //terminate the cpu threads on completion of execution of all task.

            printf("CPU-%d THREAD EXIT : ALL TASKS IN TASK FILE EXECUTED.\n.", cpuId);
            break;
            
   /*         //Thread cancellation order is set in such away that the executing thread doesn't cancel itself to not cancel remaining ones.
            //see the ordering of tid_cpu# variable to see what is meant.
            if(cpuId == 1){     //thread in execution is cpu 1
                pthread_cancel(tid_cpu3);
                pthread_cancel(tid_cpu2);
                printf("CPU-1 = %d CPU-2 = %d CPU-3 %d", cpu1_task_count, cpu2_task_count ,cpu3_task_count);
                pthread_cancel(tid_cpu1);
            }else if(cpuId == 2){   //thhread in execution is cpu 2
                pthread_cancel(tid_cpu3);
                pthread_cancel(tid_cpu1);
               printf("CPU-1 = %d CPU-2 = %d CPU-3 %d", cpu1_task_count, cpu2_task_count ,cpu3_task_count);
                pthread_cancel(tid_cpu2);
            }else{
                pthread_cancel(tid_cpu1);
                pthread_cancel(tid_cpu2);
                printf("CPU-1 = %d CPU-2 = %d CPU-3 %d", cpu1_task_count, cpu2_task_count ,cpu3_task_count);
                pthread_cancel(tid_cpu3);
            }*/
           // pthread_exit(0);
        }  
            
    }
    
    pthread_cond_broadcast(&taskCpuCondition);  //unblock.
    printf("Unblock signal broadcasted\n");
    
    pthread_exit(0);
}

/*
 void cpu(){
    struct Task *task = pop();  //get a task from ready queue.
    
    if(task != NULL){   //task available from ready queue.
        printf("CPU executing task# = %d burst = %d\n", task->task_number, task->cpu_burst);
        
        //Obtaining service time, waiting time.........................................................................
        time_t arrival_t = task->arrival_t;    //getting arrival time from the task.
        time_t service_t;   //var that stores service time. Time at which task entered the cpu.
        time(&service_t);  //sets the service_t to its value i.e. time now.

        char *service_time = getCurrentTime(); //obtaining current time in full format.
        format_time(service_time); //formatting it down to only contain the time.

        double waiting_time = getTimeElapsed(arrival_t, service_t); //compute waiting time for this task by getting the difference.
        printf("Waiting TIME ELAPSED: %f\n", waiting_time);
        //END of obtaining service time, waiting time...................................................................         
           
        //strcpy(twoTasks[0].arrival_time, time1);    //for sim logs.

        total_waiting_time = total_waiting_time + waiting_time; //compute total waiting time.
        addSimulationLog_Pre_Exec(*task, service_time); //adds record to simulation log with service time & other related fields.
        
        sleep(task->cpu_burst); //sleep for burst time, simulate cpu EXECUTING the task.
        
        //Obtaining completion time.....................................................................................
        time_t completion_t;  //var that stores completion time.
        time(&completion_t); //sets the completion_t to its value i.e. time now, which is essentially the completion time.
        
        char *completion_time = getCurrentTime(); //obtaining current time in full format.
        format_time(completion_time); //formatting it down to only contain the time.
        
        double turn_around_time = getTimeElapsed(arrival_t, completion_t);      //computes turn around time by getting the difference & other related fields.
        //End of obtaining completion time.....................................................................................
        printf("Turn Around Time: %f\n", turn_around_time);
        
        total_turnaround_time = total_turnaround_time + turn_around_time;   //computes total turn around time.
        num_tasks++;    //increment num of tasks executed by one.
        addSimulationLog_Post_Exec(*task, completion_time); //adds record to simulation log with completion time.
    }
    else{   //task not available, ready queue empty.
        printf("Empty no tasks available\n");
    }
}

 */

//Adds record to simulation log containing cpu execution info (i.e. cpu num) and task info (i.e. arrival times and task num).
// To be used in cpu().
int addSimulationLog_Pre_Exec(struct Task task, char *service_time, int *cpuId){
    //int cpuId = 1;
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
    }
    
    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nService time: %s\n", *cpuId, task.task_number, task.arrival_time, service_time);
    //printf("cpu_burst %d", cpu_burst);
    if(status < 0){
        printf("writing to simulation_log file failed\n");
        return 0;
    }
    
    fclose(pFile);
    pFile = NULL;
    return 1;
}

//Adds record to simulation log containing cpu execution info (i.e. cpu num, completion time) and task info (i.e. arrival times and task num). 
//To be used in cpu().
int addSimulationLog_Post_Exec(struct Task task, char *completion_time, int *cpuId){
    //int cpuId = 1;
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
    }
    
    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nCompletion time: %s\n", *cpuId, task.task_number, task.arrival_time, completion_time);
    //printf("cpu_burst %d", cpu_burst);
    if(status < 0){
        printf("writing to simulation_log file failed\n");
        return 0;
    }
    
    fclose(pFile);
    pFile = NULL;
    return 1;
} 

//sets the arrival time of task, to be used just before insertion.
void setArrivalTimeTask(struct Task *task){
    time_t task1_start;
    time(&task1_start);

    task->arrival_t = task1_start;    //setting times that are operator friendly. for calculations.

    char *time1 = getCurrentTime(); //obtaining current time in full format.
    format_time(time1); //getting only the time

    strcpy(task->arrival_time, time1);    //for sim logs.

    printf("Task number = %d cpu_burst = %d arrival_time = %s\n", task->task_number, task->cpu_burst, task->arrival_time);
}

