
#include <Arduino.h>
#include <stdint.h>
#include <LiquidCrystal.h>
#include "UserInterface.h"
#include "Linduino.h"
#include "LT_PMBus.h"
#include "LT_SMBus.h"
#include "nvm.h"
#include "my_Menu.h"
#include "LT_SDReader.h"

static LT_SMBusNoPec *smbusNoPec = new LT_SMBusNoPec();
static LT_SMBusPec *smbusPec = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbusNoPec);
static my_Menu mainMenu = my_Menu(5);

void setup() {
  mainMenu.addMenuItem("readVout00", readVout);

  char fileName[13];

  for (int i = 0; i < 4; i++)
  {
    sdReader.getNextDirEntry(fileName);
    if (fileName[0] == 0)
      break;
    mainMenu.addMenuItem(fileName, program);
  }

  mainMenu.printDisplay();
}

void loop() {
  mainMenu.run();
}

void readVout(char* selected)
{
   pmbus->setPage(0x5C, 0);
   float voltage = pmbus->readVout(0x5C, false);
   lcd.clear();
   lcd.print(F("LTC3882 chan0: V"));
   lcd.setCursor(1,1);
   lcd.print(voltage, DEC);
   lcd.noBlink();
   while (buttons.readButtons() == BUTTON_NONE)
     ;
   mainMenu.printDisplay();
   lcd.blink();
}

void program(char* program)
{
  uint8_t addresses[] = { 0x5C };
  NVM *nvm = new NVM(pmbus, smbusNoPec, smbusPec, addresses, 1);
  LCD_BACKLIGHT(false);
  lcd.clear();
  lcd.print(F("programming"));
  lcd.setCursor(1,1);
  lcd.print(F("please wait"));
  bool worked = nvm->load_hex_file(program);
  
  LCD_BACKLIGHT(true);
   lcd.clear();
   lcd.print(F("  Programming"));
   lcd.setCursor(0,1);
   if (worked)
     lcd.print(F("    Success"));
   else
     lcd.print(F("    Failure"));
   lcd.noBlink();
   while (buttons.readButtons() == BUTTON_NONE)
     ;
   mainMenu.reset();
   mainMenu.printDisplay();
   lcd.blink();
}
