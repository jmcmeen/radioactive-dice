/**********************************************************
  Project: Becquerel Random Number Generator
  Description: TBD
  Programmers:
  Kayleigh Post - kpost@stumail.northeaststate.edu
  Josh Pitts - tpitts@stumail.northeaststate.edu
  Adam Lothridge - alothrid@stumail.northeaststate.edu
  Kevin McBrien - kmcbrien@stumail.northeaststate.edu
  John McMeen - jnmcmeen@northeaststate.edu
  Date: 4/27/21
***********************************************************/
#include <Nextion.h>

enum operatingMode { DICE, STREAM, GEIGER, SETTINGS };
enum lightMode { OFF, BLINK, WAVE };

int uvLedPins[] = {3, 4, 5, 6, 7, 8, 9, 10, 11};

//global variables
#define LOG_PERIOD 15000  //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000  //Maximum logging period without modifying this sketch
unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement

//operatingMode op = GEIGER;

unsigned long minDiceRoll = 1; //this will be set in settings and stored in EEPROM
unsigned long maxDiceRoll = 6; //this will be set in settings and stored in EEPROM

byte nextRandom = 0;
short bitCounter = 0;
int lightOnCounter = 10000;

// Nextion Screen
// Declare a number object [page id:0, component id:2, component name: "randNumberDice"].
NexNumber randNumberDice = NexNumber(0, 2, "randNumberDice");
NexNumber randNumberDiceStream = NexNumber(1, 3, "streamNum");
NexNumber countsPerMinute = NexNumber(2, 2, "CPM");

// Declare a button object [page id:0, component id:1, component name: "generateButton"].
NexButton generateButton = NexButton(0, 1, "generateButton");

//Register objects randNumberDice and generateButton to the touch event list.
NexTouch *nex_listen_list[] =
{
  &generateButton,
  NULL
};

void tube_impulse() {      //subprocedure for capturing events from Geiger Kit
  counts++; //used in CPM calculation

  //nextRandom bitCounter - how many bits are full
  if (bitCounter < 8) {
    bitWrite(nextRandom, bitCounter, (micros() / 4) % 2);
    bitCounter++;
  }
}

//Button component pop callback function. In this example,the value of the randNumberDice
//component will become a random number between 1 and 100 when button is released.
void generateButtonPopCallback(void *ptr) {
  randNumberDice.setValue(nextRandom);
}

void setup() {
  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD;

  attachInterrupt(0, tube_impulse, FALLING); //define external interrupts

  //Serial.begin(9600);
  nexInit();

  //Register the pop event callback function of the current generateButton component.
  generateButton.attachPop(generateButtonPopCallback);

  for(byte i = 0; i < 9; i++){
    pinMode(uvLedPins[i], OUTPUT);
  }

  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);

    for(byte i = 0; i < 9; i++){
    digitalWrite(uvLedPins[i], HIGH);   // turn the LED on (HIGH is the voltage level)  
  }
}

void loop() {
  //Nextion update
  nexLoop(nex_listen_list);

  lightOnCounter--;

  if (bitCounter > 7) {
    randNumberDiceStream.setValue(nextRandom);
    bitCounter = 0;
  }

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > LOG_PERIOD){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    countsPerMinute.setValue(cpm);
    counts = 0;
  }   
/*
  if(lightOnCounter < 0){
    for(byte i = 0; i < 9; i++){
      digitalWrite(uvLedPins[i], LOW);   // turn the LED on (HIGH is the voltage level)  
    }
  }
  */
   
  
/*
  if(op == STREAM){
    if (bitCounter > 7) {
      randNumberDiceStream.setValue(nextRandom);
      bitCounter = 0;
    }
  }else if(op == GEIGER){
      unsigned long currentMillis = millis();
      if(currentMillis - previousMillis > LOG_PERIOD){
        previousMillis = currentMillis;
        cpm = counts * multiplier;
        countsPerMinute.setValue(cpm);
        counts = 0;
      }   
  }else if(op == DICE){
    if (bitCounter > 7) {
      //notify a new number is available
    }
  }  
  */
}
