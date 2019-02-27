CXX = clang++

DISABLEDWARNINGS =-Wno-unused-variable -Wno-unused-function -Wno-char-subscripts

TESTCXXFLAGS = -DEMEAKA_TEST=1 -I src/game/ -I src/engine/
TESTLDFLAGS =
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
TESTOBJS = $(patsubst test%,tmp/test%,$(TESTFILES:.cpp=.o))


all:	directories osx_emeaka emeaka_game

osx_emeaka:	$(PLATFORMOBJS)
	@echo Linking $@...
	@$(CXX) $(PLATFORMLDFLAGS) $(COMMONLDFLAGS) $(PLATFORMOBJS) -o bin/emeaka

emeaka_game: $(GAMEOBJS)
	@echo Linking $@...
	@$(CXX) $(COMMONLDFLAGS) $(GAMELDFLAGS) $(GAMEOBJS) -o bin/emeaka_game.dylib

tmp/engine/%.o: src/engine/%.cpp
	@echo Compiling $@
	@$(CXX) $(COMMONCXXFLAGS) $(PLATFORMCXXFLAGS) -c -o  $@ $<

tmp/game/%.o: src/game/%.cpp
	@echo Compiling $@
	@$(CXX) $(COMMONCXXFLAGS) $(GAMECXXFLAGS) -c -o  $@ $<

tests: $(TESTOBJS)
	@echo Linking $@...
	@$(CXX) $(TESTLDFLAGS) $(COMMONLDFLAGS) $(TESTOBJS) -o bin/emeaka_tests

tmp/test/%.o: test/%.cpp
	@echo Compiling $@
	@$(CXX) $(COMMONCXXFLAGS) $(TESTCXXFLAGS) -c -o  $@ $<

directories: 
	@mkdir -p tmp/engine tmp/game tmp/test
	@mkdir -p bin

clean:
	@rm -rf bin/ tmp/