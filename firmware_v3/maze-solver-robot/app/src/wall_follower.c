#include "sapi.h"
#include "../inc/wall_follower.h"
#include "../../drivers/inc/hc_sr04.h"
#include "../../drivers/inc/l298n.h"
#include "../../drivers/inc/robot_pins.h"

// ================== UMBRALES DE DISTANCIA ==================
// dist_front: decide si hay obstáculo al frente
// dist_left / dist_right: seguimiento y decisiones en bifurcaciones
#define FRONT_OBSTACLE_THRESHOLD 12
#define WALL_MIN_DISTANCE 7
#define WALL_MAX_DISTANCE 11
#define WALL_FAR_THRESHOLD 35

// ================== SENSOR DERECHO BUG ==================
// CASE: sensor derecho buguea cuando está muy pegado -> devuelve ~465cm (+- tol)
#define RIGHT_SENSOR_BUG_VALUE_CM   465
#define RIGHT_SENSOR_BUG_TOL_CM     10
#define RIGHT_SENSOR_BUG_MAX_LOOPS  6
#define RIGHT_SENSOR_BUG_LOOP_DELAY 10

// ================== SENSOR IZQUIERDO BUG ==================
// CASE: sensor izquierdo buguea cuando está muy pegado -> devuelve ~413cm (+- tol)
#define LEFT_SENSOR_BUG_VALUE_CM    413
#define LEFT_SENSOR_BUG_TOL_CM      10
#define LEFT_SENSOR_BUG_MAX_LOOPS   6
#define LEFT_SENSOR_BUG_LOOP_DELAY  10

// ================== ROBUSTEZ “DERECHA LIBRE” ==================
// CASE: evitar falsos positivos de "derecha libre" usando confirmación (debounce)
#define RIGHT_FREE_CONFIRM_COUNT     2
// CASE: además de derecha libre, exigir frente "bien libre" (margen extra)
#define RIGHT_FREE_FRONT_MARGIN_CM   12
// CASE: cooldown luego de un giro a derecha para evitar bucles
#define IGNORE_RIGHT_AFTER_TURN_CYCLES 10
// CASE: override para U (aunque haya cooldown, si el frente está MUY libre)
#define RIGHT_TURN_OVERRIDE_FRONT_CM  30

// ================== VELOCIDADES ==================
#define SPEED_LEFT 100
#define SPEED_RIGHT 120
#define SPEED_PULSE 220
#define SPEED_TURN_SLOW 120
#define SPEED_TURN_FAST 200

// ================== VELOCIDADES DE CORRECCIÓN ==================
// CASE: corrección a derecha "sin pivot" (ambos motores, sesgo)
#define CORR_BOTH_BASE_SPEED 140
#define CORR_BOTH_BIAS_SPEED 185
#define CORR_BOTH_TIME_MS    35

// ================== GIROS ==================
#define TURN_PULSE_MS       20
#define TURN_RIGHT_MS       130
#define TURN_LEFT_MS        170
#define TURN_180_LEFT_MS    400
#define TURN_180_RIGHT_MS   410

// CASE: bloqueo largo de "derecha libre => derecha" luego de giros que podrían generar falso giro
#define IGNORE_RIGHT_TURN_CYCLES 20

static uint8_t right_turn_ignore_cycles = 0;  // CASE: contador de cooldown post giro derecha
static uint8_t right_free_confirm = 0;        // CASE: contador debounce de derecha libre

static inline bool right_is_bug(uint32_t d){
    // CASE: lectura derecha dentro del rango bug (465 +- tol)
    return (d >= (RIGHT_SENSOR_BUG_VALUE_CM - RIGHT_SENSOR_BUG_TOL_CM)) &&
           (d <= (RIGHT_SENSOR_BUG_VALUE_CM + RIGHT_SENSOR_BUG_TOL_CM));
}

static inline bool left_is_bug(uint32_t d){
    // CASE: lectura izquierda dentro del rango bug (413 +- tol)
    return (d >= (LEFT_SENSOR_BUG_VALUE_CM - LEFT_SENSOR_BUG_TOL_CM)) &&
           (d <= (LEFT_SENSOR_BUG_VALUE_CM + LEFT_SENSOR_BUG_TOL_CM));
}

void wall_follower_init(void){
    l298n_stop_all();
    right_turn_ignore_cycles = 0;
    right_free_confirm = 0;
}

void stop(void){
    l298n_stop_all();
}

void avanzar(void){
    // CASE: avance normal (pulso inicial + velocidad estable)
    l298n_forward_left();
    l298n_forward_right();
    l298n_set_speed_left(SPEED_PULSE);
    l298n_set_speed_right(SPEED_PULSE);
    delay(20);
    l298n_set_speed_left(SPEED_LEFT);
    l298n_set_speed_right(SPEED_RIGHT);
}

static void corregir_izquierda_suave(void){
    // CASE: demasiado cerca de pared derecha -> empujar un poco hacia la izquierda (solo motor derecho)
    l298n_stop_all();
    delay(5);
    l298n_forward_right();
    l298n_set_speed_right(SPEED_PULSE);
    delay(5);
    l298n_set_speed_right(150);
    delay(15);
}

