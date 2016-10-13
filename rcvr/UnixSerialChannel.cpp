#include "UnixSerialChannel.h"

UnixSerialChannel::UnixSerialChannel() :
    CommChannel(),
    PortName("NONE"),
    CommPortHandle(-1)
{
	/// - Nothing to do
}

UnixSerialChannel::~UnixSerialChannel() {
	/// - Nothing to do
}


void UnixSerialChannel::SetupSerialAttributes(int speed, int parity)
{
    struct termios tty;
  	errorLocation = "UnixSerialChannel::SetupSerialAttributes";

    memset(&tty, 0, sizeof tty);
    if (tcgetattr(CommPortHandle, &tty) != 0)
    {
        errorOut.str("");
    	errorOut << "Error " << errno << " from tcgetattr.";
    	errorMessage = errorOut.str();
        reportWarning();
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_iflag &= IGNCR;           // ignore carriage return on input.
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(CommPortHandle, TCSANOW, &tty) != 0)
    {
        //error_message("error %d from tcsetattr", errno);
    }
}

void UnixSerialChannel::ToggleBlocking(int should_block)
{
  	errorLocation = "UnixSerialChannel::ToggleBlocking";
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(CommPortHandle, &tty) != 0)
    {
        //error_message("error %d from tggetattr", errno);
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr(CommPortHandle, TCSANOW, &tty) != 0)
    {
        //error_message("error %d setting term attributes", errno);
    }
}

void UnixSerialChannel::Validate() {
  	errorLocation = "UnixSerialChannel::Validate";
	if(PortName == "NONE")
	{
        errorOut.str("");
        errorOut << "PortName is set to: " << PortName << "." << std::endl
    	         << "PortName attribute must be set to a valid serial port."
    	         << " Try setting to \"/dev/ttyUSB0\"" << std::endl;
    	errorMessage = errorOut.str();
        reportWarning();
	}
    CommChannel::Validate();
}

void UnixSerialChannel::Connect() {
	/// - Call parent class initializaton. This will intialize
	///   the data buffer and call the validate function.
	CommChannel::Connect();
  	errorLocation = "UnixSerialChannel::Connect";
	CommPortHandle = open(PortName.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
	if (CommPortHandle < 0)
	{
        errorOut.str("");
        errorOut << "Error " << errno << " opening " << PortName << ": " << strerror(errno);
    	errorMessage = errorOut.str();
        reportWarning();
        /// - Failed to connect... Set the Connected flag false
        Connected = false;
        Active = false;
        return;
	}
    /// - Set speed to 115,200 bps, 8n1 (no parity)
	SetupSerialAttributes(B115200, 0);
	/// - Set no blocking
	ToggleBlocking(0);
    /// - Successfully connected... Set the Connected flag
    Connected = true;
    Active = true;
}

void UnixSerialChannel::Tx() {
    /// - String used for error reporting
    errorLocation = "UnixSerialChannel::Tx";
    /// - Send the packet header.
    write(CommPortHandle, &PacketHeader, 4);
    /// - Serialize the commands to a char array..
    if (!Commands.SerializeToArray(TxBuffer, Commands.ByteSize())) {
        ClearBuffersAndReset();
        return;
    }
    /// - Send the command packet contents.
    write(CommPortHandle, TxBuffer, Commands.ByteSize());
    /// - Compute and send the CRC checksum.
    uint32_t tx_crc32 = CommCrc32::crc32(TxBuffer, Commands.ByteSize(), 0);
    write(CommPortHandle, &tx_crc32, 4);
}

void UnixSerialChannel::Rx() {
    /// - String used for error reporting
    errorLocation = "UnixSerialChannel::Rx";
    /// - Read the raw header data from the serial port.
    int bytes_read;
    if (!ValidHeader())
    {
        ClearBuffersAndReset();
        errorOut.str("");
        errorOut << "Error cycling ChupComm on port : " << PortName << std::endl;
        errorOut << "Header packet not received." << std::endl;
        errorMessage = errorOut.str();
        reportWarning();
        return;
    }
    /// - Read the raw telemetry data from the serial port.
    if((bytes_read = read(CommPortHandle, RxBuffer, Telemetry.ByteSize())) < 0)
    {
        ClearBuffersAndReset();
        errorOut.str("");
        errorOut << "Error cycling ChupComm on port : " << PortName << std::endl;
        errorOut << "Telemetry packet read failure." << std::endl;
        errorMessage = errorOut.str();
        reportWarning();
        return;
    }else if(bytes_read == 0)
    {
        ClearBuffersAndReset();
        errorOut.str("");
        errorOut << "Error cycling ChupComm on port : " << PortName << std::endl;
        errorOut << "Telemetry packet not received." << std::endl;
        errorMessage = errorOut.str();
        reportWarning();
        return;
    }
    /// - Unpack the telemetry data.
    if (!Telemetry.ParseFromArray(RxBuffer, Telemetry.ByteSize())) {
        ClearBuffersAndReset();
        errorOut.str("");
        errorOut << "Error cycling ChupComm on port : " << PortName << std::endl;
        errorOut << "Telemetry packet unpack failure." << std::endl;
        errorMessage = errorOut.str();
        reportWarning();
        return;
    }
    /// - Check the CRC
    if (!ValidCrc())
    {
        ClearBuffersAndReset();
        errorOut.str("");
        errorOut << "Error cycling ChupComm on port : " << PortName << std::endl;
        errorOut << "CRC failure." << std::endl;
        errorMessage = errorOut.str();
        reportWarning();
        return;
    }
}

bool UnixSerialChannel::ValidHeader(){
    int bytes_read;
    if((bytes_read = read(CommPortHandle, RxBuffer, 4)) < 0)
        return false;

    if(bytes_read == 4){
        return RxBuffer[0] == 0x21 &&
               RxBuffer[1] == 0x45 &&
               RxBuffer[2] == 0x4F &&
               RxBuffer[3] == 0x53;
    }
    return false;
}

bool UnixSerialChannel::ValidCrc(){
    /// - Compute the CRC on the Telemetry packet we received.
    uint32_t computed_crc32 = CommCrc32::crc32(RxBuffer, Telemetry.ByteSize(), 0);
    /// - Now read the CRC that was sent.
    int bytes_read;
    if((bytes_read = read(CommPortHandle, RxBuffer, 4)) < 0)
        return false;

    uint32_t rcvd_crc32 = 0;
    rcvd_crc32 |= RxBuffer[3] << 24;
    rcvd_crc32 |= RxBuffer[2] << 16;
    rcvd_crc32 |= RxBuffer[1] << 8;
    rcvd_crc32 |= RxBuffer[0];

    if(bytes_read == 4){
        return rcvd_crc32 == computed_crc32;
    }
    return false;
}

void UnixSerialChannel::reportWarning(){
    MessageHandler::warn(__FILE__, __LINE__, errorLocation.c_str(), errorMessage.c_str());
}