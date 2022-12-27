#define BUTTON_PIN 2 // Pin for the open/close toggle button.
#define AIN1 3 // Pin 1 for Motor A
#define AIN2 4 // Pin 2 for Motor A
#define PWMA 5 // PWM Pin for Motor A. Must be an analog pin
#define STBY 6 // Standby pin for HBridge
#define PWMIN 9 // Input PWM from pixhawk

#define MOTOR_DURATION 3000  // Duration to run motors after command


// Whether or not the drop controller is currently open or closed.
bool isOpen;

// Value of last read of pwm input pin
long last_pulse_length = -1;
// Value of current read of pwm input pin
long this_pulse_length = -1;

long timer;
long prev_time;

// Motor direction constants, change these to alter motor direction
const int offsetA = 1;
const int offsetB = -1;

/* Initialize servos, serial port, and pins. */
void setup() {

  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(PWMIN, INPUT);
  digitalWrite(STBY, HIGH);
  
  // Must be done after servos are set up
  close();
  Serial.println("set_up");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (isOpen) {
      close();
    } else {
      open();
    }

    while (digitalRead(BUTTON_PIN) == HIGH) {
      delay(1);
    }
  }

// Code to read and respond to PWM goes here
  if (digitalRead(PWMIN) == LOW) {
    this_pulse_length = (pulseIn(PWMIN, HIGH));
    }

  if (((this_pulse_length - 300) > last_pulse_length) && (last_pulse_length > 0) && (this_pulse_length > 1800)) {
    Serial.println("opened");
    open();
  }
  else if (((this_pulse_length + 300) < last_pulse_length) && (this_pulse_length < 1200))  {
    close();
    Serial.println("closed");
  }
  last_pulse_length = this_pulse_length;


// open and close only start the process. This code stops the motors after some time 
  if (timer > 0 && timer < millis() - prev_time) {
    timer = -1;
    // Stop motors
    brake(AIN1, AIN2, PWMA, STBY);
    digitalWrite(STBY, LOW);
  }
}

/* Open drop servos. */
int open() {
  drive(AIN1, AIN2, PWMA, STBY, 1);
  isOpen = true;
  prev_time = millis();
  timer = MOTOR_DURATION;
}

/* Close drop servos. */
int close() {
  drive(AIN1, AIN2, PWMA, STBY, -1);
  isOpen = false;
  prev_time = millis();
  timer = MOTOR_DURATION;
}

void drive(int in1, int in2, int pwm, int stdby, int dir) {
  digitalWrite(stdby, HIGH);
  if (dir > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  analogWrite(pwm, 255);
}

void brake(int in1, int in2, int pwm, int stdby) {
  digitalWrite(stdby, HIGH);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, HIGH);
  analogWrite(pwm, 0);
}
