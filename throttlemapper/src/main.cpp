#include <Arduino.h>
//https://github.com/SolidGeek/VescUart
#include <VescUart.h>
//#include <FastLED.h>
#include <WS2812.h>
#include <U8g2lib.h>

VescUart UART;
U8G2_SSD1306_128X64_NONAME_1_HW_I2C   u8g2(U8G2_R2);

//leds
WS2812 LED(4); // 1 LED
cRGB value;

void pulse();
void turnOn();
void turnOff();
void reportStatus();
void updateDisplay(long cadence, long kph, float volts, float ah);

 //DO NOT USE.
//THIS SOFTWARE IS NOT TESTED AND UNFIT FOR ANY PURPOSE.

#define CADENCE_MAGNETS 12
 
#define THROTTLE_MIN 5   // Min value required before power is applied to the motor
#define THROTTLE_MAX 25  // A larger throttle value will not result in more power
#define THROTTLE_OFF 0
 
#define CADENCE_MIN 15  // minimum cadence for motor to run
#define CADENCE_MAX 45  // cadence value that will result in full throttle

#define THROTTLE_STEP (THROTTLE_MAX-THROTTLE_MIN)/(CADENCE_MAX-CADENCE_MIN)

#define SEND_TIMEOUT 50 //ms to send signal to vesc
#define REPORT_TIMEOUT 1000 //ms to update screen/serial

#define ACTIVITY_TIMEOUT 400 //ms for inactivity
#define START_EDGES 2 //pas edges to start (only used for boost)
#define BOOST_EDGES 18 //edges until boost stops

//Hardware constants
const int PASPin = 7;
const int ledPin = 17;
const int switchPinPos1 = 9;
const int switchPinPos3 = 8;
const int vledPin = 10; //wire voltage led to pin 21, with vcc and gnd //hmm really think pin 10 would be better .. easier to wire

// Variables
volatile int inputEdges = 0; // counter for the number of pulses since last reset
volatile unsigned long lastEdgeTime = 0; // timestamp of last PAS pulse
volatile unsigned long lastEdgeInterval = 0; //interval since last pulse
volatile unsigned long edgeInterval = 0; // interval between last 2 pulses
volatile unsigned long edgeTime = 0; // only used inside pas interrupt function

float cadence = 0.0;
float throttleStep = 0.0;

bool state=false; // variable holding information about the state of the output
int switchPos = 0;
float targetCurrent = 0.0;
volatile float throttleCurrent = 0.0;

float targetDuty = 0.0;
float throttleDuty = 0.0;
float throttleDutyMin = 0.3;
float throttleDutyMax = 1.0;

boolean startupBoost=false;

//timing loops
unsigned long curTime = 0;
unsigned long sendTime = 0;
unsigned long reportTime = 0;

char str[6];

//CRGB leds[4];

void setup() {
  Serial.begin(9600);
  
  pinMode(PASPin, INPUT); // initialize the PAS pin as a input
  attachInterrupt(digitalPinToInterrupt(PASPin), pulse, RISING); //Each rising edge on PAS pin causes an interrupt
  pinMode(ledPin, OUTPUT); // initialize the LED as an output
  
  //initialize the switch pins
  pinMode(switchPinPos1, INPUT_PULLUP);
  pinMode(switchPinPos3, INPUT_PULLUP);

  //uart to vesc
  Serial1.begin(9600);
  while (!Serial1) {;}
  UART.setSerialPort(&Serial1);
  //UART.setDebugPort(&Serial); 
  
  //init timing loops
  curTime = millis();
  sendTime = curTime;
  reportTime = curTime;

  //setup leds
  //FastLED.addLeds<WS2812, vledPin, GRB>(leds, 4); 
  LED.setOutput(vledPin); 
  //LEDS.setBrightness(100);

  //setup the display
  u8g2.begin();
  
  //TODO LH necessary ?
  delay(1000);
}


void loop() {
  
  curTime=millis();

  //If PAS signal is inactive for too long, turn off everything
  if ((curTime>lastEdgeTime)&&((curTime-lastEdgeTime)>ACTIVITY_TIMEOUT)) {
    turnOff();
  }
  
  //If system is off, check if the impulses are active
  if ((!state)&&((millis()-lastEdgeTime)<ACTIVITY_TIMEOUT)) {
    //if impulses are active, check if there were enough pulses to turn on
    if (inputEdges>START_EDGES) {
      turnOn();
    }
  }

  //logic to read 3 position switch
  //this should have some safety built in, if it loses ground it will stick in a position.
  if (digitalRead(switchPinPos1) == LOW) {
     //blue and red(gnd)
     switchPos = 1;
     targetCurrent = 0.0;
     targetDuty = 0.0;
     
  } else if (digitalRead(switchPinPos3) == LOW){
     //black and red(gnd)
     switchPos = 3;
     targetCurrent = 25.0;
     targetDuty = 0.8;
     
  } else {
     //middle pos
     switchPos = 2;
     targetCurrent = 12.5;    
     targetDuty = 0.5;
  } 

  //TODO LH need to revisit the logic here..
  //think this stops it sticking at last cadence when pedalling stops
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
  startupBoost=false;
  throttleStep = (targetDuty-throttleDutyMin)/(CADENCE_MAX-CADENCE_MIN);
  if (targetDuty == 0.0) {
    throttleDuty = 0.0;
  } else if (cadence > CADENCE_MAX) {
    throttleDuty = targetDuty; 
  } else if(cadence < CADENCE_MIN) {
    throttleDuty = 0.0;
  } else {
    //need to start collecting these up 
    throttleDuty = ((cadence-CADENCE_MIN)*throttleStep)+throttleDutyMin;
  }

  if ((curTime - sendTime) > SEND_TIMEOUT) {
    sendTime = curTime;

    //TODO LH testing if setDuty to 0.0 causing braking behaviour and wheeling backwards
    if (throttleDuty == 0.0) {
      //UART.setCurrent(0.0);
    } else {
      //UART.setDuty(throttleDuty);
    }
    
    if ((curTime - reportTime) > REPORT_TIMEOUT) {
      reportTime = curTime;
      reportStatus();
    }

  }

  //TODO LH need to rework this
  //Use LED for status info
  digitalWrite(ledPin, state);
  
}

