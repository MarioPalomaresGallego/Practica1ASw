# Módulo Software - Práctica Sistemas de Tiempo Real

## Introducción

Este proyecto se corresponde con el desarrollo del módulo software en la práctica 1, apartado A, de la asignatura de "Sistemas de Tiempo Real" en el **nuevo entorno establecido**.

En esta práctica se propone desarrollar un sistema de control de tiempo real con la siguiente arquitectura:
* **Nodo Controlador**: Contiene toda la lógica de control del sistema. En la práctica se referencia a esta parte como Módulo Software.
* **Nodo Esclavo**: Simula el componente físico a controlar. En la práctica se referencia a esta parte como Módulo Hardware.
* **Entorno**: Se simula mediante un circuito electrónico

El escenario que se simula en la prática es el control remoto de una caretilla. Esta caretilla se corresponde con el "Nodo Esclavo" y debe captar información sobre el "Entorno" y posteriormente envíarla al "Nodo Controlador". Este procesará la información recibida y responderá a la carretilla con la acción a realizar.

El entorno de desarrollo usado hasta el momento es:
* Arduino IDE -> Para desarrollar todo el código de del Módulo Hardware que ejecutará el un Arduino
* Wind River Workbanch -> Para desarrollar el código del Módulo Software. En este caso la ejecución se hace en una máquina virutal que se registra dentro del propio IDE. Esta máquina virual corre el sistema operativo de tiempo real VxWorks también desarrollado por Wind River.

El trabajo realizado con este proyecto adapta el código del Módulo Sofware a un entorno totalmente nuevo donde sustituimos el uso de Wind River Workbench por Eclipse y el uso de VxWorks por RTEMS. Dado que con estos cambios ya no se puede hacer uso de la máquina virtual de Wind River, los ejecutables pasaron a ejecutar a una placa física; RaspberryPi (RPI).

En este repositorio hablaremos de todo lo que concierne al desarrollo del Módulo Software en el nuevo entorno. 

