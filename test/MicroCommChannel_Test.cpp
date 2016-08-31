#include "MicroCommChannel_Test.h"

MicroCommChannel_Test::MicroCommChannel_Test()
{
	Tolerance = 2.0e-03;
	PacketHeader[0] = 0x21;
	PacketHeader[1] = 0x45;
	PacketHeader[2] = 0x4F;
	PacketHeader[3] = 0x53;
}

MicroCommChannel_Test::~MicroCommChannel_Test() {
	/// - Nothing to do

}

void MicroCommChannel_Test::SetUp(){
    /// - Initialize the command packet.
    float testNormalizedVoltage = 0.0;
    TestArticle.Commands.NormalizedVoltage = testNormalizedVoltage;
    TestArticle.Commands.SecondCommand = 1;
    /// - Initialize the telemetry packet.
    float testPosition = 0.0;
    float testVelocity = 0.0;
    TestTelemetry.Position = testPosition;
    TestTelemetry.Velocity = testVelocity;
    TestTelemetry.NewTelemetry = 0.0;
}

void MicroCommChannel_Test::TearDown(){
}

TEST_F(MicroCommChannel_Test, nominalEncode){
    /// - Set some fake telemetry
    float testPosition = 105.3145600;
    float testVelocity = -752.678125100;
    TestTelemetry.Position = testPosition;
    TestTelemetry.Velocity = testVelocity;
    /// - Transmit the packet.
	ASSERT_TRUE(TestArticle.Encode());
	ReadTelemetry();
    /// -  Check that everything worked. The Chup telemetry values
    ///    set above should now be read in to the test telemetry 
    ///    packet.
	ASSERT_NEAR(TestArticle.Telemetry.Position, TestTelemetry.Position, Tolerance);
    ASSERT_NEAR(TestArticle.Telemetry.Velocity, TestTelemetry.Velocity, Tolerance);
}

TEST_F(MicroCommChannel_Test, nominalRx){
    /// - Set some test command values
    float testNormalizedVoltage = -0.67;
    TestCommands.NormalizedVoltage = testNormalizedVoltage;
    /// - Transmit the packet.
    WriteCommands();
    /// -  Check that everything worked. The sent test
    ///    command values should now be read in to the
    ///    command packet of the test article.
	ASSERT_TRUE(TestArticle.Decode());
    ASSERT_NEAR(TestArticle.Commands.NormalizedVoltage, TestCommands.NormalizedVoltage, Tolerance);
}

void MicroCommChannel_Test::WriteToChup(uint8_t* txBuff, int buffStartIndx, int numBytes){
	for(uint8_t indx = 0; indx < numBytes; indx++)
	{
		TestArticle.RxBuffer[indx+buffStartIndx] = txBuff[indx];
	}
}

void MicroCommChannel_Test::ReadFromChup(uint8_t* rxBuff, int buffStartIndx, int numBytes){
	for(uint8_t indx = 0; indx < numBytes; indx++)
	{
		rxBuff[indx] = TestArticle.TxBuffer[indx+buffStartIndx];
	}
}

bool MicroCommChannel_Test::ValidHeader(){
    /// - Emulate reading in the four byte header
    uint8_t rxBuffer[4];
    ReadFromChup(rxBuffer, 0, 4);
    /// - Check that the received header is valid.
   	return rxBuffer[0] == 0x21 &&
  	       rxBuffer[1] == 0x45 &&
   	       rxBuffer[2] == 0x4F &&
   	       rxBuffer[3] == 0x53;
}

bool MicroCommChannel_Test::ValidCrc(uint8_t * rcvdData){
    /// - Emulate reading in the CRC32
    uint8_t rxCrc[COMM_MAX_BUFF_SIZE];
    ReadFromChup(rxCrc, 4+TelemetryPacket_size, 4);
    uint32_t rcvd_crc32 = 0;
    rcvd_crc32 |= rxCrc[3] << 24;
    rcvd_crc32 |= rxCrc[2] << 16;
    rcvd_crc32 |= rxCrc[1] << 8;
    rcvd_crc32 |= rxCrc[0];
    /// - Calculate the CRC32 on the received data
	uint32_t computed_crc32 = CommCrc32::crc32(rcvdData, TelemetryPacket_size, 0);
    /// - Compare calculated CRC32 with received CRC32
    return rcvd_crc32 == computed_crc32;
}

void MicroCommChannel_Test::ReadTelemetry(){
    /// - Emulate reading in the telemetry header on the PC side and 
    ///   unpacking it.
	if (!ValidHeader())
        FAIL() << "Invalid telemetry packet was sent. Bad header.";
    /// - Emulate reading in the telemetry packet on the PC side.
    uint8_t rxBuffer[COMM_MAX_BUFF_SIZE];
    ReadFromChup(rxBuffer, 4, TelemetryPacket_size);
    pb_istream_t stream = pb_istream_from_buffer(rxBuffer, TelemetryPacket_size);
    if (!pb_decode(&stream, TelemetryPacket_fields, &TestTelemetry))
    {
        FAIL() << "Failed to decode the received telemetry.";
    }
	/// - Emulate reading in the CRC32 on the PC side.
	if (!ValidCrc(rxBuffer))
        FAIL() << "Invalid telemetry packet was sent. Checksum failed.";
}

void MicroCommChannel_Test::WriteCommands(){
	/// - Emulate sending down the header
	WriteToChup(PacketHeader, 0, 4);
	/// - Encode the commands
	uint8_t txBuff[COMM_MAX_BUFF_SIZE];
    pb_ostream_t outstream = pb_ostream_from_buffer(txBuff, sizeof(txBuff));
    if(!pb_encode(&outstream, CommandPacket_fields, &TestCommands))
    {
        FAIL() << "Failed to encode the command packet";
    }
	/// - Emulate sending down the commands
	WriteToChup(txBuff, 4, CommandPacket_size);
	/// - Compute the CRC32
    uint32_t computed_crc32 = CommCrc32::crc32(txBuff, CommandPacket_size, 0);
	/// - Pack the computed CRC as 4 bytes
    uint8_t crcBytes[4];
	crcBytes[0] = (computed_crc32 & 0x000000ff);
	crcBytes[1] = (computed_crc32 & 0x0000ff00) >> 8;
	crcBytes[2] = (computed_crc32 & 0x00ff0000) >> 16;
	crcBytes[3] = (computed_crc32 & 0xff000000) >> 24;
	/// - Emulate sending down the CRC32
    WriteToChup(crcBytes, 4 + CommandPacket_size, 4);
}

