#include <LiquidCrystal.h>
#include "LcdKeypad.h"
#include "MenuData.h"
#include "Config.h"

#define ALARM_PIN 2

enum AppModeValues
{
  APP_NORMAL_MODE,
  APP_TIMER_RUNNING,
  APP_ALARM,
  APP_MENU_MODE,
  APP_PROCESS_MENU_CMD,
  APP_MENU_MODE_END
};

byte appMode = APP_NORMAL_MODE;

char strbuf[LCD_COLS + 1]; // one line of lcd display
long timerCurrentValue[3];
unsigned long alarmStartTime;
short timerFineGrainedCounter[3];
unsigned long lastMilliSecondTimerValue = 0;
char currentTimerIdx = 0;
byte btn;
Config currentConfig;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

MenuManager Menu1(cd_timer_menu_Root, menuCount(cd_timer_menu_Root));

void printTimerValue(byte timerIdx, byte showTimerName = false);


void setup() {
  pinMode(ALARM_PIN, OUTPUT);
  digitalWrite(ALARM_PIN, LOW);
  
  backLightOn();
  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);
  currentConfig.load();

  initTimers();

  printTimerValue(0, true);

  // Use soft PWM for backlight, as hardware PWM must be avoided for some LCD shields. 
  // piggy back on to timer0, which is already set to approx 1khz.
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  
  setBacklightBrightness(currentConfig.displayBrightness);
  
  //Serial.begin(9600);
}

SIGNAL(TIMER0_COMPA_vect)
{
  lcdBacklightISR();
}

void loop()
{
  btn = getButton();

  if (btn && currentConfig.buttonBeep && appMode != APP_ALARM)
  {
    byte btnFlags = btn & 192;

    if (btnFlags == BUTTON_PRESSED_IND)   // if any button pressed.
    {
      digitalWrite(ALARM_PIN, HIGH);
      delay(3);
      digitalWrite(ALARM_PIN, LOW);
    }
  }
  
  switch (appMode)
  {
    case APP_NORMAL_MODE :
      if (btn == BUTTON_SELECT_SHORT_RELEASE)
      {
        if (timerCurrentValue[currentTimerIdx] > 0)
        {
          lastMilliSecondTimerValue = millis();
          appMode = APP_TIMER_RUNNING;
          lcd.setCursor(8, 0);
          lcd.print("running");
        }
      }
      else if (btn == BUTTON_SELECT_LONG_PRESSED)
      {
        timerCurrentValue[currentTimerIdx] = currentConfig.getTimerReloadValue(currentTimerIdx);
        printTimerValue(currentTimerIdx);
      }
      else if (btn == BUTTON_UP_LONG_PRESSED)
      {
        appMode = APP_MENU_MODE;
        refreshMenuDisplay(REFRESH_DESCEND);
      }
      else if (btn == BUTTON_UP_SHORT_RELEASE)
      {
        currentTimerIdx = --currentTimerIdx < 0 ? 0 : currentTimerIdx;
        printTimerValue(currentTimerIdx, true);
      }
      else if (btn == BUTTON_DOWN_SHORT_RELEASE)
      {
        currentTimerIdx = ++currentTimerIdx > 2 ? 2 : currentTimerIdx;
        printTimerValue(currentTimerIdx, true);
      }      
      break;
    case APP_TIMER_RUNNING :
      if (btn == BUTTON_SELECT_SHORT_RELEASE || btn == BUTTON_SELECT_LONG_RELEASE)
      {
        appMode = APP_NORMAL_MODE;
      }
      else
      {
        short msDelta = (millis() - lastMilliSecondTimerValue);

        if (msDelta > 0)
        {
          lastMilliSecondTimerValue = millis();
          timerFineGrainedCounter[currentTimerIdx] += msDelta;
          
          if (timerFineGrainedCounter[currentTimerIdx] >= 1000)
          {
            timerFineGrainedCounter[currentTimerIdx] -= 1000;                     
            timerCurrentValue[currentTimerIdx]-=1;
            printTimerValue(currentTimerIdx);
  
            if (timerCurrentValue[currentTimerIdx] <= 0)
            {
              timerCurrentValue[currentTimerIdx] = currentConfig.getTimerReloadValue(currentTimerIdx);
              appMode = APP_ALARM;
              alarmStartTime = millis();
              digitalWrite(ALARM_PIN, HIGH);
            }
          }
        }
      }
      if (appMode == APP_NORMAL_MODE || appMode == APP_ALARM)
      {
        printTimerValue(currentTimerIdx);
        lcd.setCursor(8, 0);
        lcd.print("       ");
      }
      break;
    case APP_ALARM:
      if (btn)
      {
        byte btnFlags = btn & 192;

        if (btnFlags == BUTTON_SHORT_RELEASE_IND || btnFlags == BUTTON_LONG_RELEASE_IND)
        {
          appMode = APP_NORMAL_MODE;
        }
      }
      else if (millis() - alarmStartTime >= (short)currentConfig.alarmDuration * 1000)
      {
        appMode = APP_NORMAL_MODE;
      }

      if (appMode == APP_NORMAL_MODE)
      {
        digitalWrite(ALARM_PIN, LOW);
      }
      break;
    case APP_MENU_MODE :
    {
      byte menuMode = Menu1.handleNavigation(getNavAction, refreshMenuDisplay);

      if (menuMode == MENU_EXIT)
      {
        // Tidy up display
        printTimerValue(currentTimerIdx, true);
        appMode = APP_MENU_MODE_END;
      }
      else if (menuMode == MENU_INVOKE_ITEM)
      {
        appMode = APP_PROCESS_MENU_CMD;

        // Highlight selected item (if not reset command).
        if (Menu1.getCurrentItemCmdId() != mnuCmdResetToDefaults)
        {
          lcd.setCursor(0, 1);
          strbuf[0] = 0b01111110; // forward arrow representing input prompt.
          strbuf[1] = 0;
          lcd.print(strbuf);
        }
      }
      break;
    }
    case APP_PROCESS_MENU_CMD :
    {
      byte processingComplete = processMenuCommand(Menu1.getCurrentItemCmdId());

      if (processingComplete)
      {
        appMode = APP_MENU_MODE;
        // Unhighlight selected item
        lcd.setCursor(0, 1);
        strbuf[0] = ' '; // clear forward arrow
        strbuf[1] = 0;
        lcd.print(strbuf);
      }
      break;
    }
    case APP_MENU_MODE_END :
      if (btn == BUTTON_SELECT_SHORT_RELEASE || btn == BUTTON_SELECT_LONG_RELEASE)
      {
        appMode = APP_NORMAL_MODE;
      }
      break;
  }
}


