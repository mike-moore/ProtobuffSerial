CXX = g++
COMM_HOME = ../..
COMM_RCVR = ${COMM_HOME}/rcvr
SIMPLE_COMM_HOME = ${COMM_HOME}/Examples/SimpleComm
PROTO_FILE = ${SIMPLE_COMM_HOME}/comm_packet.proto
CXXFLAGS = -g -lgtest -lgtest_main -std=c++0x -pthread -lpthread
INCS = -I./ -I${COMM_RCVR}
OBJS = ${COMM_RCVR}/CommChannel.o CommChannel_Test.o \
       ${COMM_RCVR}/UnixSerialChannel.o UnixSerialChannel_Test.o \
       ${COMM_RCVR}/UsbReceiver.o UsbReceiver_Test.o \
       ${COMM_RCVR}/MessageHandler.o MessageHandler_Test.o \
       ${COMM_HOME}/CommCrc32.o \
       ${COMM_RCVR}/comm_packet.pb.o

all: testAll

.PHONY: comm_packet
comm_packet: 
	protoc --cpp_out=.. --proto_path=$(SIMPLE_COMM_HOME) $(PROTO_FILE)
-include comm_packet

testAll: $(OBJS)
	$(CXX) TestRunner.cpp $(CXXFLAGS) $(INCS) -o testAll $(OBJS) -lprotobuf

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

.c.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

clean:
	rm -rf testAll $(OBJS) testAll.xml ../*.pb.*


