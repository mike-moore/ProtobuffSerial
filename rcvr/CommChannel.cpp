#include "CommChannel.h"

CommChannel::CommChannel()
  :
  Connected(false),
  Active(false),
  CommFrequency(50),
  CommFailureCounter(0),
  PacketHeader(0x534F4521),
  CommFailureFlag(false),
  errorOut(),
  errorLocation(""),
  errorMessage("")
{
	ClearBuffers();
	/// - Initialize telemetry packet.
	Telemetry.set_position(0.0);
	Telemetry.set_velocity(0.0);
	Telemetry.set_newtelemetry(0.0);
}

CommChannel::~CommChannel()
{
	/// - Nothing to do
}

void CommChannel::Connect()
{
	/// - Nothing to do in base class except validate settings.
	Validate();
}

void CommChannel::Validate()
{
  	errorLocation = "CommChannel::Validate";
	if(CommFrequency < MIN_COMM_FREQ || CommFrequency > MAX_COMM_FREQ)
	{
        errorOut << "CommFrequency is set to: " << CommFrequency << "." << std::endl
    	         << "CommFrequency attribute must be set to a value between "
    	         << MIN_COMM_FREQ << " and " << MAX_COMM_FREQ << " Hz." << std::endl;
    	errorMessage = errorOut.str();
        MessageHandler::fail(__FILE__, __LINE__, errorLocation.c_str(), errorMessage.c_str());
	}
}

void CommChannel::TxAndRx()
{
    while(Connected && Active){
	    /// - Transimt and then sleep half of a communication frame.
    	Tx();
	    sleep_ms(1.0e3/(2*CommFrequency)); // specified in milli seconds.
	    /// - Receieve and then sleep half of a communication frame.
    	Rx();
	    sleep_ms(1.0e3/(2*CommFrequency));
    }
}

std::thread CommChannel::CreateCommThread()
{
    /// - Construct the thread for transmitting and receiving packets.
    return std::thread([=] { TxAndRx(); });
}

void CommChannel::Tx()
{
	/// - Nothing to do.
}

void CommChannel::Rx()
{
	/// - Nothing to do.
}

void CommChannel::ClearBuffersAndReset()
{
	CommFailureFlag = true;
	ClearBuffers();
}

void CommChannel::ClearBuffers(){
	for (int indx = 0; indx < COMM_MAX_BUFF_SIZE; indx++)
	{
		TxBuffer[indx] = 0x00;
		RxBuffer[indx] = 0x00;
	}
}

void CommChannel::sleep_ms(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}