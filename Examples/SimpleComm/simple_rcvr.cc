#include <iostream>
#include "UsbReceiver.h"

UsbReceiver SimpleRcvr;

void display_results(){
    if (!SimpleRcvr.CommFailure()){
	    std::cout << "==================================================================" << std::endl;
	    std::cout << " Data received from Arduino! " << std::endl;
	    std::cout << " Position " << SimpleRcvr.Position << std::endl;
	    std::cout << " Velocity " << SimpleRcvr.Velocity << std::endl;
	    std::cout << "==================================================================" << std::endl;
    }else{
	    std::cout << "==================================================================" << std::endl;
	    std::cout << " Failed to receive a valid comm packet from the Arduino." << std::endl;
	    std::cout << "==================================================================" << std::endl;
    }
}

int main( int argc, char *argv[] )
{
    if (argc != 2){
    	std::cout << "Improper usage of " << argv[0] << ". Usage: " << argv[0] << " /comm/port/to/use" << std::endl;
    	std::cout << "Use the same comm port that is used by the Arduino IDE to upload code to the board. Something like " << argv[0] << " /dev/ttyACMO" << std::endl;
    }else{
	    /// - Connect to the Comm Port
	    SimpleRcvr.CommFrequency = 100.0;
	    SimpleRcvr.PortName = argv[1];
	    SimpleRcvr.Connect();
	    if (SimpleRcvr.Connected && SimpleRcvr.Active){
		    /// - Send some sample command values
		    SimpleRcvr.NormalizedVoltage = -0.67;
		    /// - Call the run comm function.
		    SimpleRcvr.RunComm();
		    display_results(); // Telemtry values will still be positive until the next transfer
		                       // 1 frame of comm delay between when a command is sent and when
		                       // the sent back telemetry will reflect the Arduino's response.

		    SimpleRcvr.RunComm(); // called twice to allow the simple control logic to execute 
		                          // and effect a change in the telemetry

		    display_results(); // Now the telemtry values will swap to negative.
	    }else{
		    std::cout << "==================================================================" << std::endl;
		    std::cout << " Failed to connect to the Arduino." << std::endl;
		    std::cout << "==================================================================" << std::endl;
	    }

    }
    return 0;
}
