//Thank you commuity for your help,obviously i'm a beginner and I hope this helps you but really all I did was use two awesome libraries DHT and PubSubclient 
//then combine the two great examples DHT_TEST and PubSubclient example mqtt_esp8266 and alter how I packed up for publishing
//the tricky part for me was figuring out how to convert from a float supported by DHT.h to the string supported by PubSubclient see last section of code for that
//This will publish the temperature and humidity from your DHT sensor connected to your ESP8266 to an Mqtt broker, I am using Mosquitto.  

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"


DHT dht;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;

// Configure your network here

const char* ssid = "XXXXXX"; ///UPDATE with your network 
const char* password = "XXXXX"; ///Your password
const char* mqtt_server = "XXXXXX"; ///your URL or IP address

// Variables for printing temp and humidity

String temp_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;
char temp[50];
char hum[50];

//setting up Wifi didn't change this from the example PubSubclient example mqtt_esp8266

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()   
{
pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println();
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)");  //debug serial print

  
// Call out your data pin for your DHT sensor here

dht.setup(13); 

// 
}

//--(end setup )---


void loop()   
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
delay(dht.getMinimumSamplingPeriod());

//this is where you get the data from the sensor

  float humidity = dht.getHumidity(); 
  float temperature = dht.getTemperature();
  float ftemp = dht.toFahrenheit(temperature);

//Using Serial print to debug

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);

 
//counter for the messages, see if I am missing any on the Mqtt broker
  
  long now = millis(); 
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    
//Preparing for mqtt send

    temp_str = String(ftemp); //converting ftemp (the float variable above) to a string 
    temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...

    hum_str = String(humidity); //converting Humidity (the float variable above) to a string
    hum_str.toCharArray(hum, hum_str.length() + 1); //packaging up the data to publish to mqtt whoa...

    Serial.print("Publish message: ");//all of these Serial prints are to help with debuging
    
    client.publish("Workshop Temperature", temp); //money shot
    client.publish("Workshop Humidity", hum);
  }

}



