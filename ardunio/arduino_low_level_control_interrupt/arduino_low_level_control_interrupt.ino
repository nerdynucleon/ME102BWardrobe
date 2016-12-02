/* Low Level Actuator Control for ME 102B Smart Wardrobe */
#include <Stepper.h>

const int stepsPerRevolution = 200;// change this to fit the number of steps per revolution
// for your motor
const int incomingByte = 0;

// small linear actuator
const int enY = 7;
const int in1 = 6;
const int in2 = 5;
// large linear actuator
const int enZ = 2;
const int in3 = 4;
const int in4 = 3;
// enYble stepper
const int enStep = 12;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

void setup() {
  // set the speed at 200 rpm:
  myStepper.setSpeed(200);

  // initialize the serial port:
  Serial.begin(9600);

  cli(); // Disable's Interrupts

  // Set TIMER 1 Interrupt 1 Hz
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  // Set compare match registers for 1hz period
  OCR1A = 15624;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // set CS12 and CS10 bits for 1024 prescalar
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enYble timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); // enable Interrupts  

  // Initialize Pins for Linear Actuators
  pinMode(enY, OUTPUT);
  pinMode(enZ, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enStep, OUTPUT);

  // Holding Torque Off
  digitalWrite(enStep, LOW);
}

/*
 Blocks on Serial Interface waiting for byte
 */
int blocking_read()
{
  while(Serial.available() <= 0) {
  }
  return Serial.read();
}

/*
 Blocks and waits for four bytes in ASCII form over serial interface
 Converts value to int and returns
 */
int serial_parse_int()
{
  int return_val = 0;
  int multiplier = 1;
  while(Serial.available() < 4) {
  }
  if (Serial.read() == 45)
    multiplier = -1;
  return_val += (Serial.read() - 48) *100;
  return_val += (Serial.read() - 48) *10;
  return_val += Serial.read() - 48;
  return return_val * multiplier;
}

// Timer Interrupt for Y,Z Actuation
volatile int yTimer = 0;
volatile int zTimer = 0;

ISR(TIMER1_COMPA_vect)
{
  if(yTimer == 0)
  {
    digitalWrite(enY, LOW);
  }
  if(zTimer == 0)
  {
    digitalWrite(enZ, LOW);
  }
  zTimer = zTimer > 0 ? zTimer - 1 : 0;
  yTimer = yTimer > 0 ? yTimer - 1 : 0;
}

/*
 Parses Commands from high level controller and executes I/O
 Uses Serial Interface for communication and returns confirmation
 */

void loop() {
  char cmd = blocking_read();
  
  // Actuation in X Direction
  if (cmd == 'x')
  {
    int numberofsteps = 5 * serial_parse_int();
    blocking_read(); // new line char
    // Full Power
    digitalWrite(enStep, HIGH);
    myStepper.step(numberofsteps);
    // Hold torque for a second
    delay(500);
    digitalWrite(enStep, LOW);
    Serial.write("d\n"); 
  }
  // Actuation in Y Direction
  else if (cmd == 'y')
  {
    cmd = blocking_read();
    char valid = (cmd == 'U' || cmd == 'D');
    
    // check if user wants this I/O to block
    char block = blocking_read();
    if (block != 'B')
    {
      block = 0;
    }
    else
    {
      block = 1;
      blocking_read(); // read new line char
    }
    
    // Configure Direction of Motion
    if (cmd == 'U')
    {
      // Move Up
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }
    else if (cmd == 'D')
    {
      // Move Down
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH); 
    }
    
    // Perform Actuation
    if(valid)
    {
      // Set Timer and Enable
      cli(); // Disable's Interrupts
      yTimer = 8; // 8 seconds of y actuation
      digitalWrite(enY, HIGH);
      sei(); // enable Interrupts
      if(block)
      {
        delay(7000);
      }
      Serial.write("d\n"); 
    }
  }
  // Actuation in Z Direction
  else if (cmd == 'z')
  {
    cmd = blocking_read();
    char valid = (cmd == 'E' || cmd == 'R');
    
    // check if user wants this I/O to block
    char block = blocking_read();
    if (block != 'B')
    {
      block = 0;
    }
    else
    {
      block = 1;
      blocking_read(); // read new line char
    }
    
    if (cmd == 'E')
    {
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
    } 
    else if (cmd == 'R')
    {
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
    }
    
    if (valid)
    {
      // Set Timer and Enable
      cli(); // Disable's Interrupts
      zTimer = 7; // 7 seconds of z actuation
      digitalWrite(enZ, HIGH);
      sei(); // enable Interrupts
        
      if(block)
      {
        delay(6000);
      }
      Serial.write("d\n");
    }
  }
}



