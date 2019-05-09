
/*
 * File:   readyqueue.h
 * Author: W.A.A.D Gayal Sasinda Rupasinghe
 * Title: Queue data structure to handle the ready queue.
 * Created on 30 April 2019, 7:41 PM
 */

/*
 * The purpose of this header is to hold functions related to the ready queue.
 * Contains the implementation of the queue data structure.
 */


#include <time.h>
#include <string.h>
#include <pthread.h>


struct Task{
    int task_number;
    int cpu_burst;
    char arrival_time[50];  //stores time in character form, human readable.
    char service_time[50];  //stores time in character form, human readable.
    int termination_time;
    time_t arrival_t;
};

int front = -1;
int rear = -1;

int nItems = 0; //unread items
int successful_insertions = 0;  //stores how many times queue was inserted with tasks.

int MAX_SIZE = 0;

struct Task *pTaskArray = NULL; //global pointer

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nItems_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t remaining_Mutex = PTHREAD_MUTEX_INITIALIZER;

void initialize(struct Task tasks[], int size){

    /*
     * Resolved the trap 6 error I got on runtime by using this link. "You are writing memory you do not own"
     * Author: ryyker
     * Link: https://stackoverflow.com/questions/26431147/abort-trap-6-error-in-c
     * Accessed: 2 May 2019
     */

    pTaskArray = malloc(sizeof(struct Task) * size);    //Allocating required memory, the fix for trap 6. Forgot to allocate memory as a result got trap 6 error.
    //*pTaskArray = tasks[0]; //getting pointer setup to first index of task array.
    MAX_SIZE = size;

}

int isEmpty(){
    if(nItems == 0){
        return 1;
    }
    return 0;
}

int isFull(){

    if(nItems == MAX_SIZE){
        return 1;
    }else{
        return 0;
    }

}

int insert(struct Task newTask){

    pthread_mutex_lock(&queue_mutex);

    if(isFull() == 0){    //not full
        ++rear;
        if(rear == (MAX_SIZE))  //if rear at last index
            rear = rear % (MAX_SIZE);
        printf("\n\nREAR ==== %d\n\n", rear);

        *(pTaskArray + rear) = newTask;
        successful_insertions++;
        printf("\n INSERTION SUCCESSFUL = %d %d\n", (pTaskArray + rear)->task_number, (pTaskArray + rear)->cpu_burst );

        pthread_mutex_lock(&nItems_mutex);
            nItems++;
        pthread_mutex_unlock(&nItems_mutex);

        pthread_mutex_unlock(&queue_mutex);
        return 1;   //success
    }
    else{
        printf("\n INSERTION FAILED = %d %d\n", newTask.task_number, newTask.cpu_burst );
        pthread_mutex_unlock(&queue_mutex);
        return 0;   //fail
    }

}

int getRemainingSpaces(){
    return  MAX_SIZE - nItems;
}

int getSuccessfulInsertions(){
    return successful_insertions;
}

struct Task *pop(){
     pthread_mutex_lock(&queue_mutex);
    if( isEmpty() == 0 ){   //not empty
        ++front;
        if(front == (MAX_SIZE))
            front = front % (MAX_SIZE);
        pthread_mutex_lock(&nItems_mutex);
        nItems--;
        pthread_mutex_unlock(&nItems_mutex);
    pthread_mutex_unlock(&queue_mutex);
        return (pTaskArray + front);
    }
    else{
        printf("Nothing left to pop!\n");
    pthread_mutex_unlock(&queue_mutex);
        return NULL;
    }
}

void destroy_queue(){
    free(pTaskArray);
}


