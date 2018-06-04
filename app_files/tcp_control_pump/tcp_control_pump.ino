#include<TEE_ESP_WIFI.h>
#include <SoftwareSerial.h>

#define pinEN 9
#define ESP_Rx 10
#define ESP_Tx 11
#define PUMP 7

String ssid="Somboon";
String pwd ="s4114258m";

ESP wifi(ESP_Rx,ESP_Tx,pinEN);

void print_debug(String data) {
  Serial.print(data);
}

void setup() {
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP,LOW); 
  Serial.begin(9600); 
  wifi.begin(9600);
  Serial.println("PUMP CONTROL BY ESP8226..."); 
  wifi.Event_debug = print_debug; 
  wifi.reset(); 
  wifi.setmode(STATION_AP); 
  if(wifi.setAP(ssid,pwd,1,WPA_WPA2_PSK)) {
    wifi.multipleconnect(MULTIPLE); 
    wifi.startserver("8000");
    String ip = wifi.myip();
  }
}

void loop() {
  String data = wifi.readstringdata(); 
  if(data.length()) {
    if(data=="1") {
      digitalWrite(PUMP,HIGH);
    }
    if(data=="0") digitalWrite(PUMP,LOW); 
    Serial.println(data);
  }
}