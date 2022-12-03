#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "head_motor.h"

typedef struct _queue{
    mqd_t pic2motor_queue;
    mqd_t motor2pic_queue;
}queue;

queue mqueue;

int number_of_command = 0;

struct mq_attr attributes_for_motor2pic_queue = 
{
    .mq_flags = 0,
    .mq_maxmsg = MQ_MAX_NUM_OF_MESSAGES,
    .mq_msgsize = sizeof(motor2pic_t),
    .mq_curmsgs = 0
};

struct mq_attr attributes_for_pic2motor_queue = 
{
    .mq_flags = 0,
    .mq_maxmsg = MQ_MAX_NUM_OF_MESSAGES,
    .mq_msgsize = sizeof(pic2motor_t),
    .mq_curmsgs = 0
};

int init_queue()
{
    
    mqueue.pic2motor_queue = mq_open(PIC2MOTOR_QUEUE, O_WRONLY | O_CREAT, S_IWUSR, &attributes_for_pic2motor_queue);
    if (mqueue.pic2motor_queue == -1)
    {
        perror("mq_open pic2motor");
        return -1;
    }
    mqueue.motor2pic_queue = mq_open(MOTOR2PIC_QUEUE, O_RDONLY | O_CREAT, S_IWUSR, &attributes_for_motor2pic_queue);
    if (mqueue.pic2motor_queue == -1)
    {
        perror("mq_open pic2motor");
        return -1;
    }
    return 0;
}

int send_command_to_motor(motor_action_e action)
{
    pic2motor_t command;
    command.action_p2m = action;
    command.make_steps = 1;
    command.number_of_comand_p2m = ++number_of_command;
    if (mq_send(mqueue.pic2motor_queue, (const char *)&command, sizeof(command), PRIORITY_OF_QUEUE) == -1)
    {
        perror("mq_send to motor");
        return -1;
    }
    return 0;
}

motor2pic_t receive_command_from_motor()
{
    motor2pic_t action;
    if (mq_receive(mqueue.motor2pic_queue, (char *)&action, sizeof(action), NULL) == -1)
    {
        perror("mq_receive from motor");
        action.action_m2p = CAM2MOTOR_ACTION_INVALID_TYPE;
        return action;
    }
    return action;
}

void close_queue()
{
    mq_close(mqueue.pic2motor_queue);
    mq_close(mqueue.motor2pic_queue);
}