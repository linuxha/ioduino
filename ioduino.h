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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

#define BAUD_RATE LibSerial::SerialStreamBuf::BAUD_38400

typedef int IOduino_pin;

const bool INPUT = true;
const bool OUTPUT = false;
const bool HIGH = true;
const bool LOW = false;

class IOduino {
		
public:
	//!The default constructor
	/*!
		When the default constructor is called, /dev/ttyUSB[0->9] are
		searched for IOduinos and the first one found is connected to.
		It can take up to 2 seconds for the constructor to finish being 
		called. This is often the case if the arduino has just been
		plugged in and is reset when the serial port is accessed for 
		the first time.
		Throws an IOduino_exception if none found.
	*/ 
	IOduino();

	//!Alternative constructor
	/*! 
		Used when you need to specify the filename explicitly. Throws
		an IOduino_exception if not found in that location.
	*/
	explicit IOduino(std::string filename);

	//! Checks if connected to an IOduino
	bool isConnected() const;

	//! Sets an IOduino pin to either OUTPUT or INPUT
	/*!
		\param pin must be between 0 and 19 inclusive
		\param state is either OUTPUT or INPUT

		Throws IOduino_exception if pin is out of range or if IOduino 
		doesn't respond.
	*/
	void pinMode(IOduino_pin pin, bool state);

	//! Sets an IOduino pin to either HIGH or LOW
	/*!
		\param pin must be between 0 and 19 inclusive
		\param state is either HIGH or LOW

		If the pin has been configured as an OUTPUT with pinMode(), 
		its voltage will be set to the corresponding value: 5V for 
		HIGH, 0V (ground) for LOW.

		If the pin is configured as an INPUT, writing a HIGH value 
		with digitalWrite() will enable an internal 20K pullup resistor.
		Writing LOW will disable the pullup.		

		Throws IOduino_exception if pin is out of range or if IOduino 
		doesn't respond.
	*/
	void digitalWrite(IOduino_pin pin, bool state);

	//! Reads the value from a specified digital pin, either HIGH  or LOW.
	/*!
		\param pin must be between 0 and 19 inclusive
		\param state is either HIGH or LOW

		Returns HIGH or LOW

		Throws IOduino_exception if pin is out of range or if IOduino 
		doesn't respond.
	*/
	bool digitalRead(IOduino_pin pin); 

	//! Writes an analog value (PWM wave) to a pin.
	/*!
		\param pin must be 3, 5, 6, 9, 10, or 11
		\param is the duty cycle: between 0 (always off) and 255 (always on). 

		Writes an analog value (PWM wave) to a pin. Can be used to light a 
		LED at varying brightnesses or drive a motor at various speeds. 
		After a call to analogWrite(), the pin will generate a steady square 
		wave of the specified duty cycle until the next call to analogWrite() 
		(or a call to digitalRead()  or digitalWrite() on the same pin). The 
		frequency of the PWM signal is approximately 490 Hz.

		Throws IOduino_exception if pin is out of range or if IOduino 
		doesn't respond.
	*/ 
	void analogWrite(IOduino_pin pin, int value);

	//! Reads the value from the specified analog pin.
	/*!
		\param pin must be between 0 and 5 inclusive

		The Arduino board contains a 6 channel, 10-bit analog to digital 
		converter. This means that it will map input voltages between 0 and 
		5 volts into integer values between 0 and 1023. This yields a 
		resolution between readings of: 5 volts / 1024 units or, .0049 volts 
		(4.9 mV) per unit.

		Throws IOduino_exception if pin is out of range or if IOduino 
		doesn't respond.
	*/
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

		pid_t pID = fork();
		if(pID == 0) { // Child
			IOduino_stream << 'v'; // Verify it is an Arduino with the IOduino firmware
				       // It should respond with an 'i'
			char response;
			IOduino_stream >> response;
			exit(1);
		} else {
			int child_status;
			int counter = 0;
			while(waitpid(pID, &child_status, WNOHANG) == 0 && counter < 20) {
				usleep(100*1000); // 100ms
				counter++;
			}
			kill(pID, SIGKILL);
		}

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
