/**
 * Adapted from the AnalogInOutSerial sketch
 */

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // at my testing conditions my photosensor measured
  // 1kohm to 78kohm from full light to darkness
  // so by the formula Vout=Vin*(R2/(R1+R2))
  // where R2 is a resitor and the R1 the photoresistor
  // I decided R2 to be 20kohm would give a nice range and
  // therefore the forming voltage divider would feed
  // 4.76V to 1.02V into A0 from full brightness to darkness
  // corresponding to the values 974 to 208 read in A0
  // at default reference, and so we map those to the pin numbers
  outputValue = map(sensorValue, 208, 974, 2, 11);

  // print the results to the serial monitor:
  Serial.print("sensor = " );
  Serial.print(sensorValue);
  Serial.print("\t output = ");
  Serial.println(outputValue);

  for (int i = 2; i <= outputValue; i++)
    digitalWrite(i, HIGH);

  for (int i = outputValue + 1; i <= 11; i++)
    digitalWrite(i, LOW);

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);
}
