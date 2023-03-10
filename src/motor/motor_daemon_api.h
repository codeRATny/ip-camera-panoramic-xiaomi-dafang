#ifndef _MOTOR_DAEMON_API_H_
#define _MOTOR_DAEMON_API_H_

void init_contol_motor();
int calibration();
int step(int make_steps);
void goodbye_motor();

#endif