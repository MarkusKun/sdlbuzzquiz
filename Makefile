all : allbins 

SDL_CXXFLAGS := $(shell sdl-config --cflags)
SDL_LNFLAGS  := $(shell sdl-config --libs)

CXXFLAGS := $(SDL_CXXFLAGS) -Irapidxml-1.13 
LNFLAGS := $(SDL_LNFLAGS) -lSDL_ttf

CC := $(CXX) # compile all files as c++



BINFILE  := sdltest.bin
OBJFILES := sdltest.o buzzer.o drawhelper.o \
						quiz_interface.o admininterface.o \
						quiz_player.o quiz_sources.o \
						quiz_question.o


$(BINFILE) : $(OBJFILES)
	g++ -o $@ $(LNFLAGS) $^
  
allbins : $(BINFILE)

run :
	./$(BINFILE)
	
clean :
	rm -rf $(BINFILE) $(OBJFILES)
