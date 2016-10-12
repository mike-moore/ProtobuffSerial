///////////////////////////////////////////////////////////////
///  This file defines a class that is used for testing
///  the serial interface.
///
/// @author
///         $Author: Mike Moore $
///
/// Contact: mike.moore@so.engineering
///
/// Created on: Saturday June 25 2016
///
///////////////////////////////////////////////////////////////
#ifndef EXAMPLE_MICRO_COMM_TEST_HH
#define EXAMPLE_MICRO_COMM_TEST_HH

#include "gtest/gtest.h"
#include "MicroCommChannel.h"

///////////////////////////////////////////////////////////////
/// @brief   Inherit from ExampleMicroComm and befriend ExampleMicroComm_Test.
/// @details Class derived from the unit under test. It just
///          has a constructor with the same arguments as the
///          parent and a default destructor, but it befriends
///          the unit test case driver class to allow access
///          to protected data members.
///////////////////////////////////////////////////////////////
class FriendlyExampleMicroComm : public MicroCommChannel{
 public:
  FriendlyExampleMicroComm();
  virtual ~FriendlyExampleMicroComm();
  friend class ExampleMicroComm_Test;
  //////////////////////////////////////////////////////////
  /// @brief Mock implementations used to emulate what the 
  ///        device serial HW would do. This is done in order to
  ///        fully test all of the state transitions in 
  ///        RunComm.
  //////////////////////////////////////////////////////////
  virtual int InitHw();
  virtual bool ReadBytesAvailable();
  virtual int ReadPacket();
  virtual int WritePacket();
  void WriteToDevice(uint8_t* txBuff, int buffStartIndx, int numBytes);
  void ReadCommands();
  void FailHeader();
  CommandPacket TestCommands;
  TelemetryPacket TestTelemetry;
  uint8_t PacketHeader[4];
  bool FailHwInit;
  bool BytesAvailable;
  bool FailRx;
  bool FailTx;
};

///////////////////////////////////////////////////////////////
/// @class ExampleMicroComm_Test
/// @ingroup Communication
/// @brief Definition of test class for performing unit-tests
///        of the SCI communication channel interface.
///////////////////////////////////////////////////////////////
class ExampleMicroComm_Test : public testing::Test{

 public:
  //////////////////////////////////////////////////////////
  /// @brief The default c'tor constructs the ExampleMicroComm_Test
  ///        class.
  ////////////////////////////////////////////////////////////
  ExampleMicroComm_Test();
  //////////////////////////////////////////////////////////
  /// @brief The default d'tor destructs the ExampleMicroComm_Test
  ///        class.
  ////////////////////////////////////////////////////////////
  ~ExampleMicroComm_Test();

 protected:
  ////////////////////////////////////////////////////////////
  /// @brief Unit-test setup routine.
  ////////////////////////////////////////////////////////////
  virtual void SetUp();
  ////////////////////////////////////////////////////////////
  /// @brief Unit-test tear down routine.
  ////////////////////////////////////////////////////////////
  virtual void TearDown();
  ////////////////////////////////////////////////////////////
  /// @brief Instance of class under test.
  ////////////////////////////////////////////////////////////
  FriendlyExampleMicroComm TestArticle;
  ////////////////////////////////////////////////////////////
  /// @brief Unit-test tolerance for floating pt comparisons.
  ////////////////////////////////////////////////////////////
  float Tolerance;
};

#endif  // EXAMPLE_MICRO_COMM_TEST_HH
