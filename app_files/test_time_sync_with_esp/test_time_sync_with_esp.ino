/********************
- www.geekstips.com
- Arduino Time Sync from NTP Server using ESP8266 WiFi module
 ********************/
#include <SoftwareSerial.h>
#include <TimeLib.h>

// setup a commong baudrate for UNO and ESP
int baudRate = 115200;
char unixString[11];
long unixTime;
boolean dataSync = false;
// setup the ESP emulated serial channel
SoftwareSerial esp8266(0, 1);
SoftwareSerial serial1(2,3);

void setup() {
  serial1.begin(baudRate);
  esp8266.begin(baudRate);
}

void loop() {
  
  char buffer[40];
  int i = 0;

  // while the ESP output available, push it
  // into the buffer and set the flag true
  while (esp8266.available()) {
    buffer[i++] = esp8266.read();
    dataSync = true;
  }

  // if data is available, parse it
  if (dataSync == true) {
    if ((buffer[0] == 'U') && (buffer[1] == 'N') && (buffer[2] == 'X')) {
      // if data sent is the UNX token, take it
      unixString[0] = buffer[3];
      unixString[1] = buffer[4];
      unixString[2] = buffer[5];
      unixString[3] = buffer[6];
      unixString[4] = buffer[7];
      unixString[5] = buffer[8];
      unixString[6] = buffer[9];
      unixString[7] = buffer[10];
      unixString[8] = buffer[11];
      unixString[9] = buffer[12];
      unixString[10] = '\0';

      // print the UNX time on the UNO serial
      serial1.println();
      serial1.print("TIME FROM ESP: ");
      serial1.print(unixString[0]);
      serial1.print(unixString[1]);
      serial1.print(unixString[2]);
      serial1.print(unixString[3]);
      serial1.print(unixString[4]);
      serial1.print(unixString[5]);
      serial1.print(unixString[6]);
      serial1.print(unixString[7]);
      serial1.print(unixString[8]);
      serial1.print(unixString[9]);
      serial1.println();
      
      unixTime = atol(unixString);
      // Synchronize the time with the internal clock
      // for external use RTC.setTime();
      setTime(unixTime);
      dataSync = false;
    }
  }
}
