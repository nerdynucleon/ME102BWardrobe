/* Low Level Actuator Control for ME 102B Smart Wardrobe */
#define CLOCKSPEED_FUSES SPI_CLOCK_DIV64
#include <Stepper.h>

const int stepsPerRevolution = 200;// change this to fit the number of steps per revolution
                                     // for your motor
const int incomingByte = 0;

// small linear actuator
const int enA = 7;
const int in1 = 6;
const int in2 = 5;
// large linear actuator
const int enB = 2;
const int in3 = 4;
const int in4 = 12;
// enable stepper
const int enStep = 3;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

void setup() {
  // set the speed at 200 rpm:
  myStepper.setSpeed(200);
  
  // initialize the serial port:
  Serial.begin(9600);
  
  // Set Fan Speed on Stepper Driver Heatsink
  // pinMode(6, OUTPUT);
  // analogWrite(6, 120);
  
  // Initialize Pins for Linear Actuators
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enStep, OUTPUT);
  
  // Moderate Holding Torque
  analogWrite(enStep, 64);
}


/*
 Blocks on Serial Interface waiting for byte
*/
int blocking_read()
{
  while(Serial.available() <= 0) {}
  return Serial.read();
}

/*
 Blocks and waits for four bytes in ASCII form over serial interface
 Converts value to int and returns
*/
int serial_parse_int()
{
  int return_val = 0;
  while(Serial.available() < 4) {}
  return_val += (Serial.read() - 48) *1000;
  return_val += (Serial.read() - 48) *100;
  return_val += (Serial.read() - 48) *10;
  return_val += Serial.read() - 48;
  return return_val;
}


/*
 Parses Commands from high level controller and executes I/O
 Uses Serial Interface for communication and returns confirmation
*/
 
void loop() {
  char cmd = blocking_read();
  if (cmd == 'x')
  {
    int numberofsteps = 200 * serial_parse_int();
    blocking_read(); // new line char
    // Full Power
    analogWrite(enStep, 255);
    myStepper.step(numberofsteps);
    // Moderate Holding Torque
    analogWrite(enStep, 64);
    Serial.write("d\n"); 
  } 
  if (cmd == 'y')
  {
    cmd = blocking_read();
    blocking_read(); // new line char
    if (cmd == 'U')
    {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW); 
        digitalWrite(enA, HIGH);
        // Wait for Actuation to Complete (Open Loop)
        delay(6000);
        digitalWrite(enA, LOW);
    }
    if (cmd == 'D')
    {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH); 
        digitalWrite(enA, HIGH);
        // Wait for Actuation to Complete (Open Loop)
        delay(6000);
        digitalWrite(enA, LOW);
    }     
    Serial.write("d\n");
  }
  if (cmd == 'z')
  {
    cmd = blocking_read();
    blocking_read(); // new line char
    if (cmd == 'E')
    {
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH); 
        digitalWrite(enB, HIGH);
        // Wait for Actuation to Complete (Open Loop)
        delay(10000);
        digitalWrite(enB, LOW);
    } 
    if (cmd == 'R')
    {
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW); 
        digitalWrite(enB, HIGH);
        // Wait for Actuation to Complete (Open Loop)
        delay(10000);
        digitalWrite(enB, LOW);
    }    
    Serial.write("d\n");
  }
}


