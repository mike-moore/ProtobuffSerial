///////////////////////////////////////////////////////////////
///  This file defines a class that is used for testing
///  the micro communication channel interface.
///
/// @author
///         $Author: Mike Moore $
///
/// Contact: mike.moore@so.engineering
///
/// Created on: Sunday June 12 2016
///
///////////////////////////////////////////////////////////////
#ifndef MICRO_COMM_CHANNEL_TEST_HH
#define MICRO_COMM_CHANNEL_TEST_HH

#include "gtest/gtest.h"
#include "CommCrc32.h"
#include "MicroCommChannel.h"

///////////////////////////////////////////////////////////////
/// @brief   Inherit from MicroCommChannel and befriend MicroCommChannel_Test.
/// @details Class derived from the unit under test. It just
///          has a constructor with the same arguments as the
///          parent and a default destructor, but it befriends
///          the unit test case driver class to allow access
///          to protected data members.
///////////////////////////////////////////////////////////////
class FriendlyMicroCommChannel : public MicroCommChannel{
  public:
    FriendlyMicroCommChannel();
    virtual ~FriendlyMicroCommChannel();
    friend class MicroCommChannel_Test;
};
inline FriendlyMicroCommChannel::FriendlyMicroCommChannel(){};
inline FriendlyMicroCommChannel::~FriendlyMicroCommChannel(){};

///////////////////////////////////////////////////////////////
/// @class MicroCommChannel_Test
/// @ingroup Communication
/// @brief Definition of test class for performing unit-tests
///        of the SCI communication channel interface.
///////////////////////////////////////////////////////////////
class MicroCommChannel_Test : public testing::Test{

 public:
  //////////////////////////////////////////////////////////
  /// @brief The default c'tor constructs the MicroCommChannel_Test
  ///        class.
  ////////////////////////////////////////////////////////////
  MicroCommChannel_Test();
  //////////////////////////////////////////////////////////
  /// @brief The default d'tor destructs the MicroCommChannel_Test
  ///        class.
  ////////////////////////////////////////////////////////////
  ~MicroCommChannel_Test();

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
  /// @brief Helper function that returns true if a valid
  ///        header packet was received.
  ////////////////////////////////////////////////////////////
  bool ValidHeader();
  ////////////////////////////////////////////////////////////
  /// @brief Helper function that returns true if a valid
  ///        CRC checksum was received.
  ////////////////////////////////////////////////////////////
  bool ValidCrc(unsigned char * rcvdData);
  ////////////////////////////////////////////////////////////
  /// @brief Helper function that emulates writing to the
  ///        device by directly setting the RxBuffer.
  ////////////////////////////////////////////////////////////
  void WriteToDevice(uint8_t* txBuff, int buffStartIndx, int numBytes);
  ////////////////////////////////////////////////////////////
  /// @brief Helper function that emulates reading from the
  ///        device by reading directly from the Tx buffer.
  ////////////////////////////////////////////////////////////
  void ReadFromDevice(uint8_t* rxBuff, int buffStartIndx, int numBytes);
  ////////////////////////////////////////////////////////////
  /// @brief Helper function that writes commands to the 
  ///        serial channel.
  ////////////////////////////////////////////////////////////
  void WriteCommands();
  ////////////////////////////////////////////////////////////
  /// @brief Helper function that reads telemetry from the 
  ///        serial channel.
  ////////////////////////////////////////////////////////////
  void ReadTelemetry();
  ////////////////////////////////////////////////////////////
  /// @brief Instance of class under test.
  ////////////////////////////////////////////////////////////
  FriendlyMicroCommChannel TestArticle;
  ////////////////////////////////////////////////////////////
  /// @brief Test command and telemetry packets
  ////////////////////////////////////////////////////////////
  CommandPacket TestCommands;
  TelemetryPacket TestTelemetry;
  ////////////////////////////////////////////////////////////
  /// @brief Header placed on command packets sent to the 
  ///        device. Constant value used is SOE!
  ///        in hex : 0x534F4521
  ///        in dec : 1397703969
  ////////////////////////////////////////////////////////////
  uint8_t PacketHeader[4];
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  /// @brief Unit-test tolerance for floating pt comparisons.
  ////////////////////////////////////////////////////////////
  float Tolerance;
};

#endif  // SCI_CHANNEL_TEST_HH
