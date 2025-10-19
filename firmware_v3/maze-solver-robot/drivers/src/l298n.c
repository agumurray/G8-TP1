#include "../inc/l298n.h"
#include "../inc/robot_pins.h"

void l298n_init(void){
   // No es necesario boardConfig() acá, ya se hace en app.c
   gpioConfig(L298_IN1, GPIO_OUTPUT);
   gpioConfig(L298_IN2, GPIO_OUTPUT);
   gpioConfig(L298_IN3, GPIO_OUTPUT);
   gpioConfig(L298_IN4, GPIO_OUTPUT);

   pwmConfig(0, PWM_ENABLE);
   pwmConfig(L298_ENA, PWM_ENABLE_OUTPUT);
   pwmConfig(L298_ENB, PWM_ENABLE_OUTPUT);

   l298n_stop_all();
}

// --- Motor izquierdo (A) ---
void l298n_forward_left(void) {
   gpioWrite(L298_IN1, ON);
   gpioWrite(L298_IN2, OFF);
}

void l298n_backward_left(void) {
   gpioWrite(L298_IN1, OFF);
   gpioWrite(L298_IN2, ON);
}

void l298n_stop_left(void) {
   gpioWrite(L298_IN1, OFF);
   gpioWrite(L298_IN2, OFF);
}

// --- Motor derecho (B) ---
void l298n_forward_right(void) {
   gpioWrite(L298_IN3, OFF);
   gpioWrite(L298_IN4, ON);
}

void l298n_backward_right(void) {
   gpioWrite(L298_IN3, ON);
   gpioWrite(L298_IN4, OFF);
}

void l298n_stop_right(void) {
   gpioWrite(L298_IN3, OFF);
   gpioWrite(L298_IN4, OFF);
}

// --- Control de velocidad ---
void l298n_set_speed_left(uint8_t speed){
   pwmWrite(L298_ENA, speed);
}

void l298n_set_speed_right(uint8_t speed){
   pwmWrite(L298_ENB, speed);
}

// --- Stop general ---
void l298n_stop_all(void){
   l298n_stop_left();
   l298n_stop_right();
   l298n_set_speed_left(0);
   l298n_set_speed_right(0);
}
