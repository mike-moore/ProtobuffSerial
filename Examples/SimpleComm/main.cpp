#include <iostream>
#include "UsbReceiver.h"

UsbReceiver SimpleRcvr;

int main( int argc, char *argv[] )
{
    if (argc != 2){
    	std::cout << "Improper usage of " << argv[0] << ". Usage: ./" << argv[0] << " /comm/port/to/use" << std::endl;
    	std::cout << "Use the same comm port that is used by the Arduino IDE to upload code to the board. Something like /dev/tty*" << std::endl;
    }else{
	    /// - Connect to the Comm Port
	    SimpleRcvr.CommFrequency = 100.0;
	    SimpleRcvr.PortName = argv[1];
	    SimpleRcvr.Connect();
	    /// - Send some sample command values
	    float testNormalizedVoltage = -0.67;
	    SimpleRcvr.NormalizedVoltage = testNormalizedVoltage;
	    SimpleRcvr.SecondCommand = 1;
	    /// - Call the run comm function.
	    SimpleRcvr.RunComm();
    }
    return 0;
}