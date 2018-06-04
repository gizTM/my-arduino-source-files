// import adafruit RTCLib library
// -------------------------------------------------------------
#include "RTClib.h" // https://github.com/adafruit/RTClib 
RTC_DS3231 rtc;
// -------------------------------------------------------------



// -------------------------------------------------------------
// NTP stuff
// Get Time from NTP server (Network Time Protocol, RFC 5905)
// NTP uses UDP/IP packets for data transfer because of the fast
// connection setup and response times.

// The official port number for the NTP (that ntpd and ntpdate listen and talk to) is 123.
// The unit of time is in seconds, and the epoch is 1 January 1900.
// its gives you UTC time, which is the time at Greenwich Meridian (GMT)
// The NTP timestamp is a 64 bit binary value, built from an unsigned 32-bit seconds value
// and a 32-bit fractional part. In this notation the value 3.5 would be represented by the 64-bit string:
// 0000|0000|0000|0000|0000|0000|0000|0011 . 1000|0000|0000|0000|0000|0000|0000|0000
// If you take all the bits as a 64 bit unsigned integer,
// store it in a floating point variable with at least 64 bits of mantissa (usually double)
// and do a floating point divide by 2^32, you'll get the right answer.
// On a standard arduino unfortunately we don't have 64 bits doubles.
// but most RTC have just second level info, so no need to get the second half (or first byte possibly)

// Only the first four bytes of an outgoing NTP packet need to be set for what we want to achieve
// appropriately, the rest can be whatever.
//The header fields of the NTP message are as follows:
//
//LI  Leap Indicator (2 bits)
//This field indicates whether the last minute of the current day is to have a leap second applied. The field values follow:
//0: No leap second adjustment
//1: Last minute of the day has 61 seconds
//2: Last minute of the day has 59 seconds
//3: Clock is unsynchronized
//
//VN  NTP Version Number (3 bits) (current version is 4).
//
//Mode  NTP packet mode (3 bits)
//The values of the Mode field follow:
//0: Reserved
//1: Symmetric active
//2: Symmetric passive
//3: Client
//4: Server
//5: Broadcast
//6: NTP control message
//7: Reserved for private use
//
//Stratum Stratum level of the time source (8 bits)
//The values of the Stratum field follow:
//0: Unspecified or invalid
//1: Primary server
//2–15: Secondary server
//16: Unsynchronized
//17–255: Reserved
//
//Poll  Poll interval (8-bit signed integer)
//The log2 value of the maximum interval between successive NTP messages, in seconds.
//
//Precision Clock precision (8-bit signed integer)
//The precision of the system clock, in log2 seconds.

static const unsigned long ntpFirstFourBytes = 0xEC0600E3; // NTP request header, first 32 bits
const uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message


// after a successful querry we will get a 48 byte answer from the NTP server.
// To understand the structure of an NTP querry and answer
// see http://www.cisco.com/c/en/us/about/press/internet-protocol-journal/back-issues/table-contents-58/154-ntp.html
// so if we want to read the "Transmit Timestamp" then we need to
// Read the integer part which are bytes 40,41,42,43
// if we want to round to the nearest second if we want some accuracy
// Then fractionary part are byte 44,45,46,47
// if it is greater than 500ms byte 44 will be > 128
// and thus by only checking byte 44 of the answer we can round to the next second;
// 90% of the NTP servers have network delays below 100ms
// We can also account for an assumed averaged network delay of 50ms, and thus instead of
// comparing with 128 you can compare with (0.5s - 0.05s) * 256 = 115;

#define SECONDROUNDINGTHRESHOLD 115

// the epoch for NTP starts in year 1900 while the epoch in UNIX starts in 1970
// Unix time starts on Jan 1 1970. 7O years difference in seconds, that's 2208988800 seconds
#define SEVENTYYEARS 2208988800UL

// to transform a number of seconds into a current time you need to do some maths
#define NUMBEROFSECONDSPERDAY 86400UL
#define NUMBEROFSECONDSPERHOUR 3600UL
#define NUMBEROFSECONDSPERMINUTE 60UL

