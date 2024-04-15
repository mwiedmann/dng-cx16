CC=cl65
EMU=../x16emur47/x16emu

make:
	$(CC) --cpu 65SC02 -Or -Cl -C cx16-zsm-bank.cfg -o ./build/DNG.PRG -t cx16 \
	src/main.c src/globals.c src/ai.c src/config.c src/intro.c src/joy.c src/list.c src/map.c src/players.c \
	src/sprites.c src/strtbl.c src/tiles.c src/utils.c src/wait.c \
	src/sound.c src/zsmkit.lib

run:
	cd build && \
	$(EMU) -prg DNG.PRG -run

emu:
	$(EMU)

strings:
	$(CC) --cpu 65C02 -Or -Cl -o ./tools/STRBLD.PRG -t cx16 src/strbld.c src/strtbl.c
	cd tools && \
	$(EMU) -prg STRBLD.PRG -run && \
	mv STRINGS.BIN ../build/STRINGS.BIN

ldtk:
	node tools/ldtk-convert.js

img:
	node tools/gimp-img-convert.js gfx/tiles.data build/D0TILES.BIN 16 16 0 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/D1TILES.BIN 16 16 32 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/D2TILES.BIN 16 16 64 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/D3TILES.BIN 16 16 96 8 4
	node tools/gimp-img-convert.js gfx/tiles.data build/GMTILES.BIN 16 16 128 8 14
	node tools/gimp-img-convert.js gfx/tiles.data build/OVTILES.BIN 8 8 960 16 16
	node tools/gimp-img-convert.js gfx/title.data build/TITLE.BIN 320 240 0 1 1
	node tools/gimp-img-convert.js gfx/titlecmp.data build/TITLECMP.BIN 320 240 0 1 1

pal:
	node tools/gimp-pal-convert.js gfx/tiles.data.pal build/TILES.PAL
	node tools/gimp-pal-convert.js gfx/title.data.pal build/TITLE.PAL
	node tools/gimp-pal-convert.js gfx/titlecmp.data.pal build/TITLECMP.PAL

snd:
	tools/raw2zcm -q 8000 sound/welcome.raw build/WELCOME.ZCM
	tools/raw2zcm -q 8000 sound/keys.raw build/KEYS.ZCM
	tools/raw2zcm -q 8000 sound/treasure.raw build/TREASURE.ZCM
	tools/raw2zcm -q 8000 sound/scrolls.raw build/SCROLLS.ZCM
	tools/raw2zcm -q 8000 sound/food.raw build/FOOD.ZCM
	tools/raw2zcm -q 8000 sound/nigh.raw build/NIGH.ZCM
	tools/raw2zcm -q 8000 sound/laugh.raw build/LAUGH.ZCM

zip:
	cd build && \
	rm -f dng.zip && \
	zip dng.zip *

all:
	make ldtk
	make pal
	make img
	make
	make zip
