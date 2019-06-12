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

  Licencia: GNU General Public License v3.0
*/



// Iniciamos el servidor:
(
s.boot;
// Modificar el nombre del Puerto Serial (se puede consultar en el software de Arduino una vez conectada la placa)
~nombre_Puerto_Serial = "/dev/ttyACM0";
)


// Se carga el sintetizador, se abre el puerto Serial y se inicia la rutina de escucha del puerto desde Arduino
(
SynthDef("fm1", {arg bus = 0, freq = 440, armonicidad = 0, index = 0, vol = 0.05;
	// index values usually are between 0 and 24
	var mod;
	var car;

	mod = SinOsc.ar(freq / armonicidad, 0, freq * index);
	car = SinOsc.ar(freq + mod, 0, vol);

	Out.ar(bus, car)
}, [0, 0.3, 0.3, 0.3, 1]).add;
~sig = nil;

p = SerialPort(
    ~nombre_Puerto_Serial,    //edit to match your port. SerialPort.listDevices
    baudrate: 9600,    //check that baudrate is the same as in arduino sketch
    crtscts: true);

// Esta rutina recibe una cadena de texto completa desde Serial y la parte en palabras (separadas por espacio) en un array
t= Routine({
    var byte, str, res;
    inf.do{|i|
        if(p.read==10, { //Si ha comenzado nueva línea
            str = "";
            while({byte = p.read; byte !=13 }, { //mientras no sea final de línea
                str= str++byte.asAscii;
            });
			res = str.split($ );
			if (~sig==nil, {
				~sig = [
					Synth("fm1", [\freq, res[0].asFloat,
						\armonicidad, res[1].asFloat,
						\index, res[2].asFloat,
						\vol, res[3].asFloat,
						\bus, 0
					]);,
					Synth("fm1", [\freq, res[0].asFloat  *1.001,
						\armonicidad, res[1].asFloat*1.001,
						\index, res[2].asFloat*1.001,
						\vol, res[3].asFloat,
						\bus, 1
					]);

			]}, {
				~sig[0].set(
					\freq, res[0].asFloat,
					\armonicidad, res[1].asFloat,
					\index, res[2].asFloat,
					\vol, res[3].asFloat
				);
				~sig[1].set(
					\freq, res[0].asFloat*1.001,
					\armonicidad, res[1].asFloat*1.001,
					\index, res[2].asFloat*1.001,
					\vol, res[3].asFloat*1.001
				);
			});

            res.postln; //muestra el mensaje completo. Aquí se puede introducir la orden al Synth.
        });
    };
}).play;
)
