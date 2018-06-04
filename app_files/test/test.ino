//The code:
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int readkey;
void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Test push button..");
}
void loop() {
  lcd.setCursor(0, 1);
//  lcd.print("It doesn't?");
  readkey=analogRead(0);
  if (readkey<50) {
  // lcd.clear();
  lcd.print("Button Right");
  }
  else if(readkey<195) {
    // lcd.clear();
    lcd.print("Button Up");
  }
  else if(readkey<380) {
    // lcd.clear();
    lcd.print("Button Down");
  }
  else if(readkey<790) {
    // lcd.clear();
    lcd.print("Button Left");
  }
}
