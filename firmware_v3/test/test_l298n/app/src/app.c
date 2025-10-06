#include "sapi.h"

#define IN1   GPIO0
#define IN2   GPIO1
#define ENA   GPIO2

int main(void) {
   boardConfig();
   gpioConfig(IN1, GPIO_OUTPUT);
   gpioConfig(IN2, GPIO_OUTPUT);

   pwmConfig(0, PWM_ENABLE);
   pwmConfig(ENA, PWM_ENABLE_OUTPUT);

   uartConfig(UART_USB, 115200);
   printf("Prueba Motor A - L298N\r\n");

   while(TRUE) {
      printf("Giro horario\r\n");
      gpioWrite(IN1, ON);
      gpioWrite(IN2, OFF);

      for(int duty = 0; duty <= 255; duty += 25) {
         pwmWrite(ENA, duty);
         printf("PWM = %d\r\n", duty);
         delay(2000);
      }

      delay(1000);

      printf("Giro antihorario\r\n");
      gpioWrite(IN1, OFF);
      gpioWrite(IN2, ON);

      for(int duty = 255; duty >= 0; duty -= 25) {
         pwmWrite(ENA, duty);
         printf("PWM = %d\r\n", duty);
         delay(2000);
      }

      gpioWrite(IN1, OFF);
      gpioWrite(IN2, OFF);
      pwmWrite(ENA, 0);
      printf("Motor detenido\r\n");
      delay(3000);
   }
}
