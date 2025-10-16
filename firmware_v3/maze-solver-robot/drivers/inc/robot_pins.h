// drivers/inc/robot_pins.h
#ifndef ROBOT_PINS_H
#define ROBOT_PINS_H

// Pines HC-SR04
// Sensor izquierda
#define SR04_01_TRIG_GPIO   GPIO0
#define SR04_01_ECHO_GPIO   GPIO2

// Sensor central
// #define SR04_02_TRIG_GPIO   GPIO7
// #define SR04_02_ECHO_GPIO   GPIO8

// Sensor derecha
// #define SR04_03_TRIG_GPIO   T_COL2
// #define SR04_03_ECHO_GPIO   T_FIL1

// Pines L298N
#define L298_IN1 GPIO1
#define L298_IN2 GPIO3
#define L298_ENA PWM2 // T_COL0

#define L298_IN3 GPIO4
#define L298_IN4 GPIO6
#define L298_ENB PWM5 // T_COL1
#endif // ROBOT_PINS_H
