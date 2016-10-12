# ProtobuffSerial
This is an Arduino library that utilizes Google's protocol buffers in order to implement a flexible communication system.

---
### Getting started
The easiest way to get started with using this library is to first build and run the example sketches. But before you do that,
you must install the library. To do that, download this repository as a zip file and follow the instructions [here](https://www.arduino.cc/en/Guide/Libraries#toc4).

Users that are more programming-savvy can just do something like the following:

```
cd arduino-1.6.11/libraries
git clone https://github.com/mike-moore/ProtobuffSerial.git
```




If you get the following error:

```
...
ProtobuffSerial/MicroCommChannel.h:19:28: fatal error: comm_packet.pb.h: No such file or directory
 #include "comm_packet.pb.h"
                            ^
compilation terminated.
exit status 1
```

You have not yet auto-generated your comm packet interface code. To do that run the following commands:
```
cd arduino-1.6.11/libraries/Examples/SimpleComm
make
```

If you get this error:

```
         *************************************************************
         *** Could not import the Google protobuf Python libraries ***
         *** Try installing package 'python-protobuf' or similar.  ***
         *************************************************************
```
 
 You will need to install the python libraries for protobuff 2.5.0. See [here](https://github.com/google/protobuf/releases/tag/v2.5.0) 
 
 Download and then follow instructions to setup. Then do the following:
 
 ```
cd protobuf-2.5.0/python
python setup.py install
```
 

