ESP8266 firmware with NTP AT commands

builded on SDK 1.2.0 for 1024 MB Flash

sources at ESP8266_SDK/ntp subdirrectory;

binaries: ESP8266_SDK/bin subdirrectory:

eagle.flash.bin-------->0x00000

eagle.irom0text.bin---->0x40000

Commands:

AT+NTP - Initialization of NTP (GMT+0)

AT+NTP? - print (GMT+0) time in text kind and 
seconds since 1 Jan 1970 between (....) brackets for machine parsing,
if error, return (0).
