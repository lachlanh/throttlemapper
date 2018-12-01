const int ledPin = 17;
const int PWMOut = 10;
const int lowPWMValue = 45, highPWMValue = 132; // PWM values to drive throttle input, default 56 (1,1 V) and 170 (3,4 V), U=n/255*5V, n=U/5V*255

volatile int ticks = 0;
int currentPWMValue = 0;
void setup() {
  //Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(PWMOut, OUTPUT); // initialize the PWM pin as an output

  currentPWMValue = highPWMValue;

}

void loop() {
  // put your main code here, to run repeatedly:
  if (ticks >= 5000) {
    if (currentPWMValue == highPWMValue) {
      currentPWMValue = lowPWMValue;
    } else {
      currentPWMValue = highPWMValue;
    }
    ticks = 0;
  }
  if (currentPWMValue == highPWMValue) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  analogWrite(PWMOut, currentPWMValue);
  ticks++;
  //delay(50);
}
