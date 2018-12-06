/*  _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
 * / \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \  
 * \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ 
 *
 * Sets an E-Bike throttle proportinal to the Cadence of the rider
 * 
 *   for more information visit
 *    www.ideahex.com
 * 
 * Written by Damian Kleiss 
 *  _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
 * / \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \  
 * \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \*/
 
// Definitions
#define CADENCE_PIN 7 // cadence sensor connected to pin 2 external interrupt 0
#define OVERIDE_PIN 4 // overide switch sensor connected to pin 4
#define THROTTLE_PIN 5  // throttle output connected to pin 5
// note "The PWM outputs generated on pins 5 and 6 will have higher-than-expected duty cycles"
 
#define CADENCE_MAGNETS 12
 
#define THROTTLE_MIN 5   // Min value required before power is applied to the motor
#define THROTTLE_MAX 25  // A larger throttle value will not result in more power
#define THROTTLE_OFF 0
 
#define CADENCE_MIN 15  // minimum cadence for motor to run
#define CADENCE_MAX 80  // cadence value that will result in full throttle
#define MAX_CADENCE_PERIOD 60000/(CADENCE_MIN*CADENCE_MAGNETS)  // Convert rpm to period
 
#define THROTTLE_STEP (THROTTLE_MAX-THROTTLE_MIN)/(CADENCE_MAX-CADENCE_MIN) // Throttle ramp value
 
// Global Variables //
volatile int throttleValue = 0;
 
// Cadence Variables
volatile float currentCadence = 0;
volatile int cadenceState = 0;
volatile long cadencePositiveTimer = 0;
volatile long cadencePositivePeriod = 0;
volatile long cadenceNegativeTimer = 0;
volatile long cadenceNegativePeriod = 0;
volatile int cadenceInteruptFlag = 0;
 
unsigned long cadenceInterruptMillis = 0;
 
void setup()
{
  Serial.begin(9600); 
  Serial.println("ideaHex Throttle Test");
  Serial.println("Visit ideahex.com\r\n");
  pinMode(CADENCE_PIN, INPUT);
  //pinMode(OVERIDE_PIN, INPUT_PULLUP); // Use internal pullups
  //pinMode(THROTTLE_PIN, OUTPUT);
  attachInterrupt(0, CadenceInterruptHandler, CHANGE);  // External interrupt for cadence sensor
}
 
void loop()
{
  // Just set Throttle to Max if Over ride pressed
//  while(digitalRead(OVERIDE_PIN)==LOW)  // Switch pulls to ground when pressed
//  {
//    analogWrite(THROTTLE_PIN, THROTTLE_MAX);
//  }
  // As there won't be an interrupt if the pedals have stopped
  // check if a period longer than the minimum rpm has passed since
  // the last interrupt. If so set the throttle to minimum
  if(millis() - cadenceInterruptMillis > MAX_CADENCE_PERIOD) 
  {
    throttleValue = THROTTLE_OFF;
  }
  // Check if an interrupt has occured
  if(cadenceInteruptFlag == HIGH)
  {
    cadenceInterruptMillis = millis(); // reset time since last interrupt for min rpm
    ProcessCadence(); // Update cadence rpm value
    cadenceInteruptFlag = LOW; // reset flag
    Serial.print("cad = ");
    Serial.print(currentCadence,1); // Show 1 decimal place
    if(currentCadence > CADENCE_MAX) throttleValue = THROTTLE_MAX; 
    else if(currentCadence < CADENCE_MIN) throttleValue = THROTTLE_OFF;
    else throttleValue = ((currentCadence-CADENCE_MIN)*THROTTLE_STEP)+THROTTLE_MIN;
    Serial.print(", Throttle = ");
    Serial.println(throttleValue); 
  }
  //analogWrite(THROTTLE_PIN, throttleValue); // Actualy output the throttle value
}
 
void CadenceInterruptHandler()
{
  if(digitalRead(CADENCE_PIN) == HIGH) // Positive period
  {
    cadencePositiveTimer = millis();
    cadenceNegativePeriod = millis() - cadenceNegativeTimer;  
  }
 
  else if(digitalRead(CADENCE_PIN) == LOW) // Negative period
  {
    cadencePositivePeriod = millis() - cadencePositiveTimer;  
    cadenceNegativeTimer = millis();
    cadenceInteruptFlag = HIGH; // set a flag so we know a pulse has occurred
  }
}
 
void ProcessCadence()
{
  // Check if pedaling forward
  if (cadencePositivePeriod > cadenceNegativePeriod)
  {
    float period = float(cadencePositivePeriod + cadenceNegativePeriod)/60000;
    currentCadence = (1/period)/CADENCE_MAGNETS;    
  }
  // If not pedalling forward set cadence to zero
  else currentCadence = 0;  
}
