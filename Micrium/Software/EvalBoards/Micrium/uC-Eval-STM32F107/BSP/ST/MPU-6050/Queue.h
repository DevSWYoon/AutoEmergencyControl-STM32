#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <includes.h>

#define MAX_QUEUE_SIZE 10

typedef s16 acceldata[6];

typedef struct {
    uint8_t head;
    uint8_t tail;
    acceldata buffer[MAX_QUEUE_SIZE];
} queue_structure;

void init_queue(queue_structure* q);
void push(queue_structure* q, acceldata* data);
void pop(queue_structure* q, acceldata* data);
uint8_t isEmpty(queue_structure* q);