/*
El propósito de esta práctica es desarrollar paso a paso un proyecto para controlar y programar un Brazo Robot, 
simulando las funciones básicas de un robot industrial.

Programar: Registrar las posiciones de los brazos en tres dimensiones (cada registro es un "paso", un programa consiste en una serie de pasos).
Ejecutar: Realice en secuencia las posiciones registradas en el "Programa". El robot ejecutará el programa hasta que se use el comando "ABORTAR".

El proyecto se usa para controlar robots con  4 DOF ("Grados de libertad").
El robot se debe controlar en modo "REMOTO" (a través de una programa en java por medio del puerto serial).
La información para el usuario se podrá proporcionar a través de LEDS de colores, una pantalla LCD de 2 líneas y/ó sonido (un zumbador).
Debe de contener un botón de paro de emergencia (Físico).
Si existe un fallo y/o corte de energía, después de restablecerse la corriente el robot debe de continuar el programa
(aunque este no se encuentre conectado a la aplicación).
Los brazos robóticos se pueden clasificar de acuerdo con el número de "articulaciones" o "Grados de libertad" (DOF) que tienen.
  -La "Base", o "Cintura", por lo general puede girar el brazo 180o o 360o, dependiendo del   tipo de Servo utilizado
   (aquí en este proyecto, se debe utilizar un motor a pasos para girar 360o)
  -El "Hombro" es el responsable de "levantar o bajar" el brazo verticalmente
  -El "codo" hará que el brazo "avance o retroceda".
  -La "Garra" o "Pinza" funciona abriendo o cerrándose para "agarrar cosas".
*/
//Llamamos a la libreria para controlar el LCD incluida en el IDE de Arduino
#include <LiquidCrystal.h>

//Definimos los pines asignados al LCD según el esquema de conexión
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//definicion de pins
const int motorPin1 = A0;    // 28BYJ48 In1
const int motorPin2 = A1;    // 28BYJ48 In2
const int motorPin3 = A2;   // 28BYJ48 In3
const int motorPin4 = A3;   // 28BYJ48 In4
const int ledVerde = 7;     // Led de indicador verde
const int ledRojo = 6;      //Led de indicador rojo
int val = 0;                //Variable para seguir con lo programado para recoger  la pieza
int var = 0;                //Variable para seguir con lo programado para depositar la pieza
int com = 0;                //Variable para poder comprobar que se cumplio con todos los pasos programados para recoger
int ex  = 0;                //Variable para poder comprobar que se cumplio con todos los pasos programados para depositar
const int abortar = 13;     //El pin que se va usar para poner el boton de abortar32
const int validar =  30;    //Validar para seguir con lo programa en el brazo robotico
int pulsador = 0;           //Variable para poder extraer el dato del pin 30

//definicion variables
int motorSpeed = 1200;   //variable para fijar la velocidad
int stepCounter = 0;     // contador para los pasos
int stepsPerRev = 4076;  // pasos para una vuelta completa

//tablas con la secuencia de encendido (descomentar la que necesiteis)
//secuencia 1-fase
//const int numSteps = 4;
//const int stepsLookup[4] = { B1000, B0100, B0010, B0001 };

//secuencia 2-fases
//const int numSteps = 4;
//const int stepsLookup[4] = { B1100, B0110, B0011, B1001 };

//secuencia media fase
const int numSteps = 8;
const int stepsLookup[8] = { B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001 };

//Variable que recibe el valor que manda java
int control = 0;

// Incluímos la librería para poder controlar el servo
#include <Servo.h>

// Declaramos la variable para controlar el servo
Servo servoMotorElevar; //Variable para el elevar o bajar el brazo

Servo servoMotorMuneca; //Variable para el elevar o bajar la muñeca

Servo servoMotorMano; //Variable para el agarrar o soltar la pieza de la mano

//Variable para desplegar de inicio en el lcd
String entrada = "Proyecto de sistemas programables";

void setup() {
  //declarar pines como salida
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  //declaramos los pines de los led
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRojo, OUTPUT);

  // Iniciamos el servo para que empiece a trabajar con el pin 9
  servoMotorElevar.attach(9); //Elevar el brazo robotico

  servoMotorMuneca.attach(8); //Muñeca del brazo robotico

  servoMotorMano.attach(10);  //Mano del brazo robotico

  //Indicamos el LCD que usamos: 16 caracteres y 2 lineas
  lcd.begin(16, 2);

  //Declaramos el pin que se va utilizar para validar la accion
  pinMode(validar, INPUT);

  //Declaramos el pin que se va utilizar para abortar la funcionalidad del programa
  pinMode(abortar, INPUT);

  //Declaramos la velocidad de comunicacion del serial
  Serial.begin(9600);
}

