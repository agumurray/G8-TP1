// drivers/inc/robot_pins.h
#ifndef ROBOT_PINS_H
#define ROBOT_PINS_H

// Pines HC-SR04 (cambiá si corresponde)
#define SR04_TRIG_GPIO   GPIO2
#define SR04_ECHO_GPIO   GPIO3

// Pines L298N (ejemplo; cambialos por los tuyos)
#define L298_IN1 GPIO0
#define L298_IN2 GPIO1
#define L298_ENA PWM2

#define L298_IN3 GPIO2
#define L298_IN4 GPIO4
#define L298_ENB PWM5

#endif // ROBOT_PINS_H
