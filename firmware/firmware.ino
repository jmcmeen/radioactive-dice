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

//Temporarily using the RingBuf library
#include <RingBuf.h>
#include <Nextion.h>

enum operatingMode { DICE, STREAM, GEIGER, SETTINGS};
enum lightMode { OFF, BLINK, WAVE };

unsigned long minDiceRoll = 1; //this will be set in settings and stored in EEPROM
unsigned long maxDiceRoll = 6; //this will be set in settings and stored in EEPROM
unsigned int cpmLogPeriod = 5000;
unsigned int cpmMaxPeriod = 60000;


unsigned int geigerReadEventPeriod = 1;
bool logEvent = false;
unsigned long lastRandom = 0;


unsigned long currentMillis = 0;

/******CPM Stuff TODO */
unsigned long startTime;
unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis = 0;  //variable for time measurement
unsigned long cpmDisp;

/* bytes to push and pop to ring buffer*/
byte b;
byte a;


RingBuf<byte, 512> myRingBuffer;

//Nextion Screen
// Declare a number object [page id:0, component id:2, component name: "randNumber"].
NexNumber randNumber = NexNumber(0, 2, "randNumber");
// Declare a button object [page id:0, component id:1, component name: "generateButton"].
NexButton generateButton = NexButton(0, 1, "generateButton");


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
  counts++;
  logEvent = true;
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

  nexInit();

  //Register the pop event callback function of the current generateButton component.
  generateButton.attachPop(generateButtonPopCallback);
}


void loop() {
  nexLoop(nex_listen_list);

  currentMillis = millis();
  if (currentMillis - previousMillis > geigerReadEventPeriod) {
    previousMillis = currentMillis;
    if (logEvent == true) {
      b = 1;
      logEvent = false;
    } else {
      b = 0;
    }
    if (myRingBuffer.isFull()) {
      myRingBuffer.pop(a);
    }
    myRingBuffer.push(b);
  }

  if (myRingBuffer.isFull()) {
    short bitCounter = 0;
    for (int i = 0; i < myRingBuffer.maxSize(); i += 2) {
      if (bitCounter < 16) {
        if (!(i + 1 > myRingBuffer.maxSize())) {
          if (myRingBuffer[i] != myRingBuffer[i + 1]) {
            if (myRingBuffer[i] == 1) {
              bitWrite(lastRandom, bitCounter, 1);
            } else {
              bitWrite(lastRandom, bitCounter, 0);
            }
            bitCounter += 2;
          }
        }
      }
    }

    myRingBuffer.clear();
  }
}
