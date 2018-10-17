//Hardware constants
const int PASPin = 2;    // input from PAS
const int ledPin = 13, PWMOut=11;  // the pin that the LED is attached to and the PWM output pin

//Software constants
const unsigned long activityTimeoutMS = 300; // Allowed PAS signal inactivity time before turning off
const int startPulses = 2; // Number of PAS pulses needed before turning on
const int lowPWMValue = 56, highPWMValue = 170; // PWM values to drive throttle input, default 56 (1,1 V) and 170 (3,4 V), U=n/255*5V, n=U/5V*255

// Variables
volatile int inputEdges = 0; // counter for the number of pulses since last reset
volatile unsigned long lastEdgeTime = 0; // timestamp of last PAS pulse
bool state=false; // variable holding information about the state of the output
volatile int currentPWMValue = 0;

void setup() {
  pinMode(PASPin, INPUT); // initialize the PAS pin as a input
  attachInterrupt(digitalPinToInterrupt(PASPin), pulse, RISING); //Each rising edge on PAS pin causes an interrupt
  pinMode(ledPin, OUTPUT); // initialize the LED as an output
  pinMode(PWMOut, OUTPUT); // initialize the PWM pin as an output
}


void loop() {
  //If PAS signal is inactive for too long, turn off everything
  unsigned long curTime=millis();
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
  
  //Use LED for status info
  digitalWrite(ledPin, state);
}

//Turn off output, reset pulse counter and set state variable to false
void turnOff() {
  noInterrupts();
  analogWrite(PWMOut, lowPWMValue);
  inputEdges=0;
  state=false;
  interrupts();
}

//Turn on output and set state variable to true
void turnOn() {
  //analogWrite(PWMOut, highPWMValue);
  //state=true;
  state=true;
  currentPWMValue = lowPWMValue;
  unsigned long currentTime = millis();
  unsigned long intervalMS = 2;
  int rampPWMValue = 1;
  while (state && (currentPWMValue < highPWMValue)) {
    if ((millis() - currentTime) > intervalMS) {
      analogWrite(PWMOut, currentPWMValue);
      analogWrite(ledPin, currentPWMValue);
      currentPWMValue = currentPWMValue + rampPWMValue;
      currentTime = millis();
    }
  }
}

//Interrupt subroutine, refresh last impulse timestamp and increment pulse counter (until 10000 is reached)
void pulse() {
  lastEdgeTime=millis();
  if (inputEdges<100) {
    inputEdges++;
  }  
}









