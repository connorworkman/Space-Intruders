CXX = g++
CXXFLAGS = -g -std=c++11

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

all: spaceIntruders

SPACE_INTRUDERS_OBJECTS = spaceIntruders.o
spaceIntruders: $(SPACE_INTRUDERS_OBJECTS)
	$(CXX) -o $@ $(SPACE_INTRUDERS_OBJECTS) -lSDL2 -lSDL2_image -lSDL2_ttf

# Remove all objects and test program
clean: 
	$(RM) *.o spaceIntruders
