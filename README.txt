Controlador de Frecuencia Modulada en SuperCollider
===================================================
===================================================

  Este proyecto consta de dos archivos, uno de Arduino y otro de Supercollider.

  Componentes hardware:
  =====================
    * ELEGOO Mega 2560 R3 (compatible con Arduino Mega 2660)
    * Infrared Receiver module AX-1838HS
    * Arduino IR Remote Control
    * LCD display LCM1602A
    * Resistencia 220 ohm
    * Pontenciómetro 10 Kohm

  Descripción:
  ============
  Permite controlar a voluntad con una perilla y un mando de infrarrojos
  desde Arduino tres parámetros de un sonido de Frecuencia Modulada:
  Frecuencia del la señal portadora, Armonicidad e Índice de modulación.
  Para ello nos servimos de dos módulos de entrada: IR Remote y Rotatory Encoder.
  Con el mando de IR Remote podemos elegir el parámetro que queremos variar, 
  así como aumentar o disminuir su valor. La rueda de Rotatory Encoder nos permite
  elevar o disminuir el valor de dicho parámetro de forma más rápida mientras
  que el mando de IR lo hará más finamente.
  La pantalla LCD nos permite visualizar la viariación del parámetro modificado.

  Video demostrativo:
  ===================
  https://youtu.be/LHWy5auKayw

  Función de las teclas de IR Remote utilizadas:
  ==============================================
    VOL+ y VOL-               --->  Sube y baja el volumen
    FAST FORWARD y FAST BACK  --->  Cambia el parámetro a variar
    UP y DOWN                 --->  Aumenta y disminuye el valor del parámetro elegido

  La comunicación con SuperCollider se realiza por medio del puerto Serial.
  Cada vez que se modifica un dato en Arduino se envía un mensaje a SuperCollider
  para modificar los Synths correspondientes.

  Pasos a seguir para su funcionamiento:
  ======================================
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
