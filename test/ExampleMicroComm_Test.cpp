#include "ExampleMicroComm_Test.h"

ExampleMicroComm_Test::ExampleMicroComm_Test()
{
	Tolerance = 2.0e-03;
}

ExampleMicroComm_Test::~ExampleMicroComm_Test() {
	/// - Nothing to do
}

void ExampleMicroComm_Test::SetUp(){
    /// -  Clear all test flags
    TestArticle.FailHwInit = false;
    TestArticle.BytesAvailable = false;
    TestArticle.FailRx = false;
    TestArticle.FailTx = false;
}

void ExampleMicroComm_Test::TearDown(){
    /// - Nothing to do
}

TEST_F(ExampleMicroComm_Test, nominalInit){
    /// - Should start in the RECEIVING state.
    ASSERT_TRUE(TestArticle.CommState() == MicroCommChannel::RECEIVING);
    /// - Initialize the hardware
    ASSERT_TRUE(TestArticle.InitHw() == MicroCommChannel::SUCCESS);
}

TEST_F(ExampleMicroComm_Test, hwInitFail){
    /// - Set the fail hw init flag to true in FriendlyExampleMicroComm.
    TestArticle.FailHwInit = true;
    /// - Initialize the hardware
    ASSERT_TRUE(TestArticle.InitHw() == MicroCommChannel::HW_INIT_FAIL);
}

TEST_F(ExampleMicroComm_Test, rxBytesNominal){
    /// - Should be RECEIVING.
    ASSERT_TRUE(TestArticle.CommState() == MicroCommChannel::RECEIVING);
    /// - Emulate bytes being transmitted in over the wire.
    TestArticle.BytesAvailable = true;
    TestArticle.RunComm();
    /// - Should now be in state: TRANSMITTING.
    ASSERT_TRUE(TestArticle.CommState() == MicroCommChannel::TRANSMITTING);
}

TEST_F(ExampleMicroComm_Test, rxBytesFail){
    /// - Emulate bytes being transmitted in over the wire.
    TestArticle.BytesAvailable = true;
    /// - Emulate a failed receive
    TestArticle.FailRx = true;
    ASSERT_TRUE(TestArticle.RunComm() == MicroCommChannel::RX_PACKET_FAIL);
    /// - Should transition back to RECEIVING
    ASSERT_TRUE(TestArticle.CommState() == MicroCommChannel::RESETTING_COMM);
}

TEST_F(ExampleMicroComm_Test, unloadFail){
    /// - Emulate bytes being transmitted in over the wire.
    TestArticle.BytesAvailable = true;
    TestArticle.RunComm();
    /// - Emulate failing the header.
    TestArticle.FailHeader();
    /// - During this call we should be in transmitting state and will 
    ///   attempt the unloading
    ASSERT_TRUE(TestArticle.RunComm() == MicroCommChannel::UNLOAD_FAIL);
    ASSERT_TRUE(TestArticle.CommState() == MicroCommChannel::RECEIVING);
}

TEST_F(ExampleMicroComm_Test, txNominal){

    /// - Emulate bytes being transmitted in over the wire.
    TestArticle.BytesAvailable = true;
    /// - After this call we should be tx'n bytes
    TestArticle.RunComm();
    /// - During this call we should successfully tx the tlm.
    ASSERT_TRUE(TestArticle.RunComm() == MicroCommChannel::SUCCESS);
    /// - That's the full comm loop. Should be back RECEIVING
    ASSERT_TRUE(TestArticle.CommState() == MicroCommChannel::RECEIVING);
}

TEST_F(ExampleMicroComm_Test, txFail){
    /// - Emulate bytes being transmitted in over the wire.
    TestArticle.BytesAvailable = true;
    /// - After this call we should be tx'n bytes
    TestArticle.RunComm();
    /// - Emulate a Tx failure
    TestArticle.FailTx = true;
    /// - We should fail to transmit during this call.
    ASSERT_TRUE(TestArticle.RunComm() == MicroCommChannel::TX_PACKET_FAIL);
    /// - Should be back RECEIVING
    ASSERT_TRUE(TestArticle.CommState() == MicroCommChannel::RECEIVING);
}

FriendlyExampleMicroComm::FriendlyExampleMicroComm()
{
    PacketHeader[0] = 0x21;
    PacketHeader[1] = 0x45;
    PacketHeader[2] = 0x4F;
    PacketHeader[3] = 0x53;
    FailHwInit = false;
    BytesAvailable = false;
    FailRx = false;
    FailTx = false;
}

FriendlyExampleMicroComm::~FriendlyExampleMicroComm()
{
    /// - Nothing to do.
}

int FriendlyExampleMicroComm::InitHw(){
    if (FailHwInit){
        return MicroCommChannel::HW_INIT_FAIL;
    }
    return MicroCommChannel::SUCCESS;
}

bool FriendlyExampleMicroComm::ReadBytesAvailable(){
    return BytesAvailable;
}

int FriendlyExampleMicroComm::ReadPacket(){
    /// - Emulate commands being sent from the PC side and the device
    ///   reading them in. This function call populates the RxBuffer 
    ///   just like how the real ReadPacket function on the device side
    ///   will work.
    ReadCommands();
    if (FailRx){
        return MicroCommChannel::RX_PACKET_FAIL;
    }
    return MicroCommChannel::RX_PACKET_READY;
}

int FriendlyExampleMicroComm::WritePacket(){
    if (FailTx){
        return MicroCommChannel::TX_PACKET_FAIL;
    }
    return MicroCommChannel::TX_PACKET_SUCCESS;
}

void FriendlyExampleMicroComm::WriteToDevice(uint8_t* txBuff, int buffStartIndx, int numBytes){
    for(uint8_t indx = 0; indx < numBytes; indx++)
    {
        RxBuffer[indx+buffStartIndx] = txBuff[indx];
    }
}

void FriendlyExampleMicroComm::ReadCommands(){
    /// - Emulate sending down the header
    WriteToDevice(PacketHeader, 0, 4);
    /// - Encode the commands
    uint8_t txBuff[COMM_MAX_BUFF_SIZE];
    pb_ostream_t outstream = pb_ostream_from_buffer(txBuff, sizeof(txBuff));
    if(!pb_encode(&outstream, CommandPacket_fields, &TestCommands))
    {
        FAIL() << "Failed to encode the command packet";
    }
    /// - Emulate sending down the commands
    WriteToDevice(txBuff, 4, CommandPacket_size);
    /// - Compute the CRC32
    uint32_t computed_crc32 = CommCrc32::crc32(txBuff, CommandPacket_size, 0);
    /// - Pack the computed CRC as 4 bytes
    uint8_t crcBytes[4];
    crcBytes[0] = (computed_crc32 & 0x000000ff);
    crcBytes[1] = (computed_crc32 & 0x0000ff00) >> 8;
    crcBytes[2] = (computed_crc32 & 0x00ff0000) >> 16;
    crcBytes[3] = (computed_crc32 & 0xff000000) >> 24;
    /// - Emulate sending down the CRC32
    WriteToDevice(crcBytes, 4 + CommandPacket_size, 4);
}

void FriendlyExampleMicroComm::FailHeader(){
    RxBuffer[0] = 0xFF;
}
