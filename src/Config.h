//memory diagnostics
#define REPORT_MEMORY

//Hardware constants
const int PAS_PIN = 7;
const int LED_PIN = 13;
const int SWITCH_PIN_POS1 = 9;
const int SWITCH_PIN_POS3 = 8;

const int SERIAL_RX_PIN = 10;
const int SERIAL_TX_PIN = 11;

//Pedal Assist constants
const int CADENCE_MAGNETS = 12;
const int CADENCE_MIN = 15; // minimum cadence for motor to run
const int CADENCE_MAX = 45; // cadence value that will result in full throttle

//Throttle related constants
const float THROTTLE_MIN = 5.0;  // Min value required before power is applied to the motor
const float THROTTLE_MAX = 25.0; // A larger throttle value will not result in more power
const float THROTTLE_OFF = 0.0;
const float THROTTLE_STEP = (THROTTLE_MAX - THROTTLE_MIN) / (CADENCE_MAX - CADENCE_MIN);//TODO LH should this be using throttle duty ?
const float THROTTLE_DUTY_MIN = 0.3;
const float THROTTLE_DUTY_MID = 0.5;
const float THROTTLE_DUTY_MAX = 0.8;

//timing loops
const int SEND_TIMEOUT = 50;     //ms to send signal to vesc
const int REPORT_TIMEOUT = 1000; //ms to update screen/serial

//Display related constants
const int NUM_LEDS = 4;
const int VLED_PIN = 10;
