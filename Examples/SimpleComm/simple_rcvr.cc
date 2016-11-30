#include <iostream>
#include "UsbReceiver.h"

UsbReceiver SimpleRcvr;
int NUM_TEST_PACKETS_TO_SEND = 500;

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
    	std::cout << "Use the same comm port that is used by the Arduino IDE to upload code to the board. Something like " << argv[0] << " /dev/ttyACM0" << std::endl;
    }else{
	    /// - Connect to the Comm Port
	    SimpleRcvr.CommFrequency = 250;
	    SimpleRcvr.PortName = argv[1];
	    SimpleRcvr.Connect();
	    int flipSign = 1;
	    int sentPackets = 0;
	    if (SimpleRcvr.Connected && SimpleRcvr.Active){
	    	while(sentPackets < NUM_TEST_PACKETS_TO_SEND){
			    /// - Send some sample command values. Alternate the sign
			    SimpleRcvr.NormalizedVoltage = 0.67*flipSign;
			    SimpleRcvr.SecondCommand = 0.54*flipSign;
			    /// - Call the run comm function.
			    SimpleRcvr.RunComm();
			    display_results();
			    flipSign *= -1;
			    sentPackets ++;
	    	}
	    }else{
		    std::cout << "==================================================================" << std::endl;
		    std::cout << " Failed to connect to the Arduino." << std::endl;
		    std::cout << "==================================================================" << std::endl;
	    }

    }
    return 0;
}
