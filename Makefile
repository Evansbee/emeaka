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

FONTCXXFLAGS = -std=c++11 -Wno-backslash-newline-escape `freetype-config --cflags --libs` 


PLATFORMFILES = $(wildcard src/engine/osx*.cpp)
PLATFORMOBJS = $(patsubst src%,tmp%,$(PLATFORMFILES:.cpp=.o))

GAMEFILES = $(wildcard src/game/emeaka*.cpp)
GAMEOBJS = $(patsubst src%,tmp%,$(GAMEFILES:.cpp=.o))

TESTFILES = $(wildcard test/*.cpp)
TESTOBJS = $(patsubst test%,tmp/test%,$(TESTFILES:.cpp=.o))

RTFILES = $(wildcard src/rt/*.cpp)
RTOBJS = $(patsubst src%,tmp%,$(RTFILES:.cpp=.o))



all:	directories osx_emeaka emeaka_game raytrace fontmaker

fontmaker: fontmaker/main.cpp
	@echo Building $@...
	@$(CXX) $(FONTCXXFLAGS) fontmaker/main.cpp -o bin/fontmaker

osx_emeaka:	$(PLATFORMOBJS)
	@echo Linking $@...
	@$(CXX) $(PLATFORMLDFLAGS) $(COMMONLDFLAGS) $(PLATFORMOBJS) -o bin/emeaka

emeaka_game: $(GAMEOBJS)
	@echo Linking $@...
	@$(CXX) $(COMMONLDFLAGS) $(GAMELDFLAGS) $(GAMEOBJS) -o bin/emeaka_game.dylib

raytrace: $(GAMEOBJS) $(RTOBJS)
	@echo Linking $@...
	@$(CXX) $(COMMONLDFLAGS) $(GAMEOBJS) $(RTOBJS) -o bin/raytrace

tmp/engine/%.o: src/engine/%.cpp
	@echo $< \-\> $@...
	@$(CXX) $(COMMONCXXFLAGS) $(PLATFORMCXXFLAGS) -c -o  $@ $<

tmp/game/%.o: src/game/%.cpp
	@echo $< \-\> $@...
	@$(CXX) $(COMMONCXXFLAGS) $(GAMECXXFLAGS) -c -o  $@ $<

tmp/rt/%.o: src/rt/%.cpp
	@echo $< \-\> $@...
	@$(CXX) $(COMMONCXXFLAGS) -c -o  $@ $<

tests: directories $(TESTOBJS) $(GAMEOBJS) $(RTOBJS)
	@echo Linking $@...
	@$(CXX) $(TESTLDFLAGS) $(COMMONLDFLAGS) $(TESTOBJS) $(GAMEOBJS) $(RTOBJS) -o bin/emeaka_tests

tmp/test/%.o: test/%.cpp
	@echo $< \-\> $@...
	@$(CXX) $(COMMONCXXFLAGS) $(TESTCXXFLAGS) -c -o  $@ $<

directories: 
	@mkdir -p tmp/engine tmp/game tmp/test tmp/rt
	@mkdir -p bin

clean:
	@rm -rf bin/ tmp/