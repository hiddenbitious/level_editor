CXX=g++
CXXFLAGS=-c -Wall -O0 -g -MMD -Wall -pedantic -Wno-unused-result -Wno-sign-compare

LDFLAGS=-lm -lGL -lglut -lGLU

SOURCES=source/editor.cpp source/tile.cpp source/popUp.cpp source/map.cpp
OBJECTS=$(SOURCES:.cpp=.o)
DEPS = $(SOURCES:.cpp=.d)
EXECUTABLE=editor

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

-include $(DEPS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm source/*.o $(EXECUTABLE)
	rm source/*.d
