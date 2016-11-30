#include <ProtobuffSerial.h>

ProtobuffSerial serialComm;          // - Initialize an instance of the protbuff serial class to do communication
unsigned long cycleTimeMillis = 1;   // - Loop rate in milliseconds.

void setup(){
  serialComm.LoopFrequency = 1/(cycleTimeMillis/1000);
  serialComm.CommFrequency = 250; // - Must match what's configured on Usb Receiver side.
  serialComm.InitHw();
}

void loop(){
  serialComm.RunComm();
  // - Simulates acting on a received command in a way that effects
  //   the telemetry sent back to the Arduino.
  performControl();
  delay(cycleTimeMillis); // Delay for the control loop period. Milliseconds
}

void performControl(){
  if (serialComm.Commands.NormalizedVoltage < 0){
    serialComm.Telemetry.Position = -1.0;
    serialComm.Telemetry.Velocity = -2.0;
    serialComm.Telemetry.NewTelemetry = -3.0;
  }else{
    serialComm.Telemetry.Position = 1.0; 	
    serialComm.Telemetry.Velocity = 2.0;
    serialComm.Telemetry.NewTelemetry = 3.0;
  }
}
