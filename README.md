# Proyecto Arduino con LCD y Java.
>Creado por: Diego Salvador Suárez Quijas
              Angely Jazmin Martinez Ramirez
>Contacto: d.salvador0604@gmail.com

## Arduino con un LCD desplegar.
- Mensaje movimiento del Brazo (acción).
- Led de encendido o apagado
- Botones de Paro

Programas:
- Java
- Arduino IDE
- Software Libre

Librerias:
- RXTXcomm
- GiovynetDriver
- jSerialComm-1.3.11
- jgsl-0.1.0-javadoc

**Materiales:**
- 2 Arduinos Uno
- Protoboard.
- Driver Uln2003 .
- Motores paso a paso: 28BYJ-48.
- ServoMotor: TowerPro SG90 9G Mini.
- Potenciometro (10k).
- Pantalla LCD.
- Cables.
- Leds.
- Pulsadore (botones).
- resistencias 200 homs
- Base para poner los servos.

## Características principales: 
 - El proyecto se usa para controlar robots con  4 DOF ("Grados de libertad").
 - El robot se debe controlar en modo "REMOTO" (a través de una programa en java por medio del puerto serial).
 - La información para el usuario se podrá proporcionar a través de LEDS de colores, una pantalla LCD de 2 líneas y/ó sonido (un zumbador).
 - Debe de contener un botón de paro de emergencia (Físico).     
 - Si existe un fallo y/o corte de energía, después de restablecerse la corriente el robot debe de continuar el programa (aunque este no se encuentre conectado a la aplicación).
 - Los brazos robóticos se pueden clasificar de acuerdo con el número de "articulaciones" o "Grados de libertad" (DOF) que tienen.             
     - La "Base", o "Cintura", por lo general puede girar el brazo 180o o 360o, dependiendo del   tipo de Servo utilizado (aquí en este proyecto, se debe utilizar un motor a pasos para girar 360o)
     - El "Hombro" es el responsable de "levantar o bajar" el brazo verticalmente 
     - El "codo" hará que el brazo "avance o retroceda".
     - La "Garra" o "Pinza" funciona abriendo o cerrándose para "agarrar cosas". 

Librerias: para la comunicacion Arduino con Java

    >RXTXcomm
    >PanamaHitek_Arduino-3.0.0


