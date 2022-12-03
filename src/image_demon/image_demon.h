#ifndef _IMAGE_DEMON_H_
#define _IMAGE_DEMON_H_

#include <mqueue.h>
#include "head_motor.h"

#define PORT 5555

int init_queue();
int send_command_to_motor(motor_action_e action);
motor2pic_t receive_command_from_motor();
void close_queue();
#endif