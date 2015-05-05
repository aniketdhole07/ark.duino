#include "SensorEntry.h"

int read_analog(const struct SensorEntry *entry)
{
  char pinDigital = entry->pins[0];
  char pinAnalog = entry->pins[1];
  pinMode(pinDigital, OUTPUT);
  digitalWrite(pinAnalog, LOW); 
  digitalWrite(pinDigital, HIGH);
  
  delay(100);     // wait 100 milliseconds
  
  float reading = analogRead(pinAnalog);
  digitalWrite(pinDigital, LOW);
  
  reading /= 1023.0;    // divide by max reading
  
  entry->packageDataMessage(reading);
  return 0;  //no error
}
