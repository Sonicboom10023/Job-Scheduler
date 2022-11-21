#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "queue.h"


//maximum length of line, jobs, and job queue
#define LINEMAX 1000 
#define JOBSMAX 1000
#define JOBQMAX 1000

int P; //job concurrency based on user input
int curworking; //# of currently running jobs
job Jobs[JOBSMAX]; //global array of inputted jobs
queue *JobQ; //queue of pointers to jobs


void show_jobs(job *Jobs, int n, char *input){
    int i;
    if (Jobs != NULL && n != 0){
        if (strcmp(input, "showjobs") == 0){
            for (i = 0; i < n; i++){
                if (strcmp(Jobs[i].stat, "complete")!= 0){
                    sleep(1);
                    printf("Job ID: %d\nCommand: %s\nStatus: %s\n", Jobs[i].jobid,Jobs[i].command, Jobs[i].stat);
                }
            }
        }
    }
}

//create job and fill values
job create_job(char *command, int jobid){
    job j;
    j.jobid = jobid;
    j.command = command;
    j.stat = "waiting";
    j.exitstat = -1;
    j.start = NULL;
    j.stop = NULL;
    sprintf(j.jobout,"%d.out", j.jobid);
    sprintf(j.joberr,"%d.err", j.jobid);
    //free(temp);
    return j;
}

char** parsearg(char *input){
    char *temp = malloc(sizeof(char)*(strlen(input)+1));
    strcpy(temp, input);
    char *arg;
    char **argv = malloc(sizeof(char*));
    int i = 0;
    while ((arg=strtok(temp, " \n"))!=NULL){
        argv[i] = malloc(sizeof(char)*(strlen(arg)+1));
        strcpy(argv[i], arg);
        //reallocate memory for next arg or NULL, whichever it ends up being
        argv = realloc(argv, sizeof(char*)*(++i+1));
        temp = NULL;
    }
    if (i == 0){
        i = 1;
    }
    argv[i] = NULL;
    return argv;
}

void *run_job(void *arg){
    job *jobp; //job pointer from arg
    char **jobargs =(char**)malloc(sizeof(char*)*16); //job arguments, to be parsed
    pid_t pid;

    jobp = (job *) arg;

    ++curworking;
    jobp->stat = "working";
    time_t tim1 = time(NULL);
    jobp->start = ctime(&tim1);
    char *temp = malloc(sizeof(char)*16);
    pid = fork();
    
    if (pid == 0) // child process
    {
        int fd1;
        int fd2;
        // printf("Jobout: %s\n", jobp->jobout);
        // printf("Joberr: %s\n", jobp->joberr);
        if ((fd1 = open(jobp->jobout,O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1){
            fprintf(stderr,"Error: failed to open %s", jobp->jobout);
        }
        if ((fd2 = open(jobp->joberr,O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1){
            fprintf(stderr,"Error: failed to open %s", jobp->joberr);
        }
        //redirct job stderr and stdout
        dup2(fd1, STDOUT_FILENO);
        dup2(fd2, STDERR_FILENO);
        int i = 0;
        //fill jobargs array
        temp = jobp->command;
        jobargs = parsearg(jobp->command);
        execvp(jobargs[0], jobargs);
        fprintf(stderr, "Error: did not execute job %s", jobargs[0]);
        perror("execvp");
        exit(-1);
    }
    else if (pid > 0){ // parent process
        waitpid(pid, &jobp->exitstat, WUNTRACED); //wuntraced used in case child closes abruptly
        jobp->stat = "complete";
        jobp->stop = ctime(&tim1);
    }
    --curworking;
    return NULL;
}

//checks for and finishes jobs, sleep to prevent race conditions
void *do_jobs(void *arg){
    job *jobp; // job pointer
    curworking = 0;
    for(;;){
        if (JobQ->count > 0 && curworking < P){
            // pull from queue
            jobp = queue_delete(JobQ);
            //complete job with new thread
            pthread_create(&jobp->tid, NULL, run_job, jobp);
            //mark thread to free resources when done or else breaks
            //pthread_detach(jobp->tid);
        }
        sleep(3);
    }
    return NULL;
}

//takes input from std and inserts new jobs, while also running commands
void process_input(){
    int i; //keep track of job #
    char line[LINEMAX]; //reading buffer
    char *key = (char*)malloc(sizeof(char*)); //first input
    char *command = (char*)malloc(sizeof(char*)); //command input
    //size_t *buffer = malloc(sizeof(char)*LINEMAX); // buffer
    printf("Enter 'submit <command> [args]' to create a job and run it.\n"
    "Enter 'showjobs' to list all currently running/waiting jobs.\n");
    i=0;
    
    while (fgets(line, LINEMAX, stdin )!= NULL){  //check input from user
    char linecopy[LINEMAX];
    strcpy(linecopy, line);
        if ((key = strtok(linecopy," \n"))!= NULL){ //assign first seen value to key, as it will most likely be a                         
            if (strcmp(key, "submit")==0){
                if (i >= JOBSMAX){
                    printf("Job limit reached, please reset.");
                }
                else if (JobQ->count >= JobQ->size){
                    printf("Job queue full, please wait for other jobs to finish.");
                }
                else{
                    strcpy(linecopy,strtok(linecopy, " "));
                    command = line+6;
                    printf("Command: %s", command);
                    Jobs[i] = create_job(command, i);
                    queue_insert(JobQ,Jobs + i);
                    printf("Job %d added to job queue\n", ++i);
                }
            }
            else if (strcmp(key, "showjobs")==0){
                
                show_jobs(Jobs, i, key);
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
    //initialize user input value and make sure it can't be lower than 1 and less than 8
    P = atoi(argv[1]);
    if (P < 1){
        P = 1;
    }
    else if (P > 8){
        P = 8;
    }
    //getting pid name and turning into string for error log
    jiderr = malloc(sizeof(char)*(strlen(argv[0])));
    snprintf(jiderr, sizeof("11111111111.err"),"%d.err", getpid());
    int fd1;
    //redirecting output
        //open file
        if ((fd1 = open(jiderr,O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1){
            fprintf(stderr,"Error: failed to open %s", jiderr);
        }
    dup2(fd1, STDERR_FILENO);
    char *input = malloc(sizeof(char*));
    printf("Please input queue size: \n");
    fgets(input, LINEMAX, stdin);
    P = atoi(input);
    fflush(stdin);
    JobQ = queue_init(JOBQMAX);
    pthread_create(&tid, NULL, do_jobs,NULL);
    
    process_input();
    exit(-1);
}