static void corregir_izquierda_media(void){
    // CASE: corrección más fuerte a izquierda (usada para salir del bug del sensor derecho)
    l298n_stop_all();
    delay(5);
    l298n_forward_right();
    l298n_set_speed_right(SPEED_PULSE);
    delay(8);
    l298n_set_speed_right(160);
    delay(25);
}

static void corregir_derecha_suave(void){
    // CASE: salir del bug del sensor izquierdo sin pivotar (ambos motores, sesgo a derecha)
    l298n_stop_all();
    delay(5);
    l298n_forward_left();
    l298n_forward_right();
    l298n_set_speed_left(CORR_BOTH_BIAS_SPEED);
    l298n_set_speed_right(CORR_BOTH_BASE_SPEED);
    delay(CORR_BOTH_TIME_MS);
}

// ================== GIRO DERECHA ==================
void derecha(void){
    // CASE: ejecutar giro 90° derecha (y activar cooldown anti-bucle)
    right_turn_ignore_cycles = IGNORE_RIGHT_AFTER_TURN_CYCLES;
    right_free_confirm = 0;

    stop();
    delay(100);

    const int pasos = TURN_RIGHT_MS / TURN_PULSE_MS;

    for(int i = 0; i < pasos; i++){
        l298n_forward_left();
        l298n_backward_right();
        l298n_set_speed_left(SPEED_TURN_FAST);
        l298n_set_speed_right(SPEED_TURN_FAST);
        delay(TURN_PULSE_MS);
        stop();
        delay(10);
    }

    // CASE: post-giro -> pequeño avance para "entrar" al pasillo
    stop();
    delay(50);
    avanzar();
    delay(150);
    stop();
    delay(200);
}

// ================== GIRO IZQUIERDA ==================
void izquierda(void){
    // CASE: ejecutar giro 90° izquierda (y bloquear giros a derecha por varios ciclos)
    right_turn_ignore_cycles = IGNORE_RIGHT_TURN_CYCLES;
    right_free_confirm = 0;

    stop();
    delay(100);

    const int pasos = TURN_LEFT_MS / TURN_PULSE_MS;

    for(int i = 0; i < pasos; i++){
        l298n_backward_left();
        l298n_forward_right();
        l298n_set_speed_left(SPEED_TURN_FAST);
        l298n_set_speed_right(SPEED_TURN_FAST);
        delay(TURN_PULSE_MS);
        stop();
        delay(10);
    }

    // CASE: post-giro -> pequeño avance
    avanzar();
    delay(15);
    stop();
    delay(200);
}

// ================== GIRO 180 IZQUIERDA ==================
void giro_180_izquierda(void){
    // CASE: giro 180° fijo hacia la izquierda (bloquea giros a derecha por varios ciclos)
    right_turn_ignore_cycles = IGNORE_RIGHT_TURN_CYCLES;
    right_free_confirm = 0;

    stop();
    delay(100);

    const int pasos = TURN_180_LEFT_MS / TURN_PULSE_MS;

    for(int i = 0; i < pasos; i++){
        l298n_backward_left();
        l298n_forward_right();
        l298n_set_speed_left(160);
        l298n_set_speed_right(160);
        delay(TURN_PULSE_MS);
        stop();
        delay(10);
    }

    delay(200);
}

// ================== GIRO 180 DERECHA (SE DEJA POR LAS DUDAS) ==================
void giro_180_derecha(void){
    // CASE: giro 180° fijo hacia la derecha (NO se usa en la lógica actual, queda por si se necesita)
    right_turn_ignore_cycles = IGNORE_RIGHT_TURN_CYCLES;
    right_free_confirm = 0;

    stop();
    delay(100);

    const int pasos = TURN_180_RIGHT_MS / TURN_PULSE_MS;

    for(int i = 0; i < pasos; i++){
        l298n_forward_left();
        l298n_backward_right();
        l298n_set_speed_left(160);
        l298n_set_speed_right(160);
        delay(TURN_PULSE_MS);
        stop();
        delay(10);
    }

    delay(200);
}

