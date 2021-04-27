/**********************************************************
  Project: Becquerel Random Number Generator
  Description: TBD
  Programmers:
  Kayleigh Post - kpost@stumail.northeaststate.edu
  Josh Pitts - tpitts@stumail.northeaststate.edu
  Adam Lothridge - alothrid@stumail.northeaststate.edu
  Kevin McBrien - kmcbrien@stumail.northeaststate.edu
  John McMeen - jnmcmeen@northeaststate.edu
  Date: 4/23/21
***********************************************************/
#include <Nextion.h>

enum operatingMode { DICE, STREAM, GEIGER, SETTINGS};
enum lightMode { OFF, BLINK, WAVE };

operatingMode op = STREAM;

unsigned long minDiceRoll = 1; //this will be set in settings and stored in EEPROM
unsigned long maxDiceRoll = 6; //this will be set in settings and stored in EEPROM
unsigned int cpmLogPeriod = 5000;
unsigned int cpmMaxPeriod = 60000;

unsigned int geigerReadEventPeriod = 1;
bool logEvent = false;
byte lastRandom = 0;

short lastRandomCounter = 0;
unsigned long currentMillis = 0;

/******CPM Stuff TODO */
unsigned long startTime;
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis = 0;  //variable for time measurement
unsigned long cpmDisp;

bool bools[2];
short count = 0;


//Nextion Screen
// Declare a number object [page id:0, component id:2, component name: "randNumber"].
NexNumber randNumber = NexNumber(0, 2, "randNumber");
// Declare a button object [page id:0, component id:1, component name: "generateButton"].
NexButton generateButton = NexButton(0, 1, "generateButton");
NexNumber randNumberStream = NexNumber(1, 3, "streamNum");


/*
   Register objects randNumber and generateButton to the touch event list.
*/
  NexTouch *nex_listen_list[] =
  {
  &randNumber,
  &generateButton,
  NULL
  };

void tube_impulse() {      //subprocedure for capturing events from Geiger Kit
  logEvent = true;
  //Serial.println("pulse");
}


/*
   Button component pop callback function. In this example,the value of the randNumber
   component will become a random number between 1 and 100 when button is released.
*/
  void generateButtonPopCallback(void *ptr) {
  randNumber.setValue(lastRandom);
  }


void setup() {
  startTime = millis();
  attachInterrupt(0, tube_impulse, FALLING); //define external interrupts
  
  //Serial.begin(9600);
  nexInit();

  //Register the pop event callback function of the current generateButton component.
  generateButton.attachPop(generateButtonPopCallback);
}


void loop() {
  nexLoop(nex_listen_list);

  //Each millisecond
  currentMillis = millis();
  if (currentMillis - previousMillis > geigerReadEventPeriod) {
    //Serial.println(currentMillis - previousMillis);
    previousMillis = currentMillis;

    //Von Neumann debias each pair of Geiger events
    if(count != 2){
      bools[count] = logEvent;
      count++;
      if(logEvent){
        logEvent = false;
      }
    } else if(count == 2){
      if(bools[0] != bools[1]){
        bitWrite(lastRandom, lastRandomCounter, bools[0]);
        
        //Serial.print(bools[0]);
        lastRandomCounter++;
      }
      count = 0;
    }

    if(lastRandomCounter == 8){
      //Serial.println(lastRandom);
      lastRandomCounter = 0;
      if(op == STREAM){
        randNumberStream.setValue(lastRandom);
      }
    } 
  }

}
