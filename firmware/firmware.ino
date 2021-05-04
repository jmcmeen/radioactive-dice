/**********************************************************
  Project: Becquerel Random Number Generator
  Programmers:
    Kayleigh Post - kpost@stumail.northeaststate.edu
    Josh Pitts - tpitts@stumail.northeaststate.edu
    Adam Lothridge - alothrid@stumail.northeaststate.edu
    Kevin McBrien - kmcbrien@stumail.northeaststate.edu
    John McMeen - jnmcmeen@northeaststate.edu
  Date: 5/4/21
***********************************************************/
#include <Nextion.h>

//Define all the UV LED pins
int uvLedPins[] = {3, 4, 5, 6, 7, 8, 9, 10, 11};

//Global variables for Geiger mode
#define LOG_PERIOD 5000  //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000  //Maximum logging period without modifying this sketch
unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement

//Min and max for dice mode
unsigned long minDiceRoll = 1; //this will be set in settings and stored in EEPROM
unsigned long maxDiceRoll = 6; //this will be set in settings and stored in EEPROM

//Byte to store random number and a counter to keep up with what bit we are writing to
byte nextRandom = 0;
short bitCounter = 0;

//Flag for interrupt to set light to blink
bool blinkFlag = false;

// Nextion Screen
// Declare a number object [page id:0, component id:2, component name: "randNumberDice"].
NexNumber randNumberDice = NexNumber(0, 2, "randNumber");
NexNumber randNumberDiceStream = NexNumber(1, 3, "streamNum");
NexNumber countsPerMinute = NexNumber(2, 2, "CPM");

// Declare a button object [page id:0, component id:1, component name: "generateButton"].
NexButton generateButton = NexButton(0, 1, "roll");

//Register objects randNumberDice and generateButton to the touch event list.
NexTouch *nex_listen_list[] =
{
  &generateButton,
  NULL
};

//Button component pop callback function. In this example,the value of the randNumberDice
//component will become a random number between 1 and 100 when button is released.
void generateButtonPopCallback(void *ptr) {
  randNumberDice.setValue((nextRandom % maxDiceRoll) + minDiceRoll);
}

//subprocedure for capturing events from Geiger Kit
void tube_impulse() {      
  counts++; //used in CPM calculation

  //nextRandom bitCounter - how many bits are full
  if (bitCounter < 8) {
    bitWrite(nextRandom, bitCounter, (micros() / 4) % 2);
    bitCounter++;
  }
  //Set the UV lights to blink
  blinkFlag = true;
}

void setup() {
  //Initialize variables
  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD;

  //Create interrupt for Geiger counter board
  attachInterrupt(0, tube_impulse, FALLING); //define external interrupts

  //Initialize Nextion Screen
  nexInit();

  //Register the pop event callback function of the current generateButton component.
  generateButton.attachPop(generateButtonPopCallback);

  //Set pin 12 to output and turn on power LED
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);

  //Set all UV LED pins to output
  for(byte i = 0; i < 9; i++){
    pinMode(uvLedPins[i], OUTPUT);
  }
}

void loop() {
  //Nextion update
  nexLoop(nex_listen_list);

  //Streaming mode update
  if (bitCounter > 7) {
    randNumberDiceStream.setValue(nextRandom);
    bitCounter = 0;
  }

  //Geiger mode calculations
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > LOG_PERIOD){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    countsPerMinute.setValue(cpm);
    counts = 0;
  }

  //UV light controls
  if(blinkFlag){
    //Turn on all UV LEDs
    for(byte i = 0; i < 9; i++){
      digitalWrite(uvLedPins[i], HIGH);
    }
    delay(90);
    //Turn on all UV LEDs
    for(byte i = 0; i < 9; i++){
      digitalWrite(uvLedPins[i], LOW);
    }
    blinkFlag = false;
  }
}
