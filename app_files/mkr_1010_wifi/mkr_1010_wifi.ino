#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoOTA.h>

#include <OneWire.h>
#include <DallasTemperature.h>

/*
 Basic ESP8266 MQTT publish client example for a moisture sensor
*/
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "Somboon";
const char* password = "s4114258m";
//const char* mqtt_server = "192.168.1.35";
const char* mqtt_server = "broker.hivemq.com";

#define LIGHT 8
#define ONE_WIRE_BUS 5
#define WATER_LEVEL 4

WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WiFiServer server(80);            //server socket
WiFiClient wifiClient = server.available();

unsigned long lastMillis = 0;

float celcius = 0;
float fahrenheit = 0;
char celcius_char[50];

int water_level = 0;
char water_level_char[50];

void setup_wifi() {
  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  sensors.begin();
}

void reconnect() {
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("mqttdevice")) {
      Serial.println("connected");
      client.subscribe("/giz/#");
      // topic pattern: /giz/system/type/name
      // e.g. /giz/aquarium/sensor/temp
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void onMessage(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Incoming message: ");
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char*)payload;
  // Serial.println("[" + topic_str + "]: " + payload_str);
  if (topic_str.equals("/giz/device/spray")) {
    if (payload_str.equals("on")) {
      digitalWrite(LIGHT, HIGH); 
      Serial.println("light on!");
    } else if (payload_str.equals("off")) {
      digitalWrite(LIGHT, LOW);
      Serial.println("light off!");
    }
  }
}

void setup() {
  pinMode(LIGHT, OUTPUT);
  pinMode(WATER_LEVEL, INPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(onMessage);
  
  // start the WiFi OTA library with internal (flash) based storage
  ArduinoOTA.begin(WiFi.localIP(), "Arduino", "password", InternalStorage);

  server.begin();
}

void loop() {
  // check for WiFi OTA updates
//  ArduinoOTA.poll();
  
  char msg[10];
  char msgtext[25];
  String themsg;
 if (!client.connected()) {
   reconnect();
 } else {
   client.loop();
 }
 
 // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
//     Serial.println("publish /hello: world");
//     client.publish("/hello", "world"); //PUBLISH TO TOPIC /hello MSG world
  }

 sensors.requestTemperatures(); 
 celcius = sensors.getTempCByIndex(0);
 fahrenheit = sensors.toFahrenheit(celcius);

 String celcius_str = String(celcius);
 celcius_str.toCharArray(celcius_char, celcius_str.length() + 1);
  
 water_level = digitalRead(WATER_LEVEL);
 String water_level_str = String(water_level);
 water_level_str.toCharArray(water_level_char, water_level_str.length() + 1);
 Serial.println(water_level_char);

 client.publish("/giz/aquarium/sensor/waterlvl", water_level_char);
 client.publish("/giz/aquarium/sensor/temp", celcius_char);

 if (water_level == 1) {
   digitalWrite(6, HIGH);
   Serial.println("high");
 } else {
   digitalWrite(6, LOW);
   Serial.println("low");
 }

 delay(1000);
  // if (wifiClient) {                             // if you get a client,
  //   Serial.println("new client");           // print a message out the serial port
  //   String currentLine = "";                // make a String to hold incoming data from the client
  //   while (wifiClient.connected()) {            // loop while the client's connected
  //     if (wifiClient.available()) {             // if there's bytes to read from the client,
  //       char c = wifiClient.read();             // read a byte, then
  //       Serial.write(c);                    // print it out the serial monitor
  //     }
  //   }
  //   wifiClient.stop();
  //   Serial.println("client disconnected");
  // }
}
