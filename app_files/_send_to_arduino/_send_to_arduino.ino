#include <TimeLib.h>
#include "WifiConfig.h"
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>
#include <MicroGear.h>

#ifndef WIFI_CONFIG_H
#define YOUR_WIFI_SSID "Somboon"
#define YOUR_WIFI_PASSWD "s4114258m"
#endif // !WIFI_CONFIG_H

#define ONBOARDLED 2 // Built in LED on ESP-12/ESP-07

int8_t timeZone = 7;
int8_t minutesTimeZone = 0;
bool wifiFirstConnected = false;

boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

#define APPID   "HydroNet"
#define KEY "nfd68H1XcIGwoo4"
#define SECRET "lZWJRzMSIrfOuiDt4C68VKjlb"
#define ALIAS   "esp8266"

WiFiClient client;

int timer = 0;
MicroGear microgear(client);

void setup () {
    static WiFiEventHandler e1, e2, e3;
    Serial.begin (115200);
    Serial.println ();
    WiFi.mode (WIFI_STA);
    WiFi.begin (YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);

    pinMode (ONBOARDLED, OUTPUT); // Onboard LED
    digitalWrite (ONBOARDLED, HIGH); // Switch off LED

    NTP.onNTPSyncEvent ([](NTPSyncEvent_t event) {
        ntpEvent = event;
        syncEventTriggered = true;
    });

    // Deprecated
    /*WiFi.onEvent([](WiFiEvent_t e) {
        Serial.printf("Event wifi -----> %d\n", e);
    });*/

    e1 = WiFi.onStationModeGotIP (onSTAGotIP);// As soon WiFi is connected, start NTP Client
    e2 = WiFi.onStationModeDisconnected (onSTADisconnected);
    e3 = WiFi.onStationModeConnected (onSTAConnected);

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

    if (wifiFirstConnected) {
        wifiFirstConnected = false;
        NTP.begin ("asia.pool.ntp.org", timeZone, true, minutesTimeZone);
        NTP.setInterval (63);
    }

    if (syncEventTriggered) {
        processSyncEvent (ntpEvent);
        syncEventTriggered = false;
    }

    if ((millis () - last) > 5100) {
        //Serial.println(millis() - last);
        last = millis ();
        // Serial.print (i); Serial.print (" ");
        Serial.print("DDT ");
        Serial.print(NTP.getTimeDateString ()); Serial.println();
        // Serial.print (NTP.isSummerTime () ? "Summer Time. " : "Winter Time. ");
        // Serial.print ("WiFi is ");
        // Serial.print (WiFi.isConnected () ? "connected" : "not connected"); Serial.print (". ");
        // Serial.print ("Uptime: ");
        // Serial.print (NTP.getUptimeString ()); Serial.print (" since ");
        // Serial.println (NTP.getTimeDateString (NTP.getFirstSync ()).c_str ());
        // i++;
    }
    delay (100);

    /* To check if the microgear is still connected */
    if (microgear.connected()) {
        // Serial.println("connected");
        /* Call this method regularly otherwise the connection may be lost */
        microgear.loop();

        // if (timer >= 100) {
            // Serial.println("Publish...");
            // microgear.chat(ALIAS,"Hello");
            // microgear.chat("android","hello from esp8266");
            // microgear.subscribe("test");
            microgear.chat("android",Serial.readStringUntil('\n'));
            // timer = 0;
        // } else timer += 100;
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5) {
            microgear.connect(APPID);
            timer = 0;
        }
        else timer += 1;
    }
    delay(100);
}

void onSTAConnected (WiFiEventStationModeConnected ipInfo) {
    Serial.printf ("Connected to %s\r\n", ipInfo.ssid.c_str ());
}

// Start NTP only after IP network is connected
void onSTAGotIP (WiFiEventStationModeGotIP ipInfo) {
    Serial.printf ("Got IP: %s\r\n", ipInfo.ip.toString ().c_str ());
    Serial.printf ("Connected: %s\r\n", WiFi.status () == WL_CONNECTED ? "yes" : "no");
    digitalWrite (ONBOARDLED, LOW); // Turn on LED
    wifiFirstConnected = true;
}

// Manage network disconnection
void onSTADisconnected (WiFiEventStationModeDisconnected event_info) {
    Serial.printf ("Disconnected from SSID: %s\n", event_info.ssid.c_str ());
    Serial.printf ("Reason: %d\n", event_info.reason);
    digitalWrite (ONBOARDLED, HIGH); // Turn off LED
    //NTP.stop(); // NTP sync can be disabled to avoid sync errors
}

void processSyncEvent (NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print ("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println ("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println ("Invalid NTP server address");
    } else {
        Serial.print ("Got NTP time: ");
        Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    // Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    // Serial.printf("Incoming topic = %s, msg = %s \r\n", topic, msg);
    // Serial.println((char *)msg);
    String cmd = String((char*)msg);
    // Serial.println(cmd);
    if (cmd == "on") Serial.println("DPM ON"); Serial.println();
    if (cmd == "off") Serial.println("DPM OFF"); Serial.println();
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