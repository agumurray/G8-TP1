#include "../inc/hc_sr04.h"
#include "../inc/robot_pins.h"
#include <math.h>

extern uint32_t SystemCoreClock;

/* --- Temporización precisa usando DWT --- */
static void dwtInit(void){
   CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
   DWT->CYCCNT = 0;
   DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t micros(void){
   return DWT->CYCCNT / (SystemCoreClock / 1000000UL);
}

/* --- Delay en microsegundos --- */
static void delayMicroseconds(uint32_t us){
   uint32_t start = micros();
   while((micros() - start) < us) {
      __asm__("nop");
   }
}

#define ECHO_TIMEOUT_US 30000UL // 30 ms
#define SPEED_OF_SOUND_CM_PER_US 0.0343f // 343 m/s = 0.0343 cm/µs

void hc_sr04_init(uint8_t echo_pin, uint8_t trig_pin){
   dwtInit();
   gpioConfig(trig_pin, GPIO_OUTPUT);
   gpioConfig(echo_pin, GPIO_INPUT);
}

/* --- Función interna: mide el pulso de echo --- */
static uint32_t medirPulso(uint8_t pin){
   uint32_t start;

   // Esperar inicio del pulso (LOW -> HIGH)
   start = micros();
   while(!gpioRead(pin)){
      if((micros() - start) > ECHO_TIMEOUT_US) return 0;
   }

   // Medir duración mientras esté en HIGH
   uint32_t t0 = micros();
   while(gpioRead(pin)){
      if((micros() - t0) > ECHO_TIMEOUT_US) break;
   }

   return micros() - t0;
}

/* --- Versión original (compatibilidad) --- */
uint32_t hc_sr04_measure_us(uint8_t echo_pin, uint8_t trig_pin){
   // Generar pulso de 10 µs
   gpioWrite(trig_pin, OFF);
   delayMicroseconds(2);
   gpioWrite(trig_pin, ON);
   delayMicroseconds(10);
   gpioWrite(trig_pin, OFF);

   return medirPulso(echo_pin);
}

/* --- Wrapper flotante (solo convierte el tipo) --- */
static float hc_sr04_measure_us_f(uint8_t echo_pin, uint8_t trig_pin){
   return (float)hc_sr04_measure_us(echo_pin, trig_pin);
}

/* --- Versión precisa: promedio de lecturas, devuelve cm con decimales --- */
float hc_sr04_distance_cm_precise(uint8_t echo_pin, uint8_t trig_pin, uint8_t samples){
   float total = 0.0f;
   uint8_t valid = 0;

   for(uint8_t i = 0; i < samples; i++){
      float dur_us = hc_sr04_measure_us_f(echo_pin, trig_pin);
      if(dur_us > 0 && dur_us < ECHO_TIMEOUT_US){
         float dist_cm = (dur_us * SPEED_OF_SOUND_CM_PER_US) / 2.0f;
         total += dist_cm;
         valid++;
      }
      delayMicroseconds(10000); // 10 ms entre mediciones
   }

   if(valid == 0) return -1.0f; // fuera de rango o sin lecturas válidas
   return total / valid;        // promedio filtrado
}

/* --- Versión original entera, mantiene compatibilidad --- */
uint32_t hc_sr04_distance_cm(uint8_t echo_pin, uint8_t trig_pin){
   uint32_t dur_us = hc_sr04_measure_us(echo_pin, trig_pin);
   if(dur_us == 0) return 0; // fuera de rango
   uint32_t hundredths = (dur_us * 100UL) / 58UL;
   return hundredths; // centésimas de cm
}
