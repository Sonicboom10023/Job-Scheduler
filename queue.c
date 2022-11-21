/* Implementation of a simple circular queue using a static array */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

/* create the queue data structure and initialize it */
queue *queue_init(int n) {
	queue *q = (queue *)malloc(sizeof(queue));
	q->size = n;
	q->buffer = malloc(sizeof(int)*n);
	q->start = 0;
	q->end = 0;
	q->count = 0;

	return q;
}

// job create_job(char *command, int jobid){
//     job j;
//     j.jobid = jobid;
//     //initialize temporary value and null terminate it
//     char *temp = malloc(sizeof(char)*strlen(command));
//     strcpy(temp, command);
//     j.command = temp;
//     j.stat = "waiting";
//     j.exitstat = -1;
//     j.start = NULL;
//     j.stop = NULL;
//     sprintf(j.jobout,"%d.out", j.jobid);
//     sprintf(j.joberr,"%d.out", j.jobid);
//     return j;
// }

/* insert a job into the queue, update the pointers and count, and
   return the no. of jobs in the queue (-1 if queue is null or full) */
int queue_insert(queue *q, job *jobp) {
	if ((q == NULL) || (q->count == q->size))
	   return -1;

	q->buffer[q->end % q->size] = jobp;	
	q->end = (q->end + 1) % q->size;
	q->count++;

	return q->count;
}

/* delete an item from the queue, update the pointers and count, and 
   return the item deleted (-1 if queue is null or empty) */
job *queue_delete(queue *q) {
	if ((q == NULL) || (q->count == 0))
	   return (job*)-1;
	
	job *j = q->buffer[q->start];
	q->start = (q->start + 1) % q->size;
	q->count--;

	return j;
}

/* display the contents of the queue data structure */
void queue_display(queue *q) {
	int i;
	if (q != NULL && q->count != 0) {
		printf("queue has %d elements, start = %d, end = %d\n", 
			q->count, q->start, q->end);
		printf("queue contents: ");
		for (i = 0; i < q->count; i++)
	    		printf("%d ", q->buffer[(q->start + i) % q->size]->jobid);
		printf("\n");
	} else
		printf("queue empty, nothing to display\n");
}

/* delete the queue data structure */
void queue_destroy(queue *q) {
	free(q->buffer);
	free(q);
}

