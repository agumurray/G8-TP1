// middleware/inc/motor_control.h
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>

void motor_control_init(void);
void motor_control_forward(uint8_t speed);
void motor_control_backward(uint8_t speed);
void motor_control_turn_right(uint8_t speed);
void motor_control_turn_left(uint8_t speed);
void motor_control_stop(void);

#endif
