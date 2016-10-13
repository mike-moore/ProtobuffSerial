#include "UnixSerialChannel_Test.h"

UnixSerialChannel_Test::UnixSerialChannel_Test()
{
	VirtualCommPortHandle = 1;
	VirtualCommPortName = "";
	Tolerance = 2.0e-03;
	PacketHeader = 0x534F4521;
}

UnixSerialChannel_Test::~UnixSerialChannel_Test() {
	/// - Nothing to do
}

TEST_F(UnixSerialChannel_Test,testConnectNoPortName){
    /// - Test not setting the PortName
    TestArticle.Connect();
    ASSERT_FALSE(TestArticle.Connected);
    ASSERT_FALSE(TestArticle.Active);
}

void UnixSerialChannel_Test::SetUp(){
    /// - Open up a virtual comm port for testing.
    if((VirtualCommPortHandle = getpt()) == -1)
        FAIL() << "Cannot open virtual comm stream";
            
    if(grantpt(VirtualCommPortHandle) == -1)
        FAIL() << "Cannot grant virtual comm stream";
            
    if(unlockpt(VirtualCommPortHandle) == -1)
        FAIL() << "Cannot unlock virtual comm stream";

    VirtualCommPortName = ptsname(VirtualCommPortHandle);

    std::cout << "Opened virtual comm port at: " << VirtualCommPortName
              << std::endl;
    /// - Initialize the command packet.
    TestCommands.set_normalizedvoltage(0.0); 
    TestCommands.set_secondcommand(0); 
    /// - Initialize the telemetry packet.
    TestTelemetry.set_position(0.0);
    TestTelemetry.set_velocity(0.0);
    TestTelemetry.set_newtelemetry(0.0);
}

void UnixSerialChannel_Test::TearDown(){
    close(VirtualCommPortHandle);
}

TEST_F(UnixSerialChannel_Test,nominalConnect){
    /// - Connect to the Virtual Comm Port
    TestArticle.CommFrequency = 100.0;
    TestArticle.PortName = VirtualCommPortName;
    TestArticle.Connect();
    ASSERT_TRUE(TestArticle.Connected);
}

TEST_F(UnixSerialChannel_Test,nominalTx){
    /// - Connect to the Virtual Comm Port
    TestArticle.CommFrequency = 100.0;
    TestArticle.PortName = VirtualCommPortName;
    TestArticle.Connect();
    /// - Set the command values to some test values.
    float testNormalizedVoltage = -0.67;
    TestArticle.Commands.set_normalizedvoltage(testNormalizedVoltage);
    TestArticle.Commands.set_secondcommand(0.0);
    /// - Transmit the packet.
    TestArticle.Tx();
    /// - Read the command packet sent by Unix serial comm driver.
    ReadCommands();
    /// - Test the the commands were received.
    ASSERT_NEAR(TestCommands.normalizedvoltage(), testNormalizedVoltage, Tolerance);
}

TEST_F(UnixSerialChannel_Test,nominalRx){
    /// - Connect to the Virtual Comm Port
    TestArticle.CommFrequency = 100.0;
    TestArticle.PortName = VirtualCommPortName;
    TestArticle.Connect();
    /// - Set some fake telemetry
    float testPosition = 105.3145600;
    float testVelocity = -752.678125100;
    TestTelemetry.set_position(testPosition);
    TestTelemetry.set_velocity(testVelocity);
    TestTelemetry.set_newtelemetry(0.0);
    /// - Wrte the telemetry packet to the Unix serial comm driver.
    WriteTelemetry();
    TestArticle.Rx();
    /// - Test that the telemetry was received on the PC side
    ASSERT_NEAR(TestArticle.Telemetry.position(), testPosition, Tolerance);
    ASSERT_NEAR(TestArticle.Telemetry.velocity(), testVelocity, Tolerance);
}

