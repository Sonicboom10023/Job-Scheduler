#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include "queue.h"


//maximum length of line, jobs, and job queue
#define LINEMAX 1000 
#define JOBSMAX 1000
#define JOBQMAX 1000

int P; //job concurrency based on initial input
int curworking; //# of currently running jobs
job Jobs[JOBSMAX]; //global array of inputted jobs
queue *JobQ; //queue of pointers to jobs




void *run_job(void *arg){
    job *jobp; //job pointer from arg
    char **jobargs; //job arguments, to be parsed
    pid_t pid;

    jobp = (job*)arg;

}

void process_input(){
    int i; //keep track of job #
    char line[LINEMAX]; //reading buffer
    char *key; //first input
    char *command; //command input

    i=0;
    while (getline(line, LINEMAX,stdin)!= -1){  //check input from user
        if ((key = strtok(line," \n\t\r"))!= NULL){ //assign first seen value to key, as it will most likely be a
                                                    
            if (strcmp(key, "submit")==0){
                if (i >= JOBSMAX){
                    printf("Job limit reached, please reset.");
                }
                else if (JobQ->count >= JobQ->size){
                    printf("Job queue full, please wait for other jobs to finish.");
                }
                else{
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    char *jiderr; //main stderr redirect
    pthread_t tid; 
    if (argc < 2) {
        printf("Usage: %s concurrency wrong\n", argv[0]);
        exit(-1);
    }
    struct queue_init(queueP, 5);
}
