/*
* file DFRobot_PH.ino
* @ https://github.com/DFRobot/DFRobot_PH
*
* This is the sample code for Gravity: Analog pH Sensor / Meter Kit V2, SKU:SEN0161-V2
* In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
* You can send commands in the serial monitor to execute the calibration.
* Serial Commands:
* enter -> enter the calibration mode
* cal -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
* exit -> save the calibrated parameters and exit from calibration mode
*
* Copyright [DFRobot](http://www.dfrobot.com), 2018
* Copyright GNU Lesser General Public License
*
* version V1.0
* date 2018-04
*/

#include "DFRobot_PH.h"
#include <EEPROM.h>

#include <OneWire.h>
#include <DallasTemperature.h>


#define PH_PIN A1

#define ONE_WIRE_BUS 5

float voltage,phValue,temperature = 25;
DFRobot_PH ph;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float celcius = 0;
float fahrenheit = 0;
char celcius_char[50];

void setup()
{
Serial.begin(115200);
ph.begin();
}

void loop()
{
static unsigned long timepoint = millis();
if(millis()-timepoint>1000U) //time interval: 1s
{
timepoint = millis();
voltage = analogRead(PH_PIN)/1024.0*5000; // read the voltage
temperature = readTemperature(); // read your temperature sensor to execute temperature compensation
phValue = ph.readPH(voltage,temperature); // convert voltage to pH with temperature compensation
Serial.print("temperature:");
Serial.print(temperature,1);
Serial.print("^C pH:");
Serial.println(phValue,2);
}
ph.calibration(voltage,temperature); // calibration process by Serail CMD
}

float readTemperature()
{
//add your code here to get the temperature from your temperature sensor

 sensors.requestTemperatures(); 
 celcius = sensors.getTempCByIndex(0);
 fahrenheit = sensors.toFahrenheit(celcius);

 String celcius_str = String(celcius);
 celcius_str.toCharArray(celcius_char, celcius_str.length() + 1);
 return celcius;
}
