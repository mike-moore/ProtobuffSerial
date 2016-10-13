#include "CommChannel_Test.h"

CommChannel_Test::CommChannel_Test()
{
	Tolerance = 2.0e-03;
}

CommChannel_Test::~CommChannel_Test() {
	/// - Nothing to do
}

void CommChannel_Test::SetUp(){
	/// - Nothing to do
}

void CommChannel_Test::TearDown(){
	/// - Nothing to do
}

TEST_F(CommChannel_Test,constructor){
	/// - Make sure we can easily set and get the commands.
	float testNormalizedVoltageCmd = -0.67;
	TestArticle.Commands.set_normalizedvoltage(testNormalizedVoltageCmd);
	TestArticle.Commands.set_secondcommand(1);
	ASSERT_NEAR(TestArticle.Commands.normalizedvoltage(), testNormalizedVoltageCmd, Tolerance);
	/// - Make sure we can easily set and get the telemetry.
	float testPosition = 25.3145600;
    float testVelocity = -754.67800;
	TestArticle.Telemetry.set_position(testPosition);
	TestArticle.Telemetry.set_velocity(testVelocity);
	TestArticle.Telemetry.set_newtelemetry(0.0);
	ASSERT_NEAR(TestArticle.Telemetry.position(), testPosition, Tolerance);
	ASSERT_NEAR(TestArticle.Telemetry.velocity(), testVelocity, Tolerance);
}

TEST_F(CommChannel_Test,testConnectInvalidFreq){
    /// - Test setting the comm frequency to a bad low value.
    TestArticle.CommFrequency = -1.0;
	try
	{
		TestArticle.Connect();
        FAIL() << "Expected std::runtime_error";
    }
    catch(std::runtime_error const & err) {
        EXPECT_EQ(err.what(),std::string("Fatal error "));
    }
    catch(...) {
        FAIL() << "Expected std::runtime_error";
    }
    /// - Test setting the comm frequency to a bad high value.
    TestArticle.CommFrequency = 100.0e7;
	try
	{
		TestArticle.Connect();
        FAIL() << "Expected std::runtime_error";
    }
    catch(std::runtime_error const & err) {
        EXPECT_EQ(err.what(),std::string("Fatal error "));
    }
    catch(...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST_F(CommChannel_Test,nominalConnect){
	TestArticle.CommFrequency = 100.0;
	TestArticle.Connect();
	SUCCEED();
}
