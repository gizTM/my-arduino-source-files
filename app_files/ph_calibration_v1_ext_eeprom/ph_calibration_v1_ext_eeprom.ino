// For ph calibration stored on external EEPROM
/***************************************************
 This example uses software solution to calibration the ph meter,
not the potentiometer. So it is more easy to use and calibrate.
 This is for SEN0161 and SEN0169.

 Created 2016-8-11
 By youyou from DFrobot <youyou.yu@dfrobot.com>

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Troubleshooting***************
 1.Connection and Diagram can be found here http://www.dfrobot.co
m/wiki/index.php/PH_meter%28SKU:_SEN0161%29
 2.This code is tested on Arduino Uno.
 ****************************************************/

#include <WiFiUdp.h>
#include <NTPClient.h>
// #include <DateTime.h>
// #include <DateTimeStrings.h>
#include <Wire.h>
#include <TFT_22_ILI9225.h>
#include <FastLED.h>
#include <WiFiNINA.h>
#include <math.h> 
#include <stdio.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
// #include <CooperativeMultitasking.h>
// #include <MQTTClient.h>
#include <MQTT.h>

#include "./fonts/FreeMono9pt7b.h"
#include "pH.h"
#include "EC.h"
#include "DO.h"
#include "Temperature.h"
#include "Util.h"

const char* ssid = "Somboon";
const char* password = "s4114258m";
const char* mqtt_server = "192.168.1.47";
// const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_client_id = "mqttdevice";
const char* mqtt_username = "fanggiz";
const char* mqtt_password = "1mqtt;";

// const char* clientid] = " ";
// const char* topicname] = "/giz/#";

WiFiClient wifiClient;
// PubSubClient client(wifiClient);
MQTTClient client;

// CooperativeMultitasking tasks;
// MQTTClient mqttclient(&tasks, &wificlient, mqtt_server, mqtt_port, clientid, NULL, NULL);
// MQTTTopic topic(&mqttclient, topicname);



WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// NTPClient timeClient(ntpUDP, "pool.ntp.org", 60*60*24*7, 60000);

#define LIGHT 8
#define PRE_SETUP_PIN 1
#define LED 2
#define NUM_LEDS 27
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define ONE_WIRE_BUS 5
#define WATER_LEVEL 4

// r=32.34%
// g=14.01%
// b=52.66%
// |
// V
CRGB LED_COLOR = CRGB(83, 36, 134);
boolean LED_STATUS = false;
boolean LED_TRIGGER = true;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float celcius = 0;
float fahrenheit = 0;

#define debug_publish false

#define TFT_RST 6//A4
#define TFT_RS  7//A3
#define TFT_CS  3//4//A5  // SS
#define TFT_SDI 8//A2  // MOSI
#define TFT_CLK 9//A1  // SCK
#define TFT_LED 0   // 0 if wired to +5V directly

#define TFT_BRIGHTNESS 200 // Initial brightness of TFT backlight (optional)

#define ReceivedBufferLength 20
char receivedBuffer[ReceivedBufferLength+1]; // store the serialcommand
byte receivedBufferIndex = 0; 

#define SCOUNT 30 // sum of sample point
int pHAnalogBuffer[SCOUNT]; //store the sample voltage
int pHAnalogBufferIndex = 0;

int ECAnalogBuffer[SCOUNT]; //store the sample voltage
int ECAnalogBufferIndex = 0;

// ph
#define SlopeValueAddress 0 // (slope of the ph probe)store atthe beginning of the EEPROM. The slope is a float number,occupies4 bytes.
#define InterceptValueAddress (SlopeValueAddress+4)
float slopeValue, interceptValue, pHAverageVoltage, ECAverageVoltage;
boolean enterCalibrationFlag = 0;

#define pHSensorPin A1
#define ECSensorPin A2
#define VREF 5000 //for arduino uno, the ADC reference is the power(AVCC), that is 5000mV

double pHDouble, pHMaxDouble = -1, pHMinDouble = -1;
double ECDouble, ECMaxDouble = -1, ECMinDouble = -1;
double DODouble, DOMaxDouble = -1, DOMinDouble = -1;
double tempDouble, tempMaxDouble = -1, tempMinDouble = -1;

double pHX = 0, ECX = 0, DOX = 0, tempX = 0;

pH pHObj;
EC ECObj;
DO DOObj;
Temperature tempObj;

// Use hardware SPI (faster - on Uno: 13-SCK, 12-MISO, 11-MOSI)
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, TFT_CS, TFT_SDI, TFT_CLK, TFT_LED);

