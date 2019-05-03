
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

#include <time.h>
 #include <sys/time.h>

int generateTaskFile(char *fileName);
int readTaskFile(char *fileName);
struct Task *getNextTwoTasks(char *fileName);
int task(char *fileName);
long long timeInMilliseconds(void);
void format_time(char *output);
char *getCurrentTime();
double  getTimeElapsed();
int addSimulationLog_Task(struct Task task);
void cpu();
int addSimulationLog_Pre_Exec(struct Task task, char *service_time);
int addSimulationLog_Post_Exec(struct Task task, char *service_time);

int main(int argc, char** argv) {
    //File name and amount of tasks m is taken here.
    printf("Scheduler started!\n");

    struct Task tasks[4];
    initialize(tasks, 4);

    char *output = getCurrentTime();
    
    format_time(output);
    printf("from mainnnnnn  %s", output);
    
  
    time_t start, end;
    
    time(&start);
    
    //sleep(2);
    
    time(&end);
    
    printf("Time elapsed: %0.3f\n", getTimeElapsed(start, end));
  /*  
    struct Task ts1;
    ts1.task_number = 44;
    
    struct Task ts2;
    ts2.task_number = 77;
    
    struct Task ts3;
    ts3.task_number = 99;
        
    struct Task ts4;
    ts4.task_number = 101;
    
    int a,b,c; 
    
    struct Task task_1[2];
    task_1[0] = ts1;
    task_1[1] = ts2;
    
    struct Task task_2[2];
    task_2[0] = ts3;
    task_2[1] = ts4;
    
    struct Task task_3[2];
    task_3[0] = ts1;
    task_3[1] = ts3;

    
    insertTwo(task_1);  //2
    insertTwo(task_2);  //4
    pop();
    struct Task trf;
    trf.task_number = 6;
    insert(trf);
    
    
    insertTwo(task_3);  //6
    
    */
  
    task("task_file");
    
    //sleep(2);
    
      
    task("task_file");
    
    time_t starte;
    
    struct Task tsk;
    tsk.arrival_t = time(&starte);
    
    // cpu();
   // cpu();
    cpu();
    
    task("task_file");
    
    cpu();
    
    task("task_file");
   
/*
    
    for(int i=0; i<52; i++){
        struct Task *task = getNextTwoTasks("task_file");   //returns null on error or EOF else pointer is returned.
        if( task != NULL){  //if task is getting returned.
            //printf("%d %d\n", task->task_number, task->cpu_burst);
            //printf("%d %d\n", (task+1)->task_number, (task+1)->cpu_burst);
            insert(*task);
            insert(*(task+1));
        }
    }
    while(isEmpty()==0){
          struct Task *out = pop();
             printf("Popped task#= %d\n", out->task_number);
    }
    
   */
    
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


int continueInsertionNew = 1;      //determines whether new task insertion is possible or not, NOT possible when there are tasks left over needing insertion.
int outcome = 0;    //holds outcome of the insertion.
struct Task *ptask_array = NULL;   //pointer to the array that holds the 2 tasks.
/*
 * Gets two tasks from file and adds it to ready queue if available 
 * and sets appropriate fields in the task to their respective values.
 */
int task(char *fileName){
    
    if(continueInsertionNew == 1){ //previous add was successful continue with inserting new tasks.
         ptask_array = getNextTwoTasks(fileName);   //getting the next two tasks.
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
            printf("Failed to add -- top \n");
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
    return continueInsertionNew;
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
    
//This function is the function that gets executed by each cpu thread.
int num_tasks = 0;   //shared variables, shared across the 3 cpus.
double total_waiting_time = 0.0, total_turnaround_time = 0.0;   //shared vars across 3 cpus.
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

//Adds record to simulation log containing cpu execution info (i.e. cpu num) and task info (i.e. arrival times and task num).
// To be used in cpu().
int addSimulationLog_Pre_Exec(struct Task task, char *service_time){
    int cpuNum = 1;
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
    }
    
    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nService time: %s\n", cpuNum, task.task_number, task.arrival_time, service_time);
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
int addSimulationLog_Post_Exec(struct Task task, char *completion_time){
    int cpuNum = 1;
    
    FILE *pFile = fopen("simulation_log", "a");    //open for writing.        
    
    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.");
        scanf("%s", temp);
        return 0;
    }
    
    int status = fprintf(pFile, "Statistics for CPU-%d:\nTask #%d\nArrival time: %s\nCompletion time: %s\n", cpuNum, task.task_number, task.arrival_time, completion_time);
    //printf("cpu_burst %d", cpu_burst);
    if(status < 0){
        printf("writing to simulation_log file failed\n");
        return 0;
    }
    
    fclose(pFile);
    pFile = NULL;
    return 1;
}