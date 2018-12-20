
#include <VescUart.h>

VescUart UART;

 //DO NOT USE.
//THIS SOFTWARE IS NOT TESTED AND UNFIT FOR ANY PURPOSE.

#define CADENCE_MAGNETS 12
 
#define THROTTLE_MIN 1   // Min value required before power is applied to the motor
#define THROTTLE_MAX 25  // A larger throttle value will not result in more power
#define THROTTLE_OFF 0
 
#define CADENCE_MIN 15  // minimum cadence for motor to run
#define CADENCE_MAX 55  // cadence value that will result in full throttle

#define THROTTLE_STEP (THROTTLE_MAX-THROTTLE_MIN)/(CADENCE_MAX-CADENCE_MIN)

//Hardware constants
const int PASPin = 7;    // input from PAS
const int ledPin = 17;
const int switchPinPos1 = 9, switchPinPos3 = 8;  // the pin that the LED is attached to and the PWM output pin
const int PWMOut = 10;
//tested on arduino due pins

//Software constants
const unsigned long activityTimeoutMS = 400; // Allowed PAS signal inactivity time before turning off
const int startPulses = 2; // Number of PAS pulses needed before turning on
const int lowPWMValue = 45, highPWMValue = 132; // PWM values to drive throttle input, default 56 (1,1 V) and 170 (3,4 V), U=n/255*5V, n=U/5V*255

// Variables
volatile int inputEdges = 0; // counter for the number of pulses since last reset
volatile unsigned long lastEdgeTime = 0; // timestamp of last PAS pulse

volatile unsigned long edgeInterval = 0;
volatile unsigned long lastEdgeInterval = 0;
volatile unsigned long edgeTime = 0;
float cadence = 0.0;
float throttleStep = 0.0;

bool state=false; // variable holding information about the state of the output
volatile int currentPWMValue = 0;
volatile unsigned long pedallingTime = 0;
volatile int cadenceTicks = 0;
int switchPos = 0;
float targetCurrent = 0.0;
volatile float throttleCurrent = 0.0;



void setup() {
  Serial.begin(9600);
  
  pinMode(PASPin, INPUT); // initialize the PAS pin as a input
  attachInterrupt(digitalPinToInterrupt(PASPin), pulse, RISING); //Each rising edge on PAS pin causes an interrupt
  pinMode(ledPin, OUTPUT); // initialize the LED as an output
  pinMode(PWMOut, OUTPUT); // initialize the PWM pin as an output

  pinMode(switchPinPos1, INPUT_PULLUP);
  pinMode(switchPinPos3, INPUT_PULLUP);

  Serial1.begin(9600);
//  Serial1.begin(115200);
  while (!Serial1) {;}
  
  UART.setDebugPort(&Serial);
  UART.setSerialPort(&Serial1);
}


