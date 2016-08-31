#include <ProtobuffSerial.h>

ProtobuffSerial serialComm;//initialize an instance of the class
unsigned long delayTime = 100;

void setup(){
  serialComm.InitHw();
}

void loop(){
  serialComm.RunComm();
  delay(delayTime)
}