void loop() {
  pulsador = digitalRead(validar);  //Recogemos el valor que recibe de la entrada para comprobar si tiene corriente

  //Comparamos si existe corriente en el circuito
  if (pulsador == HIGH) {

    /*
     Comparamos que el boton de abortar si fue precionado o no.
     Sino esta precionado que siga con su funcionamiento normal.
     Pero si fue precionado que detenga todo.
    */
    if (digitalRead(abortar) == LOW) {
      //Leer el valor que recibe de java hacia el arduino
      control = Serial.read();

      // Movimiento del giro del brazo
      /*Comparamos el valor de la variable control para decidir
        que metodo va entrar. */
      switch (control) {
        case '1':
          lcd.clear();  //Limpia el contenido del lcd
          rotar(control); // Llamamos al metodo rotar y mandamos el valor del control para saber hacia donde se va mover

          //Aqui hacemos un for para poder hacer el recorrido del motor a pasos
          for (int i = 0; i < stepsPerRev; i++) {
            anticlockwise();  //Llamamos al metodo
            delayMicroseconds(motorSpeed);  //mandamos el tipo de controlador de pasos que vamos a usar
            int x = Serial.read();  //Recogemos el valor que recibe por el serial

            //Comparamos la variable
            if (x == '3') {
              lcd.clear();  //Limpiamos el lcd
              break;  //Detenemos el movimiento del motor a pasos
            }

            //Comparamos si fue precionado el boton de aborto
            if (digitalRead(abortar) == HIGH) {
              break;  //Detenemos el movimiento del motor a pasos
            }
          }
          lcd.clear();  //limpiamos el lcd
        break;

        case '2':
          lcd.clear();  //limpiamos el lcd
          rotar(control); // Llamamos al metodo rotar y mandamos el valor del control para saber hacia donde se va mover

          //Aqui hacemos un for para poder hacer el recorrido del motor a pasos
          for (int i = 0; i < stepsPerRev; i++) {
            clockwise();  //llamamos al metodo
            delayMicroseconds(motorSpeed);  //mandamos el tipo de controlador de pasos que vamos a usar
            int x = Serial.read();  //Recogemos el valor que recibe por el serial

            //Comparamos la variable
            if (x == '3') {
              lcd.clear();  //Limpiamos el lcd
              break;  //Detenemos el movimiento del motor a pasos
            }

            //Comparamos si fue precionado el boton de aborto
            if (digitalRead(abortar) == HIGH) {
              break;  //Detenemos el movimiento del motor a pasos
            }
          }
          lcd.clear();  //limpiamos el lcd
        break;

        case '4':
          lcd.clear(); //limpiamos el lcd
          elevar(control); //llamamos al metodo y mandamos el valor del control
          // Desplazamos a la posición 130º
          servoMotorElevar.write(130);
          // Esperamos 1 segundo
          delay(500);
          lcd.clear();  //limpiamos el lcd
        break;

        case '5':
          lcd.clear();  //limpiamos el lcd
          elevar(control); //llamamos al metodo y mandamos el valor del control
          // Desplazamos a la posición 95º
          servoMotorElevar.write(95);
          // Esperamos 1 segundo
          delay(500);
          lcd.clear(); //limpiamos el lcd
        break;

        case '6':
          lcd.clear();  //limpiamos el lcd
          muneca(control); //llamamos al metodo y mandamos el valor del control
          // Desplazamos a la posición 135º
          servoMotorMuneca.write(135);
          // Esperamos 1 segundo
          delay(500);
          lcd.clear();  //limpiamos el lcd
        break;

        case '7':
          lcd.clear();  //limpiamos el lcd
          muneca(control); //llamamos al metodo y mandamos el valor del control
          // Desplazamos a la posición 100º
          servoMotorMuneca.write(100);
          // Esperamos 1 segund
          delay(500);
          lcd.clear();  //limpiamos el lcd
        break;

        case '8':
          lcd.clear();  //limpiamos el lcd
          mano(control); //llamamos al metodo y mandamos el valor del control
          // Desplazamos a la posición 92º
          servoMotorMano.write(92);
          // Esperamos 1 segund
          delay(500);
          lcd.clear();  //limpiamos el lcd
        break;

        case '9':
          lcd.clear();  //Limpiamos el lcd
          mano(control); //llamamos al metodo y mandamos el valor del control
          // Desplazamos a la posición 72º
          servoMotorMano.write(75);
          // Esperamos 1 segund
          delay(500);
          lcd.clear();  //limpiamos el lcd
        break;

        case '0':
          recoger(); //llamamos al metodo
          lcd.clear();  //limpiamos el lcd
          val = 1;  //le damos el valor de 1 a la variable
        break;

        case 65:
          depositar(); //llamamos al metodo
          lcd.clear();  //limpiamos el lcd
          var = 2;  //le damos el valor de 2 a la variable
        break;
        default:

        break;
      }

      //Comparamos si fue completado todo los pasos programados en el brazo al recoger la pieza
      if (com == 1) {

      } else {
        //Comparamos si tiene corriente y que boton fue precionado para llamar a su metodo
        if (digitalRead(validar) == HIGH && val == 1) {
          recoger();  //llamamos al metodo
          lcd.clear();  //limpiamos el lcd
          val = 0;  //damos valor de 0 a la variable
        }
      }

      //Comparamos si fue completado todo los pasos programados en el brazo al depositar la pieza
      if (ex == 1) {

      } else {
        //Comparamos si tiene corriente y que boton fue precionado para llamar a su metodo
        if (digitalRead(validar) == HIGH && var == 2) {
          depositar();  //llamamos al metodo
          lcd.clear();  //limpiamos el lcd
          var = 0;  //damos valor de 0 a la variable
        }
      }

      //Iniciamos los led cual va estar encendido y apagado
      digitalWrite(ledVerde, HIGH);
      digitalWrite(ledRojo, LOW);
      delay(100);
      digitalWrite(ledVerde, LOW);
      delay(100);

      //Indicamos donde se va posicionar el cursor en el lcd y despues desplegar el mensaje
      lcd.setCursor(0, 0);
      lcd.print("Proyecto:");
      lcd.setCursor(0, 1);
      lcd.print("Brazo Robotico");

    } else {
      //Si se precionada el boton de abortar el led rojo se enciende
      digitalWrite(ledRojo, HIGH);
    }
  } else {
    //Sino tiene corriente el led rojo se enciende
    digitalWrite(ledRojo, HIGH);
  }
  delay(100);
}

