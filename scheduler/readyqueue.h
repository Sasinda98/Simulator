
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

int front = -1;
int rear = -1;

int nItems = 0; //unread items

int MAX_SIZE = 0;

struct Task *pTaskArray = NULL; //global pointer

void initialize(struct Task tasks[], int size){   
   
    pTaskArray = &tasks[0]; //getting pointer setup to first index of task array.
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

int insert(struct Task newTask){
    ++rear;
    
    if(isFull() == 0){    //not full
        if(rear == (MAX_SIZE - 1))  //if rear at last index
            rear = rear % (MAX_SIZE);
        
        *(pTaskArray + rear) = newTask;
        printf("\n Inserted = %d\n", *(pTaskArray + rear) );
        nItems++;
        return 1;   //success
    }
    return 0;   //fail
}

struct Task *pop(){
    ++front;
    
    if( isEmpty() == 0 ){   //not empty
    
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





