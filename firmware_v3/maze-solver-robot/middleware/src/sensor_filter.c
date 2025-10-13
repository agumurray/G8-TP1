// middleware/src/sensor_filter.c
#include "sensor_filter.h"

#define WINDOW_SIZE 5

static uint32_t window[WINDOW_SIZE];
static uint8_t idx = 0;
static uint8_t filled = 0;

void sensor_filter_init(void){
   for(int i=0;i<WINDOW_SIZE;i++) window[i]=0;
   idx = 0; filled = 0;
}

uint32_t sensor_filter_add_sample(uint32_t sample){
   window[idx++] = sample;
   if(idx >= WINDOW_SIZE) idx = 0;
   if(filled < WINDOW_SIZE) filled++;
   uint64_t sum = 0;
   for(int i=0;i<filled;i++) sum += window[i];
   return (uint32_t)(sum / filled);
}