// you might not leave in UTC time zone, set up your delta time in hours
#define UTC_DELTA_HOURS 2
#define UTC_DELTA_MINUTES 0
const long UTC_DELTA = ((UTC_DELTA_HOURS * NUMBEROFSECONDSPERHOUR) + (UTC_DELTA_MINUTES*NUMBEROFSECONDSPERMINUTE));

// MANAGE the ESP8266. Using an ESP - 01
// we control the reset pin through the arduino
#define hardRestPIN 22
#define SHORT_PAUSE 1000UL
#define LONG_PAUSE  5000UL
const char * OK_STR = "OK\r\n";


// Don't go faster than 38400 with sowftare serial for reliability
#define ESPSERIALBAUD 115200 // Set to whatever is the default for your ESP. 

// #define USESOFTWARESERIAL // uncomment if you want to use sowftare SERIAL

#ifdef USESOFTWARESERIAL
#include <SoftwareSerial.h>
#define ARDUINO_RX_PIN 10    // set here the pin number you use for software Serial Rx
#define ARDUINO_TX_PIN 11    // set here the pin number you use for software Serial Tx
SoftwareSerial esp01(ARDUINO_RX_PIN, ARDUINO_TX_PIN); // rxPin (the pin on which to receive serial data), txPin (the pin on which to transmit serial data)
#define ESPSEPRIAL esp01
#else
#define ESPSEPRIAL Serial // I'm using a Mega so I can use hardware ports. much more reliable. 
#endif

// =====================================================================================

// --------------------------------------
// emptyESP_RX waits for duration ms
// and get rid of anything arriving
// on the ESP Serial port during that delay
// --------------------------------------

void emptyESP_RX(unsigned long duration)
{
  unsigned long currentTime;
  currentTime = millis();
  while (millis() - currentTime <= duration) {
    if (ESPSEPRIAL.available() > 0) ESPSEPRIAL.read();
  }
}

// --------------------------------------
// waitForString wait max for duration ms
// while checking if endMarker string is received
// on the ESP Serial port
// returns a boolean stating if the marker
// was found
// --------------------------------------

boolean waitForString(const char * endMarker, unsigned long duration)
{
  int localBufferSize = strlen(endMarker); // we won't need an \0 at the end
  char localBuffer[localBufferSize];
  int index = 0;
  boolean endMarkerFound = false;
  unsigned long currentTime;

  memset(localBuffer, '\0', localBufferSize); // clear buffer

  currentTime = millis();
  while (millis() - currentTime <= duration) {
    if (ESPSEPRIAL.available() > 0) {
      if (index == localBufferSize) index = 0;
      localBuffer[index] = (uint8_t) ESPSEPRIAL.read();
      endMarkerFound = true;
      for (int i = 0; i < localBufferSize; i++) {
        if (localBuffer[(index + 1 + i) % localBufferSize] != endMarker[i]) {
          endMarkerFound = false;
          break;
        }
      }
      index++;
    }
    if (endMarkerFound) break;
  }
  return endMarkerFound;
}


// --------------------------------------
// espATCommand executes an AT commmand
// checks if endMarker string is received
// on the ESP Serial port for max duration ms
// returns a boolean stating if the marker
// was found
// --------------------------------------

boolean espATCommand(const char * command, const char * endMarker, unsigned long duration)
{
  ESPSEPRIAL.println(command);
  return waitForString(endMarker, duration);
}


// --------------------------------------
// epochUnixNTP returns the UNIX time
// number of seconds sice Jan 1 1970
// adjusted for timezoneDeltaFromUTC
// --------------------------------------