/*Metodos para contorolar el brazo robotico*/

//Metodo de despligue de mensaje en el LCD y endicador de led
void rotar(int x) {
  //Iniciamos los led cual va estar encendido y apagado
  digitalWrite(ledVerde, LOW); //led verde se apaga
  digitalWrite(ledRojo, HIGH); //led rojo se enciende

  //comparamos con el valor que recibimos del metodo para saber hacia donde gira
  if (x == '2') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Girando Izq.");
  }

  //comparamos con el valor que recibimos del metodo para saber hacia donde gira
  if (x == '1') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Girando Der.");
  }

}

void elevar(int x) {
  //Iniciamos los led cual va estar encendido y apagado
  digitalWrite(ledVerde, LOW); //led verde apagado
  digitalWrite(ledRojo, HIGH);  //led rojo encendido

  //comparamos con el valor que recibimos del metodo para saber que va hacer
  if (x == '4') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Bajando brazo");
  }

  //comparamos con el valor que recibimos del metodo para saber que va hacer
  if (x == '5') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Subiendo brazo");
  }
}

void muneca(int x) {
  //Iniciamos los led cual va estar encendido y apagado
  digitalWrite(ledVerde, LOW); //led verde apagado
  digitalWrite(ledRojo, HIGH);  //led rojo encendido

  //comparamos con el valor que recibimos del metodo para saber que va hacer
  if (x == '6') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Bajando Muneca");
  }

  //comparamos con el valor que recibimos del metodo para saber que va hacer
  if (x == '7') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Subiendo Muneca");
  }
}

void mano(int x) {
  //Iniciamos los led cual va estar encendido y apagado
  digitalWrite(ledVerde, LOW); //led verde apagado
  digitalWrite(ledRojo, HIGH);  //led rojo encendido

  //comparamos con el valor que recibimos del metodo para saber que va hacer
  if (x == '8') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Agarrando");
  }

  //comparamos con el valor que recibimos del metodo para saber que va hacer
  if (x == '9') {
    //Se imprime el mensaje en el lcd
    lcd.setCursor(0, 0);
    lcd.print("Realizando:");
    lcd.setCursor(0, 1);
    lcd.print("Soltando");
  }
}

