CC=g++
LDFLAGS=-lserial

examples: analogWrite_example analogRead_example digitalWrite_example 

%: %.cpp
	$(CC) $< -o $@ $(LDFLAGS) -I../

clean:
	rm -f analogWrite_example analogRead_example digitalWrite_example 
