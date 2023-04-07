CC=gcc
CFLAGS=-Wall -g -I/usr/include/GL -I.
LDFLAGS=-lSDL2 -lGL -lGLEW -lSDL2_image -lSDL2_gfx -lm -lSDL2_mixer -lSDL2_ttf
SOURCES=main.c utils.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm $(EXECUTABLE) $(OBJECTS)



