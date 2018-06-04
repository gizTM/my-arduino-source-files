// const int pingPin = 13;
// int inPin = 12;

// void setup() {
// 	Serial.begin(9600);
// }

// void loop() {
// 	long duration, cm;
// 	pinMode(pingPin, OUTPUT);
// 	digitalWrite(pingPin, LOW);
// 	delayMicroseconds(2);
// 	digitalWrite(pingPin, HIGH);
// 	delayMicroseconds(5);
// 	digitalWrite(pingPin, LOW);
// 	pinMode(inPin, INPUT);
// 	duration = pulseIn(inPin, HIGH);

// 	cm = microsecondsToCentimeters(duration);
// 	Serial.print(cm);
// 	Serial.print("cm");
// 	Serial.println();
// 	delay(100);
// }

// long microsecondsToCentimeters(long microseconds) {
// 	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// 	// The ping travels out and back, so to find the distance of the
// 	// object we take half of the distance travelled.
// 	return microseconds / 29 / 2;
// }
// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------


#include <NewPing.h>
#include <MedianFilter.h>
#include <Wire.h>
#include <MedianFilter.h>

#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 450 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

MedianFilter filter(31,0);

void setup() {
  
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.

  
  
}

void loop() {
  
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int o,uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).

  filter.in(uS);
  o = filter.out();
  Serial.print("Ping: ");
  Serial.print( o / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");

  
}