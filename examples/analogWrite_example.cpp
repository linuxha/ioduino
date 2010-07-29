#include "ioduino.h"
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {
	try {
		IOduino my_io;
		if(my_io.isConnected()) {
			cout << "Connected to IOduino!" << endl;
			my_io.pinMode(11, OUTPUT);
			for (int i=0; i<5; i++) {
				for (int brightness=0; brightness<255; brightness+=10) {
					my_io.analogWrite(11, brightness);
					usleep(20*1000);
				}	
				for (int brightness=0; brightness<255; brightness+=10) {
					my_io.analogWrite(11, 255-brightness);
					usleep(20*1000);
				}	
			}	
		} else {
			cout << "Failed to connect to an IOduino" << endl;
		}
	} catch(exception &e) {
		cout << "Exception: " << e.what() << endl;
	}

	return 0;
}
