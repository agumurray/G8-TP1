#include "sapi.h"

// --- Motor A (lado izquierdo) ---
#define IN1   GPIO3
#define IN2   GPIO5
#define ENA   PWM2   // Canal PWM T_COL0

// --- Motor B (lado derecho) ---
#define IN3   GPIO4
#define IN4   GPIO6
#define ENB   PWM3   // Canal PWM T_FIL2

// --- Velocidad base ---
#define SPEED             150   // entre 0 y 255
#define SPEED_B_FORWARD   170
#define SPEED_B_REVERSE   170   // <-- motor B más rápido al retroceder

void motorA_forward(void) {
   gpioWrite(IN1, ON);
   gpioWrite(IN2, OFF);
}

void motorA_backward(void) {
   gpioWrite(IN1, OFF);
   gpioWrite(IN2, ON);
}

void motorA_stop(void) {
   gpioWrite(IN1, OFF);
   gpioWrite(IN2, OFF);
}

void motorB_forward(void) {
   gpioWrite(IN3, OFF);
   gpioWrite(IN4, ON);
}

void motorB_backward(void) {
   gpioWrite(IN3, ON);
   gpioWrite(IN4, OFF);
}

void motorB_stop(void) {
   gpioWrite(IN3, OFF);
   gpioWrite(IN4, OFF);
}

void motors_stop(void) {
   motorA_stop();
   motorB_stop();
   pwmWrite(ENA, 0);
   pwmWrite(ENB, 0);
}

int main(void) {
   boardConfig();

   // Configuración GPIO
   gpioConfig(IN1, GPIO_OUTPUT);
   gpioConfig(IN2, GPIO_OUTPUT);
   gpioConfig(IN3, GPIO_OUTPUT);
   gpioConfig(IN4, GPIO_OUTPUT);

   // Configuración PWM
   pwmConfig(0, PWM_ENABLE);
   pwmConfig(ENA, PWM_ENABLE_OUTPUT);
   pwmConfig(ENB, PWM_ENABLE_OUTPUT);

   uartConfig(UART_USB, 115200);
   printf("Prueba de movimiento - L298N con EDU-CIAA\r\n");

   while(TRUE) {
       // --- Adelante ---
      printf("Avanzar\r\n");
      motorA_forward();
      motorB_forward();
      pwmWrite(ENA, SPEED);
      pwmWrite(ENB, SPEED_B_FORWARD);
      delay(2000);
      
      motorA_stop();
      motorB_stop();
      delay(1000);

      // --- Atrás ---
      printf("Retroceder\r\n");
      motorA_backward();
      motorB_backward();
      pwmWrite(ENA, SPEED);
      pwmWrite(ENB, SPEED_B_REVERSE);  // <-- aumento de velocidad en reversa
      delay(2000);
      
      motorA_stop();
      motorB_stop();
      delay(1000);

      // --- Giro derecha ---
      printf("Giro a la derecha\r\n");
      motorA_forward();
      motorB_backward();
      pwmWrite(ENA, SPEED);
      pwmWrite(ENB, SPEED);
      delay(2000);
      
      motorA_stop();
      motorB_stop();
      delay(1000);

      // --- Giro izquierda ---
      printf("Giro a la izquierda\r\n");
      motorA_backward();
      motorB_forward();
      pwmWrite(ENA, SPEED);
      pwmWrite(ENB, SPEED);
      delay(2000);

      // --- Stop ---
      printf("Detenido\r\n");
      motors_stop();
      delay(4000);
   }

   return 0;
}