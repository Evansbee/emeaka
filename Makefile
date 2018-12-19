CXX = clang
CXXFLAGS = -I inc/ -Wall -g -std=c++11 -DEMEAKA_INTERNAL -DEMEAKA_DEBUG
LDFLAGS =

all:	osx_emeaka emeaka_game

osx_emeaka:
	$(CXX) $(CXXFLAGS) -F /Library/Frameworks -framework SDL2 -I /Library/Frameworks/SDL2.framework/Headers src/osx_emeaka.cpp -o bin/emeaka

emeaka_game:
	$(CXX) $(CXXFLAGS) -dynamiclib src/emeaka.cpp -o bin/emeaka_game.dylib

clean:
	rm bin/*.o