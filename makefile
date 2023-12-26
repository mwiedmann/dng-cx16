CC=cl65
EMU=../x16emur46/x16emu

make:
	$(CC) --cpu 65C02 -Or -Cl -C cx16.cfg -o ./build/DNG.PRG -t cx16 \
	src/main.c src/globals.c src/ai.c src/config.c src/joy.c src/list.c src/map.c src/sprites.c src/tiles.c src/utils.c src/wait.c

run:
	cd build && \
	$(EMU) -prg DNG.PRG -run

ldtk:
	node tools/ldtk-convert.js

gimp:
	node tools/gimp-convert.js tiles

zip:
	cd build && \
	rm -f dng.zip && \
	zip dng.zip *

all:
	make ldtk
	make gimp
	make
	make zip

