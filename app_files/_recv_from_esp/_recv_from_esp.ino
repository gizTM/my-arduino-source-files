#include <SoftwareSerial.h>

#include <NewPing.h>
#include <MedianFilter.h>
#include <Wire.h>
#include <MedianFilter.h>


// #define EC 5
#define PUMP 7
#define LED 8

#define DATESIZE 10
#define TIMESIZE 8
#define ROUGHTIMESIZE 5

#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 450 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

MedianFilter filter(31,0);

int timer = 0;

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

void setup() {
	// pinMode(EC, INPUT);
	pinMode(PUMP, OUTPUT);
	pinMode(LED, OUTPUT);
	digitalWrite(PUMP,HIGH);
	Serial.begin(115200);	// serial monitor baudrate
	ESP8266.begin(115200); 	// ESP8266 baudrate
	delay(1000); // Let the module self-initialize
}

void loop() {
	// while (ESP8266.available()){
	String dataType = "",line="";
	if(ESP8266.available()) {
		LED_blink(100);
		// int sensorValue = analogRead(A5);
		// float voltage= sensorValue * (5.0 / 1023.0);
		// Serial.print("EC value: ");
		// Serial.println(sensorValue);
		line = ESP8266.readStringUntil('\n');
		Serial.println(line);
		dataType = line.substring(0,3);
		// newData = true;
	// }
	// recvDataFromESP();
	// showNewData();
	// if(newData == true) {
		// Serial.println("newData:"+newData);
		// char* dataType = substr(receivedChars,0,3);
		// if (dataType[0]=='D' && dataType[1]=='D' && dataType[2]=='T') {
		if (dataType.equalsIgnoreCase("DDT")) {
			// char* roughTime = substr(receivedChars,4,9);
			String roughTime = line.substring(4,9);
			Serial.println(roughTime);
			// control pump when timer is met
			// if (roughTime[0]=='0'&&roughTime[1]=='7'&&roughTime[3]=='0'&&roughTime[4]=='0') 
			if(roughTime.equalsIgnoreCase("07:00"))
				startPump();
			// if (roughTime[0]=='0'&&roughTime[1]=='7'&&roughTime[3]=='1'&&roughTime[4]=='5') 
			if(roughTime.equalsIgnoreCase("07:15"))
				stopPump();

			// if (roughTime[0]=='1'&&roughTime[1]=='2'&&roughTime[3]=='0'&&roughTime[4]=='0') 
			if(roughTime.equalsIgnoreCase("12:00"))
				startPump();
			// if (roughTime[0]=='1'&&roughTime[1]=='2'&&roughTime[3]=='1'&&roughTime[4]=='5') 
			if(roughTime.equalsIgnoreCase("12:15"))
				stopPump();

			// if (roughTime[0]=='1'&&roughTime[1]=='9'&&roughTime[3]=='0'&&roughTime[4]=='0') 
			if(roughTime.equalsIgnoreCase("19:00"))
				startPump();
			// if (roughTime[0]=='1'&&roughTime[1]=='9'&&roughTime[3]=='1'&&roughTime[4]=='5') 
			if(roughTime.equalsIgnoreCase("19:15"))
				stopPump();
		}
		// if (dataType[0]=='D' && dataType[1]=='P' && dataType[2]=='M') {
		if (dataType.equalsIgnoreCase("DPM")) {
			// LED_blink(50);
			Serial.println("pump control");
			// char* cmd = substr(receivedChars,4,6);
			// Serial.println('\''+cmd+'\'');
			// if (cmd[0]=='o'&&cmd[1]=='n') startPump();
			// if (cmd[0]=='o'&&cmd[1]=='f') stopPump();
			if (line.substring(4,6).equalsIgnoreCase("on")) startPump();
			if (line.substring(4,7).equalsIgnoreCase("off")) stopPump();
		}
	}
	delay(100);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
	int o,uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
	filter.in(uS);
	o = filter.out();
	// Serial.println(o/US_ROUNDTRIP_CM);
	Serial.print("Ping: ");
	Serial.print(43- o / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
	Serial.println("cm");
	// if(timer>=5) {
		// 43=0 35=7 29=14 - 8(7),6(7)
		ESP8266.print("Ping: "); ESP8266.println(43-o/US_ROUNDTRIP_CM);
		// timer = 0;
	// } else timer+=1;
	// }
		handleTogglePump();
}