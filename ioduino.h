/*  IOduino.h - The IOduino class allows easy control of an Arduino flashed
 *              with the IOduino firmware (digital/analog input/output)
 *  ----------------------------------------------------------------------
 *  Copyright (C) 2010  Harry Eakins
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *  ----------------------------------------------------------------------
 *  Author: Harry Eakins
 *  Email: harry (dot) eakins (at) googlemail (dot) com
 *  Date Modified: 11:39 29/07/2010
 */

#ifndef IODUINO_H_
#define IODUINO_H_

#include <SerialStream.h>
#include <string>
#include <sstream>
#include <exception>

#define BAUD_RATE LibSerial::SerialStreamBuf::BAUD_38400

typedef int IOduino_pin;

const bool INPUT = true;
const bool OUTPUT = false;
const bool HIGH = true;
const bool LOW = false;

class IOduino {
		
public:
	IOduino();
	explicit IOduino(std::string filename);
	bool isConnected() const;

	void pinMode(IOduino_pin pin, bool state);
	void digitalWrite(IOduino_pin pin, bool state);
	bool digitalRead(IOduino_pin pin); 
	void analogWrite(IOduino_pin pin, int value);
	int analogRead(IOduino_pin pin);
private:
	void init(std::string filename);
	bool ConnectToArduino(std::string filename);

	LibSerial::SerialStream IOduino_stream;

};

// Custom exception class - very general
class IOduino_exception : public std::exception {
public:
	IOduino_exception (std::string message) throw() :exception(), my_message(message) { };
	~IOduino_exception() throw() { };
	
	virtual const char* what() const throw() {return my_message.c_str();}
private:
	std::string my_message;
};

IOduino::IOduino() {
	init("");
};

IOduino::IOduino(std::string filename){
	init(filename);
};

void IOduino::init(std::string filename) {
	if(filename == "") { // If no filename given, then cycle through /dev/ttyUSB0->9
		for(int i = 0; i < 10; i++) {
			std::stringstream filename_stream;
			filename_stream << "/dev/ttyUSB" << i;
			if (ConnectToArduino(filename_stream.str()) == true)
				break;
		}
		if(!isConnected())
			throw IOduino_exception("Unable to connect to IOduino at ports /dev/ttyUSB[0-9]");
	} else { 
		ConnectToArduino(filename);
		if(!isConnected()) 
			throw IOduino_exception("Unable to connect to IOduino at port " + filename);
	}
}

bool IOduino::ConnectToArduino(std::string filename) {
	IOduino_stream.Open(filename);
	if(IOduino_stream.IsOpen()) { // Found a USB->serial device
		IOduino_stream.SetBaudRate(BAUD_RATE);
		IOduino_stream.SetCharSize( LibSerial::SerialStreamBuf::CHAR_SIZE_8 ) ;
		IOduino_stream.SetNumOfStopBits(1);
		IOduino_stream.SetParity(LibSerial::SerialStreamBuf::PARITY_NONE);
		IOduino_stream.SetFlowControl(LibSerial::SerialStreamBuf::FLOW_CONTROL_NONE);

		IOduino_stream << 'v'; // Verify it is an Arduino with the IOduino firmware
				       // It should respond with an 'i'
		char response;
		IOduino_stream >> response;
		if (response == 'i') {
			return true;
		} else {
			IOduino_stream.Close();
			return false;
		}
	} else { // Couldn't open file
		return false;
	}
};

bool IOduino::isConnected() const {
	return IOduino_stream.IsOpen();
};

void IOduino::pinMode(IOduino_pin pin, bool state) {
	if(pin >= 0 && pin <= 19) {
		if(state == OUTPUT)
			IOduino_stream << "pm" << pin << "o";
		else if (state == INPUT)
			IOduino_stream << "pm" << pin << "i";
		char response;
		IOduino_stream >> response;
		if(response != 'k')
			throw IOduino_exception("no acknowledgment from IOduino (pinMode)");
	} else {
		throw IOduino_exception("pin not in range [0-19] (pinMode)");
	}
};

void IOduino::digitalWrite(IOduino_pin pin, bool state) {
	if(pin >= 0 && pin <= 19) {
		if(state == HIGH)
			IOduino_stream << "dw" << pin << "h";
		else if (state == LOW)
			IOduino_stream << "dw" << pin << "l";
		char response;
		IOduino_stream >> response;
		if(response != 'k')
			throw IOduino_exception("no acknowledgment from IOduino (digitalWrite)");
	} else {
		throw IOduino_exception("pin not in range [0-19] (digitalWrite)");
	}
};

bool IOduino::digitalRead(IOduino_pin pin) {
	if(pin >= 0 && pin <= 19) {
		IOduino_stream << "dr" << pin << ".";
		int data;
		IOduino_stream >> data;
		if(data == 0)
			return LOW;
		else if (data == 1)
			return HIGH;
		else 
			throw IOduino_exception("no response from IOduino (digitalRead)");
	} else {
		throw IOduino_exception("pin not in range [0-19] (digitalRead)");
	}
}; 

void IOduino::analogWrite(IOduino_pin pin, int value) {
	if(pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || pin == 11) {
		IOduino_stream << "aw" << pin << "," << value << ".";
		char response;
		IOduino_stream >> response;
		if(response != 'k')
			throw IOduino_exception("no acknowledgment from IOduino (analogWrite)");
	} else {
		throw IOduino_exception("pin not in range [3, 5, 6, 9, 10, 11] (analogWrite)");
	}
};

int IOduino::analogRead(IOduino_pin pin) {
	if(pin >= 0 && pin <= 5) {
		IOduino_stream << "ar" << pin << ".";
		int data = -1;
		IOduino_stream >> data;
		if(data >= 0 && data <= 1023)
			return data;
		else 
			throw IOduino_exception("no response from IOduino (analogRead)");
	} else {
		throw IOduino_exception("pin not in range [0-5] (analogRead)");
	}
};

#endif // IODUINO_H_
