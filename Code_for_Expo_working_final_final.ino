int rPin = 9;
int gPin = 10;
int bPin = 11;

void setup() {
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
}

void loop() {
  int r = 225;
  int g = 0;
  int b = 125;

  analogWrite(rPin, r);
  analogWrite(gPin, g);
  analogWrite(bPin, b);
}
