#include "ProtobuffSerial.h"

const byte LED_PIN = 13;

ProtobuffSerial::ProtobuffSerial(){
    pinMode(LED_PIN, OUTPUT);
}

ProtobuffSerial::~ProtobuffSerial(){/*nothing to destruct*/}

void ProtobuffSerial::on(){
	digitalWrite(LED_PIN,HIGH);
}

void ProtobuffSerial::off(){
	digitalWrite(LED_PIN,LOW);
}

void ProtobuffSerial::blink(int time){
	on();
	delay(time/2);
	off();
	delay(time/2);
}