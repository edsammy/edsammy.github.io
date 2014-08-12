/*
NYSG Under Water Rover (ROV) Project
Code by: Eddie Samuels
Reviewed by: Matthew Liu
8/2014 
*/

//============================ Libraries =====================================//
#include <Servo.h>
  Servo camServo;
  Servo beaconServo;

//Adafruit's library for the MPL115A2 I2C pressure and temperature sensor   
#include <Adafruit_MPL115A2.h>
  Adafruit_MPL115A2 internalPressTemp;

//Adafruit_MPL115A2 library requires Wire.h for I2C communication with sensor
#include <Wire.h>

//============================ Definitions ===================================//
// Enables verbose Serial output, comment out to disable
//#define DEBUG

//CONSTANTS
const int NUMBER_OF_CONTROLS = 5;
const int NUMBER_OF_SENSORS = 4;
const float TIME_OUT = 1000;

//pinAssignments
const int leftMotorPin = 3;
const int rightMotorPin = 5;
const int zMotorEnablePin = 6;
const int zMotorPin1 = 11;
const int zMotorPin2 = 12;
const int camServoPin = 10;
const int beaconServoPin = 9;
const int servoEnablePin = 2;
const int sensor0Pin = A0;
const int sensor1Pin = A1;
const int sensor2Pin = A2;
const int sensor3Pin = A3;

//variables
int controls[NUMBER_OF_CONTROLS];
int sensors[NUMBER_OF_SENSORS];
char incomingChar;
int incomingInt;
int controlsIndex, sensorsIndex;
int leftMotorVal,rightMotorVal,zMotorVal,camServoVal,beaconServoVal;
boolean running = false; //Used to indicate if motors are running
boolean zNegative = false; //Used to recognize negative number for z motor
boolean errorReceived = false;
float timestamp;


//================================= Setup =====================================//

void setup(){
  //Begin serial communication at 9600 baud
  Serial.begin(9600);
  
  //Set all motor control pins as outputs
  pinMode(leftMotorPin, OUTPUT);
  pinMode(rightMotorPin, OUTPUT);
  pinMode(zMotorEnablePin, OUTPUT);
  pinMode(zMotorPin1, OUTPUT);
  pinMode(zMotorPin2, OUTPUT);
  pinMode(servoEnablePin, OUTPUT);
  
  
  //Make sure z motor hbridge is off
  digitalWrite(zMotorPin1, LOW);
  digitalWrite(zMotorPin2, LOW);
  
  //Setup servos
  digitalWrite(servoEnablePin, HIGH);
  camServo.attach(camServoPin);
  beaconServo.attach(beaconServoPin);
  delay(10);
  digitalWrite(servoEnablePin, LOW);
  
  //Initialize internal pressure & temperature sensor
  internalPressTemp.begin();
  
  //Set all sensor pins as inputs
  pinMode(sensor0Pin, INPUT);
  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);
  pinMode(sensor3Pin, INPUT);
}

//================================= Loop ======================================//
//Functions separated into communication, controls, and sensors (see tabs)

void loop() {
  //If there is data in the serial buffer read it
  if (Serial.available() > 0) {
    getCommands(); //(Communication)
  }
  //If there is no data in the serial buffer then check if motors need to timed out
  else {
    timeOutMotors(); //(Controls)
  }
}


//============================= Communication ==================================//
//Get serial controls from the controller as a data packet in the following format:
//leftMotorValue,RightMotorValue,ZMotorValue,CameraServoValue,BeaconServoValue
//and ends with a new line character (\n)
void getCommands() {
  timestamp = millis();
  
  incomingChar = Serial.read();
  
  //Check for negative numbers for z motor only (control field 2)
  if (controlsIndex == 2 && incomingChar == '-') {
    zNegative = true;
  }
  
  //Check if integers are being received
  else if (incomingChar >= '0' && incomingChar <= '9') {
    incomingInt = incomingChar - '0'; //char to int conversion: '0'=48, '1'=49,..., '9'=57 so subtract '0' to convert to int
    if (controlsIndex < NUMBER_OF_CONTROLS) {
      //Serial buffer only holds 1 digit at a time so multidigit numbers must be reassembled using the equation below
      controls[controlsIndex] = (controls[controlsIndex]*10) + incomingInt;
    } 
  }
  
  //Controls separated by commas so go to the next controls field after comma is received
  else if (incomingChar == ',') { 
    controlsIndex++;
  }
  
  //Control transmission is complete after a new line character is received
  else if (incomingChar == '\n') {
    if (errorReceived) {
      sendError();
    }
    //If there is no error and the controls data packet holds the correct number of fields...    
    else if (!errorReceived && controlsIndex == NUMBER_OF_CONTROLS-1) {
      //Pass the received controls to the motor control section
      checkCommands();
      
      //Read sensors and send values back to the controller
      readSensors();
      sendSensors();
      
      //Clear all variables to prepare for the next controls data packet
      clearSerial();
    }
    
    //If there is the wrong number of fields in the controls data packet send an error back to the controller
    else if (!errorReceived && controlsIndex != NUMBER_OF_CONTROLS-1) {
      sendError();
    }
  }
  //If a non-integer is received at any place in controls data packet, indicate an error
  else { 
    errorReceived = true;
  }
}


