#include <LiquidCrystal.h>

#define SPRAY 3
#define PUMP 11
#define WATER_LEVEL 13

//LCD pin to Arduino Example By ArduinoAll
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

byte customChar[] = {B00000,B00000,B01110,B10101,B11101,B10001,B01110,B00000};
byte arrow[] = {B00000,B00100,B00010,B11111,B00010,B00100,B00000,B00000};
byte wait[] = {B01110,B01010,B01010,B00100,B00100,B01010,B01010,B01110};
byte water[] = {B00100,B00100,B01110,B01110,B11111,B11111,B11111,B01110};
byte updown[] = {B00100,B01110,B11111,B00000,B00000,B11111,B01110,B00100};
byte updown_invert[] = {B11011,B10001,B00000,B11111,B11111,B00000,B10001,B11011};
byte centerdot[] = {B00000,B00100,B01110,B11111,B11111,B01110,B00100,B00000};

int test = 0;
//
signed short sprayH = 11, sprayM = 59, sprayS = 59,
  pumpH = 11, pumpM = 59, pumpS = 59,
  h, m, s;
//signed short sprayH = 0, sprayM = 0, sprayS = 10,
//  pumpH = 0, pumpM = 0, pumpS = 10,
//  h, m, s;

int constSprayT = 60, sprayTimeout = 60,/* 1min */ constPumpT = 900, pumpTimeout = 900; /* 15min */
signed short savedSprayH = sprayH - (constSprayT/3600), savedSprayM = sprayM - (constSprayT/60), savedSprayS = sprayS - (constSprayT%60),
  savedPumpH = pumpH - (constPumpT/3600), savedPumpM = pumpM - (constPumpT/60), savedPumpS = pumpS - (constPumpT%60);

char sprayTl[16], pumpTl[16];
int mode = 0, prevMode = 0;
int pressedKey;

bool spraying = false, pumping = false;

int currentCursor[2] = {0, 0};

String selectedTimer;

