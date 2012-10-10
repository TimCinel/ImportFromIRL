#Makefile: Sun Jul 22 00:24:15 EST 2012 pknowles */

DEBUG = -g
OPTIMISE = -O0

CFLAGS = `sdl-config --cflags --libs` -lGL -lGLU -lglut $(DEBUG) $(OPTIMISE)
LDFLAGS = -lGL -lGLU -lglut -lm -lGLEW `sdl-config --libs` 
EXE = ImportFromIRL
OBJECTS = ImportFromIRL.o sdlbase.o KinectReceiver.o DummyKinectInterface.o ObjectModel.o GeometryOperations.o CameraCursorReceiver.o CullPlane.o PlaneCursorReceiver.o ObjectFragment.o

#default target
all: $(EXE)

#executable
$(EXE) : $(OBJECTS)
	g++ -o $@ $(LDFLAGS) $(OBJECTS)

#general object file rule
%.o : %.cpp
	g++ -c -o $@ $(CFLAGS) $<

#additional dependencies
sdlbase.o : sdlbase.h

#clean (-f stops error if files don't exist)
clean:
	rm -f $(EXE) \
	      $(OBJECTS)
