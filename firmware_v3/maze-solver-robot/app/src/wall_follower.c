#include "../inc/wall_follower.h"
#include "../../drivers/inc/hc_sr04.h"
#include "../../drivers/inc/l298n.h"
#include "../../drivers/inc/robot_pins.h"

#define FRONT_OBSTACLE_THRESHOLD   15   // cm: obstáculo frontal
#define LEFT_WALL_TARGET_DISTANCE  15   // cm: distancia deseada a la pared izquierda
#define LEFT_WALL_TOLERANCE         5   // cm: margen de error aceptable

#define SPEED_BASE_LEFT   150
#define SPEED_BASE_RIGHT  144
#define SPEED_ADJUST       40

void wall_follower_init(void) {
    printf("Wall follower inicializado (seguimiento de pared izquierda + obstáculo frontal)\r\n");
    l298n_stop_all();
}

void wall_follower_step(void) {
    // Leer distancias (convertir a cm)
    uint32_t dist_front = hc_sr04_distance_cm(SR04_02_ECHO_GPIO, SR04_02_TRIG_GPIO) / 100;
    uint32_t dist_left  = hc_sr04_distance_cm(SR04_03_ECHO_GPIO, SR04_03_TRIG_GPIO) / 100;

    printf("Front: %lu cm | Left: %lu cm\r\n", dist_front, dist_left);

    // 1️⃣ Si hay obstáculo al frente → detenerse
    if (dist_front < FRONT_OBSTACLE_THRESHOLD) {
        printf("Obstáculo al frente -> deteniendo motores\r\n");
        l298n_stop_all();
        return;
    }

    // 2️⃣ Control de seguimiento de pared izquierda
    if (dist_left < (LEFT_WALL_TARGET_DISTANCE - LEFT_WALL_TOLERANCE)) {
        // Demasiado cerca de la pared → girar a la derecha
        printf("Pared muy cerca -> girando derecha\r\n");
        l298n_forward_left();
        l298n_forward_right();
        l298n_set_speed_left(SPEED_BASE_LEFT + SPEED_ADJUST);
        l298n_set_speed_right(SPEED_BASE_RIGHT - SPEED_ADJUST);
    } 
    else if (dist_left > (LEFT_WALL_TARGET_DISTANCE + LEFT_WALL_TOLERANCE)) {
        // Demasiado lejos de la pared → girar a la izquierda
        printf("Pared muy lejos -> girando izquierda\r\n");
        l298n_forward_left();
        l298n_forward_right();
        l298n_set_speed_left(SPEED_BASE_LEFT - SPEED_ADJUST);
        l298n_set_speed_right(SPEED_BASE_RIGHT + SPEED_ADJUST);
    } 
    else {
        // Distancia correcta → avanzar recto
        printf("Distancia estable -> avanzando recto\r\n");
        l298n_forward_left();
        l298n_forward_right();
        l298n_set_speed_left(SPEED_BASE_LEFT);
        l298n_set_speed_right(SPEED_BASE_RIGHT);
    }
}
