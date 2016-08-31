CXX = g++
COMM_HOME = ..
NANO_PB_HOME = ${COMM_HOME}/nanopb
SIMPLE_COMM_HOME = ${COMM_HOME}/Examples/SimpleComm
CXXFLAGS = -g -lgtest -lgtest_main -std=c++0x -pthread -lpthread
INCS = -I./ -I${COMM_HOME} -I${NANO_PB_HOME}
OBJS = ${COMM_HOME}/MicroCommChannel.o MicroCommChannel_Test.o \
       ExampleMicroComm_Test.o \
       ${COMM_HOME}/CommCrc32.o \
       ${NANO_PB_HOME}/pb_encode.o ${NANO_PB_HOME}/pb_decode.o ${NANO_PB_HOME}/pb_common.o \
       ${COMM_HOME}/comm_packet.pb.o

all: makeSimpleCommIf testAll

makeSimpleCommIf: 
	cd ${SIMPLE_COMM_HOME} && $(MAKE) clean && $(MAKE)

testAll: $(OBJS)
	$(CXX) TestRunner.cpp $(CXXFLAGS) $(INCS) -o testAll $(OBJS) -lprotobuf

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

.c.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

clean:
	cd ${SIMPLE_COMM_HOME} && $(MAKE) clean
	rm -rf testAll $(OBJS) testAll.xml *.pyc *.o