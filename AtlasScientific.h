#include "SensorEntry.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// atlas circuits mux reader

// using 2 UARTs simultaneously can result in read errors unless you use interrupt functions like below
// need these global for interrupt to access
// buf needs to be wiped (read = 0) when switching to a new sensor if previously read sensor misbehaves
char buf[64];
unsigned int read = 0;  // number of characters written to buffer
//const struct SensorEntry *currentSensor = NULL;    //moved to top of file

// interrupt handler for UART3
void serialEvent3() {
  {
    if (currentSensor == NULL) return;
    
    // TODO: handle buffer overflow better
    if (read >= sizeof(buf)-1) {
      read = 0;
      printlnError("reading sensor overflowed buffer");
      //currentSensor = NULL;
      return;
    }
    
    char c = SensorUART.read();
    if (c == '\n') return;      // \r\n sequence will mess up the string logic below
    buf[read++] = c;
    
    // assume response ends in "*OK\r"
    if (c == '\r' && read > 3 && !memcmp("*OK", buf+read-4, 3)) {
      read -= 4;  // remove "*OK\r"
    
      // null-terminate and process the command
      buf[read] = '\0';
    
      //replace newlines with spaces
      for (int i = 0; i < sizeof(buf)-1 && buf[i] != '\0'; i++) {
        if (buf[i] == '\r' || buf[i] == '\n') buf[i] = ' ';
      }
      
      //String s = "{\"measurement\":{\"label\":\"pH\",\"datum\":" + String(buf) + "}}";
      //Serial.println(s);
      currentSensor->packageDataMessage(buf);
      //currentSensor = NULL;
      read = 0;
    }
    
    // error
    if (c == '\r' && read > 3 && !memcmp("*ER", buf+read-4, 3)) {
      // TODO: identify sensor
      printlnError("error reading sensor");
      //currentSensor = NULL;
      read = 0;
      return;
    }
  }
  
  return;
}

int read_Atlas_Circuit(const struct SensorEntry *entry)
{
/*#ifndef SensorUART
  return -1;
#else*/
  char buf[64];
  unsigned int read = 0;  // number of characters written to buffer
  
  // switch the mux to point the the right sensor
  char muxAddress = entry->pins[2];
  pinMode(entry->pins[0], OUTPUT);
  pinMode(entry->pins[1], OUTPUT);
  digitalWrite(entry->pins[0], muxAddress & 1);
  digitalWrite(entry->pins[1], muxAddress & 2);
 
  unsigned long startTime = millis();
  const unsigned long TimeOut = 5000;  // 5 second timeout
 
  // request a reading
  currentSensor = entry;
  SensorUART.print("R\r");

  return 0;
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