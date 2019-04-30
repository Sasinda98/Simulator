
/* 
 * File:   readyqueue.h
 * Author: sasinda
 * Title: Queue data structure to handle the ready queue.
 * Created on 30 April 2019, 7:41 PM
 */

struct Task{
    int task_number;
    int cpu_burst;
    int arrival_time;
    int termination_time;
};

int top = -1;
int rear = 0;
int full = 0; //number of full indexes
int empty = 0;  //number of empty indexes

int nItems = 0;

int MAX_SIZE;
struct Task *pTaskArray = NULL; //global pointer

void initialize(int maxSize){   
    MAX_SIZE = maxSize;
    struct Task tasks[maxSize];
    pTaskArray = &tasks[0]; //getting pointer setup to first index of task array.
}  

int insert(struct Task newTask){
    if(rear <= (MAX_SIZE - 1)){
        *(pTaskArray + rear) = newTask;
        rear++;
        return 0;   //success
    }else{
        return -1;  //fail
    }
}

struct Task* pop(){
    if( ++top <= (MAX_SIZE - 1) ){
          return (pTaskArray + top);
    }
    else{
        return NULL;
    }
}

