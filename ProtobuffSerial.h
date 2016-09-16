#ifndef PROTOBUFFSERIAL_H
#define PROTOBUFFSERIAL_H

#include <Arduino.h>

class ProtobuffSerial { 
public:
	ProtobuffSerial();
	~ProtobuffSerial();
    void on();
	void off();
	void blink(int time);
};

#endif