#include "ioduino.h"
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {
	try {
		IOduino my_io;
		if(my_io.isConnected()) {
			cout << "Connected to IOduino!" << endl;
			my_io.pinMode(13, OUTPUT);
			for (int i=0; i<10; i++) {
				my_io.digitalWrite(13, HIGH);
				cout << i << ") Pin 13 -> HIGH" << endl;
				sleep(1);
				my_io.digitalWrite(13, LOW);
				cout << i << ") Pin 13 -> LOW" << endl;
				sleep(1);
			}		
		} else {
			cout << "Failed to connect to an IOduino" << endl;
		}
	} catch(exception &e) {
		cout << "Exception: " << e.what() << endl;
	}

	return 0;
}
