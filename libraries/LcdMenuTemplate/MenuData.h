#ifndef _sampleMenu_
#define _sampleMenu_
#include "MenuManager.h"
#include <avr/pgmspace.h>

/*

Generated using LCD Menu Builder at https://lcd-menu-bulder.cohesivecomputing.co.uk/
For more information, visit https://www.cohesivecomputing.co.uk/hackatronics/arduino-lcd-menu-library/

All our hackatronics projects are free for personal use. If you find them helpful or useful, please consider
making a small donation to our hackatronics fund using the donate buttons on our web pages. Thank you.
    
*/

enum sampleMenuCommandId
{
  mnuCmdBack = 0,
  mnuCmdTimer1Root,
  mnuCmdT1Hours,
  mnuCmdT1Mins,
  mnuCmdT1Secs,
  mnuCmdTimer2Root,
  mnuCmdT2Hours,
  mnuCmdT2Mins,
  mnuCmdT2Secs,
  mnuCmdTimer3Root,
  mnuCmdT3Hours,
  mnuCmdT3Mins,
  mnuCmdT3Secs,
  mnuCmdAlarmDuration,
  mnuCmdButtonBeep,
  mnuCmdResetToDefaults
};

PROGMEM const char sampleMenu_back[] = "Back";
PROGMEM const char sampleMenu_exit[] = "Exit";

PROGMEM const char sampleMenu_1_1[] = "Timer1 set hrs";
PROGMEM const char sampleMenu_1_2[] = "Timer1 set mins";
PROGMEM const char sampleMenu_1_3[] = "Timer1 set secs";
PROGMEM const MenuItem sampleMenu_List_1[] = {{mnuCmdT1Hours, sampleMenu_1_1}, {mnuCmdT1Mins, sampleMenu_1_2}, {mnuCmdT1Secs, sampleMenu_1_3}, {mnuCmdBack, sampleMenu_back}};

PROGMEM const char sampleMenu_2_1[] = "Timer2 set hrs";
PROGMEM const char sampleMenu_2_2[] = "Timer2 set mins";
PROGMEM const char sampleMenu_2_3[] = "Timer2 set secs";
PROGMEM const MenuItem sampleMenu_List_2[] = {{mnuCmdT2Hours, sampleMenu_2_1}, {mnuCmdT2Mins, sampleMenu_2_2}, {mnuCmdT2Secs, sampleMenu_2_3}, {mnuCmdBack, sampleMenu_back}};

PROGMEM const char sampleMenu_3_1[] = "Timer3 set hrs";
PROGMEM const char sampleMenu_3_2[] = "Timer3 set mins";
PROGMEM const char sampleMenu_3_3[] = "Timer3 set secs";
PROGMEM const MenuItem sampleMenu_List_3[] = {{mnuCmdT3Hours, sampleMenu_3_1}, {mnuCmdT3Mins, sampleMenu_3_2}, {mnuCmdT3Secs, sampleMenu_3_3}, {mnuCmdBack, sampleMenu_back}};

PROGMEM const char sampleMenu_1[] = "Timer1 Profile";
PROGMEM const char sampleMenu_2[] = "Timer2 Profile";
PROGMEM const char sampleMenu_3[] = "Timer3 Profile";
PROGMEM const char sampleMenu_4[] = "Alarm Duration";
PROGMEM const char sampleMenu_5[] = "Button Beep";
PROGMEM const char sampleMenu_6[] = "Reset";
PROGMEM const MenuItem sampleMenu_Root[] = {{mnuCmdTimer1Root, sampleMenu_1, sampleMenu_List_1, menuCount(sampleMenu_List_1)}, {mnuCmdTimer2Root, sampleMenu_2, sampleMenu_List_2, menuCount(sampleMenu_List_2)}, {mnuCmdTimer3Root, sampleMenu_3, sampleMenu_List_3, menuCount(sampleMenu_List_3)}, {mnuCmdAlarmDuration, sampleMenu_4}, {mnuCmdButtonBeep, sampleMenu_5}, {mnuCmdResetToDefaults, sampleMenu_6}, {mnuCmdBack, sampleMenu_exit}};

/*
case mnuCmdT1Hours :
  break;
case mnuCmdT1Mins :
  break;
case mnuCmdT1Secs :
  break;
case mnuCmdT2Hours :
  break;
case mnuCmdT2Mins :
  break;
case mnuCmdT2Secs :
  break;
case mnuCmdT3Hours :
  break;
case mnuCmdT3Mins :
  break;
case mnuCmdT3Secs :
  break;
case mnuCmdAlarmDuration :
  break;
case mnuCmdButtonBeep :
  break;
case mnuCmdResetToDefaults :
  break;
*/

/*
<?xml version="1.0"?>
<RootMenu xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <Config IdPrefix="mnuCmd" VarPrefix="sampleMenu" UseNumbering="false" IncludeNumberHierarchy="false"
          MaxNameLen="15" MenuBackFirstItem="false" BackText="Back" ExitText="Exit" AvrProgMem="true"/>
  <MenuItems>
    <Item Id="Timer1Root" Name="Timer1 Profile">
      <MenuItems>
        <Item Id="T1Hours" Name="Timer1 set hrs"/>
        <Item Id="T1Mins" Name="Timer1 set mins"/>
        <Item Id="T1Secs" Name="Timer1 set secs"/>
      </MenuItems>
    </Item>
    <Item Id="Timer2Root" Name="Timer2 Profile">
      <MenuItems>
        <Item Id="T2Hours" Name="Timer2 set hrs"/>
        <Item Id="T2Mins" Name="Timer2 set mins"/>
        <Item Id="T2Secs" Name="Timer2 set secs"/>
      </MenuItems>
    </Item>
    <Item Id="Timer3Root" Name="Timer3 Profile">
      <MenuItems>
        <Item Id="T3Hours" Name="Timer3 set hrs"/>
        <Item Id="T3Mins" Name="Timer3 set mins"/>
        <Item Id="T3Secs" Name="Timer3 set secs"/>
      </MenuItems>
    </Item>
    <Item Id="AlarmDuration" Name="Alarm Duration"/>
    <Item Id="ButtonBeep" Name="Button Beep"/>
    <Item Id="ResetToDefaults" Name="Reset"/>
  </MenuItems>
</RootMenu>

*/
#endif
