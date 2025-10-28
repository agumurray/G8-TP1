#include "../inc/wall_follower.h"
#include "../../drivers/inc/hc_sr04.h"
#include "../../drivers/inc/l298n.h"
#include "../../drivers/inc/robot_pins.h"

// Umbrales de distancia
#define FRONT_OBSTACLE_THRESHOLD 7 // cm: obstáculo frontal (como en Arduino)
#define WALL_MIN_DISTANCE 7        // cm: distancia mínima a la pared
#define WALL_MAX_DISTANCE 13       // cm: distancia máxima a la pared
#define WALL_FAR_THRESHOLD 20      // cm: umbral para considerar que no hay pared

// Velocidades de los motores
#define SPEED_LEFT 120
#define SPEED_RIGHT 115
#define SPEED_TURN_SLOW 110
#define SPEED_TURN_FAST 160

void wall_follower_init(void){
    printf("Wall follower inicializado (seguimiento de pared + giros)\r\n");
    l298n_stop_all();
}

void stop(void){
    l298n_stop_all();
}

void avanzar(void){
    printf("Avanzar\r\n");
    l298n_forward_left();    
    l298n_forward_right();   
    //l298n_set_speed_left(200);
    //l298n_set_speed_right(200);
    //delay(50);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
    delay(50);
    // delay(2000);
}

void derecha(void){
    printf("Giro derecha\r\n");
    l298n_forward_left();    
    l298n_backward_right(); 
    l298n_set_speed_left(200);
    l298n_set_speed_right(200);
    delay(50);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
    delay(350);
}

void izquierda(void){
    printf("Giro izquierda\r\n");
    l298n_backward_left();   
    l298n_forward_right();   
    l298n_set_speed_left(200);
    l298n_set_speed_right(200);
    delay(50);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
    delay(410);
}

void giro_180(void){
    printf("Giro 180\r\n");
    l298n_forward_left();    
    l298n_backward_right(); 
    l298n_set_speed_left(200);
    l298n_set_speed_right(200);
    delay(50);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
    delay(800);
}

void wall_follower_step(void){

    // leer distancias (convertir de centesimas a cm)
    uint32_t dist_front = hc_sr04_distance_cm_front() / 100;
    uint32_t dist_left = hc_sr04_distance_cm_left() / 100;
    uint32_t dist_right = hc_sr04_distance_cm_right() / 100;

    printf("Front: %lu cm | Left: %lu cm | Right: %lu cm\r\n",
           dist_front, dist_left, dist_right);

    stop();

    // si hay camino libre al frente
    if (dist_front > FRONT_OBSTACLE_THRESHOLD)
    {
        // control de seguimiento de pared derecha
        if (dist_right >= WALL_MIN_DISTANCE && dist_right <= WALL_MAX_DISTANCE){
            // Distancia correcta - avanzar recto
            l298n_forward_left();
            l298n_forward_right();
            l298n_set_speed_left(SPEED_LEFT);
            l298n_set_speed_right(SPEED_RIGHT);
        }
        else if (dist_right > WALL_MAX_DISTANCE){
            // muy lejos de la pared - girar derecha
            l298n_forward_left();
            l298n_forward_right();
            l298n_set_speed_left(SPEED_TURN_FAST);
            l298n_set_speed_right(SPEED_TURN_SLOW);
        }
        else if (dist_right < WALL_MIN_DISTANCE){
            // muy cerca de la pared - girar izquierda
            l298n_forward_left();
            l298n_forward_right();
            l298n_set_speed_left(SPEED_TURN_SLOW);
            l298n_set_speed_right(SPEED_TURN_FAST);
        }
    }
    // si hay obstaculo al frente, decidir hacia donde girar
    else{
        // si solo hay pared a la izquierda, girar a la derecha
        if (dist_left <= WALL_FAR_THRESHOLD && dist_right > WALL_FAR_THRESHOLD){
            derecha();
        }
        // si solo hay pared a la derecha, girar a la izquierda
        else if (dist_right <= WALL_FAR_THRESHOLD && dist_left > WALL_FAR_THRESHOLD){
            izquierda();
        }
        //si hay paredes a ambos lados, girar 180 grados
        else if (dist_right <= WALL_FAR_THRESHOLD && dist_left <= WALL_FAR_THRESHOLD){
            giro_180();
        }
    }
}