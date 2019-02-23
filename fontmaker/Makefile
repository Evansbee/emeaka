CXXFLAGS = -std=c++11 -Wno-backslash-newline-escape `freetype-config --cflags --libs` 
OUTPUT = fontmaker
all:	${OUTPUT}
    
${OUTPUT}:	main.cpp
	clang++ ${CXXFLAGS} main.cpp -o ${OUTPUT}

fonts:	${OUTPUT}
	./${OUTPUT} DejaVuSans.ttf 12 DejaVuSans 1
	./${OUTPUT} DejaVuSans.ttf 16 DejaVuSans 1
	./${OUTPUT} DejaVuSans.ttf 24 DejaVuSans 1
	./${OUTPUT} DejaVuSans.ttf 32 DejaVuSans 8
	./${OUTPUT} DejaVuSans.ttf 64 DejaVuSans 8
	./${OUTPUT} DejaVuSansMono.ttf 12 DejaVuSansMono 1 
	./${OUTPUT} DejaVuSansMono.ttf 16 DejaVuSansMono 1
	./${OUTPUT} DejaVuSansMono.ttf 24 DejaVuSansMono 1
	./${OUTPUT} DejaVuSansMono.ttf 32 DejaVuSansMono 8 
	./${OUTPUT} DejaVuSansMono.ttf 64 DejaVuSansMono 8
	./${OUTPUT} ProggyClean.ttf 16 Proggy 1
	./${OUTPUT} ProggyClean.ttf 32 Proggy 1
	mv -f *Font.h ../inc/Fonts/

clean:
	rm -f fontmaker *Debug.txt