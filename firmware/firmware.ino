/**********************************************************
Project: Becquerel Random Number Generator
Description: TBD
Programmers:
  Kayleigh Post - kpost@stumail.northeaststate.edu
  Josh Pitts - tpitts@stumail.northeaststate.edu
  Adam Lothridge - alothrid@stumail.northeaststate.edu
  Kevin McBrien - kmcbrien@stumail.northeaststate.edu
  John McMeen - jnmcmeen@northeaststate.edu
Date: 3/24/21
***********************************************************/

//Temporarily using the TM1638 board for UI
#include <TM1638.h>
#include <InvertedTM1638.h>
#include <SPI.h>
#define NO_MODULES  2

// define a regular module and a inverted module
TM1638 module1(5, 4, 6);
InvertedTM1638 module2(5, 4, 7);
TM1638* modules[NO_MODULES] = {
  &module1,
  &module2
};
byte modes[NO_MODULES];

#define LOG_PERIOD 5000  //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000  //Maximum logging period without modifying this sketch

unsigned long startTime;
unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement
unsigned long cpmDisp;

void tube_impulse(){       //subprocedure for capturing events from Geiger Kit
  counts++;
}

void setup() {
  startTime = millis();
  
  for (int i = 0; i < NO_MODULES; i++) {
    modules[i]->setupDisplay(true, 7);
    modes[i] = 0;
  }

  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD;      //calculating multiplier, depend on your log period
  
  Serial.begin(9600);
  attachInterrupt(0, tube_impulse, FALLING); //define external interrupts 
}

void update(TM1638* module, byte* mode) {
  byte buttons = module->getButtons();
  unsigned long runningSecs = (millis() - startTime) / 1000;
  
  // button pressed - change mode
  if (buttons != 0) {
    *mode = buttons >> 1;
    module->clearDisplay();
    module->setLEDs(0);
  }

  switch (*mode) {
    case 0:
      module->setDisplayToDecNumber(runningSecs, 1 << 7);
      break;
    case 1:
      module->setDisplayToDecNumber(runningSecs, 1 << 6, false);
      break;
    case 2:
      module->setDisplayToHexNumber(runningSecs, 1 << 5);
      break;
    case 4:
      module->setDisplayToHexNumber(runningSecs, 1 << 4, false);
      break;
    case 8:
      module->setDisplayToBinNumber(runningSecs, 1 << 3);
      break;
    case 16:
      module->clearDisplayDigit((runningSecs - 1) % 8, 0);
      module->setDisplayDigit(runningSecs % 8, runningSecs % 8, 0);
      break;
    case 32:
      char s[8];
      sprintf(s, "CPM %04d", cpm % 999);
      module->setDisplayToString(s);
      break;
      
    case 64:
      module->setDisplayToDecNumber(random(100000000) , 1 << 6, false);
      /*if (runningSecs % 2 == 0) {
        module->setDisplayToString("TM1638  ");
      } else {
        module->setDisplayToString("LIBRARY ");
  

      module->setLED(0, (runningSecs - 1) % 8);
      module->setLED(1 + runningSecs % 3, runningSecs % 8);
      }*/
      break;
    case 65:
      module->setDisplayToError();
      break;
  }
}

void loop() {
  for (int i = 0; i < NO_MODULES; i++) {
    update(modules[i], &modes[i]);

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > LOG_PERIOD){
      previousMillis = currentMillis;
      cpm = counts * multiplier;
    
      Serial.print(cpm);
      counts = 0;
    }
  }
}
