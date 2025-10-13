// middleware/src/motor_control.c
#include "motor_control.h"
#include "l298n.h"

void motor_control_init(void){
   l298n_init();
}

void motor_control_forward(uint8_t speed){
   l298n_forward_left();
   l298n_forward_right();
   l298n_set_speed_left(speed);
   l298n_set_speed_right(speed);
}

void motor_control_backward(uint8_t speed){
   l298n_backward_left();
   l298n_backward_right();
   l298n_set_speed_left(speed);
   l298n_set_speed_right(speed);
}

void motor_control_turn_right(uint8_t speed){
   l298n_forward_left();
   l298n_backward_right();
   l298n_set_speed_left(speed);
   l298n_set_speed_right(speed);
}

void motor_control_turn_left(uint8_t speed){
   l298n_backward_left();
   l298n_forward_right();
   l298n_set_speed_left(speed);
   l298n_set_speed_right(speed);
}

void motor_control_stop(void){
   l298n_stop_all();
}
