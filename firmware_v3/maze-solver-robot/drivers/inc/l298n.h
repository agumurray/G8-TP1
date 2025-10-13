// drivers/inc/l298n.h
#ifndef L298N_H
#define L298N_H

#include "sapi.h"
#include <stdint.h>

void l298n_init(void);
void l298n_set_speed_left(uint8_t speed);   // 0..255
void l298n_set_speed_right(uint8_t speed);
void l298n_forward_left(void);
void l298n_backward_left(void);
void l298n_stop_left(void);
void l298n_forward_right(void);
void l298n_backward_right(void);
void l298n_stop_right(void);
void l298n_stop_all(void);

#endif // L298N_H
