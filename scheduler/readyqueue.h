
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


int insertTwo(struct Task newTask[]){

    int remaining = MAX_SIZE - nItems;
    
    if(remaining >= 2){
        insert(newTask[0]);
        insert(newTask[1]);
    }
    else{
        printf("Fails the insertion for 2 tasks\n");
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





