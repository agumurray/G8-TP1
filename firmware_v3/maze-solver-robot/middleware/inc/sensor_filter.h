// middleware/inc/sensor_filter.h
#ifndef SENSOR_FILTER_H
#define SENSOR_FILTER_H

#include <stdint.h>

void sensor_filter_init(void);
uint32_t sensor_filter_add_sample(uint32_t sample); // devuelve valor filtrado

#endif
