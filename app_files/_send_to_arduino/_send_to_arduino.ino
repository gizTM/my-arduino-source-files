#include <TimeLib.h>
#include "WifiConfig.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <WiFiUdp.h>

#ifndef WIFI_CONFIG_H
#define YOUR_WIFI_SSID "Somboon"
#define YOUR_WIFI_PASSWD "s4114258m"
#endif // !WIFI_CONFIG_H

#define APPID   "HydroNet"
#define KEY "nfd68H1XcIGwoo4"
#define SECRET "lZWJRzMSIrfOuiDt4C68VKjlb"
#define ALIAS   "esp8266"

WiFiClient client;

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 25200, 5000);

int timer = 0;

MicroGear microgear(client);

void setup () {
    Serial.begin (115200);
    Serial.println ();
    WiFi.mode (WIFI_STA);
    WiFi.disconnect();
    WiFi.begin (YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print('.');
    }
    Serial.println("WIFI ON");
    timeClient.begin();
    
    // microgear
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(PRESENT,onFoundgear);
    microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);

    microgear.init(KEY,SECRET,ALIAS);

    microgear.connect(APPID);
}

void loop () {
    static int i = 0;
    static int last = 0;
    if ((millis () - last) > 5100) {
        last = millis ();
        Serial.print("DDT ");
        timeClient.update();
        Serial.print(timeClient.getFormattedTime());
        Serial.print(WiFi.isConnected () ? " (wifi connected)" : " (wifi not connected)"); Serial.println (". ");
    }
    delay (100);

    /* To check if the microgear is still connected */
    if (microgear.connected()) {
//        Serial.println("microgear connected");
        /* Call this method regularly otherwise the connection may be lost */
        microgear.loop();
        microgear.chat("android",Serial.readStringUntil('\n'));
        
    } else {
      Serial.println("microgear connection lost, reconnect...");
      if (timer >= 5) {
        microgear.connect(APPID);
        timer = 0;
      }
      else timer += 1;
    }
    delay(100);
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.printf("Incoming topic = %s, msg = %s \r\n", topic, msg);
    Serial.println((char *)msg);
    String cmd = String((char*)msg);
    if (cmd == "on") Serial.println("DPM ON");
    if (cmd == "off") Serial.println("DPM OFF");
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Found new member --> ");
    for (int i=0; i<msglen; i++) Serial.print((char)msg[i]);
    Serial.println();  
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Lost member --> ");
    for (int i=0; i<msglen; i++) Serial.print((char)msg[i]);
    Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}
