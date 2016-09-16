#include <ProtobuffSerial.h>

ProtobuffSerial serialComm;//initialize an instance of the class

void setup(){/*nothing to setup*/}

void loop(){
  serialComm.blink(200);//stay one second on, then a second off
}
