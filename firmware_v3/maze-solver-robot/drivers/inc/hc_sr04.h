// drivers/inc/hc_sr04.h
#ifndef HC_SR04_H
#define HC_SR04_H

#include "sapi.h"
#include <stdint.h>

// void hc_sr04_init(void);
void hc_sr04_init(uint8_t echo_pin, uint8_t trig_pin);
uint32_t hc_sr04_measure_us(uint8_t echo_pin, uint8_t trig_pin); // devuelve microsegundos
uint32_t hc_sr04_distance_cm(uint8_t echo_pin, uint8_t trig_pin); // devuelve distancia en centésimas o cm

#endif // HC_SR04_H
