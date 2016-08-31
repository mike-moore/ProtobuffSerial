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
	return SUCCESS;
}

int ProtobuffSerial::ReadPacket() {
	// short rx_fifo_bytes = SciaRegs.SCIFFRX.bit.RXFFST;
 //    if(rx_fifo_bytes > 0) {
 //    	for(int indx = 0; indx < rx_fifo_bytes; indx++){
	// 		RxBuffer[RxByteCounter++] = SciaRegs.SCIRXBUF.all;
 //    	}
 //    }
	// if (RxByteCounter >= CommandPacket_size + 8){
	// 	RxByteCounter = 0;
	// 	return RX_PACKET_READY;
	// }
	return RX_READING_PACKET;
}

int ProtobuffSerial::WritePacket() {
	// while(TxByteCounter < TelemetryPacket_size + 8){
	// 	WriteByte();
	// }
	// TxByteCounter = 0;
    return TX_PACKET_SUCCESS;
}

void ProtobuffSerial::WriteByte() {
    // while (SciaRegs.SCICTL2.bit.TXRDY == 0) {}
    // SciaRegs.SCITXBUF = TxBuffer[TxByteCounter];
    // TxByteCounter++;
}

bool ProtobuffSerial::ValidCrc(){
    // /// - Compute the CRC on the command packet we received.
    // uint32_t computed_crc32 = CommCrc32::crc32(&RxBuffer[4], CommandPacket_size, 0);
    // /// - Unload the CRC32 that was sent over the wire.
    // uint32_t rcvd_crc32 = 0;
    // uint16_t crcStartIndx = 4 + CommandPacket_size;
    // uint16_t lowerWord = (RxBuffer[crcStartIndx+1] << 8) | RxBuffer[crcStartIndx];
    // uint16_t upperWord = (RxBuffer[crcStartIndx+3] << 8) | RxBuffer[crcStartIndx+2];
    // rcvd_crc32 = ((uint32_t) upperWord << 16) | lowerWord;
    // /// - Check received CRC with what was computed
    // return rcvd_crc32 == computed_crc32;
}
