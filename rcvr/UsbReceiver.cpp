#include "UsbReceiver.h"

UsbReceiver::UsbReceiver() :
    UnixSerialChannel()
{
    NormalizedVoltage = 0.0;
    SecondCommand = 0;
    Position = 0.0;
    Velocity = 0.0;
    NewTelemetry = 0.0;
    RxWaitCycles = 4;
    ResetWaitCycles = 4;
}

UsbReceiver::~UsbReceiver() {
	/// - Nothing to do
}

void UsbReceiver::RunComm()
{
    /// - String used for error reporting
    errorLocation = "UsbReceiver::RunComm";
    /// - Start with no failure. Failure flag is only set if we have to 
    ///   clear buffers and reset on Rx fail. See the ClearBuffersAndReset
    ///   function in the CommChannel base class.
    CommFailureFlag = false;
    /// - Limit range on normalized voltage to -1.0 to 1.0
    Commands.set_normalizedvoltage(clip(NormalizedVoltage, -1.0, 1.0));
    Commands.set_secondcommand(SecondCommand);
    if(Connected && Active){
        /// - Transmit.
        Tx();
        /// - Give the USB device time to respond.
        WaitToRx();
        /// - Receive.
        Rx();
    }else{
        errorOut.str("");
        errorOut << "Error cycling UsbReceiver on port : " << PortName << std::endl;
        errorOut << "Comm is not connected and active." << std::endl;
        errorMessage = errorOut.str();
        reportWarning();
        return;
    }
    /// - Unpack telemetry as long as there were no comm failures.
    if (!CommFailureFlag){
        Position = Telemetry.position();
        Velocity = Telemetry.velocity();
        NewTelemetry = Telemetry.newtelemetry();
    }else{
        /// - Comm failure on PC side. Give the USB receiver time to detect it and reset its
        ///   own Tx and Rx buffers.
        CommFailureCounter++;
        usleep(ResetWaitCycles * (1.0e6/CommFrequency));
    }
}

void UsbReceiver::Tx() {
    /// - String used for error reporting
    errorLocation = "UsbReceiver::Tx";
    /// - Send the packet header.
    write(CommPortHandle, &PacketHeader, 4);
    usleep(1.0e6/(CommFrequency));
    /// - Serialize the commands to a char array..
    if (!Commands.SerializeToArray(TxBuffer, Commands.ByteSize())) {
        errorOut.str("");
        errorOut << "Error writing command packet to : " << PortName;
        errorMessage = errorOut.str();
        reportWarning();
    }
    /// - Now compute the number of times we will have to send 4 bytes down
    ///   and the remainder bytes.
    int numBytes = (int) (Commands.ByteSize());
    for (int bytesSent = 0; bytesSent < numBytes; bytesSent += 4){
        if((numBytes - bytesSent) >= 4)
        {
            write(CommPortHandle, &TxBuffer[bytesSent], 4);
        }
        else
        {
            write(CommPortHandle, &TxBuffer[bytesSent], numBytes - bytesSent);
        }
        usleep(1.0e6/(CommFrequency));
    }
    /// - Compute CRC32
    uint32_t tx_crc32 = CommCrc32::crc32(TxBuffer, Commands.ByteSize(), 0);
    /// - Filler bytes used to tack on the end of transmission if needed.
    write(CommPortHandle, &tx_crc32, 4);
}

float UsbReceiver::clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

void UsbReceiver::WaitToRx() {
    usleep(RxWaitCycles * (1.0e6/CommFrequency));
}