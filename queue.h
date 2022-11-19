/* Header file for the simple circular queue example */
#ifndef __QUEUE_H__
#define __QUEUE_H__

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

typedef struct _queue {
	int size;    /* maximum size of the queue */
	job **buffer; /* queue buffer */
	int start;   /* index to the start of the queue */
	int end;     /* index to the end of the queue */
	int count;   /* no. of elements in the queue */
} queue;



queue *queue_init(int n);
int queue_insert(queue *q, job *jobp);
job *queue_delete(queue *q);
void queue_display(queue *q);
void queue_destroy(queue *q);

#endif
