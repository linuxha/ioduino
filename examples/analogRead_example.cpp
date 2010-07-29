#include "ioduino.h"
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {
	try {
		IOduino my_io;
		if(my_io.isConnected()) {
			cout << "Connected to IOduino!" << endl;
			for (int i=0; i<10; i++) {
				cout << i << ") " << my_io.analogRead(0) << endl;
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
