CXX = clang++

DISABLEDWARNINGS =-Wno-unused-variable -Wno-unused-function -Wno-char-subscripts

GAMECXXFLAGS =
GAMELDFLAGS =-dynamiclib
PLATFORMCXXFLAGS =`sdl2-config --cflags`
PLATFORMLDFLAGS =`sdl2-config --static-libs`
COMMONCXXFLAGS =-I inc/ -Wall -Werror -g -std=c++11 -DEMEAKA_INTERNAL=1 -DEMEAKA_SLOW=1 -DEMEAKA_DEBUG=1 $(DISABLEDWARNINGS)
COMMONLDFLAGS =

PLATFORMFILES = $(wildcard src/engine/osx*.cpp)
PLATFORMOBJS = $(patsubst src%,tmp%,$(PLATFORMFILES:.cpp=.o))
GAMEFILES = $(wildcard src/game/emeaka*.cpp)
GAMEOBJS = $(patsubst src%,tmp%,$(GAMEFILES:.cpp=.o))
TESTFILES = $(wildcard test/*.cpp)
TESTOBJS = $(patsubst test%,tmp/test/%,$(TESTFILES:.cpp=.o))

all:	directories osx_emeaka emeaka_game

osx_emeaka:	$(PLATFORMOBJS)
	$(CXX) $(COMMONCXXFLAGS) $(PLATFORMCXXFLAGS) $(PLATFORMLDFLAGS) $(COMMONLDFLAGS) $(PLATFORMOBJS) -o bin/emeaka

emeaka_game: $(GAMEOBJS)
	$(CXX) $(COMMONCXXFLAGS) $(GAMECXXFLAGS) $(COMMONLDFLAGS) $(GAMELDFLAGS) $(GAMEOBJS) -o bin/emeaka_game.dylib

tmp/engine/%.o: src/engine/%.cpp
	$(CXX) $(COMMONCXXFLAGS) $(PLATFORMCXXFLAGS) -c -o  $@ $<

tmp/game/%.o: src/game/%.cpp
	$(CXX) $(COMMONCXXFLAGS) $(GAMECXXFLAGS) -c -o  $@ $<

tests: test/test_main.o test/test_memory.cpp
	$(CXX) $(COMMONCXXFLAGS) -DEMEAKA_TEST=1 -I src/ test/test_main.o test/test_memory.cpp -o bin/emeaka_tests

test/test_main.o: test/test_main.cpp
	$(CXX) $(COMMONCXXFLAGS) -DEMEAKA_TEST=1 -I src/ test/test_main.cpp -c -o test/test_main.o

directories: 
	@mkdir -p tmp/engine tmp/game tmp/test
	@mkdir -p bin
	
clean:
	rm -rf bin/ tmp/