// ================== WALL FOLLOWER STEP ==================
void wall_follower_step(void){

    // CASE: inicio de step -> frenar y estabilizar lectura de sensores
    stop();
    delay(40);

    // CASE: leer distancias (cm)
    uint32_t dist_front = hc_sr04_distance_cm_front() / 100;
    uint32_t dist_left  = hc_sr04_distance_cm_left()  / 100;
    uint32_t dist_right = hc_sr04_distance_cm_right() / 100;
   

    // CASE: detectar si algún sensor está en el valor típico de bug
    bool right_bug = right_is_bug(dist_right);
    bool left_bug  = left_is_bug(dist_left);

    // CASE: cooldown anti-bucle luego de giro a derecha
    if (right_turn_ignore_cycles > 0){
        right_turn_ignore_cycles--;
    }

    // CASE: BUG DERECHO (465+-tol) -> corregir a izquierda hasta que deje de buguear
    if (right_bug){
        for (int i = 0; i < RIGHT_SENSOR_BUG_MAX_LOOPS; i++){
            corregir_izquierda_media();
            delay(RIGHT_SENSOR_BUG_LOOP_DELAY);
            dist_right = hc_sr04_distance_cm_right() / 100;
            if (!right_is_bug(dist_right)) break;
        }
        // CASE: si hubo bug derecho, no acumulamos “derecha libre”
        right_free_confirm = 0;
    }

    // CASE: BUG IZQUIERDO (413+-tol) -> corregir a derecha hasta que deje de buguear
    if (left_bug){
        for (int i = 0; i < LEFT_SENSOR_BUG_MAX_LOOPS; i++){
            corregir_derecha_suave();
            delay(LEFT_SENSOR_BUG_LOOP_DELAY);
            dist_left = hc_sr04_distance_cm_left() / 100;
            if (!left_is_bug(dist_left)) break;
        }
    }

    // ================== CASE: FRENTE LIBRE ==================
    if (dist_front > FRONT_OBSTACLE_THRESHOLD){

        // CASE: detectar "derecha libre" (derecha muy lejos + frente suficientemente libre)
        const bool right_free_raw =
            (!right_bug) &&
            (dist_right > WALL_FAR_THRESHOLD) &&
            (dist_front > (FRONT_OBSTACLE_THRESHOLD + RIGHT_FREE_FRONT_MARGIN_CM));

        // CASE: debounce de derecha libre (requiere lecturas consecutivas)
        if (right_free_raw){
            if (right_free_confirm < 255) right_free_confirm++;
        } else {
            right_free_confirm = 0;
        }

        // CASE: override para U (aunque haya cooldown, si el frente está MUY libre y derecha muy confirmada)
        const bool override_ok =
            (right_turn_ignore_cycles > 0) &&
            (right_free_confirm >= (RIGHT_FREE_CONFIRM_COUNT + 1)) &&
            (dist_front > (FRONT_OBSTACLE_THRESHOLD + RIGHT_TURN_OVERRIDE_FRONT_CM));

        // CASE: girar a derecha por “derecha libre” (condición normal o override)
        if ((right_turn_ignore_cycles == 0 && right_free_confirm >= RIGHT_FREE_CONFIRM_COUNT) || override_ok){
            delay(60);
            derecha();
            return;
        }

        // CASE: seguir recto (distancia derecha dentro del rango ideal) O derecha muy lejos (no bug)
        if (
            (dist_right >= WALL_MIN_DISTANCE && dist_right <= WALL_MAX_DISTANCE) ||
            ((!right_bug) && (dist_right > WALL_FAR_THRESHOLD))
        ){
            avanzar();
            delay(15);
        }
        // CASE: demasiado lejos de la pared derecha -> corregir hacia la derecha (solo motor izquierdo)
        else if ((!right_bug) && dist_right > WALL_MAX_DISTANCE){
            l298n_forward_left();
            l298n_set_speed_left(150);
            delay(15);
        }
        // CASE: demasiado cerca de la pared derecha -> corregir hacia la izquierda (solo motor derecho)
        else if ((!right_bug) && dist_right <= WALL_MIN_DISTANCE){
            corregir_izquierda_suave();
        }
        // CASE: fallback frente libre -> avanzar
        else{
            avanzar();
        }
    }

    // ================== CASE: FRENTE BLOQUEADO ==================
    else{

        // CASE: frente bloqueado -> reset debounce de derecha libre
        right_free_confirm = 0;

        // CASE: frente bloqueado + solo pared izquierda (izq cerca, der libre) -> girar derecha
        if (dist_left <= WALL_FAR_THRESHOLD && (!right_bug && dist_right > WALL_FAR_THRESHOLD)){
            derecha();
        }
        // CASE: frente bloqueado + solo pared derecha (der cerca, izq libre) -> girar izquierda
        else if ((!right_bug && dist_right <= WALL_FAR_THRESHOLD) && dist_left > WALL_FAR_THRESHOLD){
            izquierda();
        }
        // CASE: frente bloqueado + paredes a ambos lados -> GIRO 180 (SIEMPRE a izquierda)
        else if ((!right_bug && dist_right <= WALL_FAR_THRESHOLD) && dist_left <= WALL_FAR_THRESHOLD){
            // 🔥 AHORA SIEMPRE 180 A IZQUIERDA 🔥
            giro_180_izquierda();
            // CASE: post 180 -> avanzar un poquito
            avanzar();
            delay(50);
        }
        // CASE: frente bloqueado + ambos lados libres -> prioridad derecha
        else if (dist_left > WALL_FAR_THRESHOLD && dist_right > WALL_FAR_THRESHOLD){
            derecha();
        }
        // CASE: fallback frente bloqueado -> avanzar
        else{
            avanzar();
        }
    }
}

void wall_follower_try(void){
    // CASE: función de prueba -> ejecutar un 180 a izquierda
    stop();
    delay(1000);
    giro_180_izquierda();
    stop();
    delay(2000);
}