void setup() {
  pinMode(PRE_SETUP_PIN, OUTPUT);
  digitalWrite(PRE_SETUP_PIN, HIGH);
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED, COLOR_ORDER>(leds, NUM_LEDS);//.setCorrection(TypicalLEDStrip);
  // FastLED.setBrightness(BRIGHTNESS);
  turnOffLight();
  Serial.begin(115200);
  tft.begin();
  tft.setOrientation(1);
  setup_wifi();
  timeClient.begin();
  setup_mqtt();
  Wire.begin();
  digitalWrite(PRE_SETUP_PIN, LOW);
  //  delay(1);
  randomSeed(analogRead(0));
  pHObj.begin(); //read the slope and intercept of the ph probe
  ECObj.begin();
}

int ii = 0;

void loop() {
  timeClient.update();
  // Serial.print("Current time: ");
  // Serial.print(timeClient.getFormattedTime());
  // Serial.print(" > ");
  // timeClient.setTimeOffset(7*24*60*60);
  // Serial.print(timeClient.getDay());
  // Serial.print(" ");
  // Serial.println(timeClient.getFormattedTime());
  // timeClient.setTimeOffset(0);
  char msg[10];
  char msgtext[25];
  String themsg;
  check_mqtt();
  handlepH();
  handleEC();
  handleDO();
  handleTemperature();
  // if (LED_TRIGGER == true) {
  //   LED_TRIGGER = false;
    if (LED_STATUS == true) turnOnLight();
    else turnOffLight();
  // }
  ii++;
  delay(1000);
}

void turnOnLight() {
  Serial.println("light on!");
  for (int i = 0; i<NUM_LEDS; i+=1) { leds[i] = LED_COLOR; }
  FastLED.show();
}
void turnOffLight() {
  Serial.println("light off!");
  for (int i = 0; i<NUM_LEDS; i+=1) { leds[i] = CRGB::Black; }
  FastLED.show();
}

void handlepH() {
  static unsigned long sampleTimepoint = millis();
  if(millis() - sampleTimepoint > 40U) {
    sampleTimepoint = millis();
    pHAnalogBuffer[pHAnalogBufferIndex] = analogRead(pHSensorPin)/1024.0*VREF; //read the voltaage and store into the buffer,every 40ms
    pHAnalogBufferIndex++;
    if(pHAnalogBufferIndex == SCOUNT) pHAnalogBufferIndex = 0;
    pHAverageVoltage = getMedianNum(pHAnalogBuffer,SCOUNT); // readthe stable value by the median filtering algorithm
  }

  static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint>3000U) { // display every 5000ms = 5s
    printTimepoint = millis();
    pHDouble = pHObj.readValue(pHAverageVoltage, tempDouble==-127.0?25.0:tempDouble);
    // pHDouble = random(15);
    pHMaxDouble = max(pHMaxDouble==-1?pHDouble:pHMaxDouble, pHDouble);
    pHMinDouble = min(pHMinDouble==-1?pHDouble:pHMinDouble, pHDouble);
    if (pHObj.isQueueFull()) pHObj.popData();
    pHObj.pushData(pHDouble);
    pHObj.draw(tft, pHX, pHDouble, pHMinDouble, pHMaxDouble);
    String ph_str = String(pHDouble);
    char ph_char[50];
    ph_str.toCharArray(ph_char, ph_str.length() + 1);
    StaticJsonDocument<200> doc;
    doc["sensorId"] = "ph";
    doc["time"] = timeClient.getEpochTime();
    doc["value"] = pHDouble;
    String data = "";
    serializeJson(doc, data);
    char data_char[50];
    data.toCharArray(data_char, data.length() + 1);
    if (debug_publish) Serial.println("publish: pH: " + data);
    client.publish("/giz/aquarium/sensor/ph", data_char);
  }
  pHObj.calibration(pHAverageVoltage, tempDouble==-127.0?25.0:tempDouble);
}

