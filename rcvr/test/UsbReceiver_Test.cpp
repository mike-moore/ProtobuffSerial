#include "UsbReceiver_Test.h"

UsbReceiver_Test::UsbReceiver_Test()
{
	VirtualCommPortHandle = 1;
	VirtualCommPortName = "";
	Tolerance = 2.0e-03;
}

UsbReceiver_Test::~UsbReceiver_Test() {
	/// - Nothing to do
}

void UsbReceiver_Test::SetUp(){
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
}

void UsbReceiver_Test::TearDown(){
    close(VirtualCommPortHandle);
}

TEST_F(UsbReceiver_Test,runComm){
    /// - Connect to the Virtual Comm Port
    TestArticle.CommFrequency = 1000.0;
    TestArticle.PortName = VirtualCommPortName;
    TestArticle.Connect();
    /// - Set the command values within the test article to some test values.
    float testNormalizedVoltage = -0.67;
    TestArticle.NormalizedVoltage = testNormalizedVoltage;
    TestArticle.SecondCommand = 1;
    /// - Call the run comm function.
    TestArticle.RunComm();
    SUCCEED();
}
