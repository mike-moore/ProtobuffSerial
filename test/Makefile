CXX = g++
COMM_HOME = ..
SIMPLE_COMM_HOME = ${COMM_HOME}/Examples/SimpleComm
CXXFLAGS = -g -lgtest -lgtest_main -std=c++0x -pthread -lpthread
INCS = -I./ -I${COMM_HOME}
OBJS = ${COMM_HOME}/MicroCommChannel.o MicroCommChannel_Test.o \
       ExampleMicroComm_Test.o \
       ${COMM_HOME}/CommCrc32.o \
       ${COMM_HOME}/pb_encode.o ${COMM_HOME}/pb_decode.o ${COMM_HOME}/pb_common.o \
       ${COMM_HOME}/comm_packet.pb.o

all: testAll

.PHONY: comm_packet
comm_packet: 
	cd ${SIMPLE_COMM_HOME} && $(MAKE) clean && $(MAKE)
-include comm_packet

testAll: $(OBJS)
	$(CXX) TestRunner.cpp $(CXXFLAGS) $(INCS) -o testAll $(OBJS) -lprotobuf

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

.c.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

clean:
	cd ${SIMPLE_COMM_HOME} && $(MAKE) clean
	rm -rf testAll $(OBJS) testAll.xml *.pyc *.o