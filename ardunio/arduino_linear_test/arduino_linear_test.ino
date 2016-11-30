
// motor one
int enA = 7;
int in1 = 6;
int in2 = 5;
// motor two
int enB = 2;
int in3 = 4;
int in4 = 3;

void setup()
{
  // set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop()
{
  // turn off actuator
  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  delay(10000);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);
  
  delay(10000);
  digitalWrite(enA, LOW);
  digitalWrite(enB, LOW);
  delay(10000);
  
  // now change motor directions
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);
  delay(10000);
}
