c

#include <VescUart.h>

/** Initiate VescUart class */
VescUart UART;

float current_on = 100.0; /** The current in amps */
float current_off = 0.0;
const int ledPin = 17;

void setup() {
  Serial.begin(9600);
  /** Setup UART port (Serial1 on Atmega32u4) */
  Serial1.begin(19200);
  
  while (!Serial1) {;}

  /** Define which ports to use as UART */
  UART.setSerialPort(&Serial1);
  UART.setDebugPort(&Serial);
}

void loop() {
  if ( UART.getVescValues() ) {
//    Serial.println("print vesc values");
//    Serial.println(UART.data.rpm);
//    Serial.println(UART.data.inpVoltage);
//    Serial.println(UART.data.ampHours);
//    Serial.println(UART.data.tachometerAbs);
    UART.printVescValues();
  }
  else
  {
    Serial.println("Failed to get data!");
  }
  
  /** Call the function setCurrent() to set the motor current */
  UART.setCurrent(current_on);

  digitalWrite(ledPin, LOW);

  delay(100);

//  UART.setCurrent(0.0);
//
//  digitalWrite(ledPin, HIGH);
//
//  delay(1000);
//
//  UART.setDuty(0.5);
//
//  delay(1000);
//
//  UART.setDuty(0.0);
//
//  delay(1000);
//
//  UART.setRPM(1000);
//
//  delay(1000);
//
//  UART.setRPM(0);

  
  
}