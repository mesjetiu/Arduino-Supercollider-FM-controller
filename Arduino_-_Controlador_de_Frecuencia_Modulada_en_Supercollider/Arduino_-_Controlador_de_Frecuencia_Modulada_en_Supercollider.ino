/*
  Controlador de Frecuencia Modulada en SuperCollider.

  Este proyecto consta de dos archivos, uno de Arduino y otro de Supercollider.
  Componentes hardware:
    * ELEGOO Mega 2560 R3 (compatible con Arduino Mega 2660)
    * Infrared Receiver module AX-1838HS
    * Arduino IR Remote Control
    * LCD display LCM1602A
    * Resistencia 220 ohm
    * Pontenciómetro 10 Kohm

  Descripción:
  Permite controlar a voluntad con una perilla y un mando de infrarrojos
  desde Arduino tres parámetros de un sonido de Frecuencia Modulada:
  Frecuencia del la señal portadora, Armonicidad e Índice de modulación.
  Para ello nos servimos de dos módulos de entrada: IR Remote y Rotatory Encoder.
  Con el mando de IR Remote podemos elegir el parámetro que queremos variar, 
  así como aumentar o disminuir su valor. La rueda de Rotatory Encoder nos permite
  elevar o disminuir el valor de dicho parámetro de forma más rápida mientras
  que el mando de IR lo hará más finamente.
  La pantalla LCD nos permite visualizar la viariación del parámetro modificado.

  Función de las teclas de IR Remote utilizadas:
    VOL+ y VOL-               --->  Sube y baja el volumen
    FAST FORWARD y FAST BACK  --->  Cambia el parámetro a variar
    UP y DOWN                 --->  Aumenta y disminuye el valor del parámetro elegido

  La comunicación con SuperCollider se realiza por medio del puerto Serial.
  Cada vez que se modifica un dato en Arduino se envía un mensaje a SuperCollider
  para modificar los Synths correspondientes.

L
  Pasos a seguir para su funcionamiento:
    1. Conectar Arduino y la circuitería al ordenador por USB
    2. Cargar el sofware a la placa (este paso se puede obviar si ya fue cargado previamente)
    3. Ejecutar el código de supercollider (modificar el nombre del Puerto Serial si es necesario)
    4. Interaccionar con Arduino por medio de los sensores.

  Creado por Carlos Arturo Guerra Parra
  8 de junio de 2019
  para el módulo Electrónica para Instalaciónes
  Profesora: Lina Bautista
  Máster en Arte Sonoro
  Universidad de Barcelona

  Este proyecto es de dominio público.
*/


#include "IRremote.h"
#include <LiquidCrystal.h>

#define COLS 16 // Columnas del LCD
#define ROWS 2 // Filas del LCD
#define input_Rotatory_CLK 2
#define input_Rotatory_DT 3
//#define input_Rotatory_SW 4
#define input_IR 5

struct Fm {
  float freq = 0;
  float armonicidad = 0;
  float index = 0;
  float vol = 0;
} sonido;

void imprime_LCD(String texto1, String texto2 = "");
unsigned int parametro_actual = 3*10000; //Tomará tres valores: 0 = freq carrier, 1 = armonicidad, 2 = índice de modulación

LiquidCrystal lcd(12, 11, 9, 8, 7, 6);
IRrecv irrecv(input_IR);
decode_results results; 

void setup() { 
  pinMode (input_Rotatory_CLK,INPUT);
  pinMode (input_Rotatory_DT,INPUT);
  lcd.begin(COLS, ROWS);
  Serial.begin (9600);
  irrecv.enableIRIn(); // Start the receiver

  // Generamos un sonido con parámetros aleatorios para que no comience siempre igual
  randomSeed(analogRead(0));
  sonido.freq = random(30,200);
  sonido.armonicidad = random(2,30)/100.0;
  sonido.index = random(1,20);
  sonido.vol = 0.05;
  Serial.println(construyeMensaje(sonido));
  imprime_parametro_actual();
}


