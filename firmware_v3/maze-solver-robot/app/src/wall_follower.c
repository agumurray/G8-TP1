#include "sapi.h"
#include "../inc/wall_follower.h"
#include "../../drivers/inc/hc_sr04.h"
#include "../../drivers/inc/l298n.h"
#include "../../drivers/inc/robot_pins.h"

// Umbrales de distancia
#define FRONT_OBSTACLE_THRESHOLD 8 // cm: obstáculo frontal
#define WALL_MIN_DISTANCE 7        // cm: distancia mínima a la pared
#define WALL_MAX_DISTANCE 11      // cm: distancia máxima a la pared
#define WALL_FAR_THRESHOLD 17      // cm: umbral para considerar que no hay pared a los costados

// Velocidades de los motores
#define SPEED_LEFT 115
#define SPEED_RIGHT 115
#define SPEED_PULSE 220
#define SPEED_TURN_SLOW 120
#define SPEED_TURN_FAST 170

void wall_follower_init(void){
    printf("Wall follower inicializado (seguimiento de pared + giros)\r\n");
    l298n_stop_all();
}

void stop(void){
    l298n_stop_all();
}

void avanzar(void){
    l298n_forward_left();    
    l298n_forward_right();   
    l298n_set_speed_left(SPEED_PULSE);
    l298n_set_speed_right(SPEED_PULSE);
    delay(20);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
}

void derecha(void){
    l298n_forward_left();    
    l298n_backward_right(); 
    l298n_set_speed_left(SPEED_PULSE);
    l298n_set_speed_right(SPEED_PULSE);
    delay(10);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
    delay(305);
}

void izquierda(void){
   l298n_backward_left();   
   l298n_forward_right();   
    l298n_set_speed_left(SPEED_PULSE);
    l298n_set_speed_right(SPEED_PULSE);
    delay(10);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
    delay(350);
}

void giro_180(void){
    l298n_backward_left();    
    l298n_forward_right(); 
    l298n_set_speed_left(160);
    l298n_set_speed_right(160);
    delay(20);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
    delay(500);
    stop();
    delay(50);
}

void wall_follower_step(void){
   stop();
   delay(15);

    // leer distancias (convertir de centesimas a cm)
    uint32_t dist_front = hc_sr04_distance_cm_front() / 100;
    uint32_t dist_left = hc_sr04_distance_cm_left() / 100;
    uint32_t dist_right = hc_sr04_distance_cm_right() / 100;

    //resetear leds
    gpioWrite(LED1, OFF);
    gpioWrite(LED2, OFF);
    gpioWrite(LED3, OFF);

    // si hay camino libre al frente
    if (dist_front > FRONT_OBSTACLE_THRESHOLD){
        // control de seguimiento de pared derecha
        if (dist_right >= WALL_MIN_DISTANCE && dist_right <= WALL_MAX_DISTANCE){
            // Distancia correcta - avanzar recto
            avanzar();
            delay(40);
        }
        // muy cerca de la pared iquierda - girar derecha
        else if (dist_right > WALL_MAX_DISTANCE){
            l298n_forward_left();
            l298n_forward_right();
            l298n_set_speed_left(SPEED_TURN_FAST);
            l298n_set_speed_right(SPEED_TURN_SLOW);
            delay(30);
        }
        // muy cerca de la pared derecha - girar izquierda
        else if (dist_right <= WALL_MIN_DISTANCE){
            l298n_forward_left();
            l298n_forward_right();
            l298n_set_speed_left(SPEED_TURN_SLOW);
            l298n_set_speed_right(SPEED_TURN_FAST + 10);
            delay(30);
        }
        else {
           avanzar();
           delay(40);
        }
    }
    // si hay obstaculo al frente, decidir hacia donde girar
    else{
        // si solo hay pared a la izquierda, girar a la derecha
        if (dist_left <= WALL_FAR_THRESHOLD && dist_right > WALL_FAR_THRESHOLD){
            gpioWrite(LED1, ON);
            derecha();
            avanzar();
            delay(40);
        }
        // si solo hay pared a la derecha, girar a la izquierda
        else if (dist_right <= WALL_FAR_THRESHOLD && dist_left > WALL_FAR_THRESHOLD){
            gpioWrite(LED2, ON);
            izquierda();
            avanzar();
            delay(40);
        }
        // si hay paredes a ambos lados, girar 180 grados
        else if (dist_right <= WALL_FAR_THRESHOLD && dist_left <= WALL_FAR_THRESHOLD){
            gpioWrite(LED3, ON);
            giro_180();
           avanzar();
           delay(50);
        }
        else{
            avanzar();
            delay(30);
        }
    }
}