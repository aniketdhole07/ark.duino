#include "SensorEntry.h"

volatile static int NbTopsFan;  //count the rising edges of the signal

void rpm()    //This is the function that the interupt calls 
{ 
  NbTopsFan++;  //measure the rising and falling edge of the hall effect sensors signal
} 

// The setup() method runs once, when the sketch starts
int readHallSensor(const struct SensorEntry *entry)
{ 
  pinMode(entry->pins[0], INPUT); //initializes digital pin 2 as an input
  attachInterrupt(0, rpm, RISING); //and the interrupt is attached

  NbTopsFan = 0;   //Set NbTops to 0 ready for calculations
  delay (1000);
  detachInterrupt(0);

  float result = NbTopsFan * 60 / 7.5; // (Pulse frequency x 60) / 7.5Q = flow rate in L/hour
  entry->packageDataMessage(result);
  return 0;  // no error
}