void setup() {
  pinMode(SPRAY, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(WATER_LEVEL, INPUT);
  digitalWrite(SPRAY, HIGH);
  digitalWrite(PUMP, HIGH);
  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
  lcd.createChar(1, arrow);
  lcd.createChar(2, water);
  lcd.createChar(3, updown);
  lcd.createChar(4, updown_invert);
  lcd.createChar(5, centerdot);
  lcd.setCursor(1, 0);
  lcd.print("Watering timer");
  lcd.setCursor(6, 1);
  lcd.write(2); lcd.write(2); lcd.write(2);
  delay(1000);
  lcd.clear();
  Serial.begin(115200);
}
//
//void testKey() {
//  int prevPress = pressedKey;
//  int readKey = analogRead(0);
//  pressedKey = readButton(readKey);
//  if (prevPress != pressedKey) lcd.clear();
//
//  char text[16];
//  sprintf(text, "key: %d -> %0.2d", readKey, pressedKey);
//  lcd.setCursor(0, 0);
//  lcd.print(text);
////  lcd.print("key: ");
////  lcd.setCursor(0, 5);
////  lcd.write(button);
//}


void startSpray(){
  if (digitalRead(SPRAY) == HIGH && digitalRead(WATER_LEVEL) == HIGH) {
    digitalWrite(SPRAY,LOW);
    Serial.println("start spray!!!"); 
  }
}

void stopSpray() {
//  if (digitalRead(SPRAY) == LOW) {
    digitalWrite(SPRAY,HIGH);
    Serial.println("stop spray!!!"); 
//  }
}

void startPump(){
  if (digitalRead(PUMP) == HIGH && digitalRead(WATER_LEVEL) == HIGH) {
    digitalWrite(PUMP,LOW);
    Serial.println("start pump!!!"); 
  }
}

void stopPump() {
//  if (digitalRead(PUMP) == LOW) {
    digitalWrite(PUMP,HIGH);
    Serial.println("stop pump!!!"); 
//  }
}

void setStartSpray() { 
  sprayS = 0; 
  startSpray(); 
  spraying = true; 
}

void setStartPump() {
  pumpS = 0; 
  startPump(); 
  pumping = true; 
}

void setStopSpray() {
  spraying = false;
  sprayTimeout = constSprayT;
  stopSpray();
  lcd.setCursor(7, 1);
  lcd.print(" ");
}

void setStopPump() {
  pumping = false;
  pumpTimeout = constPumpT;
  stopPump();
  lcd.setCursor(7, 1);
  lcd.print(" ");
}

void countDown() {
  delay(1000);
  Serial.println("countdown");
  if (sprayH >= 0 && sprayM >= 0 && sprayS >= 0) {
    if (sprayS == 0) {
      if (sprayM > 0) { sprayS = 60; sprayM--; }
      else if (sprayH > 0) { sprayS = 60; sprayM = 59; sprayH--; }
      else setStartSpray();
    }
    if (sprayM < 0) {
      if (sprayH > 0) { sprayM = 59; sprayH--; }
    }
    if (sprayH < 0) sprayH = 0;
    sprayS--;
    if (sprayS < 0) sprayS = 0;
  }
  if (pumpH >= 0 && pumpM >= 0 && pumpS >= 0) {
    if (pumpS == 0) {
      if (pumpM > 0) { pumpS = 60; pumpM--; }
      else if (pumpH > 0) { pumpS = 60; pumpM = 59; pumpH--; }
      else setStartPump();
    }
    if (pumpM < 0) {
      if (pumpH > 0) { pumpM = 59; pumpH--; }
    }
    if (pumpH < 0) pumpH = 0;
    pumpS--;
    if (pumpS < 0) pumpS = 0;
  }
}

void displayOnlyTime(signed short sH, signed short sM, signed short sS, signed short pH, signed short pM, signed short pS, int sel = -1) {
  if (sel == 0 || sel == -1) {
    lcd.setCursor(8, 0);
    sprintf(sprayTl,"%0.2d:%0.2d:%0.2d", sH, sM, sS);
    lcd.print(sprayTl);
    Serial.print("Spray time: ");
    Serial.println(sprayTl);
  }
  if (sel == 1 || sel == -1) {
    lcd.setCursor(8, 1);
    sprintf(pumpTl,"%0.2d:%0.2d:%0.2d", pH, pM, pS);
    lcd.print(pumpTl);
    Serial.print("Pump time: ");
    Serial.println(pumpTl);
  }
}

void displayTime(signed short sH, signed short sM, signed short sS, signed short pH, signed short pM, signed short pS) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Spray");
  lcd.setCursor(6, 0);
  lcd.write(1);
  
  lcd.setCursor(0, 1);
  lcd.print("Pump");
  lcd.setCursor(6, 1);
  lcd.write(1);

  displayOnlyTime(sH, sM, sS, pH, pM, pS);
}

void displaySpraying() {
  lcd.setCursor(7, 0);
  // blinking water icon
//  if (sprayTimeout % 2 == 1) lcd.write(2);
//  else lcd.print(" ");
  lcd.write(2);
  sprayTimeout--;
  if (sprayTimeout < 0) {
    displayOnlyTime(0, 0, 0,
      pumpTimeout/3600, pumpTimeout/60, pumpTimeout%60, 0);
    setStopSpray();
    sprayH = savedSprayH;
    sprayM = savedSprayM;
    sprayS = savedSprayS;
  } else {
    displayOnlyTime(sprayTimeout/3600, sprayTimeout/60, sprayTimeout%60,
      pumpTimeout/3600, pumpTimeout/60, pumpTimeout%60, 0);
  }
}
void displayPumping() {
  lcd.setCursor(7, 1);
  // blinking water icon
//  if (pumpTimeout % 2 == 1) lcd.write(2);
//  else lcd.print(" ");
  lcd.write(2);
  pumpTimeout--;
  if (pumpTimeout < 0) {
    displayOnlyTime(sprayTimeout/3600, sprayTimeout/60, sprayTimeout%60,
      0, 0, 0, 1);
    setStopPump();
    pumpH = savedPumpH;
    pumpM = savedPumpM;
    pumpS = savedPumpS;
  } else {
    displayOnlyTime(sprayTimeout/3600, sprayTimeout/60, sprayTimeout%60, 
      pumpTimeout/3600, pumpTimeout/60, pumpTimeout%60, 1);
  }
}

