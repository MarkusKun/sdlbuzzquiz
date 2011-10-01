all : sdltest.bin

SDL_CXXFLAGS := $(shell sdl-config --cflags)
SDL_LNFLAGS  := $(shell sdl-config --libs)

CXXFLAGS := $(SDL_CXXFLAGS)
LNFLAGS := $(SDL_LNFLAGS) -lSDL_ttf

sdltest.bin : sdltest.o buzzer.o drawhelper.o quiz_interface.o
	g++ -o $@ $(LNFLAGS) $^
  
  
sdltest.o : sdltest.cc
	g++ -c $(CXXFLAGS) -o $@ $^
	
run :
	./sdltest.bin
	