unsigned long epochUnixNTP()
{
  unsigned long secsSince1900 = 0UL;
  unsigned long epochUnix;

  // AT+CIPSTART="UDP","fr.pool.ntp.org",123
  espATCommand("AT+CIPSTART=\"UDP\",\"asia.pool.ntp.org\",123", OK_STR, LONG_PAUSE);
  espATCommand("AT+CIPSEND=48", OK_STR, SHORT_PAUSE); //send 48 (NTP_PACKET_SIZE) bytes
  emptyESP_RX(1000UL); // empty the buffer (we get a > character)
  ESPSEPRIAL.write((char*) &ntpFirstFourBytes, NTP_PACKET_SIZE); // the first 4 bytes matters, then we don't care, whatever is in the memory will do

  // skip  AT command answer ("Recv 48 bytes\r\n\r\nSEND OK\r\n\r\n+IPD,48:")
  waitForString(":", SHORT_PAUSE);

  // read the NTP packet, extract the TRANSMIT TIMESTAMP in Seconds from bytes 40,41,42,43
  for (int i = 0; i < NTP_PACKET_SIZE; i++) {
    while (!ESPSEPRIAL.available());
    int c = ESPSEPRIAL.read();
    if ((i >= 40) && (i < 44))  secsSince1900 = (secsSince1900 << 8) + (unsigned long) ((uint8_t) (c & (int) 0x00FF)); // Read the integer part of sending time
    else if (i == 44) secsSince1900 += (((uint8_t) c) > SECONDROUNDINGTHRESHOLD ? 1 : 0);
  }

  // subtract seventy years:
  epochUnix = secsSince1900 - SEVENTYYEARS;


  espATCommand("AT+CIPCLOSE", OK_STR, SHORT_PAUSE); // close connection
  return epochUnix + UTC_DELTA;
}

// =====================================================================================

void setup()
{
  Serial.begin(9600); while (!Serial);
  ESPSEPRIAL.begin(ESPSERIALBAUD); while (!ESPSEPRIAL);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1); // can't go further
  }


  //reset the device (pull the RST pin to ground)
  digitalWrite(hardRestPIN, LOW);
  delay(100);
  digitalWrite(hardRestPIN, HIGH);

  // connect to my WiFi network
  espATCommand("AT+RESTORE", "ready", LONG_PAUSE); // reset
  emptyESP_RX(SHORT_PAUSE);
  espATCommand("AT", OK_STR, SHORT_PAUSE); //is all OK?
  espATCommand("AT+CWMODE=1", OK_STR, SHORT_PAUSE); //Set the wireless mode
  espATCommand("AT+CWQAP", OK_STR, SHORT_PAUSE);   //disconnect  - it shouldn't be but just to make sure
  espATCommand("AT+CWJAP=\"Somboon\",\"s4114258m\"", "OK", LONG_PAUSE); // connect to wifi
  espATCommand("AT+CIPMUX=0", OK_STR, SHORT_PAUSE); //set the single connection mode
}


void loop()
{
  int c;
  unsigned long currentTime;
  static unsigned long lastTime = 0;
  unsigned long epochUnix;
  uint8_t  ntpHours, ntpMinutes, ntpSeconds;
  DateTime now;

  if (millis() - lastTime >= 5000UL) { // change 5000UL to adjust your RTC once a day use (NUMBEROFSECONDSPERDAY * 1000UL)
    now = rtc.now();
    epochUnix = epochUnixNTP();
    ntpHours = (epochUnix  % NUMBEROFSECONDSPERDAY) / NUMBEROFSECONDSPERHOUR;
    ntpMinutes = (epochUnix % NUMBEROFSECONDSPERHOUR) / NUMBEROFSECONDSPERMINUTE;
    ntpSeconds = epochUnix % NUMBEROFSECONDSPERMINUTE;

    // here you should actually check if now.hour() versus ntpHours are not way off. if so you might have been unlucky and caught midnight.
    // I'm not doing it - left to the reader :-)
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), ntpHours, ntpMinutes, ntpSeconds));
    now = rtc.now(); // get the adjusted time

    // print the hour, minute and second:
    Serial.print("The local NTP time is ");
    Serial.print(ntpHours); // print the hour ()
    Serial.print(':');
    if ( ntpMinutes < 10 ) Serial.print('0');    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print(ntpMinutes);
    Serial.print(':');
    if (ntpSeconds < 10) Serial.print('0');    // In the first 10 seconds of each minute, we'll want a leading '0'
    Serial.println(ntpSeconds); // print the second

    // print the hour, minute and second:
    Serial.print("The RTC time is ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" - ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    lastTime = millis();
  }

  // do something else here

}