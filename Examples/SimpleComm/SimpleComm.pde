#include <ProtobuffSerial.h>

ProtobuffSerial serialComm;    // - Initialize an instance of the protbuff serial class to do communication
unsigned long delayTime = 100; // - Comm cycle time in milliseconds

void setup(){
  serialComm.InitHw();
}

void loop(){
  serialComm.RunComm();
  delay(delayTime)
}
