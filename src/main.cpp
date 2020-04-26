#include <Arduino.h>
//https://github.com/SolidGeek/VescUart
#include <VescUart.h>
#include <Display.h>
#include "Config.h"




void pulse();
void reportStatus();


//DO NOT USE.
//THIS SOFTWARE IS NOT TESTED AND UNFIT FOR ANY PURPOSE.


// Variables
volatile unsigned long lastEdgeTime = 0;     // timestamp of last PAS pulse
volatile unsigned long lastEdgeInterval = 0; //interval since last pulse
volatile unsigned long edgeInterval = 0;     // interval between last 2 pulses
volatile unsigned long edgeTime = 0;         // only used inside pas interrupt function

float cadence = 0.0;
float throttleStep = 0.0;

bool state = false; // variable holding information about the state of the output
int switchPos = 0;

float targetDuty = 0.0;
float throttleDuty = 0.0;

VescUart UART;

//timing loops
unsigned long curTime = 0;
unsigned long sendTime = 0;
unsigned long reportTime = 0;

char str[6];

void setup()
{
  Serial.begin(9600);

  pinMode(PAS_PIN, INPUT);  // initialize the PAS pin as a input
  //Each rising edge on PAS pin causes an interrupt
  attachInterrupt(digitalPinToInterrupt(PAS_PIN), pulse, RISING);
  pinMode(LED_PIN, OUTPUT); // initialize the LED as an output

  //initialize the switch pins
  pinMode(SWITCH_PIN_POS1, INPUT_PULLUP);
  pinMode(SWITCH_PIN_POS3, INPUT_PULLUP);

  //uart to vesc
  Serial1.begin(9600);
  while (!Serial1)
  {
    ;
  }
  UART.setSerialPort(&Serial1);
  //UART.setDebugPort(&Serial);

  //init timing loops
  curTime = millis();
  sendTime = curTime;
  reportTime = curTime;

  //setup leds
  setupDisplay();
  
  
  //TODO LH necessary ?
  delay(100);
}

void loop()
{

  curTime = millis();


  //logic to read 3 position switch
  //this should have some safety built in, if it loses ground it will stick in a position.
  if (digitalRead(SWITCH_PIN_POS1) == LOW)
  {
    //blue and red(gnd)
    switchPos = 1;
    targetDuty = THROTTLE_OFF;
  }
  else if (digitalRead(SWITCH_PIN_POS3) == LOW)
  {
    //black and red(gnd)
    switchPos = 3;
    targetDuty = THROTTLE_DUTY_MAX;
  }
  else
  {
    //middle pos
    switchPos = 2;
    targetDuty = THROTTLE_DUTY_MID;
  }

  //TODO LH need to revisit the logic here..
  //think this stops it sticking at last cadence when pedalling stops
  lastEdgeInterval = curTime - lastEdgeTime;
  if (lastEdgeInterval > edgeInterval)
  {
    edgeInterval = lastEdgeInterval;
  }
  
  //calculate cadence
  if (edgeInterval > 0.0)
  {
    //this calculation always gives multiples of 5.. think it is faulty
    //cadence = (1000 / edgeInterval) * (60 / 12); //should give rpm this is getting truncated somehow.. 

    cadence = 60000 / (edgeInterval * CADENCE_MAGNETS); 
  }
  else
  {
    cadence = 0.0;
  }
  throttleStep = (targetDuty - THROTTLE_DUTY_MIN) / (CADENCE_MAX - CADENCE_MIN);
  if (targetDuty == THROTTLE_OFF)
  {
    throttleDuty = THROTTLE_OFF;
  }
  else if (cadence > CADENCE_MAX)
  {
    throttleDuty = targetDuty;
  }
  else if (cadence < CADENCE_MIN)
  {
    throttleDuty = THROTTLE_OFF;
  }
  else
  {
    //need to start collecting these up
    throttleDuty = ((cadence - CADENCE_MIN) * throttleStep) + THROTTLE_DUTY_MIN;
  }

  if ((curTime - sendTime) > SEND_TIMEOUT)
  {
    sendTime = curTime;

    //TODO LH testing if setDuty to 0.0 causing braking behaviour and wheeling backwards
    if (throttleDuty == THROTTLE_OFF)
    {
      UART.setCurrent(THROTTLE_OFF);
    }
    else
    {
      UART.setDuty(throttleDuty);
    }

    if ((curTime - reportTime) > REPORT_TIMEOUT)
    {
      reportTime = curTime;
      reportStatus();
    }
  }

  //TODO LH need to rework this
  //Use LED for status info
  digitalWrite(LED_PIN, state);
}

//Interrupt subroutine, get time between pulses. no longer tooks at pulse counts
void pulse()
{
  edgeTime = millis();
  edgeInterval = edgeTime - lastEdgeTime;
  lastEdgeTime = edgeTime;
}

void reportStatus()
{
  Serial.print(F("cad: "));
  Serial.print(cadence, 1); // Show 1 decimal place
  // Serial.print(",tcur: ");
  // Serial.print(throttleCurrent);
  Serial.print(F(",tduty: "));
  Serial.print(throttleDuty);
  Serial.print(F(",tstep: "));
  Serial.println(throttleStep);
  Serial.print(F("edgeInterval : "));
  Serial.print(edgeInterval);
  Serial.print(F("\n"));

  // Serial.print("lastEdgeTime : ");
  // Serial.println(lastEdgeTime);

  if (UART.getVescValues())
  {
    // Serial.println("-------VESC-------");
    // Serial.print("rpm: ");
    // Serial.print(UART.data.rpm);
    // Serial.print(",volt: ");
    // Serial.print(UART.data.inpVoltage);
    // Serial.print(",ah: ");
    // Serial.println(UART.data.ampHours);
    
    
    updateDisplay(cadence, UART.data.rpm, UART.data.inpVoltage, UART.data.ampHours);
  }
  else
  {
    Serial.println(F("vesc data not available"));
    updateDisplay(cadence, 0, 0, 0);
  }
}