void reportStatus() {
   Serial.print("cad: ");
   Serial.print(cadence,1); // Show 1 decimal place
   Serial.print(",tcur: ");
   Serial.print(throttleCurrent);
   Serial.print(",tduty: ");
   Serial.print(throttleDuty); 
   Serial.print(",tstep: ");
   Serial.println(throttleStep);
   Serial.print("edgeInterval : ");
   Serial.print(edgeInterval);
   Serial.print("\n");

   Serial.print("lastEdgeTime : ");
   Serial.println(lastEdgeTime);

   if ( UART.getVescValues() ) {
     Serial.println("-------VESC-------");
     Serial.print("rpm: ");
     Serial.print(UART.data.rpm);
     Serial.print(",volt: ");
     Serial.print(UART.data.inpVoltage);
     Serial.print(",ah: ");
     Serial.println(UART.data.ampHours);
     if (UART.data.inpVoltage >= 40.0) {
        //leds[0] = CRGB::Green;
     } else if (UART.data.inpVoltage >= 38.0) {
        //leds[0] = CRGB::Yellow;
     } else if (UART.data.inpVoltage >= 36.0) {
        //leds[0] = CRGB::Orange;
     } else {
        //leds[0] = CRGB::Red;
     }
     //FastLED.show();
     //sprintf(str, "%ld rpm", UART.data.rpm);
     updateDisplay(cadence, UART.data.rpm, UART.data.inpVoltage, UART.data.ampHours);
     //Serial.println(UART.data.tachometerAbs);
   } else {
     Serial.println("vesc data not available");
     value.b = 255; value.g = 0; value.r = 0; // RGB Value -> Blue
	  LED.set_crgb_at(0, value); // Set value at LED found at index 0
	  LED.sync();//
     //leds[0] = CRGB::Red;
     //FastLED.show();
     updateDisplay(0, 0, 0, 0);
   }
   
}

//Turn off output, reset pulse counter and set state variable to false
void turnOff() {
  noInterrupts();
  inputEdges=0;
  state=false;
  interrupts();
}

//Turn on output and set state variable to true
void turnOn() {
  state=true;
}

//Interrupt subroutine, refresh last impulse timestamp and increment pulse counter (until 10000 is reached)
void pulse() {
  edgeTime = millis();
  edgeInterval = edgeTime - lastEdgeTime;
  lastEdgeTime = edgeTime;

  inputEdges++;
}

void updateDisplay(long cadence, long kph, float volts, float ah) {
  u8g2.firstPage();
  do {
    //x offset, y offset
    /* u8g2.setFont(u8g2_font_logisoso26_tr);
    sprintf(str, "%2ld", cadence);
    u8g2.drawStr(0,26,str);
    sprintf(str, "%2ld", kph);
    u8g2.drawStr(0,56,str);
    dtostrf(volts,2, 0, str);
    u8g2.drawStr(0,86,str);
    dtostrf(ah,2, 0, str);
    u8g2.drawStr(0,116,str);

    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.drawStr(34,26,"cd");//x offset, y offset
    u8g2.drawStr(34,56,"kh");
    u8g2.drawStr(34,86,"v");
    u8g2.drawStr(34,116,"ah");
 */
        //x offset, y offset
    u8g2.setFont(u8g2_font_logisoso26_tr);
    sprintf(str, "%2ld", cadence);
    u8g2.drawStr(0,26,str);
    sprintf(str, "%2ld", kph);
    u8g2.drawStr(0,56,str);
    dtostrf(volts,2, 0, str);
    u8g2.drawStr(66,26,str);
    dtostrf(ah,2, 0, str);
    u8g2.drawStr(66,56,str);

    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.drawStr(34,26,"cd");//x offset, y offset
    u8g2.drawStr(34,56,"rpm");
    u8g2.drawStr(100,26,"v");
    u8g2.drawStr(100,56,"ah");

    u8g2.setFont(u8g2_font_artossans8_8r);
    u8g2.drawStr(0,64,"hello");
  } while ( u8g2.nextPage() );
}