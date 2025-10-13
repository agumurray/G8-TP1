// app/src/app.c

#include "sapi.h"
#include "wall_follower.h"
#include "../../drivers/inc/hc_sr04.h"
#include "../../drivers/inc/l298n.h"
#include "../../drivers/inc/robot_pins.h"

#define TEST_MODE  2   // 0=normal, 1=sensor, 2=motores

int main(void) {
    boardConfig();
    uartConfig(UART_USB, 115200);

#if TEST_MODE == 1
    printf("Test sensor HC-SR04\r\n");
    hc_sr04_init();
    while(TRUE) {
        uint32_t dist = hc_sr04_distance_cm(SR04_ECHO_GPIO, SR04_TRIG_GPIO);
        printf("Distancia: %lu cm\r\n", dist/100);
        delay(300);
    }

#elif TEST_MODE == 2
    printf("Test motores L298N\r\n");
    l298n_init();
    while(TRUE) {
        printf("Avanzar\r\n");
        l298n_forward_left();
        l298n_forward_right();
        l298n_set_speed_left(150);
        l298n_set_speed_right(150);
        delay(2000);

        printf("Giro derecha\r\n");
        l298n_forward_left();
        l298n_backward_right();
        delay(1000);

        printf("Stop\r\n");
        l298n_stop_all();
        delay(3000);
    }

#else
    printf("Maze solver - iniciando...\r\n");
    wall_follower_init();
    while(TRUE) {
        wall_follower_step();
        delay(100);
    }
#endif
}