//Movimiento automatizado para recoger la pieza
int cont = 1; //iniciamos el contador de los movimientos
void recoger() {
  digitalWrite(ledVerde, LOW); //led verde apagado
  digitalWrite(ledRojo, HIGH);  //led rojo encendido

  //comparamos en que movimiento esta
  if (cont == 1) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();        
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Soltando");
        // Desplazamos a la posición 72º
        servoMotorMano.write(75);
        // Esperamos 1 segund
        delay(500);
        cont += 1;  //incrementamos el contador
        com = 0;  //damos valor de 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (cont == 2) {

    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();       
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Bajando brazo");
        // Desplazamos a la posición 150º
        servoMotorElevar.write(130);
        // Esperamos 1 segundo
        delay(500);
        lcd.clear();
        cont += 1;  //incrementamos el contador
        com = 0;  //damos valor de 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (cont == 3) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Bajando Muneca");
        // Desplazamos a la posición 135º
        servoMotorMuneca.write(135);
        // Esperamos 1 segundo
        delay(500);
        lcd.clear();
        cont += 1;  //incrementamos el contador
        com = 0;  //damos valor de 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (cont == 4) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();        
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Agarrando");
        // Desplazamos a la posición 92º
        servoMotorMano.write(92);
        // Esperamos 1 segund
        delay(500);
        cont += 1;  //incrementamos el contador
        com = 0;  //damos valor de 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (cont == 5) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Subiendo Muneca");
        // Desplazamos a la posición 100º
        servoMotorMuneca.write(100);
        // Esperamos 1 segund
        delay(500);
        cont += 1;  //incrementamos el contador
        com = 0;  //damos valor de 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (cont == 6) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();        
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Subiendo brazo");
        // Desplazamos a la posición 95º
        servoMotorElevar.write(95);
        // Esperamos 1 segundo
        delay(500);
        cont += 1;  //incrementamos el contador
        com = 0;  //damos valor de 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (cont == 7) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        cont += 1;  //damos valor de 1 a la variable
        com = 0;  //damos valor de 0 a la variable
      }
    }
  }

  delay(100);
}

//Movimiento automatizado para depositar la pieza
int con = 1; //iniciamos el contador de movimientos
void depositar() {
  
  digitalWrite(ledVerde, LOW); //led verde apagado
  digitalWrite(ledRojo, HIGH);  //led rojo encendido

  //comparamos en que movimiento esta
  if (con == 1) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Bajando brazo");
        // Desplazamos a la posición 150º
        servoMotorElevar.write(130);
        // Esperamos 1 segundo
        delay(500);
        lcd.clear();
        con += 1; //incrementamos el contador
        ex = 0; //le damos el valor 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (con == 2) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Bajando Muneca");
        // Desplazamos a la posición 135º
        servoMotorMuneca.write(135);
        // Esperamos 1 segundo
        delay(500);
        lcd.clear();
        con += 1; //incrementamos el contador
        ex = 0; //le damos el valor 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (con == 3) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Soltando");
        // Desplazamos a la posición 72º
        servoMotorMano.write(75);
        // Esperamos 1 segund
        delay(500);
        con += 1; //incrementamos el contador
        ex = 0; //le damos el valor 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (con == 4) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Subiendo Muneca");
        // Desplazamos a la posición 100º
        servoMotorMuneca.write(100);
        // Esperamos 1 segund
        delay(500);
        con += 1; //incrementamos el contador
        ex = 0; //le damos el valor 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (con == 5) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Agarrando");
        // Desplazamos a la posición 92º
        servoMotorMano.write(92);
        // Esperamos 1 segund
        delay(500);
        con += 1; //incrementamos el contador
        ex = 0; //le damos el valor 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (con == 6) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Realizando:");
        lcd.setCursor(0, 1);
        lcd.print("Subiendo brazo");
        // Desplazamos a la posición 95º
        servoMotorElevar.write(95);
        // Esperamos 1 segundo
        delay(500);
        con += 1; //incrementamos el contador
        ex = 0; //le damos el valor 0 a la variable
      }
    }
  }

  //comparamos en que movimiento esta
  if (con == 7) {
    
    //comparamos que tenga corriente
    if (digitalRead(validar) == HIGH) {
      //comparamos que no este precionado el boton de abortar
      if (digitalRead(abortar) == LOW) {
        con = 1;  //le damos el valor 1 al contador
        ex = 1; //le damos el valor de 1 a la variable
      }
    }
  }

  delay(100);
}


//Movimiento del motor a pasos hacia la derecha
void clockwise() {
  stepCounter++;
  if (stepCounter >= numSteps) stepCounter = 0;
  setOutput(stepCounter);
}

//Movimiento del motor a pasos hacia la izquuerda
void anticlockwise() {
  stepCounter--;
  if (stepCounter < 0) stepCounter = numSteps - 1;
  setOutput(stepCounter);
}

//Escribimos la salida que va estar recibiendo el motor a pasos
void setOutput(int step) {
  digitalWrite(motorPin1, bitRead(stepsLookup[step], 0));
  digitalWrite(motorPin2, bitRead(stepsLookup[step], 1));
  digitalWrite(motorPin3, bitRead(stepsLookup[step], 2));
  digitalWrite(motorPin4, bitRead(stepsLookup[step], 3));
}