void loop(){
  static int estado_Rotatory = 0;
  static float cantidad_variacion_Rotatory = 1;
  // Si se gira la rueda:

  if (gira_Rotatory(estado_Rotatory)) {
      switch (parametro_actual%3) {
      case 0:
        cantidad_variacion_Rotatory = 10;
        break;
      case 1:
        cantidad_variacion_Rotatory = 10;
        break;
      case 2:
        cantidad_variacion_Rotatory = 1;
        break;
      }
    modifica_parametro(estado_Rotatory* cantidad_variacion_Rotatory);
    Serial.println(construyeMensaje(sonido));
    imprime_parametro_actual();
  }

  // Si se recive un tecla por IR
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    ejecutaIR(results.value);
    irrecv.resume(); // receive the next value
  }  
}

int ejecutaIR(int tecla_pulsada) {
    static int ultima_tecla = 0; //excepto REPEAT
    static Fm sonido_vol0 = sonido;
     switch(tecla_pulsada)
    {
    case 0xFF629D:  //VOL+
      sonido.vol += 0.001;
      if (sonido.vol > 1) sonido.vol = 1;
      Serial.println(construyeMensaje(sonido));
      imprime_LCD("Volumen:", String(sonido.vol, 4));
      ultima_tecla = tecla_pulsada;
      break;
    case 0xFF22DD:  //FAST BACK
      parametro_actual = (parametro_actual-1);
      imprime_parametro_actual();
      ultima_tecla = tecla_pulsada;
      break;
    case 0xFFC23D:  //FAST FORWARD
      parametro_actual = (parametro_actual+1);
      imprime_parametro_actual();
      ultima_tecla = tecla_pulsada;
      break;
    case 0xFFE01F:   //DOWN
      modifica_parametro(-0.1);
      Serial.println(construyeMensaje(sonido));
      imprime_parametro_actual();
      ultima_tecla = tecla_pulsada;
      break;
    case 0xFFA857:   //VOL-
      sonido.vol -= 0.001;
      if (sonido.vol < 0) sonido.vol = 0;
      Serial.println(construyeMensaje(sonido));
      imprime_LCD("Volumen:", String(sonido.vol, 4));
      ultima_tecla = tecla_pulsada;
      break;      
    case 0xFF906F:  //UP
      modifica_parametro(0.1);
      Serial.println(construyeMensaje(sonido));
      imprime_parametro_actual();
      ultima_tecla = tecla_pulsada;
      break;
    case 0xFFFFFFFF: //REPEAT
      ejecutaIR(ultima_tecla);
      break;  
    default:
      ultima_tecla = tecla_pulsada;
      break;
    }
    delay(20);
}

void modifica_parametro(float cantidad) {
  switch (parametro_actual%3) {
    case 0:
      sonido.freq += cantidad;
      break;
    case 1:
      sonido.armonicidad += cantidad/100.0;
      break;
    case 2:
      sonido.index += cantidad;
    default:
      break;
  }
}

void imprime_parametro_actual() {
  static String text = "";
  static float valor = 0;
  switch (parametro_actual%3) {
    case 0:
      text = "Frec. Portadora:";
      valor = sonido.freq;
      break;
    case 1:
      text = "Armonicidad:";
      valor = sonido.armonicidad;
      break;
    case 2:
      text = "Indice modul.:";
      valor = sonido.index;
      break;
    default:
      exit;
  }
  imprime_LCD (text, String(valor,4));
}

String construyeMensaje(Fm sonido) {
  return String(sonido.freq, 4)+ " " +
    String(sonido.armonicidad,4)+ " " +
    String(sonido.index, 4)+ " " +
    String(sonido.vol, 4);
} 

void imprime_LCD(String texto1, String texto2 = "") {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(texto1);
  lcd.setCursor(0,1);
  lcd.print(texto2);
}

bool gira_Rotatory(int &estado_Rotatory) {
  static int aState = 0;
  static int aLastState = digitalRead(input_Rotatory_DT);  
  static bool rotado = false; 
  aState = digitalRead(input_Rotatory_DT); // Reads the "current" state of the outputA
  // If the previous and the current state of the outputA are different, that means a Pulse has occured
    if (aState != aLastState){
      // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
      if (digitalRead(input_Rotatory_CLK) != aState) {
        estado_Rotatory = 1;
        } else {
          estado_Rotatory = -1;
        }
        rotado = true;
        delay(2);
    } else {
      rotado = false;
    }
    aLastState = aState; // Updates the previous state of the outputA with the current state
    return rotado;
}
