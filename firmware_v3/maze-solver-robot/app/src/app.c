// app/src/app.c

#include "sapi.h"
#include "wall_follower.h"
#include "../../drivers/inc/hc_sr04.h"
#include "../../drivers/inc/l298n.h"
#include "../../drivers/inc/robot_pins.h"

#define TEST_MODE 0  // 0=normal, 1=sensor, 2=motores
// #define SPEED_LEFT 115
// #define SPEED_RIGHT 110


int main(void) {
    boardConfig();
    uartConfig(UART_USB, 115200);

    // Inicialización de sensores y motores (comun a todos los modos)
    hc_sr04_init(SR04_01_ECHO_GPIO, SR04_01_TRIG_GPIO);  // Derecha
    hc_sr04_init(SR04_02_ECHO_GPIO, SR04_02_TRIG_GPIO);  // Central
    hc_sr04_init(SR04_03_ECHO_GPIO, SR04_03_TRIG_GPIO);  // Izquierda

    l298n_init();
   
    delay(1000);

#if TEST_MODE == 1
    printf("Test 3 sensores HC-SR04 simultáneo\r\n");

    while(TRUE) { 
        uint32_t dist_right = hc_sr04_distance_cm_right() / 100;
        delay(50); // pequeña separación entre pulsos para evitar interferencias

        uint32_t dist_front = hc_sr04_distance_cm_front() / 100;
        delay(50);

        uint32_t dist_left  = hc_sr04_distance_cm_left() / 100;

        printf("Front: %lu cm | Right: %lu cm | Left: %lu cm\r\n",
               dist_front, dist_right, dist_left);

        delay(200);
    }

#elif TEST_MODE == 2
    printf("Test motores L298N - modo sincronizado\r\n");

    while(TRUE) {
       
        avanzar();
        delay(2000);

        derecha();
        delay(350);

        avanzar();
        delay(2000);

        izquierda();
        delay(410);

        avanzar();
        delay(2000);

        giro_180();
        delay(800);

        printf("Stop\r\n");
        l298n_stop_all();       
        delay(1000);
    }

#else
    printf("Maze solver - modo 0 (wall follower)\r\n");
    wall_follower_init();
    delay(5000);

    l298n_forward_left();    
    l298n_forward_right();   
    l298n_set_speed_left(200);
    l298n_set_speed_right(200);

    while(TRUE) {
        wall_follower_step();
    }
#endif
}