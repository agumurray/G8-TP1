/*============================================================================
 * Test HC-SR04 en EDU-CIAA (sin uso de flotantes en printf)
 *===========================================================================*/

#include "sapi.h"

#define TRIG_GPIO   GPIO2   // Cambiá según tu conexión real
#define ECHO_GPIO   GPIO3

#define ECHO_TIMEOUT_US 30000UL // 30 ms (~5 m)

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

/* --- Medición del pulso HIGH en microsegundos --- */
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

/* --- Programa principal --- */
int main(void){
   boardConfig();
   dwtInit();

   gpioConfig(TRIG_GPIO, GPIO_OUTPUT);
   gpioConfig(ECHO_GPIO, GPIO_INPUT);
   uartConfig(UART_USB, 115200);

   printf("Iniciando medicion HC-SR04...\r\n");

   while(TRUE){
      // Generar pulso de 10 µs
      gpioWrite(TRIG_GPIO, OFF);
      delayMicroseconds(2);
      gpioWrite(TRIG_GPIO, ON);
      delayMicroseconds(10);
      gpioWrite(TRIG_GPIO, OFF);

      // Medir tiempo del eco
      uint32_t dur_us = medirPulso(ECHO_GPIO);

      if(dur_us == 0){
         printf("Fuera de rango o sin eco\r\n");
      } else {
         // Distancia = tiempo / 58 (µs/cm)
         uint32_t hundredths = (dur_us * 100UL) / 58UL; // centésimas de cm
         uint32_t cm = hundredths / 100UL;
         uint32_t dec = hundredths % 100UL;
         printf("Distancia: %lu.%02lu cm\r\n",
                (unsigned long)cm, (unsigned long)dec);
      }

      delay(200);
   }
   return 0;
}
