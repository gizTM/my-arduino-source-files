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

#include <Wire.h>
#include <TFT_22_ILI9225.h>
#include <WiFiNINA.h>
#include <math.h> 
#include <stdio.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

#include "./fonts/FreeMono9pt7b.h"
#include "pH.h"
#include "EC.h"
#include "DO.h"
#include "Temperature.h"
#include "Util.h"

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

float celcius = 0;
float fahrenheit = 0;

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
 Serial.begin(115200);
 tft.begin();
 tft.setOrientation(1);
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(onMessage);
 Wire.begin();
//  delay(1);
 randomSeed(analogRead(0));
 pHObj.begin(); //read the slope and intercept of the ph probe
 ECObj.begin();
}

void loop() {
  char msg[10];
  char msgtext[25];
  String themsg;
  if (!client.connected()) {
    reconnect();
  } else {
    client.loop();
  }
  handlepH();
  handleEC();
  handleDO();
  handleTemperature();
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
    client.publish("/giz/aquarium/sensor/temp", celcius_char);
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
