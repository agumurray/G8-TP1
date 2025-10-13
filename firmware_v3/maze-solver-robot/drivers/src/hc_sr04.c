// drivers/src/hc_sr04.c
#include "../inc/hc_sr04.h"
#include "../inc/robot_pins.h"

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

void hc_sr04_init(void){
   dwtInit();
   gpioConfig(SR04_TRIG_GPIO, GPIO_OUTPUT);
   gpioConfig(SR04_ECHO_GPIO, GPIO_INPUT);
}

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

uint32_t hc_sr04_measure_us(uint8_t echo_pin, uint8_t trig_pin){
   // Generar pulso de 10 µs
   gpioWrite(trig_pin, OFF);
   delayMicroseconds(2);
   gpioWrite(trig_pin, ON);
   delayMicroseconds(10);
   gpioWrite(trig_pin, OFF);

   return medirPulso(echo_pin);
}

uint32_t hc_sr04_distance_cm(uint8_t echo_pin, uint8_t trig_pin){
   uint32_t dur_us = hc_sr04_measure_us(echo_pin, trig_pin);
   if(dur_us == 0) return 0; // fuera de rango
   // Distancia = tiempo / 58 (µs/cm). Retornamos en centésimas de cm para mayor resolución opcionalmente.
   uint32_t hundredths = (dur_us * 100UL) / 58UL;
   // devolvemos en centésimas; si querés solo cm, retorná hundredths/100
   return hundredths; // centésimas de cm
}
