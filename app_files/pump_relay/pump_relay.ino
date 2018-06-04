int pump_trigger = 7;
void setup() {
  // put your setup code here, to run once:
  pinMode(pump_trigger,OUTPUT);
  // Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  // pump control
  digitalWrite(pump_trigger,1);
  delay(5000);
  digitalWrite(pump_trigger,0);
  delay(5000);
  // Serial.print("relay triggered\n");
}
