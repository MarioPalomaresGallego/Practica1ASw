# Módulo Software - Práctica Sistemas de Tiempo Real

## Introducción

Este proyecto se corresponde con el desarrollo del módulo software en la práctica 1, apartado A, de la asignatura de "Sistemas de Tiempo Real". En esta práctica se propone desarrollar un sistema de control de tiempo real con la siguiente arquitectura:
* **Nodo Controlador**: Contiene toda la lógica de control del sistema. En la práctica se referencia a esta parte como Módulo Software.
* **Nodo Esclavo**: Simula el componente físico a controlar. En la práctica se referencia a esta parte como Módulo Hardware.
* **Entorno**: Se simula mediante un circuito electrónico

El escenario que se simula en la prática es el control remoto de una caretilla. Esta caretilla se corresponde con el "Nodo Esclavo" y debe captar información sobre el "Entorno" y posteriormente envíarla al "Nodo Controlador". Este procesará la información recibida y responderá a la carretilla con la acción a realizar. La comunicación entre ambos elementos se realiza a través de USB.

El software de desarrollo utilizado actualmente en la práctica es el siguiente:
* Arduino IDE -> Para desarrollar todo el código de del Módulo Hardware que lógicamente ejecutará el un Arduino
* Wind River Workbanch -> Para desarrollar el código del Módulo Hardware que ejecuta en una máquina virutal creada en el propio IDE la cual correo el sistema operativo de tiempo real, VxWorks.

El trabajo realizado con este proyecto adapta el código del Módulo Sofware a un entorno totalmente nuevo donde sustituimos el uso de Wind River Workbench por Eclipse y el uso de VxWorks por RTEMS. Derivados de estos cambios, los ejecutables pasaron a ejecutar a una placa física; RaspberryPi (RPI).

En este repositorio hablaremos de todo lo que concierne al desarrollo del Módulo Software en el nuevo entorno. 

Para saber más sobre el módulo hardware: [Módulo Hardware - Práctica Sistemas de Tiempo Real](https://github.com/MarioPalomaresGallego/Practica1AHw.git)

## Objetivo

Con el nuevo entorno el desarrollo del Módulo Software implica trabajar sobre un sistema empotrado, en este caso la RPI. En este tipo de desarrollo el programa se edita y compila en un ordenador normal y posteriormente se carga en el sistema empotrado para ejecutar, a esto se le conoce como desarrollo cruzado. Normalmente este proceso se lleva a cabo de esta forma ya que el sistema empotrado no tiene las capacidades de ejecutar un sistema operativo. La RPI en cambio es principalmente conocida por la capacidad de correr Raspbian, un sistema operativo basado en Linux por lo que en la RPI, de manera excepcional se puede llevar a cabo un desarrollo "normal". En nuestro caso el uso de Raspbian no sirve ya que como hemos dicho el programa a ejecutar tiene restricciones de tiempo real y por ello va empaquetado con su propio sistema operativo de tiempo real. 

En resumen, nos tenemos que ceñir al desarollo en crudo sobre sistemas empotrados. En este mundo cosas tan sencillas como ejecutar o depurar se vuelven más compiladas por la simple razón de que el ejecutable se encuentra en otra máquina diferente. Es por esto que el objetivo de este documento es especificar una guía de todas las instalaciones y configuraciones necesarias para poder acercarnos todo lo posible a un desarrollo "normal".

**IMPORTANTE**

* La instalación que se va a mostrar a continuación funciona para **Eclipse bajo el sitema operativo Ubuntu 18.04**.

* Esta guía de instlación en ningún momento comenta como instalar RTEMS ni cómo compilar los ficheros. Para ello es muy recomentable leer primero: [Instalación de RTEMS para la RPI](https://www.raspberrypi.org/forums/viewtopic.php?t=147130) && [Integración RTEMS Eclipse](https://devel.rtems.org/wiki/Developer/Eclipse/Plugin)


## Carga Remota

En el caso de la RPI, si nos ceñimos al proceso estandar, probar un ejecutable implica:
1. Compilar el fichero
2. Conectar la micro-SD al ordenador
3. Cargar el ejecutable
4. Volver a conectar la micro-SD a la RPI y enchufarla a la corriente

(En la RPI la micro-SD funciona como disco duro y por ello en al encenderla busca ahi todos los ficheros de arranque y el kernel)

Como veis se trata de un proceso bastante lento e incómodo que puede incluso llegar a dañar el slot de la micro-SD. Para ello se suelen utilizan lo que se conoce como bootloaders que no son más que programas que se encargan de la puesta en marcha del sistema. En nuestro caso vamos a usar U-boot. Este es un bootloader cuya función es configurar el sistema antes de pasar la ejecución al programa del ususario. Lo que nos interesa es que nos permite cargar los ficheros via serie o por red, evitando asi el proceso de la micro-SD. A continuación explicamos como establecer ambos métodos.

### Serial

### Red

## Simulación

## Depuración remota
