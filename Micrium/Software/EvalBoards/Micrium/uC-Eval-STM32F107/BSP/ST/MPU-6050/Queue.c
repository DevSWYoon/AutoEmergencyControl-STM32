#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <includes.h>
#include "Queue.h"

void init_queue(queue_structure* q) {
    q->head = 0;
    q->tail = 0;
    memset(q->buffer, 0, sizeof(q->buffer));
}

void push(queue_structure* q, acceldata* data) {
		int i;
    if ((q->head + 1) % MAX_QUEUE_SIZE == q->tail) {
        return;
    }
		for(i = 0; i<6; ++i) {
			q->buffer[q->head][i] = (*data)[i];
		}

    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
}

void pop(queue_structure* q, acceldata* data) {
	  int i;
    if (q->head == q->tail) {
         return;
    }
		
		for(i=0; i<6; ++i) {
			(*data)[i] = q->buffer[q->tail][i];
		}

    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
}

uint8_t isEmpty(queue_structure* q) {
    return q->head == q->tail;
}
