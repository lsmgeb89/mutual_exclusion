CC=g++
INC=-I./utils
CFLAGS=-c -Wall -std=c++0x -D NDEBUG
LDFLAGS=-pthread
SOURCES=main.cc
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=lock_compare

all:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):	$(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@

clean:
	rm *.o lock_compare
