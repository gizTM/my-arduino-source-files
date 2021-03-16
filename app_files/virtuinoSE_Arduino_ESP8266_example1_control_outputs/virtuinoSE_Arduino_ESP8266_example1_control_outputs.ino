/* VirtuinoSE example: Enable or disable the led of pin 13
 * Supported boards: Arduino Uno or Nano + ESP8266 module 
 * Created by Ilias Lamprou
 * Updated Oct 27 2018
 * More examples: http://virtuino.com/index.php/virtuino-se/20-virtuino-se-code-examples
 */
// ------ VirtuinoSE V Memory Table
//  V13 memory -> Pin 13   digital output Relay1


//--------------- USER SETTINGS ---------------------------------------
const char* ssid = "WIFI NETWORK";          // WIFI network SSID or AP SSID
const char* password = "PASSWORD";          // WIFI network PASSWORD

#define VIRTUINO_KEY  "1234"                // Virtuino KEY, only the requests that include this key will be acceptable
                                            // disable the line if you don't want to use a key                          

#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin

int port =8000;
const char* serverIP = "192.168.1.150";       // The three first numbers have to be the same with the router IP

unsigned long timeStrored=0;
boolean debug=true;

//============================================================== connectToWiFiNetwork
void connectToWiFiNetwork(){
    Serial.println("Connecting to "+String(ssid));
    clearESP_buffer(1000);
    espSerial.println("AT+GMR");
    waitForResponse("OK",1000);
    
    espSerial.println("AT+CWMODE=1");  // configure as client
    waitForResponse("OK",1000);
    
    //--- connect
    espSerial.print("AT+CWJAP=\"");
    espSerial.print(ssid);
    espSerial.print("\",\"");
    espSerial.print(password);
    espSerial.println("\"");
    waitForResponse("OK",10000);

    espSerial.print("AT+CIPSTA=\"");
    espSerial.print(serverIP);
    espSerial.println("\"");   
    waitForResponse("OK",5000);

    espSerial.println("AT+CIPSTA?");
    waitForResponse("OK",3000); 
    
    espSerial.println("AT+CIFSR");           // get ip address
    waitForResponse("OK",1000);
    
    espSerial.println("AT+CIPMUX=1");         // configure for multiple connections   
    waitForResponse("OK",1000);
    
    espSerial.print("AT+CIPSERVER=1,");
    espSerial.println(port);
    waitForResponse("OK",1000);
   
}

//=================================================================
void initAccessPoint(){
      if (debug) Serial.println( "Init as AP...");
      clearESP_buffer(1000);
      espSerial.println("AT+GMR");
      waitForResponse("OK",1000);

      espSerial.println("AT+CWMODE=2");  // configure as client
      waitForResponse("OK",1000);
    
     espSerial.println("AT+CIFSR");
     waitForResponse("OK",1000);

     espSerial.print("AT+CWSAP=\"");
     espSerial.print(ssid);
     espSerial.print("\",\"");
     espSerial.print(password);
     espSerial.println("\",2,3");
     waitForResponse("OK",5000);
     
     espSerial.println("AT+CIPMUX=1");         // configure for multiple connections   
     waitForResponse("OK",2000);
    
     espSerial.print("AT+CIPSERVER=1,");
     espSerial.println(port);
     waitForResponse("OK",1000);           
}


//============================================================== setup
//==============================================================
void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);       // You have to set the baud rate of the ESP8266 to 9600 before 
  connectToWiFiNetwork();      //enable this line to connect the module your wifi network
  //initAccessPoint();         //enable this line to use the esp as AP  
  
  pinMode(13,OUTPUT);        
  }

//============================================================== onCommandReceived
//==============================================================
/* This function is called every time VirtuinoSE app sends a request to server 
 * The 'variableIndex' is the V memory of Virtuino app
 * The 'valueAsText' is the value that has sent from the app
 * If the 'valueAsText" is ? you have to return the value that you want to be displayed on Virtuino panel
 */
 
String onCommandReceived(int variableIndex, String valueAsText){
  if (debug) Serial.println("New command: variableIndex="+String(variableIndex)+"   valueAsText="+valueAsText);
   
  //-------- Example:  enable or disable the board pin 13
  if (variableIndex==13){                     // in this example Virtuino V13 memory contains the state of pin 13
      if (valueAsText=="?") {
        return String(digitalRead(13));       // return the state of pin 13 as text
      }
       else if (valueAsText=="1") digitalWrite(13,HIGH);          // change the state of pin 13 as text
        else if (valueAsText=="0") digitalWrite(13,LOW);
   }

  /* //-------- Example:  enable or disable the board pin 12
  if (variableIndex==12){                     // in this example Virtuino V12 memory contains the state of pin 12
      if (valueAsText=="?") {
        return String(digitalRead(12));       // return the state of pin 12 as text
      }
       else if (valueAsText=="1") digitalWrite(12,HIGH);          // change the state of pin 12 as text
        else if (valueAsText=="0") digitalWrite(12,LOW);
   }
   */

   
  return "";
}


//============================================================== loop
//==============================================================
void loop() {
  virtuinoRun();
  //--- enter your loop code below
  


}












//================== VirtuinoSE Library (included in the sketch)=============
//=============================================================================
//=============================================================================

/* Only the next two functions are nedded to communicate with Virtuino 
 * Avoid to make changes to the code below 
 */


#define WRONG_KEY_MESSAGE  "Wrong Key"  
#define WELLCOME_MESSAGE  "Hello Virtuino"  

