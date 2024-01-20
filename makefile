CC=cl65
EMU=../x16emur46/x16emu

make:
	$(CC) --cpu 65C02 -Or -Cl -C cx16-zsm-bank.cfg -o ./build/DNG.PRG -t cx16 \
	src/main.c src/globals.c src/ai.c src/config.c src/intro.c src/joy.c src/list.c src/map.c src/players.c \
	src/sprites.c src/strtbl.c src/tiles.c src/utils.c src/wait.c \
	src/sound.c src/zsmkit.lib

strings:
	$(CC) --cpu 65C02 -Or -Cl -o ./tools/STRBLD.PRG -t cx16 src/strbld.c src/strtbl.c
	cd tools && \
	$(EMU) -prg STRBLD.PRG -run && \
	mv STRINGS.BIN ../build/STRINGS.BIN

run:
	cd build && \
	$(EMU) -prg DNG.PRG -run

ldtk:
	node tools/ldtk-convert.js


const inputFile = process.argv[2];
const outputFile = process.argv[3];

const frameWidth = process.argv[4];
const frameHeight = process.argv[5];
const startingTile = process.argv[6];
const xTiles = process.argv[7];
const yTiles = process.argv[8];

img:
	node tools/gimp-img-convert.js gfx/tiles.data build/D0TILES.BIN 16 16 0 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/D1TILES.BIN 16 16 32 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/D2TILES.BIN 16 16 64 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/D3TILES.BIN 16 16 96 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/GMTILES.BIN 16 16 128 8 14
	node tools/gimp-img-convert.js gfx/tiles.data build/OVTILES.BIN 8 8 960 16 16

pal:
	node tools/gimp-pal-convert.js

zip:
	cd build && \
	rm -f dng.zip && \
	zip dng.zip *

clean:
	find build/ -name "DNG.PRG.0[A-F|1|3-9]" -delete

all:
	make ldtk
	make pal
	make img
	make
	make clean
	make zip

