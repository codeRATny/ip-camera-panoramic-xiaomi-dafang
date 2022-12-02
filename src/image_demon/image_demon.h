#pragma once
#include <mqueue.h>

#define PORT 5555

typedef struct _queue{
    mqd_t pic2motor_queue;
    mqd_t motor2pic_queue;
}queue;

queue mqueue;