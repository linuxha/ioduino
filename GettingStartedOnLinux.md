This page will guide you through the process of flashing the IOduino firmware and compiling the C++ and Python examples.

IOduino is a firmware for the Arduino which enables it's pins to be controlled remotely (over serial RS232). Analog/digital reads and writes are all possible. It comes bundled with a c++ library (IOduino.h) which allows easy control from a computer.

# Details #

## Download IOduino ##
You can find the library and example code [here](http://ioduino.googlecode.com/files/IOduino-v1.1.zip)

## Flash Arduino with IOduino firmware ##
From Arduino IDE open the IOduino project ioduino/IOduino firmware/ioduino/ioduino.pde
Upload the firmware to the Arduino.

## Install dependancies ##
If you are using C++, then you will need to install libserial
```
sudo apt-get install libserial-dev
```

If you are using python, then you will need to install libserial and swig1.3
```
sudo apt-get install libserial-dev swig1.3
```

## Test C++ examples ##
From the ioduino directory run make `_`examples, then try out the examples with an arduino connected :
```
cd ioduino/
make _examples
cd examples
./analogWrite_example
```

### Test python examples ###
From the ioduino directory run make `_`python, then try out the examples with an arduino connected :
```
cd ioduino/
make _python
cd python
python digitalWrite_example.py
```