# This makefile calls nanopb to auto-generate the comm inteface
COMM_HOME = ../..
NANO_PB_HOME = ${COMM_HOME}/nanopb
include ${NANO_PB_HOME}/extra/nanopb.mk
PROTO_FILE = ./comm_packet.proto

all: nano_proto_c

nano_proto_c:
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=../../ $(PROTO_FILE)

clean:
	rm -rf ${COMM_HOME}/*.pb.*