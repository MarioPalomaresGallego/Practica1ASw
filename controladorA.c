/*
 * controladorA.c
 *
 *  Created on: 18 mar. 2020
 *      Author: mario
 */

/********************
 *  INCLUDES
 *******************/
#include <rtems.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "displayA.h"
#include <bsp.h>
#include <bsp/i2c.h>
#include <fcntl.h>


/********************
 *  Constants
 ********************/
#define CS_TIME 10
#define MSG_LEN 9

/********************
 *  Global Variables
 *******************/
float speed = 0.0;
struct timespec mixer;
int state_mixer = 0;
int file;

/********************
 *  Function: task_speed
 *******************/
int task_speed()
{
	char request[10];
	char answer[10];

   	//--------------------------------
    //  request speed and display it
    //--------------------------------

    //clear request and answer
    memset(request,'\0',10);
    memset(answer,'\0',10);


    // request speed
	strcpy(request,"SPD: REQ\n");

	//uncomment to use the simulator
	//simulator(request, answer);

	// uncoment to access serial module

	if(write(file,request,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Request SPD\n");
	  	pthread_exit(NULL);
	}

	if(read(file,answer,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Answer SPD\n");
	  	pthread_exit(NULL);
	}

	// display speed
	if (1 == sscanf (answer,"SPD:%f\n",&speed)){
		if(speed > 70 || speed < 40) pthread_exit(NULL);
		displaySpeed(speed);
	} else if (0 == strcmp(answer,"MSG: ERR\n")) {
		printf("MSG: ERR --> SPD\n");
		pthread_exit(NULL);
	}

	return 0;
}

/********************
 *  Function: task_slope
 *******************/
int task_slope()
{
	char request[10];
	char answer[10];

	//--------------------------------
	//  request slope and display it
	//--------------------------------

	//clear request and answer
	memset(request,'\0',10);
	memset(answer,'\0',10);

	// request slope
	strcpy(request,"SLP: REQ\n");

	//uncomment to use the simulator
	//simulator(request, answer);

	// uncoment to access serial module
	if(write(file,request,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Request SPD\n");
	  	pthread_exit(NULL);
	}

	if(read(file,answer,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Answer SPD\n");
	  	pthread_exit(NULL);
	}

	// display slope
	if (0 == strcmp(answer,"SLP:DOWN\n")) {
		displaySlope(-1);
	}
	if (0 == strcmp(answer,"SLP:FLAT\n")) {
		displaySlope(0);
	}
	if (0 == strcmp(answer,"SLP:  UP\n")) {
		displaySlope(1);
	}
	if (0 == strcmp(answer,"MSG: ERR\n")) {
		printf("MSG: ERR --> SLP\n");
		pthread_exit(NULL);
	}

	return 0;
}

/********************
 *  Function: task_gas
 *******************/
int task_gas()
{
	char request[10];
	char answer[10];

	//--------------------------------
	//  request gas and display it
	//--------------------------------

	//clear request and answer
	memset(request,'\0',10);
	memset(answer,'\0',10);

	int state = 0;
	// request gas
	if (speed >= 55.0){
		strcpy(request,"GAS: CLR\n");
	}
	else{
		strcpy(request,"GAS: SET\n");
		state = 1;
	}
	//uncomment to use the simulator
	//simulator(request, answer);



	// uncoment to access serial module
	if(write(file,request,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Request SPD\n");
	  	pthread_exit(NULL);
	}

	if(read(file,answer,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Answer SPD\n");
	  	pthread_exit(NULL);
	}


	// display gas
	if (0 == strcmp(answer,"GAS:  OK\n")){
		displayGas(state);
	}
	else if (0 == strcmp(answer,"MSG: ERR\n")){
		printf("MSG: ERR --> GAS\n");
		pthread_exit(NULL);
	}

	return 0;
}

/********************
 *  Function: task_break
 *******************/
int task_brake()
{
	char request[10];
	char answer[10];

	//--------------------------------
	//  request brake and display it
	//--------------------------------

	//clear request and answer
	memset(request,'\0',10);
	memset(answer,'\0',10);

	int state = 0;
	// request brake
	if (speed <= 55.0){
		strcpy(request,"BRK: CLR\n");
	}
	else{
		strcpy(request,"BRK: SET\n");
		state = 1;
	}
	//uncomment to use the simulator
	//simulator(request, answer);

	// uncoment to access serial module
	if(write(file,request,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Request SPD\n");
	  	pthread_exit(NULL);
	}

	if(read(file,answer,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Answer SPD\n");
	  	pthread_exit(NULL);
	}

	// display brake
	if (0 == strcmp(answer,"BRK:  OK\n")){
		displayBrake(state);
	}
	else if(0 == strcmp(answer,"MSG: ERR\n")){
		printf("MSG: ERR --> BRK\n");
		pthread_exit(NULL);
	}

	return 0;
}

/********************
 *  Function: task_mixer
 *******************/
int task_mixer()
{
	char request[10];
	char answer[10];

	//--------------------------------
	//  request mixer and display it
	//--------------------------------

	//clear request and answer
	memset(request,'\0',10);
	memset(answer,'\0',10);

	struct timespec mixer2;
	clock_gettime(CLOCK_REALTIME,&mixer2);
	int diff = mixer2.tv_sec - mixer.tv_sec;

	// request mixer
	if (diff > 30) {
		state_mixer = 1 - state_mixer;
		if (state_mixer == 0) {
			strcpy(request,"MIX: CLR\n");
		} else if (state_mixer == 1){
			strcpy(request,"MIX: SET\n");
		}
		clock_gettime(CLOCK_REALTIME, &mixer);
	} else {
		return 0;
	}

	//uncomment to use the simulator
	//simulator(request, answer);

	// uncoment to access serial module
	if(write(file,request,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Request SPD\n");
	  	pthread_exit(NULL);
	}

	if(read(file,answer,MSG_LEN)<MSG_LEN){
	  	printf("ERROR: Answer SPD\n");
	  	pthread_exit(NULL);
	}

	// display mixer
	if (0 == strcmp(answer,"MIX:  OK\n")){
		displayMix(state_mixer);
	}
	else if (0 == strcmp(answer,"MSG: ERR\n")){
		printf("MSG: ERR --> MIX\n");
		pthread_exit(NULL);
	}

	return 0;
}

/********************
 *  Function: controller
 *******************/
void *controller(void *arg)
{
	//int sc = 0;
	struct timespec end, begin, diff;
	clock_gettime(CLOCK_REALTIME, &mixer);

    // Endless loop
    while(1) {
    	clock_gettime(CLOCK_REALTIME, &begin);

    	task_speed();
		task_slope();
		task_gas();
		task_brake();
		task_mixer();

		//sc = (sc + 1) % 10;
		clock_gettime(CLOCK_REALTIME, &end);
		diff.tv_sec = 10 - (end.tv_sec - begin.tv_sec);
		diff.tv_nsec = end.tv_nsec - begin.tv_nsec;

		if (diff.tv_sec < 0) {
			pthread_exit(NULL);
		} else {
			clock_nanosleep(CLOCK_REALTIME,0,&diff,NULL);
		}
		begin.tv_sec += 10;
    }
}

/********************
 *  Function: main
 *******************/
rtems_task Init(
		rtems_task_argument ignored
){
    pthread_t thread_ctrl;
	sigset_t alarm_sig;
	int i;

	/* Block all real time signals so they can be used for the timers.
	   Note: this has to be done in main() before any threads are created
	   so they all inherit the same mask. Doing it later is subject to
	   race conditions */
	sigemptyset (&alarm_sig);

	for (i = SIGRTMIN; i <= SIGRTMAX; i++) {
		sigaddset (&alarm_sig, i);
	}
	sigprocmask (SIG_BLOCK, &alarm_sig, NULL);

    // init display
	displayInit(SIGRTMAX);

	// Inicializamos el driver para comunicarnos via I2C con el arduino

	 /* Registramos el bus. Esta llamada internamente inicializa los pines necesarios para la comunicación
	  * y registra el dispositivo bajo /dev/i2c
	  */

	rpi_i2c_init();
	rpi_i2c_register_bus("/dev/i2c", 10000);

	 // Abrimos el fichero y guardamos el descriptor en una variable global
	 if((file = open("/dev/i2c",O_RDWR))<0){
		printf("ERROR AL ABRIR EL FICHERO");
	 }

	 // Establecemos la dirección del esclavo con el que nos vamos a comunicar
	 if(ioctl(file,I2C_SLAVE,6)<0){
		printf("ERROR IOCTL\n");
		exit(-1);
	 }


    /* Create first thread */
    pthread_create(&thread_ctrl, NULL, controller, NULL);
    pthread_join (thread_ctrl, NULL);
    exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 10
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 30
#define CONFIGURE_MAXIMUM_DIRVER 10
#define CONFIGURE_MAXIMUM_POSIX_THREADS 2
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>



