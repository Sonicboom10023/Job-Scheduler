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


typedef struct job{
    int jobid;     // job id
    pthread_t tid; //thread id
    char *command; //command for job to run
    char *stat;    // job status
    int exitstat; //job exit status
    char *start;  // job start time
    char *stop;   // job stop time
    char jobout[10]; //filename for stdout redirect
    char joberr[10]; //filename for err redirect
} job;


int P; //job concurrency based on initial input
int curworking; //# of currently running jobs
job Jobs[JOBSMAX]; //global array of inputted jobs
queue *JobQ; //queue of pointers to jobs


void list_jobs(job *jobs, int n, char *mode){
    int i;
    if (jobs != NULL && n != 0){
        if (strcmp(mode, "showjobs") == 0 ){
            for (i = 0; i < n; i++){
                printf("Job ID: %d\nCommand: %s\nStatus: %s\n\n", jobs[i].jobid, jobs[i].command, jobs[i].stat);
            }
        }
    }
}

//create job and fill values
job create_job(char *command, int jobid){
    job j;
    j.jobid = jobid;
    //initialize temporary value and null terminate it
    char *temp = malloc(sizeof(char)*strlen(command));
    temp[strlen(temp)+1] = (char*)strcat(temp, command);
    j.command = temp;
    j.stat = "waiting";
    j.exitstat = -1;
    j.start = NULL;
    j.stop = NULL;
    sprintf(j.jobout,"%d.out", j.jobid);
    sprintf(j.joberr,"%d.out", j.jobid);
    return j;
}



void *run_job(void *arg){
    job *jobp; //job pointer from arg
    char **jobargs; //job arguments, to be parsed
    pid_t pid;

    jobp = (job *)arg;
    ++curworking;
    jobp->stat = "working";
    time_t tim1 = time(NULL);
    jobp->start = ctime(&tim1);
}

//takes input from std and inserts new jobs, while also running commands
void process_input(){
    int i; //keep track of job #
    char line[LINEMAX]; //reading buffer
    char *key; //first input
    char *command; //command input
    size_t *buffer = malloc(sizeof(char)*LINEMAX);

    i=0;
    while (getline(line, buffer,stdin)!= -1){  //check input from user
        if ((key = strtok(line," \n\t\r"))!= NULL){ //assign first seen value to key, as it will most likely be a
                                                    
            if (strcmp(key, "submit")==0){
                if (i >= JOBSMAX){
                    printf("Job limit reached, please reset.");
                }
                else if (JobQ->count >= JobQ->size){
                    printf("Job queue full, please wait for other jobs to finish.");
                }
                else{
                    command = strtok(line, "\n");
                    Jobs[i] = create_job(command, i);
                    queue_insert(JobQ, Jobs + i);
                    printf("Job %d added to job queue\n", i++);
                }
            }
        }
    }
    kill(0,SIGINT);
}

int main(int argc, char **argv) {
    char *jiderr; //main stderr redirect
    pthread_t tid; 
    if (argc < 2) {
        printf("Usage: %s concurrency wrong\n", argv[0]);
        exit(-1);
    }
    struct queue_init(5);
}
