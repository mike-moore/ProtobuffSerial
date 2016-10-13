#include <ProtobuffSerial.h>

ProtobuffSerial serialComm;    // - Initialize an instance of the protbuff serial class to do communication

void setup(){
  serialComm.InitHw();
}

void loop(){
  serialComm.RunComm();
  // - Simulates acting on a received command in a way that effects
  //   the telemetry sent back to the Arduino.
  performControl();
}

void performControl(){
  if (serialComm.Commands.NormalizedVoltage < 0){
    serialComm.Telemetry.Position = -1.0;
    serialComm.Telemetry.Velocity = -2.0;
  }else{
    serialComm.Telemetry.Position = 1.0; 	
    serialComm.Telemetry.Velocity = 2.0;
  }
}
