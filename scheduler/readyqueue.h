
/* 
 * File:   readyqueue.h
 * Author: sasinda
 * Title: Queue data structure to handle the ready queue.
 * Created on 30 April 2019, 7:41 PM
 */
#include <time.h>
#include <string.h>


struct Task{
    int task_number;
    int cpu_burst;
    char arrival_time[20];  //stores time in character form, human readable.
    char service_time[20];  //stores time in character form, human readable.
    int termination_time;
    time_t arrival_t;   
    time_t service_t;   //arrival - service is waiting time
    time_t termination_t;   //termination - arrival is turn around time.  
};

int front = -1;
int rear = -1;

int nItems = 0; //unread items

int MAX_SIZE = 0;

struct Task *pTaskArray = NULL; //global pointer

void initialize(struct Task tasks[], int size){   
    
    /*
     * Resolved the trap 6 error I got on runtime by using this link. "You are writing memory you do not own"
     * Author: ryyker
     * Link: https://stackoverflow.com/questions/26431147/abort-trap-6-error-in-c
     * Accessed: 2 May 2019
     */
    
    pTaskArray = malloc(sizeof(struct Task) * size);    //Allocating required memory, the fix for trap 6. Forgot to allocate memory as a result got trap 6 error.
    *pTaskArray = tasks[0]; //getting pointer setup to first index of task array.
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
    }
    return 0;
}

int isFullTwo(){
    if(( MAX_SIZE % 2 ) == 0){
        if(nItems == MAX_SIZE){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{   //if max size is odd
        if(nItems == (MAX_SIZE - 1)){   //cannot fill last one because two by two insertion. therefore queue is considered full even if one space is avail.
            return 1;
        }
        else{
            return 0;
        }
    }
}

int insert(struct Task newTask){
    ++rear;
    
    if(isFull() == 0){    //not full
        if(rear == (MAX_SIZE - 1))  //if rear at last index
            rear = rear % (MAX_SIZE);
        
        
        *(pTaskArray + rear) = newTask;
        printf("\n Inserted = %d\n", (pTaskArray + rear)->task_number );
        nItems++;
        return 1;   //success
    }
    else{
        printf("Insertion Failed.\n");
        return 0;   //fail
    }

}

//inserts two tasks if spaces available at one go, if not it'll add only one. else error.
//returns 1 when 1st task gets inserted.
//returns 2 when both tasks are inserted.
//returns 0 when insertion fails.
int insertTwo(struct Task newTask[]){

    int remaining = MAX_SIZE - nItems;
    
    if(remaining >= 2){     //two spaces or more available.
        insert(newTask[0]);
        insert(newTask[1]);
        return 2;   //both tasks inserted.
    }
    else if(remaining == 1){    //one space available.
        insert(newTask[0]);
        return  1;  //first task was inserted.
    }else{  //no space
        printf("Fails the insertion for 2 tasks\n");
        return 0;   //fail
    }
}


struct Task *pop(){
 
    if( isEmpty() == 0 ){   //not empty
        ++front;
        if(front == (MAX_SIZE - 1))
            front = front % (MAX_SIZE);
     
        nItems--;
        return (pTaskArray + front);
    }
    else{
        printf("Nothing left to pop!\n");
        return NULL;
    }
}


