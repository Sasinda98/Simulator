/* 
 * File:   timeTask.h
 * Author: W.A.A.D Gayal Sasinda Rupasinghe
 */

/*
 * The purpose of this header is to hold functions related to time, specifically to work with task times.
 * named timeTask.h as time.h is already an existing header in C.
 */

#include <unistd.h>

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