void handleEC() {
  static unsigned long sampleTimepoint = millis();
  if(millis() - sampleTimepoint > 40U) {
    sampleTimepoint = millis();
    ECAnalogBuffer[ECAnalogBufferIndex] = analogRead(ECSensorPin)/1024.0*VREF; //read the voltaage and store into the buffer,every 40ms
    ECAnalogBufferIndex++;
    if(ECAnalogBufferIndex == SCOUNT) ECAnalogBufferIndex = 0;
    ECAverageVoltage = getMedianNum(ECAnalogBuffer,SCOUNT); // readthe stable value by the median filtering algorithm
  }

  static unsigned long printECTimepoint = millis();
  if(millis()-printECTimepoint>5000U) { // display every 5000ms = 5s
    printECTimepoint = millis();
    // ECAverageVoltage = analogRead(ECSensorPin)/1024.0*VREF;
    ECDouble = ECObj.readValue(ECAverageVoltage, tempDouble==-127.0?25.0:tempDouble);
    // ECDouble = random(15);
    ECMaxDouble = max(ECMaxDouble==-1?ECDouble:ECMaxDouble, ECDouble);
    ECMinDouble = min(ECMinDouble==-1?ECDouble:ECMinDouble, ECDouble);
    if (ECObj.isQueueFull()) ECObj.popData();
    ECObj.pushData(ECDouble);
    String ec_str = String(ECDouble);
    char ec_char[50];
    ec_str.toCharArray(ec_char, ec_str.length() + 1);
    StaticJsonDocument<200> doc;
    doc["sensorId"] = "ec";
    doc["time"] = timeClient.getEpochTime();
    doc["value"] = ECDouble;
    String data = "";
    serializeJson(doc, data);
    char data_char[50];
    data.toCharArray(data_char, data.length() + 1);
    if (debug_publish) Serial.println("publish: EC: " + data);
    client.publish("/giz/aquarium/sensor/ec", data_char);
    ECObj.draw(tft, ECX, ECDouble, ECMinDouble, ECMaxDouble);
  }
  ECObj.calibration(ECAverageVoltage, tempDouble==-127.0?25.0:tempDouble);
}

void handleDO() {
  static unsigned long printDOTimepoint = millis();
  if(millis()-printDOTimepoint>3000U) { // display every 4000ms = 5s
    printDOTimepoint = millis();
    DODouble = random(15);
    DOMaxDouble = max(DOMaxDouble==-1?DODouble:DOMaxDouble, DODouble);
    DOMinDouble = min(DOMinDouble==-1?DODouble:DOMinDouble, DODouble);
    if (DOObj.isQueueFull()) DOObj.popData();
    DOObj.pushData(DODouble);
    String do_str = String(DODouble);
    char do_char[50];
    do_str.toCharArray(do_char, do_str.length() + 1);
    StaticJsonDocument<200> doc;
    doc["sensorId"] = "do";
    doc["time"] = timeClient.getEpochTime();
    doc["value"] = DODouble;
    String data = "";
    serializeJson(doc, data);
    char data_char[50];
    data.toCharArray(data_char, data.length() + 1);
    if (debug_publish) Serial.println("publish: DO: " + data);
    client.publish("/giz/aquarium/sensor/do", data_char);
    DOObj.draw(tft, DOX, DODouble, DOMinDouble, DOMaxDouble);
  }
}

void handleTemperature() {
  static unsigned long printTemperatureTimepoint = millis();
  if(millis()-printTemperatureTimepoint>4000U) { // display every 4000ms = 5s
    printTemperatureTimepoint = millis();
    sensors.requestTemperatures(); 
    celcius = sensors.getTempCByIndex(0);
    // tempDouble = random(10, 45);
    tempDouble = celcius==-127.0?0:celcius;
    fahrenheit = sensors.toFahrenheit(celcius);
    String celcius_str = String(celcius);
    char celcius_char[50];
    celcius_str.toCharArray(celcius_char, celcius_str.length() + 1);
    StaticJsonDocument<200> doc;
    doc["sensorId"] = "temp";
    doc["time"] = timeClient.getEpochTime();
    doc["value"] = tempDouble;
    String data = "";
    serializeJson(doc, data);
    char data_char[50];
    data.toCharArray(data_char, data.length() + 1);
    if (debug_publish) Serial.println("publish: temp: " + data);
    client.publish("/giz/aquarium/sensor/temp", data_char);
    tempMaxDouble = max(tempMaxDouble==-1?tempDouble:tempMaxDouble, tempDouble);
    tempMinDouble = min(tempMinDouble==-1?tempDouble:tempMinDouble, tempDouble);
    if (tempObj.isQueueFull()) tempObj.popData();
    tempObj.pushData(tempDouble);
    tempObj.draw(tft, tempX, tempDouble, tempMinDouble, tempMaxDouble);
  }
}

void printHex(uint8_t num) {
  char hexCar[2];

  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}

void printUint16(uint16_t num) {
  char hexCar[4];

  sprintf(hexCar, "%04X", num);
  Serial.println(hexCar);
}

void printDouble(double value, String label) {
  Serial.print(label);
  char tmpStr[10];
  ftoa(value, tmpStr, 2);
  Serial.println(tmpStr);
}

