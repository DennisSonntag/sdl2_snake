CC=gcc
CFLAGS=-Wall -g -I/usr/include/GL -I.
LDFLAGS=-lSDL2  -lm 
SOURCES=main.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm $(EXECUTABLE) $(OBJECTS)



