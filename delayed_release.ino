// Laelaps Delayed Release Firmware
// v1.0 10/12/2022
// James Sorber
// Aerial Robotics Club at NC State

/*  
    This sketch's primary purpose is to deploy the parachute from
    Laelaps (The bottle frame) after a short delay once Laelaps is
    dropped. The microcontroller on Laelaps (currently Seeed Studio XIAO SAMD21)
    watches for a loss of external voltage to signal having been dropped, and then
    waits a specified time before releasing the parachute. This sketch
    also performs several other tasks to make interacting with Laelaps
    more user friendly. These tasks are explained in following comments
*/

#include <Servo.h>

#define AKELA_CONNECTION 0
#define ARM_BUTTON 1
#define TOGGLE_BUTTON 2
#define PWM_OUT 3
#define ARMED_LED 4
#define DELAY 500           // This is the time between detaching from Akela and deploying the parachute in milliseconds


bool is_armed = false;      // Whether parachute will release when not attached to Akela
bool is_released;           // Whether the parachute is being held or released
bool in_countdown = false;  // Has voltage input been lost, but parachute not released yet?
long time_detached = -1;     // Time Laelaps was dropped. (or lost external power for other reason)

Servo parachute_servo;      // Creates servo "object"



void setup(){

    pinMode(AKELA_CONNECTION, INPUT);
    pinMode(ARM_BUTTON, INPUT);
    pinMode(TOGGLE_BUTTON, INPUT);
    pinMode(PWM_OUT, OUTPUT);
    pinMode(ARMED_LED, OUTPUT);

    parachute_servo.attach(PWM_OUT, 1000, 2000);    // Attach parachute servo to pin 3, set min and max pwm widths
    SerialUSB.begin(9600);
    close();
}


void loop(){
    // Every time the toggle button is pressed, change position of servo
    // Program is locked while button is being held
    if (digitalRead(TOGGLE_BUTTON) == HIGH) {
        if (is_released) {
            close();
        }
        else {
            release();
        }
        
        while (digitalRead(TOGGLE_BUTTON) == HIGH) {
            delay(1);
        }
    }

    // Every time the arm button is pressed toggle "armed" status
    // Program is locked while button is being held
    if (digitalRead(ARM_BUTTON) == HIGH) {
        if (is_armed) {
            is_armed = false;
        }
        else {
            is_armed = true;
        }
        
        while (digitalRead(TOGGLE_BUTTON) == HIGH) {
            delay(1);
        }
    }

    // If Laelaps is armed turn on indicator LED
    if (is_armed) {
        digitalWrite(ARMED_LED, HIGH);
    }
    else {
        digitalWrite(ARMED_LED, LOW);
    }

    // If voltage from Akela goes to zero, wait DELAY ms, then release parachute
    // Will only take effect if laelaps is armed. This is to prevent servo from
    // Opening before laelaps is attached to Akela
    if (digitalRead(AKELA_CONNECTION) == LOW && is_armed == true){
        // If this is the first time through loop that power has been lost, enter countdown
        if (!in_countdown){
            in_countdown = true;
            time_detached = millis();
        }
        if ((time_detached > 0) && (DELAY < millis() - time_detached)){
            release();
            in_countdown = false;
            is_armed = false;
        }
    }

    // If voltage from Akela is restored before parachute is released, abort parachute release
    // This is to prevent the parachute from being released in Akela if Laelaps looses connection
    // For a moment due to vibration, etc.
    if (digitalRead(AKELA_CONNECTION) == HIGH){
        in_countdown = false;
    }
}

// Release parachute
void release(){
    parachute_servo.writeMicroseconds(1000);
    SerialUSB.println("released");
    is_released = true;
}

// Move servo to closed position
void close(){
    parachute_servo.writeMicroseconds(2000);
    SerialUSB.println("closed");
    is_released = false;
}
