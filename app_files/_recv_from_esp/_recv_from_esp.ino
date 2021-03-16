#include <SoftwareSerial.h>
#include <Wire.h>

// #define EC 5
#define PUMP 7
#define LED 8
#define SPRAY 9
#define BTN 2

#define DATESIZE 10
#define TIMESIZE 8
#define ROUGHTIMESIZE 5

int timer = 0;
int value = 0;
int isEspAvailable = 0;

// We'll use a software serial interface to connect to ESP8266
const byte rxPin = 10; // Wire this to Tx Pin of ESP8266
const byte txPin = 11; // Wire this to Rx Pin of ESP8266
SoftwareSerial ESP8266 (rxPin, txPin);
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

void LED_blink(int d) {
	digitalWrite(LED,HIGH);
	delay(d);
	digitalWrite(LED,LOW);
	delay(d);
}

void test_blink(int d) {
  int c = 1;
  if (d > 0) c = 2;
  for (int i=0; i<c; i++) {
    digitalWrite(5,HIGH);
    delay(100);
    digitalWrite(5,LOW);
    delay(100);
  }
}

int readBTN() {
  int reading = digitalRead(BTN);
  if (reading != lastButtonState) lastDebounceTime = millis();
  if (debounceState) {
    debounceState = LOW;
    buttonState = LOW;
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (reading == HIGH) debounceState = HIGH;
    }
  }
  if (debounceState) digitalWrite(RESET_LED,HIGH);
  else digitalWrite(RESET_LED,LOW);
  return debounceState;
}

void startSpray(){
  if (digitalRead(SPRAY) == HIGH) {
    digitalWrite(SPRAY,LOW);
    Serial.println("start spray!!!"); 
  }
}

void stopSpray() {
  if (digitalRead(SPRAY) == LOW) {
    digitalWrite(SPRAY,HIGH);
    Serial.println("stop spray!!!"); 
  }
}

void startPump(){
	if (digitalRead(PUMP) == HIGH) {
		digitalWrite(PUMP,LOW);
		Serial.println("start pump!!!"); 
	}
}

void stopPump() {
	if (digitalRead(PUMP) == LOW) {
		digitalWrite(PUMP,HIGH);
		Serial.println("stop pump!!!"); 
	}
}

void handleTogglePump() {
	ESP8266.println((digitalRead(PUMP)==HIGH)?"DPM OFF":"DPM ON");
}

void handleMonitorPump() {
  ESP8266.println((digitalRead(PUMP)==HIGH)?"PMP ON":"PMP OFF");
}

void handleMonitorSpray() {
  ESP8266.println((digitalRead(SPRAY)==HIGH)?"PMS ON":"PMS OFF");
}

void setup() {
	// pinMode(EC, INPUT);
	pinMode(PUMP, OUTPUT);
  pinMode(SPRAY, OUTPUT);
	pinMode(LED, OUTPUT);
  pinMode(5, OUTPUT);
	digitalWrite(PUMP,HIGH);
	Serial.begin(115200);	// serial monitor baudrate
	ESP8266.begin(115200); 	// ESP8266 baudrate
	delay(1000); // Let the module self-initialize
}

void loop() {
	// while (ESP8266.available()){
	String dataType = "",line="";
  if (isEspAvailable != ESP8266.available()) {
    isEspAvailable = ESP8266.available();
    test_blink(isEspAvailable);
    Serial.print("isEspAvailable: "); Serial.println(isEspAvailable);
  }
	if(ESP8266.available()) {
		LED_blink(100);
    Serial.println('esp available');
		line = ESP8266.readStringUntil('\n');
    String dataType = line.substring(0, 3);
    Serial.print('from esp: '); Serial.println(line);
		if (dataType.equalsIgnoreCase("DDT")) {
			String roughTime = line.substring(4,9);
			Serial.print('roughTime: '); Serial.println(roughTime);
     
			// control pump when timer is met
			if(roughTime.equalsIgnoreCase("07:00")) startPump();
			if(roughTime.equalsIgnoreCase("07:15")) stopPump();

			if(roughTime.equalsIgnoreCase("12:00")) startPump();
			if(roughTime.equalsIgnoreCase("12:15")) stopPump();
     
			if(roughTime.equalsIgnoreCase("19:00")) startPump();
			if(roughTime.equalsIgnoreCase("19:15")) stopPump();

     
      if(roughTime.equalsIgnoreCase("7:00")) startSpray();
      if(roughTime.equalsIgnoreCase("7:01")) stopSpray();
     
      if(roughTime.equalsIgnoreCase("19:00")) startSpray();
      if(roughTime.equalsIgnoreCase("19:01")) stopSpray();
		}
   
		if (dataType.equalsIgnoreCase("DPM")) {
			Serial.println("pump control");
			if (line.substring(4,6).equalsIgnoreCase("on")) startPump();
			if (line.substring(4,7).equalsIgnoreCase("off")) stopPump();
		}
	}
//	handleTogglePump();
  handleMonitorPump();
  handleMonitorSpray();
}