TEST_F(UnixSerialChannel_Test,runComm){
    /// - Connect to the Virtual Comm Port
    TestArticle.CommFrequency = 1000.0;
    TestArticle.PortName = VirtualCommPortName;
    TestArticle.Connect();
    /// - Set the command values within the test article to some test values.
    float testNormalizedVoltage = -0.67;
    TestArticle.Commands.set_normalizedvoltage(testNormalizedVoltage);
    TestArticle.Commands.set_secondcommand(0.0);

    /// - Set some fake telemetry
    float testPosition = 0.0;
    float testVelocity = 0.0;
    TestTelemetry.set_position(testPosition);
    TestTelemetry.set_velocity(testVelocity);
    TestTelemetry.set_newtelemetry(0.0);
    /// - Define the number of bi-directional packets to send for
    ///   this test.
    int num_tests_to_run = 2000;
    /// - Print out a message of the test description.
    std::cout << "Comm test in progress : " << std::endl << " Sending and receiving "
              << num_tests_to_run << " comm packets at " << TestArticle.CommFrequency
              << " Hz." << std::endl;
    /// - Create comm thread.
    std::thread unix_serial_comm_thread = TestArticle.CreateCommThread();
    for (int testNum = 0; testNum < num_tests_to_run; testNum++)
    {
        /// - Read the command packet sent by Unix serial comm driver.
        ReadCommands();
        usleep(1.0e6/(2*TestArticle.CommFrequency));
        /// - Write a new telemetry packet with an updated term.
        float newPosition = TestArticle.Telemetry.position() + 1.0;
        TestTelemetry.set_position(newPosition);
        TestTelemetry.set_velocity(0.0);
        TestTelemetry.set_newtelemetry(0.0);
        WriteTelemetry();
        usleep(1.0e6/(2*TestArticle.CommFrequency));
    }
    ASSERT_NEAR(TestArticle.Telemetry.position(), num_tests_to_run, Tolerance);
    /// - Deactivate comm thread.
    TestArticle.Active = false;
    unix_serial_comm_thread.join();
}


bool UnixSerialChannel_Test::ValidHeader(){
	int bytes_read;
	unsigned char rxBuffer[4];
	if((bytes_read = read(VirtualCommPortHandle, rxBuffer, 4)) < 0)
		return false;

    if(bytes_read == 4){
    	return rxBuffer[0] == 0x21 &&
    	       rxBuffer[1] == 0x45 &&
    	       rxBuffer[2] == 0x4F &&
    	       rxBuffer[3] == 0x53;
    }
    return false;
}

bool UnixSerialChannel_Test::ValidCrc(unsigned char * rcvdData){
	int bytes_read;
	unsigned char rxCrc[4];

	if((bytes_read = read(VirtualCommPortHandle, rxCrc, 4)) < 0){
		return false;
	}

    uint32_t rcvd_crc32 = 0;
    rcvd_crc32 |= rxCrc[3] << 24;
    rcvd_crc32 |= rxCrc[2] << 16;
    rcvd_crc32 |= rxCrc[1] << 8;
    rcvd_crc32 |= rxCrc[0];

	uint32_t computed_crc32 = CommCrc32::crc32(rcvdData, TestCommands.ByteSize(), 0);

    if(bytes_read == 4){
    	return rcvd_crc32 == computed_crc32;
    }
    return false;
}

void UnixSerialChannel_Test::ReadCommands(){
	int bytes_read;
	unsigned char rxBuffer[COMM_MAX_BUFF_SIZE];
	if (!ValidHeader())
        FAIL() << "Invalid command packet was sent. Bad header.";
		
	if((bytes_read = read(VirtualCommPortHandle, rxBuffer, TestCommands.ByteSize())) < 0)
        FAIL() << "Cannot read commands from Unix PC.";
		
	else if(bytes_read == 0)
        FAIL() << "Unix PC is not responding.";

	/// - Check the CRC
	if (!ValidCrc(rxBuffer))
        FAIL() << "Invalid command packet was sent. Checksum failed.";

	/// - Read the command packet
	if (!TestCommands.ParseFromArray(rxBuffer, TestCommands.ByteSize())) {
        FAIL() << "Failed to parse command packet from Chup.";
	}
}

void UnixSerialChannel_Test::WriteTelemetry(){
    /// - Send the command packet contents.
    write(VirtualCommPortHandle, &PacketHeader, 4);
    // Write the telemetry back to the Unix PC
	unsigned char txBuffer[COMM_MAX_BUFF_SIZE];
    if (!TestTelemetry.SerializeToArray(txBuffer, TestTelemetry.ByteSize())) {
      FAIL() << "Error writing telemetry packet to : " << VirtualCommPortName;
    }
    /// - Send the command packet contents.
    write(VirtualCommPortHandle, txBuffer, TestTelemetry.ByteSize());
    /// - Compute the CRC 32 bit checksum.
	uint32_t computed_crc32 = CommCrc32::crc32(txBuffer, TestTelemetry.ByteSize(), 0);
    /// - Send the CRC 32 bit checksum.
    write(VirtualCommPortHandle, &computed_crc32, 4);
}
