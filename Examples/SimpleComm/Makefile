# This makefile calls nanopb to auto-generate the comm inteface
COMM_HOME = ../..
NANO_PB_HOME = ${COMM_HOME}/nanopb
include ${NANO_PB_HOME}/extra/nanopb.mk
COMM_RCVR = ${COMM_HOME}/rcvr
PROTO_FILE = comm_packet.proto
CXXFLAGS = -g -std=c++0x -pthread -lpthread
INCS = -I./ -I${COMM_RCVR}
OBJS = ${COMM_RCVR}/CommChannel.o \
       ${COMM_RCVR}/UnixSerialChannel.o \
       ${COMM_RCVR}/UsbReceiver.o \
       ${COMM_RCVR}/MessageHandler.o \
       ${COMM_HOME}/CommCrc32.o \
       ${COMM_RCVR}/comm_packet.pb.o
EXE = simple_rcvr

all: simple_rcvr

.PHONY: comm_packet
comm_packet:
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=../../ $(PROTO_FILE)
	protoc --cpp_out=$(COMM_RCVR) $(PROTO_FILE)
-include comm_packet

simple_rcvr: $(OBJS)
	$(CXX) simple_rcvr.cc $(CXXFLAGS) $(INCS) -o $(EXE) $(OBJS) -lprotobuf

clean:
	rm -rf $(COMM_HOME)/*.pb.*
	rm -rf $(EXE) $(OBJS) $(COMM_RCVR)/*.pb.*