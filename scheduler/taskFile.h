/* 
 * File:   taskFile.h
 * Author: W.A.A.D Gayal Sasinda Rupasinghe
 */

/*
 * The purpose of this header is to hold functions related to task file operations.
 */

#include <unistd.h>

//Generates task file
void generateTaskFile(char *fileName){

    FILE *pFile = fopen(fileName, "w");    //open/create file for writing.

    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.\n");
        scanf("%s", temp);
        exit(-1);
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
            printf("ERROR: Writing/creating to task file failed, Application will quit...\n");
            exit(-1);
        }
    }

    fclose(pFile);  //closing opened file.
    pFile = NULL; //making sure ref is not there anymore.

}

//Returns number of tasks available in task file.
long readHead;
int getMaxTaskNumber(char *fileName){
    int numberOfTasks = 0;
    FILE *pFile = fopen(fileName, "r");    //open for writing.

    if(pFile == NULL){
        char temp[3];
        printf("Failed to open file, press any key followed by enter key to exit.\n");
        scanf("%s", temp);
        exit(-1);    //quit entire app
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
    printf("TASK FILE HAS: %d tasks.\n", numberOfTasks);

    fclose(pFile);  //closing opened file
    pFile = NULL; //making sure reference is not there anymore.
    return numberOfTasks;
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
        printf("Failed to open file, press any key followed by enter key to exit.\n");
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

