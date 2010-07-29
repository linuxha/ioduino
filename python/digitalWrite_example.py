from ioduino import *
from time import sleep

myio = IOduino()

for i in range(1, 10):
	myio.digitalWrite(13, HIGH);
	print "On";
	sleep(0.5);
	myio.digitalWrite(13, LOW);
	print "Off";
	sleep(0.5);
	
