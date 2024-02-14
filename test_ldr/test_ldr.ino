#define Analog  36  //Analog output of Light Sensor Module connected to A0 of BBFuino

//Global variable
int AnalogValue = 0;  //to store analog value

void setup() {
  //setup the input or output pin
  pinMode(Analog, INPUT);
}

void loop() {
  AnalogValue = analogRead(Analog);
  delay(10);
}