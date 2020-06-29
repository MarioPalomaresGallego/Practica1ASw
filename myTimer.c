#include "myTimer.h"
#include "unistd.h"

#include <rtems/irq-extension.h>

/*
 * Librería que hace uso de los timers internos del chip ARM para poder ejecuter en QEMU.
 * (Los timers que implemente la BSP no están implementados en QEMU)
 *
 * Timer Virtual -> Usado para llamdas sleep -> ThreadController
 * Timer Físico -> Usado para sustiturir el timer -> ThreadDisplay
 *
 * la diferencia entre ambos timers es el valor que toman como contador. Ambos usas el contador de systema
 * (global para todos los cores) pero en el caso del timer virtual se le resta un offset.
 * En nuestro caso no hacemos uso de esta "virtualización" por lo que podríamos decir que amos son
 * iguales.
 *
 */

#define CORE0_TIMER_IRQCNTL 0x40000040 //Registro para activar/desactivar las interrupciónes de los disitintos timers tanto en la lines IRQ como FIQ

static uint32_t cntfrq = 0;

/*
 * Función que escirbe en un registro mapeado en memoria
 */
static inline void mmio_write(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*)reg = data;
}

/*
 * Dirige las interruptciones del timer virtual y el físico al la linea IRQ del core 0
 */
void routing_core0_irq(void)
{
    mmio_write(CORE0_TIMER_IRQCNTL, 0x09);
}

/*
 * Habilita el timer correspondiente
 *
 * type = 0 -> Virtual
 * type = 1 -> Físico
 */
void enable_cnt(int type)
{
    uint32_t cntv_ctl=1;
    if(type==0) asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl));
    else asm volatile("mcr p15, 0, %0, c14, c2, 1" :: "r"(cntv_ctl));
}

/*
 * Deshabilita el timer correspondiente
 *
 * type = 0 -> Virtual
 * type = 1 -> Físico
 */
void disable_cnt(int type)
{
	uint32_t cntv_ctl=0;
	if(type==0) asm volatile ("mcr p15, 0, %0, c14, c3, 1" :: "r"(cntv_ctl) );
	else  asm volatile ("mcr p15, 0, %0, c14, c2, 1" :: "r"(cntv_ctl) );
}


/*
 * Lee el valor del contador de sistema dependiendo del timer especiciado.
 * Ambos timers tienen el mismo contador pero en el caso del virtual, se le puede aplicar un offset para que vea un tiempo difertene (ver @read_cntv_off)
 *
 * type = 0 -> Virtual
 * type = 1 -> Físico
 */
uint64_t read_cnt_ct(int type)
{
	uint64_t val;
	if(type==0) asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (val));
	else asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (val));
	return (val);
}

/*
 * Lee el valor del offset asignado al TIMER VIRTUAL.
 * En caso de ser 0 el timer virtual tiene la misma percepción del tiempo que el físico
 */
uint64_t read_cntv_off(void)
{
    uint64_t val;
    asm volatile("mrrc p15, 4, %Q0, %R0, c14" : "=r" (val));
    return (val);
}

/*
 * Leemos el valor, en ciclos, que debe eseprar el timer para lanzar la interrupción
 *
 * type = 0 -> Virtual
 * type = 1 -> Físico
 */
uint32_t read_cnt_tval(int type)
{
    uint32_t val;
    if(type==0) asm volatile ("mrc p15, 0, %0, c14, c3, 0" : "=r"(val) );
    else asm volatile ("mrc p15, 0, %0, c14, c2, 0" : "=r"(val) );
    return val;
}

/*
 * Escribimos el valor, en ciclos, que debe eseprar el timer para lanzar la interrupción
 *
 * type = 0 -> Virtual
 * type = 1 -> Físico
 */
void write_cnt_tval(int type,uint32_t val)
{
    if(type==0) asm volatile ("mcr p15, 0, %0, c14, c3, 0" :: "r"(val) );
    else asm volatile ("mcr p15, 0, %0, c14, c2, 0" :: "r"(val) );
    return;
}

/*
 * Leemos la frecuenta a la que está funcionando el contador del sistema
 */
uint32_t read_cntfrq(void)
{
    uint32_t val;
    asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r"(val) );
    return val;
}


/*
 * ISR destinado para el control del timer virual
 */
extern pthread_t controller_Thread;
rtems_isr_entry virtual_timer_handler()
{
	disable_cnt(0);
	//Realmente vale cualquier señal
	pthread_kill(controller_Thread,SIGRTMAX);
    return NULL;
}

/*
 * ISR destinado para el control del timer físico
 */
extern pthread_t display_Thread;
rtems_isr_entry physical_timer_handler()
{
	write_cnt_tval(1,cntfrq/2);
	pthread_kill(display_Thread,SIGRTMAX);
    return NULL;
}


void mySleep(struct timespec sleep){
	int totalSleep= cntfrq*sleep.tv_sec;
	float partial= sleep.tv_nsec * pow(10,-9);
	partial = partial*cntfrq;
	totalSleep+=partial;
	write_cnt_tval(0,totalSleep);
    enable_cnt(0);
    pause();
}

void loopTimer(){
	write_cnt_tval(1,cntfrq/2);
	enable_cnt(1);

}

void myGetTime(struct timespec* result){
	uint64_t counter= read_cnt_ct(1);
	result->tv_sec=counter/cntfrq;
	float partial = (float)(counter%cntfrq)/cntfrq;
	result->tv_nsec= partial * pow(10,9);
}

void setupTimers(){
    cntfrq = read_cntfrq();
    routing_core0_irq();
    rtems_interrupt_handler_install(27,NULL,RTEMS_INTERRUPT_SHARED,(rtems_interrupt_handler)virtual_timer_handler,NULL);
    rtems_interrupt_handler_install(30,NULL,RTEMS_INTERRUPT_SHARED,(rtems_interrupt_handler)physical_timer_handler,NULL);

}
