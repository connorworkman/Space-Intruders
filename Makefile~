CXX = g++
CXXFLAGS = -g -std=c++11
LIB = -L/home/users/norris/public/cis330/soft/SDL2-2.0.3/lib/
INC = -I/home/users/norris/public/cis330/soft/SDL2-2.0.3/include/

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $<

all: spaceIntruders

SPACE_INTRUDERS_OBJECTS = spaceIntruders.o

spaceIntruders: $(SPACE_INTRUDERS_OBJECTS)
	$(CXX) -o $@ $(SPACE_INTRUDERS_OBJECTS) -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

ix: $(SPACE_INTRUDERS_OBJECTS)
	$(CXX) $(LIB) -o $@ $(SPACE_INTRUDERS_OBJECTS) -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Remove all objects and test program
clean: 
	$(RM) *.o spaceIntruders
