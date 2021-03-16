/* I2C LCD with Arduino example code. More info: https://www.makerguides.com */

// Include the libraries:
// LiquidCrystal_I2C.h: https://github.com/johnrickman/LiquidCrystal_I2C
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 2); // Change to (0x27,16,2) for 16x2 LCD.

byte Heart[] = {B00000,B01010,B11111,B11111,B01110,B00100,B00000,B00000};
byte Bell[] = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};
byte Alien[] = {B11111,B10101,B11111,B11111,B01110,B01010,B11011,B00000};
byte Check[] = {B00000,B00001,B00011,B10110,B11100,B01000,B00000,B00000};
byte Speaker[] = {B00001,B00011,B01111,B01111,B01111,B00011,B00001,B00000};
byte Sound[] = {B00001,B00011,B00101,B01001,B01001,B01011,B11011,B11000};
byte Skull[] = {B00000,B01110,B10101,B11011,B01110,B01110,B00000,B00000};
byte Lock[] = {B01110,B10001,B10001,B11111,B11011,B11011,B11111,B00000};

signed short m, s;
char timeline[16];

void setup() {
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, Heart);
  lcd.createChar(1, Bell);
  lcd.createChar(2, Alien);
  lcd.createChar(3, Check);
  lcd.createChar(4, Speaker);
  lcd.createChar(5, Sound);
  lcd.createChar(6, Skull);
  lcd.createChar(7, Lock);
  
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Water next...");

//  lcd.setCursor(0, 1);
//  lcd.write(0);
//  lcd.setCursor(2, 1);
//  lcd.write(1);
//  lcd.setCursor(4, 1);
//  lcd.write(2);
//  lcd.setCursor(6, 1);
//  lcd.write(3);
//  lcd.setCursor(8, 1);
//  lcd.write(4);
//  lcd.setCursor(10, 1);
//  lcd.write(5);
//  lcd.setCursor(12, 1);
//  lcd.write(6);
//  lcd.setCursor(14, 1);
//  lcd.write(7);

  lcd.setCursor(1, 1);
  sprintf(timeline,"%0.2d mins %0.2d secs", m, s);
  lcd.print(timeline);
  
  delay(1000);
  s++;
  
  if (s == 60) {
    s = 0;
    m++;
  }
}
