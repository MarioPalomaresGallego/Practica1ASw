#include <time.h>
#include <math.h>

/*
 * Funcion que hace uso del Timer Virtual para implementar la función sleep
 */
void mySleep(struct timespec);

/*
 * Función que utiliza el contador físico para obtener el "wall time" del programa - Sustituye clock_gettime
 */
void myGetTime(struct timespec*);

/*
 * Función que inictializa tanto el timer virtual como el físico para poder hacer uso de ellos
 */
void setupTimers();

/*
 * Función que lanza por primera vez el timer físico
 */
void loopTimer();
