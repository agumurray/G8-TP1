#include "../inc/wall_follower.h"
#include "../../drivers/inc/hc_sr04.h"
#include "../../drivers/inc/l298n.h"
#include "../../drivers/inc/robot_pins.h"

#define FRONT_OBSTACLE_THRESHOLD 15  // cm: debajo de esto se detiene
#define SPEED_LEFT  120
#define SPEED_RIGHT 130

void wall_follower_init(void) {
    printf("Wall follower inicializado (solo detección de obstaculo frontal)\r\n");
    l298n_stop_all();
}

void wall_follower_step(void) {
    // Leer sensor frontal
    uint32_t dist_front = hc_sr04_distance_cm(SR04_02_ECHO_GPIO, SR04_02_TRIG_GPIO) / 100;

    printf("Front: %lu cm\r\n", dist_front);

    // Si hay obstáculo cerca → detenerse
    if (dist_front < FRONT_OBSTACLE_THRESHOLD) {
        printf("Obstaculo detectado al frente: deteniendo motores\r\n");
        l298n_stop_all();
    } 
    else {
        // Si el camino está libre → avanzar recto
        l298n_forward_left();
        l298n_forward_right();
        l298n_set_speed_left(SPEED_LEFT);
        l298n_set_speed_right(SPEED_RIGHT);
    }
}
