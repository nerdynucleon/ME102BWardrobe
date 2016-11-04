void setup()
{
  // start serial port at 9600 bps
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
}

char on_12 = 0;
char on_11 = 0;
char on_10 = 0;

void input_process(char c)
{
  if(c == 'q')
    on_12 = !on_12;
  if(c == 'w')
    on_11 = !on_11;
  if(c == 'e')
    on_10 = !on_10;
}

void output()
{
  digitalWrite(12, on_12);
  digitalWrite(11, on_11);
  digitalWrite(10, on_10);
}

void loop()
{
  char inByte;
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    input_process(inByte);
    output();
    Serial.print(inByte);               
  }
}
