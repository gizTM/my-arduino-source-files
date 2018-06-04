/*

EasyNTPClient example: ArduinoEspWifiShield

This example shows the basic usage of the EasyNTPClient on an Arduino UNO with an ESP-01 (ESP8266) WiFi module.
The output is visible in the Serial Monitor at 9600 baud rate.

For more details see: https://github.com/aharshac/EasyNTPClient

An example by Claran Martis
https://www.collaborizm.com/profile/SJne7FcMg
 
*/


/*

Pin Connectiions

+--------+-----------------------------+
| ESP-01 | Connection                  |
+--------+-----------------------------+
| TXD    | Arduino D10                 |
+--------+-----------------------------+
| CH_PD  | External Power Supply +3.3V |
+--------+-----------------------------+
| RST    | Arduino Reset               |
+--------+-----------------------------+
| VCC    | External Power Supply +3.3V |
+--------+-----------------------------+
| RXD    | Arduino D11                 |
+--------+-----------------------------+
| GPIO0  | {None}                      |
+--------+-----------------------------+
| GPIO2  | {None}                      |
+--------+-----------------------------+
| GND    | Common GND                  |
+--------+-----------------------------+

*/

#include <SoftwareSerial.h>

#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <WiFiEspUdp.h>

#include <EasyNTPClient.h>


char ssid[] = "Somboon";            // your network SSID (name)
char password[] = "s4114258m";        // your network password
char ntpServer[] = "asia.pool.ntp.org";

const int pinEspRx = 10;  //  Esp Rx <----> Arduino Tx
const int pinEspTx = 11; //  Esp Tx <----> Arduino Rx
SoftwareSerial ESP8266(pinEspTx, pinEspRx);

WiFiEspUDP udp;
EasyNTPClient ntpClient(udp, ntpServer, ((7*60*60)+(0*60))); // IST = GMT + 7:00

void setup(){
  Serial.begin(115200);
  ESP8266.begin(115200);
  WiFi.init(&ESP8266);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  udp.begin(123);
}

void loop() {
  Serial.println(ntpClient.getUnixTime());
  
  delay(20000); // wait for 20 seconds before refreshing.
}
