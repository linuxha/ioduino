CC=g++
LDFLAGS=-lserial

python: _ioduino.so
	rm -f ioduino_wrap.cxx ioduino_wrap.o

_ioduino.so : ioduino_wrap.o
	$(CC) -shared ioduino_wrap.o -o _ioduino.so $(LDFLAGS)

ioduino_wrap.o: ioduino_wrap.cxx
	$(CC) -c ioduino_wrap.cxx -I `locate Python.h | sed s/Python\.h//` -I ../

ioduino_wrap.cxx: ioduino.i
	swig -python -c++ -I../ ioduino.i

clean: 
	rm -f ioduino.py ioduino.pyc _ioduino.so ioduino_wrap.cxx ioduino_wrap.o
