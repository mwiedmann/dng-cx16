/*
Read the .ldtk file and turn it into a series of .bin files (one per level)
The bin file is formatted for the C data structs so they can be malloc'd and loaded in
*/
const fs = require("fs");

const rawText = fs.readFileSync("gfx/dng.ldtk");
const d = JSON.parse(rawText);

const tileWidth = 32

const levelStrings = []
const levelStringLengths = []
const levelStringAddr = []

const createLevelCode = (levelNum, level) => {
  const createLayer = (val) => {
    return new Array(tileWidth*tileWidth*2).fill(val);
  }

  const createMapArray = (val) => {
    return new Array(tileWidth*tileWidth).fill(val);
  }

  const getMapIdFromTile = (tileId) => {
    if (tileId >= 2 && tileId <= 17) { // Wall
      return 3;
    } else if (tileId == 30 || tileId == 31) { // Door
      return 7;
    } else if (tileId == 38) { // Key
      return 8;
    } else if (tileId == 35) { // Chest
      return 9;
    } else if (tileId == 36) { // Gold
      return 13;
    } else if (tileId == 37) { // Silver
      return 14;
    } else if (tileId == 32) { // Exit 1
      return 10;
    } else if (tileId == 33) { // Exit +5
      return 16;
    } else if (tileId == 34) { // Exit +10
      return 17;
    } else if (tileId == 39) { // Scroll
      return 11;
    } else if (tileId == 40) { // Food Big
      return 12;
    } else if (tileId == 41) { // Food Small
      return 15;
    } else if (tileId == 42) { // Speed Boost
      return 18;
    } else if (tileId == 43) { // Melee Boost
      return 19;
    } else if (tileId == 44) { // Ranged Boost
      return 20;
    } else if (tileId == 45) { // Magic Boost
      return 21;
    } else if (tileId == 46) { // Armor Boost
      return 22;
    } else if (tileId == 47) { // Teleporter
      return 23;
    }else {
      return 2; // Floor
    }
  }

  const getTileData = (gridTiles, override0) => {
      const tileBytes = createLayer(0);
      const mapBytes = createMapArray(0);

      gridTiles.forEach((g) => {
        const xTile = g.px[0]/16;
        const yTile = g.px[1]/16;
        const flip =
          ((g.f === 2 || g.f === 3 ? 1 : 0) << 3) +
          ((g.f === 1 || g.f === 3 ? 1 : 0) << 2);

        tileBytes[(yTile*2*tileWidth)+(xTile*2)] = g.t
        tileBytes[((yTile*2*tileWidth)+(xTile*2))+1] = flip

        mapBytes[(yTile*tileWidth)+xTile] = getMapIdFromTile(g.t);
      });
  
      return {
        tileBytes,
        mapBytes
      }
    };

  const addMapData = (gridTiles, currentMapData) => {
    // { "px": [144,16], "src": [96,112], "f": 0, "t": 62, "d": [99], "a": 1 },
    gridTiles.forEach((g) => {
      const x = g.px[0] / 16;
      const y = g.px[1] / 16;
      currentMapData[(y * tileWidth) + x] = 
        g.t >= 96 && g.t <= 132 // Entities
          ? 33 + ((g.t-96)/4) // 33 Entity map tile start
          : g.t >= 48 && g.t <= 55  // Generator
            ? 24 + g.t-48 // 23 Generator map tile start
            : g.t == 30 || g.t == 31
              ? 7 // Door
              : g.t == 47 // Teleporter
              ? 23
              : 5; // Guy
    });
  }

  const convertToPETSCII = (c) => c.charCodeAt(0) + (c >= 'A' && c <= 'Z' ? 128 : 0)

  // Add the two level strings
  levelStrings.push(...[...level.fieldInstances[0].__value.split('').map(convertToPETSCII), 0])
  levelStrings.push(...[...level.fieldInstances[1].__value.split('').map(convertToPETSCII), 0])

  levelStringLengths.push(level.fieldInstances[0].__value.length + 1)
  levelStringLengths.push(level.fieldInstances[1].__value.length + 1)

  let data = undefined
  level.layerInstances.forEach((li) => {
    switch (li.__identifier) {
      case "Map":
        data = getTileData(li.gridTiles);
        outputFilename = `L${levelNum}TILE.BIN`;
        output = new Uint8Array(data.tileBytes);
        
        fs.writeFileSync(`build/${outputFilename}`, output, "binary");
        break;

      case "Objects":
        addMapData(li.gridTiles, data.mapBytes);
        outputFilename = `L${levelNum}MAP.BIN`;
        output = new Uint8Array(data.mapBytes);
        
        fs.writeFileSync(`build/${outputFilename}`, output, "binary");
        break;
    }
  });
};

d.levels.forEach((l) => {
  const levelNum = l.identifier.split("_")[1];
  createLevelCode(levelNum, l);
});


// 2 byte address for each string
const stringArrayOffset = levelStringLengths.length * 2

// Start the level strings 4k into the bank
// This means we are limited to 4k size (check this)
let addr = 0xa000 + 4096 + stringArrayOffset

levelStringLengths.map(l => {
  levelStringAddr.push(addr & 0b0000000011111111)
  levelStringAddr.push(addr >>8)
  addr += l
})

fs.writeFileSync(`build/LVLSTR.BIN`, new Uint8Array([...levelStringAddr, ...levelStrings]), "binary");
