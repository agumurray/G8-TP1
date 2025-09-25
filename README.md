# TdP-Grupo8-2024

**Robot resolutor de laberintos con EDU-CIAA**

Este proyecto consiste en el diseño y construcción de un robot móvil capaz de resolver laberintos físicos con paredes de manera autónoma. El sistema utiliza como controlador principal la **EDU-CIAA**, que gestiona los sensores y motores, y ejecuta algoritmos de navegación para alcanzar la salida sin necesidad de un mapa previo.

## Características principales

* Exploración autónoma de laberintos físicos.
* Algoritmos de resolución basados en estrategias clásicas (wall-following, Pledge, Trémaux).
* Plataforma móvil con dos motores DC controlados mediante un puente H L298N.
* Sensores ultrasónicos para la detección de paredes y obstáculos.
* Posibilidad de extender funcionalidades con:

  * Telemetría mediante ESP32 e interfaz web.
  * Algoritmos de búsqueda óptima (Flood Fill, A*).
  * Sensores adicionales como giroscopio para corrección de trayectoria.

## Tecnologías utilizadas

* **Hardware:**

  * EDU-CIAA (controlador central).
  * Motores DC con puente H L298N.
  * Sensores ultrasónicos HC-SR04.
  * Batería y sistema de alimentación dedicado.
  * (Opcional) ESP32 para telemetría y giroscopio.

* **Software:**

  * Lenguaje C para firmware en la EDU-CIAA.
  * Algoritmos de filtrado de señales y resolución de laberintos.
  * (Opcional) HTML/CSS/JavaScript para interfaz web de monitoreo.

## Objetivos

* **General:** Desarrollar un robot controlado por EDU-CIAA capaz de explorar físicamente un laberinto con paredes y alcanzar la salida mediante estrategias de navegación autónoma.

* **Primarios:**

  1. Diseñar la plataforma mecánica y de potencia (chasis, ruedas, transmisión, alimentación).
  2. Integrar sensado frontal para detección de paredes/obstáculos.
  3. Implementar un algoritmo de resolución de laberintos con información local.
  4. Validar el funcionamiento en laberintos de prueba con métricas definidas.

* **Secundarios:**

  1. Incorporar una interfaz web para recolectar telemetría.
  2. Implementar algoritmos de búsqueda óptima.
  3. Integrar un sensor giroscópico para corrección de maniobras.

## Integrantes

Este proyecto fue desarrollado por el **Grupo 8** en el marco de la materia **Taller de Proyecto I** en la Facultad de Ingeniería de la UNLP:

* Cabral Ramiro Nicolas
* Murray Agustín
* González Juan
* Mendoza Tomás

---