#include "MicroCommChannel.h"

MicroCommChannel::MicroCommChannel()
{
    /// - Start off in RECEIVING state.
    ActiveState = RECEIVING;
    PacketHeader = 0x534F4521;
    TxCrc32 = 0x0000;
    /// - Initialize the buffer to all zeros.
    for (int indx = 0; indx < COMM_MAX_BUFF_SIZE; indx++)
    {
        TxBuffer[indx] = 0x00;
        RxBuffer[indx] = 0x00;
    }
}

MicroCommChannel::~MicroCommChannel() {
	/// - Nothing to do
}

int MicroCommChannel::RunComm() {
    int rx_status, tx_status;

    switch(ActiveState)
    {
        case RECEIVING:
			rx_status = ReadPacket();
			if(rx_status == RX_PACKET_READY){
				ActiveState = TRANSMITTING;
			}else if (rx_status == RX_READING_PACKET){
				return RX_READING_PACKET;
			}else if (rx_status == RX_PACKET_FAIL){
				return RX_PACKET_FAIL;
			}
        break;

        case TRANSMITTING:
            /// - First, decode the received commands.
            if (!Decode()){
                ActiveState = RECEIVING;
                return UNLOAD_FAIL;
            }
            /// - Next, encode the telemetry.
            if (!Encode()){
                ActiveState = RECEIVING;
                return LOAD_FAIL;
            }
            /// - Now, write the packet out to the channel
            tx_status = WritePacket();
            if(tx_status == TX_PACKET_SUCCESS){
                ActiveState = RECEIVING;
                return SUCCESS;
            }else if(tx_status == TX_PACKET_FAIL){
                ActiveState = RECEIVING;
                return TX_PACKET_FAIL;
            }
            ActiveState = RECEIVING;
        break;

        default:
            /// - invalid state - go back to RECEIVING
            ActiveState = RECEIVING;
        break;
    }
    /// - Successful cycle through the comm state machine.
    return SUCCESS;
}

int MicroCommChannel::InitHw() {
    /// - Derived class needs to implement this routine.
    return HW_INIT_FAIL;
}

int MicroCommChannel::ReadPacket() {
    /// - Derived class needs to implement this routine.
    return RX_PACKET_FAIL;

}

int MicroCommChannel::WritePacket() {
    /// - Derived class needs to implement this routine.
    return TX_PACKET_FAIL;
}

bool MicroCommChannel::Encode() {
    /// - Write the packet header to the Tx buffer.
    WriteHeader();
    /// - Create a stream to encode the Tx buffer.
    pb_ostream_t outstream = pb_ostream_from_buffer(&TxBuffer[4], sizeof(TxBuffer));
    /// - Encode the Tx buffer.
    if(!pb_encode(&outstream, TelemetryPacket_fields, &Telemetry))
    {
        return false;
    }
    /// - Compute and write the CRC32 to the end of the Tx buffer.
    TxCrc32 = CommCrc32::crc32(&TxBuffer[4], outstream.bytes_written, 0);
    WriteCrc32();
    /// - Tx Buffer is now ready to be written over the SCI channel.
    return true;
}

bool MicroCommChannel::Decode() {
    /// - SCI hardware must fill the RxBuffer with header, data, and crc32
    ///   before this function is called.
    if (!ValidHeader())
    {
        return false;
    }
    /// - Create a stream that reads from the receive buffer... decode from
    ///   byte 5 of the Rx Buffer to command packet size
    pb_istream_t stream = pb_istream_from_buffer(&RxBuffer[4], CommandPacket_size);
    /// - Decode the command packet from the RxBuffer
    if (!pb_decode(&stream, CommandPacket_fields, &Commands))
    {
        return false;
    }
    /// - Check the CRC
    if (!ValidCrc())
    {
        return false;
    }
    return true;
}

void MicroCommChannel::WriteHeader() {
    TxBuffer[0] = (PacketHeader & 0x000000ff);
    TxBuffer[1] = (PacketHeader & 0x0000ff00) >> 8;
    TxBuffer[2] = (PacketHeader & 0x00ff0000) >> 16;
    TxBuffer[3] = (PacketHeader & 0xff000000) >> 24;
}

void MicroCommChannel::WriteCrc32() {
    uint_least8_t crcStartIndx = 4 + TelemetryPacket_size;
    TxBuffer[crcStartIndx] = (TxCrc32 & 0x000000ff);
    TxBuffer[crcStartIndx+1] = (TxCrc32 & 0x0000ff00) >> 8;
    TxBuffer[crcStartIndx+2] = (TxCrc32 & 0x00ff0000) >> 16;
    TxBuffer[crcStartIndx+3] = (TxCrc32 & 0xff000000) >> 24;
}

bool MicroCommChannel::ValidHeader(){
    return RxBuffer[0] == 0x21 &&
           RxBuffer[1] == 0x45 &&
           RxBuffer[2] == 0x4F &&
           RxBuffer[3] == 0x53;
}

bool MicroCommChannel::ValidCrc(){
    /// - Compute the CRC on the command packet we received.
    uint32_t computed_crc32 = CommCrc32::crc32(&RxBuffer[4], CommandPacket_size, 0);
    /// - Unload the CRC32 that was sent over the wire.
    uint32_t rcvd_crc32 = 0;
    uint_least8_t crcStartIndx = 4 + CommandPacket_size;
    rcvd_crc32 |= RxBuffer[crcStartIndx+3] << 24;
    rcvd_crc32 |= RxBuffer[crcStartIndx+2] << 16;
    rcvd_crc32 |= RxBuffer[crcStartIndx+1] << 8;
    rcvd_crc32 |= RxBuffer[crcStartIndx];
    /// - Check received CRC with what was computed
    return rcvd_crc32 == computed_crc32;
}