//============================================================== virtuinoRun
//==============================================================
void virtuinoRun(){
     if(espSerial.available()){
       delay(1);
       String espBuffer = espSerial.readStringUntil('\r');
          if (debug) Serial.print(espBuffer);
          int pos = espBuffer.indexOf("+IPD,");
          if (pos>=0){                                        // check for GET command from Virtuino app
              clearESP_buffer(100);                           // can be higher than 100
              int connectionId = espBuffer.charAt(pos+5)-48;  // get connection ID
              pos = espBuffer.indexOf("GET /");
              if (pos!=-1){                                   // We have a GET message
                   espBuffer.remove(0,pos+5);                 // Clear the characters GET / 
                   int k= espBuffer.indexOf(" HTTP/");       
                   if (k>0) espBuffer.remove(k);                       // Clear the tail of the line
                   String responseData = getResponseData(&espBuffer);  // The espBuffer contains the Virtuino data
                                                                       
                    // Create HTTP Header and data
                    String httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: ";
                    httpResponse += responseData.length();
                    httpResponse += "\r\nConnection: close\r\n\r\n";
                    httpResponse += responseData;
                    httpResponse += (" ");
                    //---Send response to Virtuino SE
                    String cipSend = "AT+CIPSEND=";
                    cipSend += connectionId;
                    cipSend += ",";
                    cipSend += httpResponse.length();
                    cipSend += "\r\n";
                    for (int i=0;i<cipSend.length();i++) espSerial.write(cipSend.charAt(i));
                    if (waitForResponse(">",2000)) {
                        for (int i=0;i<httpResponse.length();i++) {
                            espSerial.write(httpResponse.charAt(i));
                            if (debug) Serial.print(httpResponse.charAt(i));
                        }
                    }
              }
          }
     }

}

//============================================================== prepareResponse
//==============================================================
String getResponseData(String* commands){
   
   //---check the KEY
   #ifdef VIRTUINO_KEY 
      String virtuinoKey="";
      int keyPosStart = commands->indexOf("&KEY=");
      if (keyPosStart!=-1) {
        int keyPosEnd = commands->indexOf('&',keyPosStart+5);
        if (keyPosEnd!=-1) virtuinoKey= commands->substring(keyPosStart+5,keyPosEnd);
        else virtuinoKey= commands->substring(keyPosStart);
      }
      if (!virtuinoKey.equals(VIRTUINO_KEY)) return WRONG_KEY_MESSAGE;
   #endif
   
  //--- read all virtuino commands from request data
  String commandResponse="";
  boolean checkNext=true;
  int cStart=-1;
  while (checkNext){
    cStart=commands->indexOf('&',cStart+1);
    if (cStart!=-1) {
      int cEnd=commands->indexOf('&',cStart+1);
      String oneCommand="";
      if (cEnd<0) oneCommand=commands->substring(cStart+1); 
      else oneCommand=commands->substring(cStart+1,cEnd); 
      if (debug) Serial.println("cmd: "+oneCommand);
      char commandType = oneCommand.charAt(0);
      if (commandType=='C') return "&CNT="+String(WELLCOME_MESSAGE);               // Virtuino sends the command &C=? to check the connection status   
      else if ((commandType=='V') || (commandType=='T')) {
        int equalCharPosition = oneCommand.indexOf('=');
        int variableIndex  = oneCommand.substring(1,equalCharPosition).toInt(); 
        String valueString = oneCommand.substring(equalCharPosition+1); 
        String returnedValue = onCommandReceived(variableIndex,valueString);  
       if (returnedValue.length()>0) {
          commandResponse+='&';
          commandResponse+=commandType;
          commandResponse+=String(variableIndex);
          commandResponse+='=';
          commandResponse+=returnedValue;
        }
      }
    }
    else checkNext=false;   // the last command has checked
  }
  if (debug) Serial.println("Response="+commandResponse );
   if (commandResponse.length()==0) commandResponse="OK";       
  return commandResponse;
}

//======================================================================================== clearESP_buffer
//========================================================================================
void clearESP_buffer(int timeout){
    long t=millis();
    char c;
    while (t+timeout>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (debug) Serial.print(c);
      }
    }
   }

//======================================================================================== waitForResponse
//========================================================================================
boolean waitForResponse(String target1,  int timeout){
    String data;
    char a;
    unsigned long start = millis();
    boolean rValue=false;
    while (millis() - start < timeout) {
        while(espSerial.available() > 0) {
            a = espSerial.read();
            if (debug) Serial.print(a);
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            rValue=true;
            break;
        } 
    }
    return rValue;
}
  
//----------------------------------------------------------------------------
/* Virtuino GET request to upload values of V1 and V4 memories to board  (KEY=1234)
 *  GET /&KEY=1234&V1=12.3&V4=1
 *  Response: OK 
 *  
 * Virtuino GET request to read values from board (KEY=1234)
 *  GET /&KEY=1234&V2=?&V3=?
 *  Response: &V2=123&V3=1
 *  
 *  * Virtuino GET request to upload and read values from board (KEY=1234)
 *  GET /&KEY=1234&V2=45.6&V3=?
 *  Response: &V3=1
 *  
 *  Virtuino GET request to check the connection  (KEY=1234)
 *  GET /&KEY=1234&CNT=?
 *  Response: &CNT=Hello Virtuino I am the ESP8266 board
 *  
 * Virtuino GET request to upload value as TEXT from memory V1 to board  (KEY=1234)
 *  GET /&KEY=1234&T1=Hello
 *  Response: OK 

 * Virtuino GET request to read value a TEXT from board  (KEY=1234)
 *  GET /&KEY=1234&T1=?
 *  Response &T1=the text you want to send 
 *  or
 *  GET /&KEY=1234&V1=?
 *  Response &V1=the text you want to send 
 *  
 *  All the texts that are sent from Virtuino are encoded 
 *  All the texts that received from Virtuino will be decoded
 */



