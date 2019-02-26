CXX = clang++

DISABLEDWARNINGS =-Wno-unused-variable -Wno-unused-function -Wno-char-subscripts

GAMECXXFLAGS =
GAMELDFLAGS =-dynamiclib
PLATFORMCXXFLAGS =`sdl2-config --cflags`
PLATFORMLDFLAGS =`sdl2-config --static-libs`
COMMONCXXFLAGS =-I inc/ -Wall -Werror -g -std=c++11 -DEMEAKA_INTERNAL=1 -DEMEAKA_SLOW=1 -DEMEAKA_DEBUG=1 $(DISABLEDWARNINGS)
COMMONLDFLAGS =

all:	osx_emeaka emeaka_game

osx_emeaka:
	$(CXX) $(COMMONCXXFLAGS) $(PLATFORMCXXFLAGS) $(PLATFORMLDFLAGS) $(COMMONLDFLAGS) src/osx_emeaka.cpp -o bin/emeaka

emeaka_game:
	$(CXX) $(COMMONCXXFLAGS) $(GAMECXXFLAGS) $(COMMONLDFLAGS) $(GAMELDFLAGS) src/emeaka.cpp -o bin/emeaka_game.dylib

tests:
	$(CXX) $(COMMONCXXFLAGS) -DEMEAKA_TEST=1 -I src/ test/test_main.cpp -o bin/emeaka_tests

clean:
	rm -rf bin/*