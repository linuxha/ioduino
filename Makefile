default: all

all: _examples _python

_examples:
	make -C examples

_python: 
	make -C python

clean:
	make -C examples clean
	make -C python clean