String Format(double val, int dec, int dig ) {
  int addpad = 0;
  char sbuf[20];
  String condata = (dtostrf(val, dec, dig, sbuf));


  int slen = condata.length();
  for ( addpad = 1; addpad <= dec + dig - slen; addpad++) {
    condata = " " + condata;
  }
  return (condata);
}
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}

void blinkSetupStatus() {
  digitalWrite(PRE_SETUP_PIN, LOW);
  delay(500);
  digitalWrite(PRE_SETUP_PIN, HIGH);
  delay(500);
}

void setup_wifi() {
  // Connecting to a WiFi network
  tft.setFont(Terminal6x8);
  tft.drawText(10, 50, "Setting up wifi...", COLOR_WHITE);
  WiFi.begin(ssid, password);
  int wifiLoad = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiLoad += 1;
    tft.setFont(Terminal6x8);
    if (wifiLoad%3==0) tft.fillRectangle(10, 50, 13, 60, COLOR_BLACK);
    tft.drawText(10+wifiLoad, 50, ".", COLOR_WHITE);
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  tft.setFont(Terminal6x8);
  tft.drawText(10, 50, "Wifi connected.", COLOR_WHITE);
  tft.drawText(10, 60, "IP address: ", COLOR_WHITE);
  char ipStr[50];
  intToStr(WiFi.localIP(), ipStr, 0);
  tft.drawText(40, 60, ipStr, COLOR_WHITE);
  sensors.begin();
  delay(1000);
  tft.fillRectangle(10, 50, 100, 80, COLOR_BLACK);
}

void setup_mqtt() {
  // client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(onMessage);
  client.begin(mqtt_server, mqtt_port, wifiClient);
  client.onMessage(messageReceived);
  connect();
}
void connect() {  
  while (!client.connect("", mqtt_username, mqtt_password)) {
    Serial.print(".");
    delay(1000);
  }
}
void check_mqtt() {
  // if (!client.connected()) {
  //   reconnect();
  // } else {
  //   client.loop();
  // }
  client.loop();
  if (!client.connected()) {
    Serial.print("In reconnect MQTT...");
    blinkSetupStatus();
    connect();
    Serial.print(".");
  } else {
    Serial.println("connected");
    client.subscribe("/giz/#");
    digitalWrite(PRE_SETUP_PIN, LOW);
  }
}
void reconnect() {
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected()) {
    blinkSetupStatus();
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("/giz/#");
      // topic pattern: /giz/system/type/name
      // e.g. /giz/aquarium/sensor/temp
    } else {
      Serial.print("failed, rc=");
      // Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
  digitalWrite(PRE_SETUP_PIN, LOW);
}

void onMessage(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Incoming message: ");
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char*)payload;
  StaticJsonDocument<200> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload_str);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  String value = doc["value"];
  // Serial.println("[" + topic_str + "]: " + payload_str);
  if (topic_str.equals("/giz/aquarium/device/led")) {
    // Serial.println("[" + topic_str + "]: " + value);
    // LED_TRIGGER = true;
    if (value.equals("on")) {
      turnOnLight();
      LED_STATUS = true;
    } else {
      turnOffLight();
      LED_STATUS = false;
    }
  }
}

// |
// V
void messageReceived(String &topic, String &payload) {
 Serial.println("incoming: " + topic + " - " + payload);
//  payload[length] = '\0';
  // String topic_str = topic, payload_str = (char*)payload;
  StaticJsonDocument<200> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  String value = doc["value"];
  // Serial.println("[" + topic + "]: " + payload);
  if (topic.equals("/giz/aquarium/device/led")) {
    // Serial.println("[" + topic + "]: " + value);
    // LED_TRIGGER = true;
    if (value.equals("on")) {
      turnOnLight();
      LED_STATUS = true;
    } else {
      turnOffLight();
      LED_STATUS = false;
    }
  } else if (topic.equals("/giz/aquarium/device/led/rgb")) {
    // u_int8_t r = value["r"]; 
    // u_int8_t g = value["g"]; 
    // u_int8_t b = value["b"];
    // LED_COLOR = CRGB(r, g, b);
    StaticJsonDocument<200> rgbDoc;
    // Deserialize the JSON document
    DeserializationError errorRgb = deserializeJson(rgbDoc, value);
    // Test if parsing succeeds.
    if (errorRgb) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(errorRgb.f_str());
      return;
    }
    char r[10]; char g[10]; char b[10];
    intToStr(rgbDoc["r"], r, 0); intToStr(rgbDoc["g"], g, 0); intToStr(rgbDoc["b"], b, 0);
    LED_COLOR = CRGB(rgbDoc["r"], rgbDoc["g"], rgbDoc["b"]);
  }
}