int readButton(int x) {
  // 0 -- 60 -- 200 -- 400 -- 600 -- 800
  //   R     U       D      L     S
  //     ^  3
  //     |
  // 1 <---> 2
  //     |
  //     v  4
  if (x < 60) return 2;       // R
  else if (x < 200) return 3; // U
  else if (x < 400) return 4; // D
  else if (x < 600) return 1; // L
  else if (x < 800) return 0; // S
  return -1;
}

void printSetTime(bool setTime = false) {
  lcd.setCursor(0, 1);
  char timeout[16];
  signed short hh = setTime == true ? h : selectedTimer.equals("pump") ? pumpH : sprayH,
    mm = setTime == true ? m : selectedTimer.equals("pump") ? pumpM : sprayM,
    ss = setTime == true ? s : selectedTimer.equals("pump") ? pumpS : sprayS;
  sprintf(timeout, "%0.2d:%0.2d:%0.2d", hh, mm, ss);
  lcd.print(timeout);
}

void enterMode(int m) {
  mode = m;
  lcd.clear();
  switch (mode) {
    case 1:
      lcd.noBlink();
      currentCursor[0] = 0;
      lcd.setCursor(0, 0);
      lcd.print("Select timer..."); 
      lcd.setCursor(0, 1);
      lcd.write(1); 
      lcd.setCursor(1, 1);
      lcd.print("Spray"); 
      lcd.setCursor(8, 1);
      lcd.print("Pump"); 
      lcd.setCursor(14, 1);
      lcd.print("No"); 
      break;
     case 2:
      lcd.setCursor(0, 0);
      lcd.print(selectedTimer + " again in...");
      printSetTime();
      lcd.setCursor(10, 1);
      lcd.print("Set");
      lcd.setCursor(14, 1);
      lcd.print("No");
      currentCursor[0] = 0;
      currentCursor[1] = 1;
      lcd.setCursor(currentCursor[0], currentCursor[1]);
      lcd.blink();
      break;
     case 3:
      lcd.clear();
      lcd.noBlink();
      printSetTime(true);
      lcd.setCursor(0, 0);
      lcd.print("Confirm " + selectedTimer + "?");
      lcd.setCursor(11, 1);
      lcd.print("Y");
      lcd.setCursor(14, 1);
      lcd.print("N");
      break;
     case -1:
      lcd.clear();
      lcd.noBlink();
      lcd.setCursor(0, 0);
      lcd.print("Test manual!");
      test = 1;
      break;
     default:
      lcd.noBlink();
      break;
  }
}

void handleSelectTimer(int inputKey) {
  if (inputKey == 1) {
    // push left
    if (currentCursor[0] == 7) {
      lcd.setCursor(0, 1);
      lcd.write(1); 
      lcd.setCursor(7, 1);
      lcd.print(" "); 
      selectedTimer = "spray";
      currentCursor[0] = 0;
    } else if (currentCursor[0] == 13) {
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(7, 1); 
      lcd.write(1);
      selectedTimer = "pump";
      currentCursor[0] = 7;
    }
  } else if (inputKey == 2) {
    // push right
    if (currentCursor[0] == 0) {
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(7, 1); 
      lcd.write(1);
      selectedTimer = "pump";
      currentCursor[0] = 7;
    } else if (currentCursor[0] == 7) {
      lcd.setCursor(7, 1);
      lcd.print(" ");
      lcd.setCursor(13, 1); 
      lcd.write(1);
      currentCursor[0] = 13;
    }
  } else if (inputKey == 0) {
    // push select
    if (currentCursor[0] != 13) {
      enterMode(2);
      h = selectedTimer == "pump" ? pumpH : sprayH;
      m = selectedTimer == "pump" ? pumpM : sprayM;
      s = selectedTimer == "pump" ? pumpS : sprayS;
    } else {
      enterMode(0);
    }
  }
}

