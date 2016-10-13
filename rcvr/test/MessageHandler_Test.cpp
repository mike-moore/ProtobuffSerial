#include "MessageHandler_Test.h"

MessageHandler_Test::MessageHandler_Test()
{
	/// - Nothing to do
}

MessageHandler_Test::~MessageHandler_Test() {
	/// - Nothing to do
}

void MessageHandler_Test::SetUp(){
	/// - Nothing to do
}

void MessageHandler_Test::TearDown(){
	/// - Nothing to do
}

TEST_F(MessageHandler_Test,warn){
    std::string warning_location = "MessageHandler::warn unit-test";
    std::string warning_message = "This is a test of the MessageHandler::warn function.";
    MessageHandler::warn(__FILE__, __LINE__, warning_location.c_str(), warning_message.c_str()); 
}

TEST_F(MessageHandler_Test,error){
    std::string error_location = "MessageHandler::error unit-test";
    std::string error_message = "This is a test of the MessageHandler::error function.";
    MessageHandler::error(__FILE__, __LINE__, error_location.c_str(), error_message.c_str()); 
}

TEST_F(MessageHandler_Test,fail){
    std::string fail_location = "MessageHandler::fail unit-test";
    std::string fail_message = "This is a test of the MessageHandler::fail function.";
	try
	{
        MessageHandler::fail(__FILE__, __LINE__, fail_location.c_str(), fail_message.c_str()); 
        FAIL() << "Expected std::runtime_error";
    }
    catch(std::runtime_error const & err) {
        EXPECT_EQ(err.what(),std::string("Fatal error "));
    }
    catch(...) {
        FAIL() << "Expected std::runtime_error";
    }
}

