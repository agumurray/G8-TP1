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

/* --- Versión base de medición --- */
uint32_t hc_sr04_measure_us(uint8_t echo_pin, uint8_t trig_pin){
   gpioWrite(trig_pin, OFF);
   delayMicroseconds(2);
   gpioWrite(trig_pin, ON);
   delayMicroseconds(10);
   gpioWrite(trig_pin, OFF);
   return medirPulso(echo_pin);
}

/* --- Conversión genérica: microsegundos → centésimas de cm --- */
static uint32_t us_to_cm100(uint32_t dur_us){
   if(dur_us == 0) return 0;
   // distancia = (dur_us / 58) cm → multiplicamos por 100 para devolver centésimas
   return (dur_us * 100UL) / 58UL;
}

/* ===========================================================
   FUNCIONES DE CALIBRACIÓN POR SENSOR
   =========================================================== */

/* Factores de calibración (ajustables) */
static float cal_front = 1.00f;  // sensor frontal
static float cal_left  = 0.75f;  // sensor izquierdo
static float cal_right = 0.80f;  // sensor derecho

/* --- Setters para calibración --- */
void hc_sr04_set_cal_front(float factor){ cal_front = factor; }
void hc_sr04_set_cal_left(float factor){ cal_left = factor; }
void hc_sr04_set_cal_right(float factor){ cal_right = factor; }

/* --- Funciones específicas para cada sensor --- */
uint32_t hc_sr04_distance_cm_front(void){
   uint32_t dur = hc_sr04_measure_us(SR04_02_ECHO_GPIO, SR04_02_TRIG_GPIO);
   uint32_t cm100 = us_to_cm100(dur);
   return (uint32_t)(cm100 * cal_front);
}

uint32_t hc_sr04_distance_cm_left(void){
   uint32_t dur = hc_sr04_measure_us(SR04_03_ECHO_GPIO, SR04_03_TRIG_GPIO);
   uint32_t cm100 = us_to_cm100(dur);
   return (uint32_t)(cm100 * cal_left);
}

uint32_t hc_sr04_distance_cm_right(void){
   uint32_t dur = hc_sr04_measure_us(SR04_01_ECHO_GPIO, SR04_01_TRIG_GPIO);
   uint32_t cm100 = us_to_cm100(dur);
   return (uint32_t)(cm100 * cal_right);
}

/* ===========================================================
   COMPATIBILIDAD CON API ORIGINAL
   =========================================================== */
uint32_t hc_sr04_distance_cm(uint8_t echo_pin, uint8_t trig_pin){
   uint32_t dur_us = hc_sr04_measure_us(echo_pin, trig_pin);
   return us_to_cm100(dur_us);
}
