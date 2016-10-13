#include "ProtobuffSerial.h"

ProtobuffSerial::ProtobuffSerial() : MicroCommChannel()
{
	RxByteCounter = 0;
	TxByteCounter = 0;
}

ProtobuffSerial::~ProtobuffSerial() {
	/// - Nothing to do
}

int ProtobuffSerial::InitHw() {
	Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }
	return SUCCESS;
}

int ProtobuffSerial::ReadPacket() {
    while (Serial.available() > 0) {
	    // read the incoming byte:
	    RxBuffer[RxByteCounter++] = Serial.read();
    }
	if (RxByteCounter >= CommandPacket_size + 8){
	 	RxByteCounter = 0;
	 	return RX_PACKET_READY;
	}
	return RX_READING_PACKET;
}

int ProtobuffSerial::WritePacket() {
    Serial.write(TxBuffer, TelemetryPacket_size + 8);
    return TX_PACKET_SUCCESS;
}

bool ProtobuffSerial::ValidCrc(){
    /// - Compute the CRC on the command packet we received.
    uint32_t computed_crc32 = CommCrc32::crc32(&RxBuffer[4], CommandPacket_size, 0);
    /// - Unload the CRC32 that was sent over the wire.
    uint32_t rcvd_crc32 = 0;
    uint16_t crcStartIndx = 4 + CommandPacket_size;
    uint16_t lowerWord = (RxBuffer[crcStartIndx+1] << 8) | RxBuffer[crcStartIndx];
    uint16_t upperWord = (RxBuffer[crcStartIndx+3] << 8) | RxBuffer[crcStartIndx+2];
    rcvd_crc32 = ((uint32_t) upperWord << 16) | lowerWord;
    /// - Check received CRC with what was computed
    return rcvd_crc32 == computed_crc32;
}
