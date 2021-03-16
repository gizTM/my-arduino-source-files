#include "Config.h"
#include "LcdKeypad.h"
#include "MenuData.h"
#include <avr/eeprom.h>

const char NotImp[] = "Not Implemented";
static char strbuf[LCD_COLS+1];

  
char *toTimeStr(char *buf, long timeval, char separator)
{
  char sepstr[] = {separator, 0};

  short hours = timeval / 3600;
  long tmpVal = timeval - ((long)hours * 3600);
  short mins = tmpVal / 60;
  short secs = tmpVal - (mins * 60);

  char intbuf[8];
  char h[3];
  char m[3];
  char s[3];

  inttostr(intbuf, hours);
  lpad(h, intbuf, '0', 2);
  inttostr(intbuf, mins);
  lpad(m, intbuf, '0', 2);
  inttostr(intbuf, secs);
  lpad(s, intbuf, '0', 2);

  fmt(buf, 5, h, sepstr, m, sepstr, s);
  return buf;
}

//------------------------------------------------------------------------------
long addToTime(short delta, long timeval, long minval, long maxval)
{
  long newtime = timeval+delta;

  if (newtime >= minval && newtime <= maxval) return newtime;

  return timeval;
}

//------------------------------------------------------------------------------
char *Config::getFormattedStr(byte cmdId)
{
  char intbuf[8];
  
  switch (cmdId)
  {
    case mnuCmdT1Hours :
    case mnuCmdT1Mins :
    case mnuCmdT1Secs :
      toTimeStr(strbuf, timer1ReloadValue);
      break;
    case mnuCmdT2Hours :
    case mnuCmdT2Mins :
    case mnuCmdT2Secs :
      toTimeStr(strbuf, timer2ReloadValue);
      break;
    case mnuCmdT3Hours :
    case mnuCmdT3Mins :
    case mnuCmdT3Secs :
      toTimeStr(strbuf, timer3ReloadValue);
      break;
    case mnuCmdResetToDefaults:
      strbuf[0] = 0;
      break;
    case mnuCmdAlarmDuration:
      fmt(strbuf, 2, inttostr(intbuf, alarmDuration), " seconds");
      break;
    case mnuCmdButtonBeep :
      if (buttonBeep)
      {
        fmt(strbuf, 1, "on");
      }
      else
      {
        fmt(strbuf, 1, "off");
      }
      break;
    case mnuCmdDisplayBrightness :
    {
      byte brightnessPC = ((unsigned short) displayBrightness * 100) / 3;
      inttostr(intbuf, brightnessPC);
      fmt(strbuf, 2, intbuf, "%");
      break;
    }
    default:
      strcpy(strbuf, NotImp);
      break;
  }
  return strbuf;
}

//------------------------------------------------------------------------------
long Config::getTimerReloadValue(byte timerIdx)
{
  long reloadValue;
  
  switch (timerIdx)
  {
    case 0:
      reloadValue = timer1ReloadValue;
      break;
    case 1:
      reloadValue = timer2ReloadValue;
      break;
    case 2:
      reloadValue = timer3ReloadValue;
      break;
    default:
      reloadValue = 0;
      break;
  }
  return reloadValue;
}

//------------------------------------------------------------------------------
void Config::save()
{
  eeprom_write_block(this, (void *)0, sizeof (Config));
}


//------------------------------------------------------------------------------
void Config::load()
{
  // Attempt to load config from EEPROM
  eeprom_read_block(this, (void *)0, sizeof (Config));

  if (strcmp(this->appNameAndVersion, NameAndVersion) != 0)
  {
    setDefaults();
  }
}


//------------------------------------------------------------------------------
void Config::setDefaults()
{
  strcpy(appNameAndVersion, NameAndVersion);

  timer1ReloadValue = 30;
  timer2ReloadValue = 60;
  timer3ReloadValue = 5*60;
  alarmDuration = 2;
  buttonBeep = true;
  displayBrightness = 3;
}


//------------------------------------------------------------------------------
void Config::copyTo(Config *dest)
{
  memcpy(dest, this, sizeof(Config));
}