//----------------------------------------------------------------------
void printTimerValue(byte timerIdx, byte showTimerName)
{
  if (showTimerName)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    char intbuf[2];

    inttostr(intbuf, timerIdx + 1);
    
    fmt(strbuf, 2, "Timer ", intbuf);
    lcd.print(strbuf);
  }

  lcd.setCursor(0,1);
  toTimeStr(strbuf, timerCurrentValue[timerIdx]);
  lcd.print(strbuf);
}


void initTimers()
{
  timerCurrentValue[0] = currentConfig.timer1ReloadValue;
  timerCurrentValue[1] = currentConfig.timer2ReloadValue;
  timerCurrentValue[2] = currentConfig.timer3ReloadValue;

  timerFineGrainedCounter[0] = 0;
  timerFineGrainedCounter[1] = 0;
  timerFineGrainedCounter[2] = 0;
}


//----------------------------------------------------------------------
// Addition or removal of menu items in MenuData.h will require this method
// to be modified accordingly. 
byte processMenuCommand(byte cmdId)
{
  byte complete = false;  // set to true when menu command processing complete.
  byte configChanged = false;

  if (btn == BUTTON_SELECT_PRESSED)
  {
    if (cmdId != mnuCmdResetToDefaults) complete = true;
  }

  switch (cmdId)
  {
    // Process menu commands here:
    case mnuCmdT1Hours :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer1ReloadValue = addToTime(3600, currentConfig.timer1ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer1ReloadValue = addToTime(-3600, currentConfig.timer1ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT1Mins :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer1ReloadValue = addToTime(60, currentConfig.timer1ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer1ReloadValue = addToTime(-60, currentConfig.timer1ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT1Secs :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer1ReloadValue = addToTime(1, currentConfig.timer1ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer1ReloadValue = addToTime(-1, currentConfig.timer1ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT2Hours :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer2ReloadValue = addToTime(3600, currentConfig.timer2ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer2ReloadValue = addToTime(-3600, currentConfig.timer2ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT2Mins :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer2ReloadValue = addToTime(60, currentConfig.timer2ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer2ReloadValue = addToTime(-60, currentConfig.timer2ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT2Secs :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer2ReloadValue = addToTime(1, currentConfig.timer2ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer2ReloadValue = addToTime(-1, currentConfig.timer2ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT3Hours :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer3ReloadValue = addToTime(3600, currentConfig.timer3ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer3ReloadValue = addToTime(-3600, currentConfig.timer3ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT3Mins :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer3ReloadValue = addToTime(60, currentConfig.timer3ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer3ReloadValue = addToTime(-60, currentConfig.timer3ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdT3Secs :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.timer3ReloadValue = addToTime(1, currentConfig.timer3ReloadValue);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.timer3ReloadValue = addToTime(-1, currentConfig.timer3ReloadValue);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdAlarmDuration:
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.alarmDuration = ++currentConfig.alarmDuration > 10 ? 10 : currentConfig.alarmDuration;
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.alarmDuration = --currentConfig.alarmDuration < 1 ? 1 : currentConfig.alarmDuration;
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdButtonBeep:
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.buttonBeep = true;
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.buttonBeep = false;
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdDisplayBrightness :
      configChanged = true;
      if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
      {
        currentConfig.displayBrightness++;
        currentConfig.displayBrightness = constrain(currentConfig.displayBrightness, 1, 3);
        setBacklightBrightness(currentConfig.displayBrightness);
      }
      else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
      {
        currentConfig.displayBrightness--;
        currentConfig.displayBrightness = constrain(currentConfig.displayBrightness, 1, 3);
        setBacklightBrightness(currentConfig.displayBrightness);
      }
      else
      {
        configChanged = false;
      }
      break;
    case mnuCmdResetToDefaults:
      if (btn == BUTTON_SELECT_LONG_PRESSED)
      {
        currentConfig.setDefaults();
        setBacklightBrightness(currentConfig.displayBrightness);
        lcd.setCursor(1, 1);
        lcd.print("Defaults loaded");
      }
      else if (/*btn == BUTTON_SELECT_SHORT_RELEASE ||*/ btn == BUTTON_SELECT_LONG_RELEASE)
      {
        complete = true;
      }
      break;
    default:
      break;
  }

  if (configChanged && cmdId != mnuCmdResetToDefaults)
  {
    lcd.setCursor(1, 1);
    lcd.print(rpad(strbuf, currentConfig.getFormattedStr(cmdId))); // Display config value.
  }
  if (complete)
  {
    currentConfig.save();
    initTimers();
  }
  return complete;
}


//----------------------------------------------------------------------
// Callback to convert button press to navigation action.
byte getNavAction()
{
  byte navAction = 0;
  byte currentItemHasChildren = Menu1.currentItemHasChildren();
  
  if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) navAction = MENU_ITEM_PREV;
  else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) navAction = MENU_ITEM_NEXT;
  else if (btn == BUTTON_SELECT_PRESSED || (btn == BUTTON_RIGHT_PRESSED && currentItemHasChildren)) navAction = MENU_ITEM_SELECT;
  //else if (btn == BUTTON_LEFT_PRESSED) navAction = MENU_BACK;
  return navAction;
}


//----------------------------------------------------------------------
const char EmptyStr[] = "";

// Callback to refresh display during menu navigation, using parameter of type enum DisplayRefreshMode.
void refreshMenuDisplay (byte refreshMode)
{
  char nameBuf[LCD_COLS+1];

  lcd.setCursor(0, 0);
  if (Menu1.currentItemHasChildren())
  {
    rpad(strbuf, Menu1.getCurrentItemName(nameBuf));
    strbuf[LCD_COLS-1] = 0b01111110;            // Display forward arrow if this menu item has children.
    lcd.print(strbuf);
    lcd.setCursor(0, 1);
    lcd.print(rpad(strbuf, EmptyStr));          // Clear config value in display
  }
  else
  {
    byte cmdId;
    rpad(strbuf, Menu1.getCurrentItemName(nameBuf));

    if ((cmdId = Menu1.getCurrentItemCmdId()) == 0)
    {
      strbuf[LCD_COLS-1] = 0b01111111;          // Display back arrow if this menu item ascends to parent.
      lcd.print(strbuf);
      lcd.setCursor(0, 1);
      lcd.print(rpad(strbuf, EmptyStr));        // Clear config value in display.
    }
    else
    {
      lcd.print(strbuf);
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.print(rpad(strbuf, currentConfig.getFormattedStr(cmdId))); // Display config value.
    }
  }
}
