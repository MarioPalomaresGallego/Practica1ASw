# Módulo Software - Práctica Sistemas de Tiempo Real

## Introducción

Este proyecto se corresponde con el desarrollo del módulo software en la práctica 1, apartado A, de la asignatura de Sistemas de Tiempo Real. En esta práctica se propone desarrollar un sistema de control de tiempo real con la siguiente arquitectura:
* **Nodo Controlador**: Contiene toda la lógica de control del sistema. Ejecuta en una RPI sobre un sistema operativo de tiempo real RTEMS.
* **Nodo Esclavo**: Simula el componente físico a controlar. Ejecuta sobre un Arduino 
* **Entorno**: El entorno sobre el que se desarrolla la prácitca. Se simula mediante un circuito electrónico

En este reposositorio se encuenta todo el desarrollo correspondiente con el nodo Controlador (módulo software según la nomenclatura de la práctica).

El escenario que se simula en la prática es el control remoto de una caretilla. Esta caretilla (Arduino) debe captar información sobre el medio (circuito electrónico) y posteriormente envía la información al modulo controlador (RPI). Este procesará la información recibida y posteriormente responderá a la carretilla con la acción a realizar.
