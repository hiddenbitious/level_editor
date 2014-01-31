CC=clang++
CFLAGS=-c -Wall -O0 -g

LDFLAGS=-lm -lGL -lglut -lGLU

SOURCES=source/editor.cpp source/tile.cpp source/popUp.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=editor

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm source/*.o $(EXECUTABLE)
