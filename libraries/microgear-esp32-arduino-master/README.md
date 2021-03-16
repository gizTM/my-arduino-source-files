# microgear-esp32-arduino

microgear-esp32-arduino  is a client library that is used to connect an ESP32 chip to the NETPIE Platform's service for developing IoT applications. For more details on the NETPIE Platform, please visit https://netpie.io .

## Compatibility

We have tested this library and found it compatible with (but not limited to) the following hardware
- ESP32 Dev Board

## Outgoing Network Port

Make sure ther following ports are allowed to connect from your network.
- Non-TLS mode : 8080 and 1883 (the library uses this mode by default)
- TLS mode : 8081 and 8883 (still under testing)
