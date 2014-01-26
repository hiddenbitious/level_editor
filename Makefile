CC=clang++
CFLAGS=-c -Wall -O0 -g

GL_PATH=/usr/lib/x86_64-linux-gnu/mesa/
GLEW_PATH=/usr/lib/x86_64-linux-gnu/
GLUT_PATH=/usr/lib/x86_64-linux-gnu/

#LDFLAGS=-Wl,-rpath=../oglRenderer
LDFLAGS=
LIBS=-L$(GLEW_PATH) -L$(GL_PATH) -lm -lGL -lglut -lGLU

SOURCES=source/editor.cpp source/tile.cpp source/popUp.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=editor

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm source/*.o $(EXECUTABLE)