void loop() {
  //If PAS signal is inactive for too long, turn off everything
  unsigned long curTime=millis();
  if (pedallingTime == 0) {
    pedallingTime = curTime;
  }
  if ((curTime>lastEdgeTime)&&((curTime-lastEdgeTime)>activityTimeoutMS)) {
    turnOff();
  }
  
  //If system is off, check if the impulses are active
  if ((!state)&&((millis()-lastEdgeTime)<activityTimeoutMS)) {
    //if impulses are active, check if there were enough pulses to turn on
    if (inputEdges>startPulses) {
      turnOn();
    }
  }

  //logic to read 3 position switch
  //this should have some safety built in, if it loses ground it will stick in a position.
  if (digitalRead(switchPinPos1) == LOW) {
     //blue and red(gnd)
     switchPos = 1;
     targetCurrent = 0.0;
     state = false; //remove this just for switch as accelerator
  } else if (digitalRead(switchPinPos3) == LOW){
     //black and red(gnd)
     switchPos = 3;
     targetCurrent = 25.0;
     
  } else {
     //middle pos
     switchPos = 2;
     targetCurrent = 12.5;    
     
  } 

  lastEdgeInterval = curTime - lastEdgeTime;
  if (lastEdgeInterval > edgeInterval) {
    edgeInterval = lastEdgeInterval;
  }
  //calculate cadence
  if (edgeInterval > 0.0) {
    cadence = (1000/edgeInterval)*(60/12); //should give rpm
  } else {
    cadence = 0.0;
  }

  throttleStep = (targetCurrent-THROTTLE_MIN)/(CADENCE_MAX-CADENCE_MIN);
  if (targetCurrent == 0.0) {
    throttleCurrent = THROTTLE_OFF;
  } else if (inputEdges > 2 && inputEdges < 36) {
    throttleCurrent = THROTTLE_MAX;
  } else if (cadence > CADENCE_MAX) {
    throttleCurrent = THROTTLE_MAX; 
  } else if(cadence < CADENCE_MIN) {
    throttleCurrent = THROTTLE_OFF;
  } else {
    throttleCurrent = ((cadence-CADENCE_MIN)*throttleStep)+THROTTLE_MIN;
  }

 

  
  if ((curTime - pedallingTime) > 5) {
    pedallingTime = curTime;
    //if (state) {
      //Serial.println("state is on switching current on");
      UART.setCurrent(throttleCurrent);
    //}

//    Serial.print("cad = ");
//    Serial.print(cadence,1); // Show 1 decimal place
//    Serial.print(", Throttle = ");
//    Serial.print(throttleCurrent); 
//    Serial.print(", Throttle = ");
//    Serial.println(throttleStep);
//    Serial.print("edgeInterval : ");
//    Serial.print(edgeInterval);
//    Serial.print("\n");
//
//    Serial.print("lastEdgeTime : ");
//    Serial.print(lastEdgeTime);
//    Serial.print("\n");
  }

//  if ((curTime - pedallingTime) > 5000) {
//    Serial.print("cadenceTicks : ");
//    Serial.print(cadenceTicks);
//    Serial.print("\n");
//    int rpm = (cadenceTicks*60)/12;
//    Serial.print("rpm : ");
//    Serial.print(rpm);
//    Serial.print("\n");
//     
//    pedallingTime = curTime;
//    noInterrupts();
//    cadenceTicks=0;
//    interrupts();
//
//    Serial.print("switch position : ");
//    Serial.print(switchPos);
//    Serial.print("\n");
//
//
//    //digitalWrite(ledPin, true);
//    //UART.printVescValues();
//
////    if ( UART.getVescValues() ) {
////    Serial.println("print vesc values");
////    Serial.println(UART.data.rpm);
////    Serial.println(UART.data.inpVoltage);
////    Serial.println(UART.data.ampHours);
////    Serial.println(UART.data.tachometerAbs);
////
////  }
////  else
////  {
////    Serial.println("Failed to get data!");
////  }
//
//    Serial.print("state : ");
//    Serial.print(state);
//    Serial.print("\n");
//
//  
//  }
  
  //Use LED for status info
  digitalWrite(ledPin, state);
  
//  if (state) {
//    Serial.println("state is on switching current on");
//    UART.setCurrent(targetCurrent);
//  }
  //delay(50);
}

//Turn off output, reset pulse counter and set state variable to false
void turnOff() {
  noInterrupts();
  //analogWrite(PWMOut, lowPWMValue);
  //UART.setDuty(0.0);
  inputEdges=0;
  //edgeInterval=0;
  state=false;
  interrupts();
}

//Turn on output and set state variable to true
void turnOn() {
  //Serial.print("sensor = ");
  //Serial.print(sensorValue);
  //analogWrite(PWMOut, highPWMValue);
  //UART.setDuty(0.1);
  state=true;
}

//Interrupt subroutine, refresh last impulse timestamp and increment pulse counter (until 10000 is reached)
void pulse() {
  edgeTime = millis();
  edgeInterval = edgeTime - lastEdgeTime;
  lastEdgeTime = edgeTime;

  if (inputEdges<100) {
    inputEdges++;
  }  
  cadenceTicks++;
}