//Sends error back to the controller
void sendError(){
  Serial.println('E');
  clearSerial();
}


//Sends sensor packet to the controller
void sendSensors() {
  for (sensorsIndex = 0; sensorsIndex < NUMBER_OF_SENSORS; sensorsIndex++) {
    Serial.print(sensors[sensorsIndex]);
    if (sensorsIndex == NUMBER_OF_SENSORS - 1) {
      Serial.println();
    }
    else {
      Serial.print(',');
    }
  }
}


//Resets controls array, sensors array, and all variables
void clearSerial() {
  controlsIndex = 0;
  sensorsIndex = 0;
  zNegative = false;
  errorReceived = false;
  
  //void memset(void *str, int c, size_t n)
  //memset() fills '*str' with 'int c', 'size_t n' is size of '*str'
  memset(controls, 0, sizeof(controls));
  memset(sensors, 0, sizeof(sensors));
}


//================================ Controls ===================================//
//Runs if no serial commands have been received, turns all motors off after variable length TIME_OUT has expired
void timeOutMotors() {
  if (running && millis()-timestamp > TIME_OUT) {
    disableMotors();
    running = false;
  }
}


//Disable left, right, & Z motors using motor control functions
void disableMotors() {
  motorControl(leftMotorPin, 0);
  motorControl(rightMotorPin, 0);
  zMotorControl(0);
}


//Set commands array to correct variables and check that values are within the limits
//Arduino analogWrite() is limited between 0 and 255 http://arduino.cc/en/Reference/analogWrite
void checkCommands() {
  //Set "easy" variable names to controls array
  leftMotorVal = controls[0];
  rightMotorVal = controls[1];
  if (zNegative) zMotorVal = -1 * controls[2];
  else zMotorVal = controls[2];
  camServoVal = controls[3];
  beaconServoVal = controls[4];
  
  if (leftMotorVal > 255 || leftMotorVal < 0) {
    sendError();
  }
  
  else if (rightMotorVal > 255 || rightMotorVal < 0) {
    sendError();
  }
  
  else if (zMotorVal > 255 || zMotorVal < -255) {
    sendError();
  }
  
  //All commands are within the limits so pass commands to motors
  else {
    parseCommands();
  }
}


//Send commands to control functions
void parseCommands() {
  motorControl(leftMotorPin, leftMotorVal);
  motorControl(rightMotorPin, rightMotorVal);
  zMotorControl(zMotorVal);
  camServoControl(camServoVal);
  beaconServoControl(beaconServoVal);
  running = true;
}


//Left and right motor control
void motorControl(int pin, int val) {
  #ifdef DEBUG  
    Serial.print("Pin ");
    Serial.print(pin);
    Serial.print(" set to: ");
    Serial.println(val, DEC);
  #endif
  //Disable power to both servos to ensure they do not move; had issues without this
  digitalWrite(servoEnablePin, LOW);
  analogWrite(pin, val);
  delay(10);
}


//Z motor control using L298 H-bridge, positive values spin up, negative values spin down
void zMotorControl(int val) {
  //Disable power to both servos to ensure they do not move; had issues without this
  digitalWrite(servoEnablePin, LOW);
  int speedVal;
  if (val < 0) {
    //Set z motor to spin down mode
    digitalWrite(zMotorPin1, HIGH); 
    digitalWrite(zMotorPin2, LOW);
    delay(10); //Delay to let signal propagate through H-bridge
    speedVal = val * -1;
    analogWrite(zMotorEnablePin, speedVal);
  }
  
  else if (val > 0) {
    //Set z motor to spin up mode
    digitalWrite(zMotorPin1, LOW);
    digitalWrite(zMotorPin2, HIGH);    
    delay(10); //Delay to let signal propagate through H-bridge
    speedVal = val;
    analogWrite(zMotorEnablePin, speedVal);
  }

  else if (val == 0) {
    digitalWrite(zMotorPin1, LOW);
    digitalWrite(zMotorPin2, LOW);
    delay(10); //Delay to let signal propagate through H-bridge
  }
  #ifdef DEBUG
    Serial.print("Z-Motor set to: ");
    Serial.println(val);
  #endif
}

void camServoControl(int val) {
  //Only write to the servos if the value has changed from last time
  //This prevents servos from jittering...
  if(camServo.read() != val) {
    digitalWrite(servoEnablePin, HIGH);
    camServo.write(val);
  }
}


void beaconServoControl(int val) {  
  if(beaconServo.read() != val) {
    digitalWrite(servoEnablePin, HIGH);
    beaconServo.write(val);
  }
}



//================================ Sensors ===================================//
//Read sensor values and make any neccesary conversions
void readSensors() {
  sensors[0] = internalPressTemp.getPressure() * 0.145; //converts kPa to PSI
  sensors[1] = internalPressTemp.getTemperature() * 1.8 + 32; //converts degC to degF
  
  //No sensors are currently hooked up for sensor2 and 3, these are simply here as dummy sensors
  sensors[2] = analogRead(sensor2Pin);
  sensors[3] = analogRead(sensor3Pin);
}