int inc(int val) {
  if (val < 59) return val + 1;
  return 0;
}

int dec(int val) {
  if (val > 0) return val - 1;
  return 59;
}

void handleSetTime(int inputKey) {
  if (inputKey == 1) {
    if (currentCursor[0] == 3) currentCursor[0] = 0;
    if (currentCursor[0] == 6) currentCursor[0] = 3;
    if (currentCursor[0] == 10) currentCursor[0] = 6;
    if (currentCursor[0] == 14) currentCursor[0] = 10;
  } else if (inputKey == 2) {
    if (currentCursor[0] == 0) currentCursor[0] = 3;
    if (currentCursor[0] == 3) currentCursor[0] = 6;
    if (currentCursor[0] == 6) currentCursor[0] = 10;
    if (currentCursor[0] == 10) currentCursor[0] = 14;
  } else if (inputKey == 3) {
    if (currentCursor[0] == 0) h++;
    else if (currentCursor[0] == 3) m = inc(m);
    else if (currentCursor[0] == 6) s = inc(s);
  } else if (inputKey == 4) {
    if (currentCursor[0] == 0) { if (h > 0) h--; }
    else if (currentCursor[0] == 3) m = dec(m);
    else if (currentCursor[0] == 6) s = dec(s);
  }
  printSetTime(true);
  lcd.setCursor(currentCursor[0], currentCursor[1]);
  if (inputKey == 0 && currentCursor[0] == 10) {
    currentCursor[0] = 0;
    enterMode(3);
  }
  else if (inputKey == 0 && currentCursor[0] == 14) enterMode(1);
}

void handleConfirmSetTime(int inputKey) {
  if (inputKey == 1) {
    lcd.setCursor(10, 1);
    lcd.write(1);
    lcd.setCursor(13, 1);
    currentCursor[0] = 10;
    lcd.print(" ");
  } else if (inputKey == 2) {
    lcd.setCursor(13, 1);
    lcd.write(1);
    lcd.setCursor(10, 1);
    currentCursor[0] = 13;
    lcd.print(" ");
  } else if (inputKey == 0 && currentCursor[0] == 10) {
    if (selectedTimer == "pump") {
      pumpH = h;
      pumpM = m;
      pumpS = s;
      savedPumpH = h - (constPumpT/3600);
      savedPumpM = m - (constPumpT/60);
      savedPumpS = s - (constPumpT%60);
      setStopPump();
    } else {
      sprayH = h;
      sprayM = m;
      sprayS = s;
      savedSprayH = h - (constSprayT/3600);
      savedSprayM = m - (constSprayT/60);
      savedSprayS = s - (constSprayT%60);
      setStopSpray();
    }
    enterMode(0);
  } else if (inputKey == 0 && currentCursor[0] == 13) {
    enterMode(2);
  }
}

void loop() {
//  testKey();
  int pressedButton = readButton(analogRead(0));
  if (test == 0) countDown();
  // test mode: -1
  if (mode == -1) {
    if (pressedButton == 3) {
      // up
      startSpray();
      lcd.setCursor(0, 1);
      lcd.print("spray on ");
    } else if (pressedButton == 4) {
      // down
      stopSpray();
      lcd.setCursor(0, 1);
      lcd.print("spray off");
    }
  }
  // mode: 0 -> init, 1 -> select timer, 2 -> set time, 3 -> confirm set time
  if (mode == 0) {
    displayTime(sprayH, sprayM, sprayS, pumpH, pumpM, pumpS);
    if (spraying == true) displaySpraying();
    if (pumping == true) displayPumping();
    if (pressedButton == 2) enterMode(1);
    if (pressedButton == 1) enterMode(-1);
  }
  if (mode == 1) {
    handleSelectTimer(pressedButton);
  }
  if (mode == 2) {
    handleSetTime(pressedButton);
  }
  if (mode == 3) {
    handleConfirmSetTime(pressedButton);    
  }
}
