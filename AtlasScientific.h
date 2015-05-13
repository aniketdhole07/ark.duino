#include "SensorEntry.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// atlas circuits mux reader

const unsigned long TimeOut = 5000;  // timeout in milliseconds

#ifdef SensorUART

// using 2 UARTs simultaneously can result in read errors unless you use interrupt functions like below
// need these global for interrupt to access
// buf needs to be wiped (read = 0) when switching to a new sensor if previously read sensor misbehaves
char buf[48];
unsigned int read = 0;  // number of characters written to buffer
/*volatile*/ const struct SensorEntry *currentSensor = NULL;    //moved to top of file

// interrupt handler for UART3
void serialEvent3() {
  if (currentSensor == NULL) return;
 
  // TODO: handle buffer overflow better
  if (read >= sizeof(buf)-1) {
    printlnStatus("buffer overflow reading sensor");
    buf[0] = '\0';
    read = 0;
    currentSensor = NULL;
    return;
  }
  
  char c = SensorUART.read();
  if (c == '\n') return;      // only pay attention to \n in \r\n sequences
  buf[read++] = c;

  // assume response ends in "*OK\r"
  if (c == '\r' && read > 3 && !memcmp("*OK", buf+read-4, 3)) {
    read -= 4;  // remove "*OK\r"
  
    // null-terminate and process the command
    buf[read] = '\0';
    currentSensor->packageDataMessage(buf);
    read = 0;
    currentSensor = NULL;


    //String s = "{\"measurement\":{\"label\":\"pH\",\"datum\":" + String(buf) + "}}";
    //Serial.println(s);
    return;
  }
  
  // error
  if (c == '\r' && read > 3 && !memcmp("*ER", buf+read-4, 3)) {
    // TODO: identify sensor
    printlnError("error reading sensor");
    buf[0] = '\0';
    read = 0;
    currentSensor = NULL;
    return;
  }
}
#endif

int read_Atlas_Circuit(const struct SensorEntry *entry)
{
#ifndef SensorUART
  return -1;
#else
  //char buf[64];
  //unsigned int read = 0;  // number of characters written to buffer
  
  // switch the mux to point the the right sensor
  // first pin indicates which mux pins must be set high
  PinType muxAddress = entry->pins[0];
  
  // use all remaining pin numbers that aren't -1 as mux address pins
  for (int i = 1; i < NUM_PINS; i++) {
    PinType pin = entry->pins[i];
    if(pin == -1) break;
    pinMode(pin, OUTPUT);
    
    // select only the bit which corresponds to this pin
    int value = (muxAddress >> (i-1)) & 1;
    digitalWrite(pin, value);
  }
 
  unsigned long startTime = millis();
 
  // request a reading
  currentSensor = (SensorEntry *)entry;
  SensorUART.print("R\r");
  Serial.println(currentSensor->label);
  
  while(true) {
    if (currentSensor == NULL) break;
  
    // don't loop infinitely
    if(millis() - startTime > TimeOut) {
      break;                  // volatile not working, so try a different variable we haven't touched
      //printlnError("sensor timed out");    // TODO: identify sensor
      //return -1;
    }
  }
  
  Serial.println(currentSensor->label);
   
  //replace newlines with spaces
  for (int i = 0; i < sizeof(buf)-1 && buf[i] != '\0'; i++) {
    if (buf[i] == '\r' || buf[i] == '\n') buf[i] = ' ';
  }
  
  entry->packageDataMessage(buf);
  return 0;
#endif
/*
  // wait for response over serial
  // TODO: don't wait infinitely!
  while(true) {
    // don't loop infinitely
    if(millis() - startTime > TimeOut) {
      printlnError("sensor timed out");    // TODO: identify sensor
      return -1;
    }
    
    if (SensorUART.available() == 0) continue;
    char c = SensorUART.read();
      
    // TODO: handle buffer overflow better
    if (read >= sizeof(buf)-1) {
      read = 0;
      printlnError("reading sensor overflowed buffer");
      return -1;
    }
    
    buf[read++] = c;
    
    // assume response ends in "*OK\r"
    if (c == '\r' && read > 3 && !memcmp("*OK", buf+read-4, 3)) {
      read -= 4;  // remove "*OK\r"
    
      // null-terminate and process the command
      buf[read] = '\0';
      //Serial.println(buf);
      break;
    }
    
    // error
    if (c == '\r' && read > 3 && !memcmp("*ER", buf+read-4, 3)) {
      // TODO: identify sensor
      printlnError("error reading sensor");
      return -1;
    }
  }
  
  //replace newlines with spaces
  for (int i = 0; i < sizeof(buf)-1 && buf[i] != '\0'; i++) {
    if (buf[i] == '\r' || buf[i] == '\n') buf[i] = ' ';
  }
  
  // successfully got a reading
  entry->packageDataMessage(buf);
  //printlnStatus("leaving atlas do sensor func");
  return 0;  //no error
*/
}