Para saber más sobre el módulo hardware: [Módulo Hardware - Práctica Sistemas de Tiempo Real](https://github.com/MarioPalomaresGallego/Practica1AHw.git)

Todos los scripts que se van a comentar en esta explicación se encuentran en el repositorio [Development-Scripts](https://github.com/MarioPalomaresGallego/Development_Scripts)

## Objetivo

Con el nuevo entorno el desarrollo del Módulo Software implica trabajar sobre un sistema empotrado, en este caso la RPI. En este tipo de desarrollo el programa se edita y compila en un ordenador normal y posteriormente se carga en el sistema empotrado para ejecutar, a esto se le conoce como desarrollo cruzado. Normalmente este proceso se lleva a cabo de esta forma ya que el sistema empotrado no tiene las capacidades de ejecutar un sistema operativo. La RPI en cambio es principalmente conocida por la capacidad de correr Raspbian, un sistema operativo basado en Linux por lo que en la RPI, de manera excepcional se puede llevar a cabo un desarrollo "normal". En nuestro caso el uso de Raspbian no sirve ya que como hemos dicho el programa a ejecutar tiene restricciones de tiempo real y por ello va empaquetado con su propio sistema operativo de tiempo real. 

En resumen, nos tenemos que ceñir al desarollo en crudo sobre sistemas empotrados. En este ámbito cosas tan sencillas como ejecutar (ver la consola) o depurar se vuelven más compiladas por la simple razón de que el ejecutable se encuentra en otra máquina diferente. Es por esto que el objetivo de este documento es especificar una guía de todas las instalaciones y configuraciones necesarias para poder acercarnos todo lo posible a un desarrollo "normal", todo ello con el objetivo final de integrarlo en Eclipse.

**IMPORTANTE**

* La instalación que se va a mostrar a continuación funciona para **Eclipse bajo el sitema operativo Ubuntu 18.04. La RPI usada es una RaspberryPi2B v1.2** (También es válido para cualquier otra RPI2).

* Esta guía de instlación en ningún momento comenta como instalar RTEMS ni cómo integrar el desarrollo de aplicaciones RTEMS en Eclipse (edición y compilación). Para ello es muy recomentable leer primero: [Instalación de RTEMS para la RPI](https://www.raspberrypi.org/forums/viewtopic.php?t=147130) && [Integración RTEMS Eclipse](https://devel.rtems.org/wiki/Developer/Eclipse/Plugin)


## Visualización de la Consola

Lo primero que se debe conseguir cuando se están desarrollando programas para un sistema empotrado, es consiguir acceso a la consola. En el caso de la RPI podemos acceder a ella a través del puerto UART que ofrece en los pines 14 y 15 (8 y 10 en la cabecera).
[[Ver Pinout RPI2](https://i2.wp.com/raspberryparatorpes.net/wp-content/uploads/2016/07/raspberry-pi-gpio-alimentacion-esquema.png?ssl=1)]

Este puerto realiza una comunicación asíncorna con dos señæles
* TX: Transmitir Datos
* RX: Recibir Datos

Lo importante es que estas señales funcionana a 3.3V. Para poder conectarnos a el necesitamos usar un adaptador que respete estos niveles de voltaje. Nosotros vamos a usar el [FT2232H Mini-Module](https://uk.rs-online.com/web/p/interface-development-kits/6877768/). Este chip coniente dos canales MPSSE (MultiProtocol Serial Synchronous Engine) se trata de una funcionalidad de te permite realizar tanto comunicaciones síncronas (I2C, JTAG, SPI) como asíncornos (UART). No debe confundirse con chips como [FT232-RL](https://www.amazon.es/AZDelivery-Adaptador-FT232RL-Arduino-Adapter/dp/B01N9RZK6I/ref=asc_df_B01N9RZK6I/?tag=googshopes-21&linkCode=df0&hvadid=339932012056&hvpos=&hvnetw=g&hvrand=1411042811171982420&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9047045&hvtargid=pla-388910303692&psc=1) que únicamente pueden realizar por hardware comunicaciones asíncronas, las síncronas las smulan por software (bit-banging) lo cual tiene un desempeño peor. Además tiene dos canales independientes por lo que podemos usar un solo chip para dos comunicaciones. Todas estas características nos van a servir más adelante para realizar el método de depuración remota. Para aquellos que no quieran reailzar depuración remota, con el FT232-RL es suficiente.

La conexión que debemos realizar para el FT232-RL:

| RPI | FT232-RL |
| :---: | :---: |
| TX (8) | RX |
| RX (10)| TX |
| GND (6) | GND |

(Los pines en este chip FTDI solo vienen etiquetados por ello no ponemos numeración)

Este chip te permite escoger la salida de voltaje de los pines por lo que debemos cerciorarnos de que el chip tiene la conexión VCC - 3.3V (Se localiza fácilmente en el chip). En caso de estar VCC - 5V SE PUEDE DAÑAR LA RPI AL CONECTARLO.

La conexión para el FT2232 para el canal secundario; CN3 (el canal CN2 queda reservado para la depuración remota):

| FT2232-H | FT2232-H |
| :---: | :---: |
| CN3-1 | CN3-3 |
| CN2-1 | CN2-11 |

(Estas conexiones son obligatorias y sirven para dar corriente al chip)

| RPI | FT2232-H |
| :---: | :---: |
| TX (8) | RX (CN3-25) |
| RX (10)| TX (CN3-26) |
| GND (6) | GND |

Para conectar el adaptador al ordenador necesitaremos un cable USB-microUSB, una vez conectado al para poder acceder al adaptador podemos usar distintos emuladores de terminales (minicom, screen). Nosotros vamos a usar screen ya que es más ligero y sencillo de usar que minicom. Lo primero que debemos hacer es detectar que puerto ha asignado el kernel al adaptador, para ello:

```bash
dmesg | tail -10
```

Deberemos ver un mensaje como el siguiente:


En este caso podemos ver que se le ha asignado el puerto:. Teniendo esto en cuenta para conectarnos a el realizamos:

```bash
screen <puerto> 115200
```

Con el adaptador establecido pasamso a realizar la primera prueba de un ejecutable de RTEMS, para ello una vez tenemos el fichero compilado, lo cargamos en la micro-SD junto con los ficheros de firmware usados en la fase de arranque. Estos ficheros se pueden obtener del [repositorio de la Raspberry Pi](https://github.com/raspberrypi/firmware/tree/master/boot). Realmente en este directorio están los ficheros de firmware para todas las placas diferentes que hay, los que nos interesan son:
* bootcode.bin
* start.elf
* fixup.dat
* bcm2709-rpi-2-b.dtb (Para rpi2B v1.1)
* bcm2710-rpi-2-b.dtb (Para rpi2B v1.2)

(Los ficheros describen el hardware de la placa, sustituye al protocolo PCI (Peripheral Component Interconnect) de los ordenadores normales)

Con esta configuración el progrmaa todavía no se cargará correctamente. Necesitamos añædir un fichero adicional que servirá para configurar la fase de arranque de la RPI, se trata del "config.txt". Este es un fichero de texto con formato 'clave=valor' leido por el firmware en la fase de arranque que configura determinado parámetros (periféticos a usar, velocidad de reloj...).En este fichero debemos añædir los siguientes parámteros.

```
kernel=<nombre fichero ejecutable>
kernel_address=0x200000
```

Esto simplemente especifica el fichero que debe buscar para cargalo en memoria cuando termine la fase de arranque y la dirección donde cargarlo (se trata del el punto de entrada del ejecutable de RTEMS).

Si se han efectuado todos los pasos correctamente deberíamos ver las impresiones de nuestro programa. 

**PROBELMAS TÍPICOS**
* Error al poner la dirección kernel_address
* No configurar el ejecutable de RTEMS para que cargue el driver para la consola. #define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
* Conectar TX <-> TX y RX <-> RX.

## Carga Remota - U_Boot

En el caso de la RPI, si nos ceñimos al proceso estandar, probar un ejecutable implica:
1. Compilar el fichero
2. Conectar la micro-SD al ordenador
3. Cargar el ejecutable
4. Volver a conectar la micro-SD a la RPI y enchufarla a la corriente

(En la RPI la micro-SD funciona como disco duro y por ello en al encenderla busca ahi todos los ficheros de arranque y el kernel)

Como veis se trata de un proceso bastante lento e incómodo que puede incluso llegar a dañar el slot de la micro-SD. Para ello se suelen utilizan lo que se conoce como bootloaders que no son más que programas que se encargan de la puesta en marcha del sistema. En el proceso de puesta en marcha tenemos distintas fases diferenciadas en función del papel que juega en el procoso. En nuestro caso vamos a usar U-boot que es un bootloader stage-3. Esto quiere decir que su función es configurar el sistema antes de pasar la ejecución al programa del ususario, toda la inicialización del hardware ya se ha completado en gran medida en fases anteriores. Lo que nos interesa es que nos permite cargar los ficheros via serie o por red, evitando asi el proceso de carga por la micro-SD, todas estas acciones las ejectuas escribiendo comandos desde una shell que te presenta nada más inicializar. A continuación vamso a explicar primera mente como compilar u-boot para la RPI y posteriormente pasaremos a explicar como configurar cada uno de los métodos de carga remota.


### Preparación Ejecutable
 

U-boot necesita unas cabeceras especiales para poder interpretar el fichero ejecutable cargado en memoria, esto implica que antes de enviar el ejeutable este tiene que estar listo en el formato correspondiente. Estos son los comandos necesarios para preparar un ejecutable para U-boot.


1. Transformar el ejecutable a formato binario. El formato que obtenemos tras la compilación es ELF (Executable Linkable File), se trata de un formato con una serie de metadatos pensados para ser interpretados por el cargador de programas del sistema operativo. Dado que no 

```bash
<path_toolchain>/arm-rtems5-objcopy <Nombre_Ejecutable>.exe -O binary <Nombre_Ejecutable>.bin
```

2. Realizamos una compresión de la imágen, esto va a ser extremadamente útil para poder transmitir el archivo de forma más rápida por cualquiera de los métodos que vamos a contar a continuación

```bash
gzip -9 -f -c <Nombre_Ejecutable>.bin > <Nombre_Ejecutable>.bin.gz
```

3. Añadimos unas cabeceras específicas para la lectura por u-boot. Estas contienen información sobre el punto de entrada (en este caso especificamos 0x200000) y el tipo de compresión realizada al archivo. Esto permite a u-boot saber donde cargar el ejecutable y cómo descomprimirlo antes de pasar a ejecutarlo.

```bash
mkimage -A arm -O linux -T kernel -a 0x200000 -e 0x200000 -n RTEMS -d <Nombre_Ejecutable>.bin.gz kernel.img
```


### Instalación

``` bash
git clone ​git://git.denx.de/u-boot.git
cd u-boot
export CROSS_COMPILE=<ruta toolchain RTEMS>
make rpi_2_defconfig
make
```

Tras este proceso se creara un fichero "U-boot.bin" que será el que debemos colocar en la micro-SD junto con el resto de ficheros de firmware usados en la fase de arranque. En este caso en el fichero config.txt debemos eliminar la entrada "kernel_address" ya que el ejecutable de u-boot se compila automaticamente para la dirección estándar de carga de ejecutable (0x8000).

De nuevo realizamos la conexión anterior para visualizar la consola y conectamos la RPI a la corriente. Deberemos ver una impresión como la siguiente.

### Carga Serial

U-boot te permite cargar los ficheros a través del puerto UART que contiene la RPI, el mismo que se usa para poder visualizar la consola, es por esto que las conexiones y hardware a usar para este método son los mismos que especficiamos en el apartado "Visualización de la Consola". En cuanto al protocolo de transferencia de archivos u-boot nos ofrece distintas opciones: Kermit, X/Z/Y/Modem, Ascii... Todos son muy similares en el rendimiento. A continueación vamos a explicar como configurar todo para la carga mediante Kermit.


En el lado de u-boot debemos escribir los siguientes commandos:

1. Leemos de la micro-SD el fichero .dtb y lo cargamos en la dirección $fdt_addr_r (se trata de una variable de entorno). Es necesario cargar este fichero para que posteriormente se pueda poner en marcha el ejecutable de forma correcta (el código de inicialización de RTEMS lo requiere).

```bash
fatload mmc 0 $fdt_addr_r bcm2710-rpi-2-b.dtb
```

2.  Abrimos el puerto UART inicializamos la comunicación kermit y esperamos que nos llegue el fihero. U-boot se queda bloqueado esperando que le llegue el fichero. Una vez tenga el fichero lo cargará en la dirección de memoria especificada por la variable de entorno. Esta dirección no es la dirección donde ejecutará el programa, cuando posteriormente ejecutemos el progrma, u-boot leera las cabeceras y copiará el ejecutable en la dirección de memoria especificada en estas cabeceras (0x200000).

```bash
loadb $kernel_addr_r
```

3. Esperamos un segundo ya que en las instrucciones 1. y 2. realizadas en el ordenador hay un pequeño delay producido por el cierre y la apertura del puerto serial.

```bash
sleep 1
```

4. Ejecutamos el programa indicando la dirección en memoria del ejecutable y el .dtb. El segundo parámetro sirve para especificar el fichero initrd. Este es un fichero necesario para arrancar determinados sistemas operativos, se trara de un pequeño sistema de ficheros que se carga en mmeoria durante la fase de arranque. En nuestro caso dado que no es necesario lo marcamos con "-". 

```bash
bootm $kernel_addr_r - $fdt_addr_r
```

En el lado del ordenador:

1. Enviamos el fichero ejecutable (preparado para U-boot). Es importante que este comando se ejecute una vez u-boot se encuentre en el paso 2, de lo contrario la comunicación no se llevará a cabo.

```bash
kermit -i -l /dev/ttyUSB0 -b 115200 -s <fichero ejecutable>
```

2. Abrimos el puerto mediante "screen" para poder ver las impresiones del programa.

```bash
screen <puerto> 115200
```

(Para que le proceso sea lo más rápido posible es recomentable juntar los dos comandos anteriores mediante ";" -> kermit ... ; screen ...)


### Carga por Red

Para poder realizar este método es neceasrio un cable ethernet para conectar a la RPI a la red. Lo primero que debemo hacer es configurar el servidor DHCP en el host.

1. Instalamos los paquetes necesarios

```bash
sudo apt-get install tftp-hpa tftpd-hpa
```	

2. Editamos el fichero /etc/default/tftpd-hpa, modificamos el valor de la variable TFTP_DIRECTORY

```bash
TFTP_DIRECTORY="/svr/tftp"
```

3. Creamos el directorio asignado anteriormente y le asignamos los permisos de nuestro ususario para que no haya problemas al acceder a el

```bash
mkdir /svr/tftp
sudo chown -R $(whoami) /srv/tftp
```

4. Reiniciamos el servicio TFTP

 ```bash
sudo service tftpd-hpa restart
```

Con esto ya tenemos preparado el servidor TFTP, todos los ficheros queramos que sean visibles a la comunicación deberán estar presentes en el directorio /svr/tftp. 

Para obtener los ficheros desde u-boot debemos realizar

1. Especificar en la variable de entorno "serverip" la ip de la máquina donde ejecuta el servidor TFTP.

```bash
setenv serverip 192.168.1.216
```

2. Al igual que antes cargamos el .dtb

```bash
fatload mmc 0 $fdt_addr_r bcm2710-rpi-2-b.dtb
```

3. Este comando realiza dos acciones. Inicialmente pide una ip al servidor dhcp de la red. Una vez se le ha asignado una pide el fichero especificado al servidor TFTP (identificado por la IP del paso 1.) y carga el ejecutable en la dirección correspondiente. 

```bash
dhcp $kernel_addr_r kernel.img
```	

En caso de no tener servidor dhcp en la red local se deberá realizar:

```bash
setenv ipaddr <ip dentro de la red>
tftp $kernel_addr_r kernel.img
```

4. Ejecutamos

```bash
bootm $kernel_addr_r - $fdt_addr_r
```

Este método presenta dos ventajas con el "Serial":
* Te permite tener la RPI separada de la máquina de desarrollo
* Te permite abrir el puerto serie para ver la consola desde el primer momento.

### Automatización U-boot

Como se puede ver ambos métodos anteriores suponen introducir en u-boot 4 comandos diferentes cosa que hace el tiempo de despliegue bastante largo. Para solucionarlo, u-boot te ofrece la posibilidad de crear un script el cual se ejecutará automaticamente una vez inicie u-boot. Este script deberá llamarse "boot.scr". Para generarlo simplemente:

```bash
nano boot.cmd
//Añædir los comandos vistos anteriormente según el método de carga que se desee
mkimage -C none -A arm -T script -d boot.cmd boot.scr
```

Una vez se tiene el fichero "boot.scr" este simplemente se carga en la micro-SD junto con el resto de archivos. Al arrancar deberíamos ver como u-boot no presenta la shell sino que ejecuta directamente el contenido de este script.

Realmente si nos fijamos no se ha eliminado los 2 segundos de delay que u-boot te deja para cancelar el auto-boot. Si queremos quitarnos estos 2 segundos debemos entrar en el modo Shell y ecribir lo siguiente:

```bash
setenv bootdelay=0
saveenv
```
	
Esto modifica el valor de la varaible de entorno que establece el tiempo de delay y hará el cambio persistente. Si inspeccionamos la micro-SD tras realizar una ejecución con estos parámetros veremos que se ha generado un fichero uEnv.txt con todas las variables de entorno, todas ellas con el valor por defecto excepto la que acabamos de cambiar. En caso de que queramos volver a usar la shell debemos buscar esta variable en el fichero y establecerle un valor distinto de 0.


### Eclipse

Todo el desarrollo que hemos explicado (Preparación del ejecutable, carga remota) se ha integrado dentro de Eclipse para que el desarrollo sea más cómodo, a conitnuación explicamos las configuraciones necesarias a realizar para la integración:

Lo primero que debemos hacer es modifiar el proceso de compilación para que incluya los pasos definidos en "Preparación Ejecutable", para ello:

Click Derecho <Proyecto> -> Propiedades -> C/C++ build -> Settings -> Build Steps -> Post Build Steps
	
En ese cuadro añædimos todos los comandos especificados en "Preparación Ejecutable" separados por ";" y además añædimos el siguiente:

```bash
mv kernel.img /svr/tftp; rm -f ${ProjName}.*
```

Que dejará el ejecutable preparado en el directorio del servidor TFTP listo para ser solicitado.Con esto ya podríamos desde Eclipse realizar el despliegue de un ejecutable por TFTP.

Para poder además realizar la carga serial, debemos realizar un paso más. Para ello hacemos uso de las "External Tools", por defecto instaladas en Eclipse. Esta funcionalidad de permite ejecutar programas que existen fuera del workspace de Eclipse. De esta manera lo que vamos a realizar es añædir un External Tool que ejecute los dos comandos explicados en "Carga Serial". Para ello:

Down Arrow Simbolo External Tool (Al lado del de "Run")  -> External Tool Configuration -> New External Tool (Simbolo hoja)
 
Nos aparece un cuadro con distintas pestañas, esto es lo que debemos realizar en cada una:

* Name: El que quieras
* Main: Rellenamos los valores:
  * Location: /usr/bin/gnome-terminal
  * Arguments: - bash -c "kermit -i -l /dev/ttyUSB0 -b 115200 -s /svr/tftp/kernel.img ; screen /dev/ttyUSB0 115200"
* Refresh:
  * Indicarle que solo refresque el proyecto para el proyecto para el que vais a usar esta herramienta
* Build:
  * Indicarle que solo compile el proyecto para el proyecto para el que vais a usar esta herramienta
* Environment: Nada
* Common: Nada

Como podemos ver en vez de ejecutar los comandos que hemos dicho estamos primeramente ejecutando una terminal que posteriormente ejecuta essos comandos. Es necesario realizarlo de esta manera por dos razones:
* "Kermit" no dirige a la salida estandar sus mensajes de impresión por lo que en la consla de Eclipse no podremos ver nada.
* El comando "Screen" necesita ejecutar sobre una terminar de lo contrario da error.

Lo bueno de esta metodoloǵia es que la terminal que se crea, una vez salimos de screen se cierra automáticamente.

De esta manera el funcionamiento es el siguiente:

* Carga por Red: Compilamos el fichero, abrimos una terminal desde Eclipse y encendemos la RPI.
* Carga serial: Encendemos la RPI y ejecutamos con la configuración de External Tool Creada

## Simulación - QEMU

La carga remota sin duda nos hace el proceso de pruebas sobre el hardware mucho más llevadero, pero sin duda poder probar los ejecutables sin necesidad de realizar las conexiones anteriores y con un tiempos de despliegue inmediatos es una herramienta adicional que nos puede servir sobre todo en la primera fase de pruebas. Es por ello que el uso del simulador es esencial.

### Instalación

Por un lado necesitamos instalar el simulados; QEMU. Para ello podemos descargarnos el código fuente de github y compilarlo o podemos obtener directamente los binarios de apt. Dado que los binarios parecen estar up-to-date tomamos esa opción. QEMU es un software que permite emular distintas arquitecturas, dado que la RPI se trata de una arquitectura ARM, instalamos únicamente el binario para esa arquitectura.

```bash
sudo apt-get install qemu-system-arm
```

Por otro lado para poder integrar el uso del simulador dentro de Eclipse vamos a necesitar hacer uso del paquete GNU/MCU Eclipse. Este provee facilidades para el desarrollo en sistemas empotrados. 

Help -> Eclipse MarketPlace -> Search "GNU/MCU Eclipse" -> Install

### Funcionamiento

QEMU te ofrece la posibilidad de simular los programas, pero además puedes depurarlos. En cuanto al formato de los programas, QEMU es capaz de interpretar el formato ELF por lo que no es necesario realizar ningúna transofrmación sobre el fichero que obtenemos tras las compilación.

Actualmente la simulación "normal" no puede usarse con los ejecutables de RTEMS. El comando a utilizar es:

```bash
qemu-system-arm -M raspi2 -m 1G -kernel <Ejecutable> -serial mon:stdio -nographic -dtb <fichero.dtb>
```

 * -M: Tipo de placa a simular
 * -m: Tamaño de la memoria RAM
 * -kernel: Fichero ejecutable que debe cargarse en memoria
 * -serial: Multiplexar la salida del programa y el monitor de qemu en la salida estándar. Se cambia entre ellos con Ctrl+a, c.
 * -dtb: Fichero .dtb

La razón de que no funcione se debe a que los ejecutables de RTEMS dado que incorporan un sistema operativo tienen que previamente inicializarlo. En el caso de la RPI este proceso de inciialización lee el fichero .dtb para poder realizar siguientes configuraciones. El problema es que la dirección donde QEMU situa el fichero .dtb no es la misma donde RTEMS espera tenerlo, es por ello que la fase de inicialización falla. Como veis en el comando especificamos el fichero pero no hay forma de indicarle en que dirección cargarlo.

Por suerte este problema se puede solventar mediante la simulación con depuración. El comando es pŕacticamente igual solo añade -s -S lo que le indica que tras inicializar el simulador debe parar la ejecución y esperar la conexión de un cliente GDB en el puerto 1234.

```bash
qemu-system-arm -M raspi2 -m 1G -kernel <Ejecutable> -serial mon:stdio -nographic -s -S
```

Una vez se ha lanzado QEMU podemos conectarnos con GDB de la siguiente forma:

```bash
<ruta>/arm-rtems5-gdb <Ejecutable>
gdb>tar remote:1234
gdb>load
gdb>restore <ruta>/fichero.dtb binary 0x2ef00000
gdb>set $r2=0x2ef00000
gdb>set scheduler-locking on
gdb>continue
```
	
Como veis lo que estamos haciendo es colocar manualmente el fichero .dtb en memoria. Para no extendernos demasiado podeis leer [aqui] (https://sourceware.org/gdb/current/onlinedocs/gdb/All_002dStop-Mode.html) para saber más sobre la opción "set scheduler-locking on".

Todos estos comandos pueden incluirse en un fichero .gdb para que GDB lo ejecute automáticamente.

```bash
arm-rtems5-gdb <Ejecutable> -x <nombre>.gdb
```

Con esta configuración podemos depurar cualquier tipo de programa que no haga uso del timer. La razón se debe a que el timer que implemente la BSP es distinto que el timer que QEMU simula.

### Eclipse

El Plug-in GNU/MCU Eclipse te provee de una herramienta espcifica para depurar programas simulados mediante GDB, de manera que se lanzan tanto el simulador como el GDB automáticamente. Esta herramienta no funciona con la versión de QEMU oficial sino con un fork del proyecto que se hizo en 2015. El problema es que esta versión no soporta la simulación de la RPI por lo que no podemos usar esta herramienta del Plug-in.

La herramienta del Plug-In que si que podemos aprovechar es la que te permite configurar el GDB de forma aislada. De esta manera lo que hacemos es lanzar el simulador mediante un "External Tool" y el GDB mediente el Plug-In. A continuación mostramos como realizar la configuración de cada uno de ellos.

**QEMU** (External Tool): Down Arrow Simbolo External Tool (Al lado del de "Run")  -> External Tool Configuration -> New External Tool (Simbolo hoja)

* Name: El que quieras
* Main: Rellenamos los valores:
  * Location: /usr/bin/qemu-system-arm
  * Arguments: -M raspi2 -m 1G -kernel <Ejecutable> -serial mon:stdio -nographic -s -S
* Refresh:
  * Indicarle que solo refresque el proyecto para el proyecto para el que vais a usar esta herramienta
* Build:
  * Indicarle que solo compile el proyecto para el proyecto para el que vais a usar esta herramienta
* Environment: Nada
* Common: Nada

**GDB** (GNU/MCU Plug-in): Down Arrow Launch Configuration -> New Launch Configuration -> GDB Hardware Debugging

* Name: El que quieras
* Main: Debería autorrellenarse en caso contrario refrescar el proyecto y volver repetir desde el principio
* Debugger:
  * GDB Command: Buscamos el GDB de la Toolchain instalada de RTEMS
  * JTAG Device = Generic TCP/IP
  * Port = 1234
* StartUp
  * Unclick "Reset And Delay"
  * Initialización Commands [Cuadro] añædir
```
restore /home/mario/Downloads/bcm2709-rpi-2-b.dtb binary 0x2ef00000
set $r2 = 0x2ef00000
set scheduler-locking on
```
* Source: Nada
* Common: Nada

Hasta aqui hemos creado dos configuraciones de ejecución independientes, por lo que tendremos que manualmente lanzar una y después la otra. Para que ambas se lancen automáticamente debemos crear un "Launch Group" (instalado por defecto). Esta herramienta nos permite crear configuraciones donde especifiquemos el orden de ejecución de distintos programas. Para configurarla:

Down Arrow "Run" -> Run Configuracions -> Launch Group -> New Launch Group. Ponerle el nombre que quieras
&emsp;Add -> Program -> "Nombre que le hayas dado" -> Post Launch Action = Delay = 2 -> OK
&emsp;Add -> GDB Hardware Debugging -> "Nombre que le hayas dado" -> Launch Mode = Debug -> OK

Una vez tengamos esto ya podemos ejecutar especificancho esta opción como tipo de ejecución y se lanza una consola para QEMU y se cambia la perspectiva de Eclipse para Debueo. Deberemos ver como abre el fichero Start.s y nos muestra la linea donde QEMU ha parado su ejecución esperando al cliente GDB. Le damos a continue y si hemos establecido BP veremos como para en ellos.


## Depuración remota - OpenOCD

Si bien el uso del simulador es muy util para hacer las primeras pruebas de un progrma. Normlamente los simuladores se encuentran limitados en la cantidad de periféricos que pueden simular, limitando asi el tipo de programas a desarrollar sobre ellos. Es por ello que poder depurar un programa sobre la placa física es bastante importante.

Para ello muchas placas incorporan actualmente un puerto JTAG. Se trata de un puerto que en orgien se desarrollo para agilizar el proceso de comporbación de la circuitería de una placa tras su fabriación. Posteriormente se ha usado ampliamente para depurar programas. Para poder hacer uso de este puerto son necesarios dos elmentos:
* Adaptador: Que como ya comentamos en "Visualización de la Consola", el chip FT2232H Mini-Module que usamos para la consola lo podemos usar simultáneamente para realizar depuración.
* Software: Para poder manejar la comunicación vía JTAG necesitamos un software que la implemente. Vamos a utilizar OpenOCD principalmente porque es gratis y tiene soporte para distintas placas y adaptadores.

### Conexión

La RPI tiene una cabecera con 40 pines los cuales se pueden configurar de distintas formas. Las más comunes son "entrada" o "salida", pero pueden tener distintas configuraciones que se denotan con ALTX (Alternative X). La funcionalidad JTAG se activa en distintos pines según la función alternativa que usemos, :

| Señal | ATL4 | ALT5 |
| :---: | :---: | :---: |
| TRST  | 22 (11) |  -   |
| RTCK  | 23 (16) | 06 (31) |
| TDO   | 24 (18) | 05 (29) |
| TCK   | 25 (22) | 13 (33) |
| TDI   | 26 (37) | 04 (07) |
| TMS   | 27 (13) | 12 (32) |

(En paréntesis ponemos la numeración del pin en la cabecera, la otra numeración se corresponde con la usada en el datasheet)

La mejor opción que tenemos para activarlos es usar una opción del fichero "config.txt"

```
enable_jtag_gpio=1
```
	
Esto utiliza la configuración ALT4. De todas esas señæles la única que no usamos es RTCK ya que el adaptador que usamos no tiene la señal corresponidente para sopotar la funcionalidad asociada a ese pin (frecuentacia de reloj adaptativa, útil cuando el programa a debugear porne a la CPU en espera). La señal TRST tampoco la contiene explicitamente pero dado que su funcionalidad es simple, podremos configurar un pin normal para que haga ese papel. Teniendo esto en cuenta la conexión entre la RPI y el FT2232 es la siguiente:

| FT2232-H | FT2232-H |
| :---: | :---: |
| CN3-1 | CN3-3 |
| CN2-1 | CN2-11 |

(Estas conexiones son obligatorias y sirven para dar corriente al chip)

| Señæl | RPI | FT2232-H |
| :---: | :---: | :---: |
| TRST  | 11 | 14 |
| TDO   | 18 | 09 |
| TCK   | 22 | 07 |
| TDI   | 37 | 10 |
| TMS   | 13 | 12 |

### Instalación

Para poder instalar OpenOCD de forma correcta es necesario tener los siguientes paquetes instalados:

* make
* libtool
* pkg-config >= 0.23 (o compatible)
* autoconf >= 2.64
* automake >= 1.14
* texinfo

Para instalar 

```bash
git clone git://repo.or.cz/openocd.git
cd openocd.git
./bootstrap
cd .. && mkdir build && cd build
../openocd.git/configure --enable-ftdi  --prefix=<ruta donde realzar la instalación una vez compilado>
make
make install
```
	
Una vez finalizado en el directorio especificado en "prefix" debemos tener una carpeta /bin con el ejecutable "openocd".

### Preparación de Ejecutable

Para que los programas se puedan depurar de forma correcta es necesario que en el proceso de compilación se añæda información de depuración; -g, y que no haya ninguna opción de optimización activada, -O0. 

Desde Eclipse la optimización de los ejecutables está activada por defecto, para desactivarla.

Click Derecho en el Proyecto -> Properties -> C/C++ Build -> Settings 
&emsp;-> RTEMS C Compiler -> Optimization -> Optimization Level = -O0
&emsp;-> RTEMS C Assembler -> Optimization -> Optimization Level = -O0

Estos programas si se quieren depurar desde el principio, o si son especialmente cortos deberán contener un bucle sobre una varaible que nos permite "parar" la ejecución del programa:

```c
	int a = 0
	while(!a);
```
	
De lo contrario perderemos el control sobre el incio del programa o incluso si termina antes de realizr la conexión, esta no podrá llevarse a cabo ya que la RPI se estará reinciando.

### Funcionamiento

Para que openocd pueda establecer la conexión de forma correcta es necesario pasarle mediante argumentos los ficheros de configuración del adaptador y de la placa a conectarse. Si nos vamos al directorio <prefix>/share/openocd/scripts vemos distintas carpetas, estas son las rutas donde openocd buscará los ficheros de configuración pasados por argumento. Las carpetas que nos interesan son:

* Boards: Contiene ficheros de configuración para todas las placas que soporta
* Interfaces: Contiene ficheros de configuración para todos los adaptadores que soporta

Dentro de esos directorio no tenemos ficheros de configuración ni para la RPI ni para el FT2232-H es por ello que hay que descargarse los ficheros del siguiente [repositorio](https://github.com/MarioPalomaresGallego/Development_Scripts/tree/master/OpenOCD) y colocarlos en los directorios correspondientes. 

Con esto ya tenemos toda la configuración para poder realizar la primera prueba de depuración, los pasos a seguir son:
* Realizar las conexiones de JTAG
* Realizar las conexiones de la consola -> Opcional
* Encender la RPI y esperar a que le programa (configurado según hemos comentadocole) se haya cargado

./openocd -f interfaces/ft2232.cfg -f board/rpi2.cfg

Si todo va bien debemos ver una impresión como la siguiente:

	Open On-Chip Debugger 0.10.0+dev-00740-g380502d8 (2019-03-14-11:29)
	Licensed under GNU GPL v2
	For bug reports, read
		http://openocd.org/doc/doxygen/bugs.html
	adapter speed: 10 kHz
	Info : JTAG tap: rpi2.tap tap/device found: 0x4ba00477 (mfg: 0x23b (ARM Ltd.), part: 0xba00, ver: 0x4)
	Info : rpi2.a53.0: hardware has 6 breakpoints, 4 watchpoints
	Info : rpi2.a53.1: hardware has 6 breakpoints, 4 watchpoints
	Info : rpi2.a53.2: hardware has 6 breakpoints, 4 watchpoints
	Info : rpi2.a53.3: hardware has 6 breakpoints, 4 watchpoints
	Info : Listening on port 3333 for gdb connections
	Info : Listening on port 3334 for gdb connections
	Info : Listening on port 3335 for gdb connections
	Info : Listening on port 3336 for gdb connections

En caso de error comprobar bien la conexión realizada y revsar el fichero config.txt para ver si se ha escrito correctamente la opción.

Lo que ha hecho openocd es:
* Detectar todas los ciruitos conectados, en nuestro caso solo una RPI -> JTAG tap: rpi3.tap...
* Analizar todas las CPU que contiene la RPI, en nuestro caso 4
* Levantar un servidor Telnet para controlar el programa y un servidor GDB por cada CPU para poder pepurarlas de forma aislada. En nuestro caso nuestro programa solo ejecuta en una CPU que por defecto es la primera rpi2.a53.0. Las conexiones Telnet únicamente las vamos a usar para cerrar el programa de forma correcta. 

Para depurar el programa debemos ejecutar:

```bash
<ruta>/arm-rtems5-gdb <Ejecutable>
gdb>tar remote:3333
```

Si todo va bien debemos ver en la consola de GDB un mensaje que nos imprime la dirección de memoria donde se ha parado la ejecución, y a su vez la línea de código con la que corresponde. Esta debería corresponderse con el bucle definido al principio del programa. 

A partir de aqui el funcioncionamiento es el que conocemos con GDB con la diferencia de que, por porblemas de invaliación de caché, solo podemos usar Hardware Breakpoints.

### Eclipse

La integración dentro de Eclipse se hace fácilmente con la herramienta que te ofrece el plug-in GNU/MCU; OpenOCD Debugging. A continuación mostramos como configurarla:

Down Arrow Launch Configuration -> New Launch Configuration -> OpenOCD Debugging

* Name: El que quieras
* Main: Debería autorrellenarse en caso contrario refrescar el proyecto y volver repetir desde el principio
* Debugger:
  * OpenOCD SetUp:
    * Executable Path = <Ruta al ejecutable openOCD>
    * Config Options = -f interfaces/ft2232.cfg -f board/rpi2.cfg
  * Start GDB session:
    * Executable Name = <Ruta al ejecutable del GDB de RTEMS> 
  * Remote Target:
     * Port = 3333
* StartUp: Unclick todo menos "Load Symbols" lo dejamos con la opción por defecto
* Source: Nada
* Common: Nada
* SVD: Nada
