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
#include <RingBuf.h>

#define LOG_PERIOD 5000  //Logging period in milliseconds, recommended value 15000-60000.
#define MAX_PERIOD 60000  //Maximum logging period without modifying this sketch
#define LOG_EVENT_PERIOD 1
unsigned long startTime;
unsigned long counts;     //variable for GM Tube events
unsigned long cpm;        //variable for CPM
unsigned int multiplier;  //variable for calculation CPM in this sketch
unsigned long previousMillis;  //variable for time measurement
unsigned long cpmDisp;
bool logEvent = false;
byte b;
byte lb;
unsigned long randomNumber; //4,294,967,295 (2^32 - 1).
RingBuf<byte, 512> myRingBuffer;

void tube_impulse(){       //subprocedure for capturing events from Geiger Kit
  counts++;
  //Serial.print(1);
  logEvent=true;
}

void setup() {
  startTime = millis();
  Serial.begin(9600);
  attachInterrupt(0, tube_impulse, FALLING); //define external interrupts 
}


void loop() { 
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > LOG_EVENT_PERIOD){
    previousMillis = currentMillis;
    if(logEvent == true){
      b = 1;
      logEvent = false;
    }else{
      b = 0;
    }
    if(myRingBuffer.isFull()){
      myRingBuffer.pop(lb);
    }
    myRingBuffer.push(b);
  }

  if(myRingBuffer.isFull()){
    short bitCounter = 0;
    for(int i=0; i<myRingBuffer.maxSize(); i+=2){
      if(bitCounter < 16){
       if(!(i+1 >myRingBuffer.maxSize())){
       if(myRingBuffer[i] != myRingBuffer[i+1]){
        if(myRingBuffer[i] ==1){
          bitWrite(randomNumber, bitCounter, 1);
        }else{
          bitWrite(randomNumber, bitCounter, 0);
        }
        bitCounter += 2;
      }
       } 
      }
    }
Serial.println(randomNumber);
    myRingBuffer.clear();
    randomNumber = 0;
    
  }
}
