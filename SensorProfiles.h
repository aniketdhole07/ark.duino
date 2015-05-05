

// Add your own measurement function and sensor IDs here.
// Make sure to include the header file that contains the measurement function.
// This is not a traditional header file; it's intended to be included at the top of SensorManager.cpp.

//#include "SensorManager.h"

#include "AnalogSensor.h"
#include "EnvTmp.h"
#include "DFRobot_pH.h"
#include "ManyLabs_pH.h"
#include "AtlasScientific.h"
//#include "profiles/DS18B20.h"

// pins 0 and 1 are mux 'selector' pins (0=B, 1=B)
// pin 2 is the mux 'address', which is broken into bits and sent to pins 0 and 1
int read_Atlas_Circuit(const struct SensorEntry *entry);

const SensorFunctionMapEntry SensorManager::sensorFunctionMap[] = {
  {"ANALOG",          read_analog},
  {"ENV-TMP",         read_tmp_env},
  {"DFROBOT-PH",      read_DFRobot_pH},
  {"MANYLABS-PH",     read_ManyLabs_pH},
  {"ATLAS-CIRCUIT",   read_Atlas_Circuit}
};

