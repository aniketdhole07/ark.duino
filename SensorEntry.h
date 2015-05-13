#pragma once
#include "Globals.h"
#include "SensorID.h"

////////////////////////////////////////////////////////////////////////////////////
// Globals for knowing what sensors are plugged into the arduino and on what pins
////////////////////////////////////////////////////////////////////////////////////

const int NUM_PINS = 4;
typedef char PinType;

// All the info for a sensor plugged into the arduino
struct SensorEntry {
  char label[15];                       // label applied to data from this sensor
  bool disabled;                        // sensor is on by default
  unsigned long msMeasurementPeriod;    // measurement period in milliseconds
  PinType pins[NUM_PINS];               // some number of pins used by the sensor
                                        // (order is dictated the sensor's read function)
  //char sensorID[MAX_SENSOR_ID_LENGTH];  // the "class" type
  SensorMeasurementFunc func;           // the sensor "type", this will be a function in gSensorFuncMap
                                        // this field is PURPOSELY LAST to simplify writing to EEPROM


  SensorEntry() {
    setEmpty();
  }
  
  void setEmpty() {
    func = NULL;
    disabled = false;
    label[0] = '\0';
    msMeasurementPeriod = 0;
    memset(pins, -1, sizeof(pins));
  }
  
  bool isEmpty() const {
    return func == NULL;
  }
  
  void enable(bool state) {
    disabled = !state;
  }
  
  // TODO add a separate flag for enabled!
  bool isEnabled() const {
    return !disabled;
  }
  
  bool isDisabled() const {
    return disabled;
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////
  // Use these 2 methods because we might change how an entry's sensor is identified
  const char* getSensorID() const;
  SensorMeasurementFunc getMeasurementFunc() const;  
  
  ///////////////////////////////////////////////////////////////////////////////////////////
  // send data message
  // TODO: may want to not transmit here, instead put in buffer?
  void packageDataMessage(float data) const;
  void packageDataMessage(const char *data) const;
  
  // print JSON version of this object
  // pass an index value >= 0 if you want an index to be included in the JSON
  void print(int index = -1) const;

  // returns number of bytes written to EEPROM (longer than the size of this struct)
  int writeToEEPROM(int address);
  
  // returns number of bytes read from EEPROM (longer than the size of this struct)
  int readFromEEPROM(int address);
  
  // pre-allocated array for sensors
  // an entry is unused if it's sensorID is ""
  //static SensorEntry gSensorEntries[NUM_SENSOR_ENTRIES];
  
  //static SensorFunctionMapEntry gSensorFunctionMap[];

  //static int DummySensorMeasurementFunc(const struct SensorEntry *config) { return 0; }
};  


