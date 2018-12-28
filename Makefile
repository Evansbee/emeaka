CXX = clang

DISABLEDWARNINGS =-Wno-unused-variable -Wno-unused-function -Wno-char-subscripts

GAMECXXFLAGS =
GAMELDFLAGS =-dynamiclib
PLATFORMCXXFLAGS =-I /Library/Frameworks/SDL2.framework/Headers
PLATFORMLDFLAGS =-F /Library/Frameworks -framework SDL2
COMMONCXXFLAGS =-I inc/ -Wall -Werror -g -std=c++11 -DEMEAKA_INTERNAL -DEMEAKA_DEBUG $(DISABLEDWARNINGS)
COMMONLDFLAGS =

all:	osx_emeaka emeaka_game

osx_emeaka:
	$(CXX) $(COMMONCXXFLAGS) $(PLATFORMCXXFLAGS) $(PLATFORMLDFLAGS) $(COMMONLDFLAGS) src/osx_emeaka.cpp -o bin/emeaka

emeaka_game:
	$(CXX) $(COMMONCXXFLAGS) $(GAMECXXFLAGS) $(COMMONLDFLAGS) $(GAMELDFLAGS) src/emeaka.cpp -o bin/emeaka_game.dylib

clean:
	rm -rf bin/*