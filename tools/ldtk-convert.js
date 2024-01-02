/*
Read the .ldtk file and turn it into a series of .bin files (one per level)
The bin file is formatted for the C data structs so they can be malloc'd and loaded in
*/
const fs = require("fs");

const rawText = fs.readFileSync("gfx/dng.ldtk");
const d = JSON.parse(rawText);

const tileWidth = 32

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
    } else if (tileId >= 40 && tileId <= 42) { // Door
      return 7;
    } else if (tileId == 48) { // Key
      return 8;
    } else if (tileId == 44) { // Chest
      return 9;
    } else if (tileId == 45) { // Gold
      return 13;
    } else if (tileId == 46) { // Silver
      return 14;
    } else if (tileId == 43) { // Exit
      return 10;
    } else if (tileId == 49) { // Scroll
      return 11;
    } else if (tileId == 50) { // Food Big
      return 12;
    } else if (tileId == 51) { // Food Small
      return 15;
    } else {
      return 2; // Floor
    }
  }

  const getTileData = (gridTiles, override0) => {
      const tileBytes = createLayer(0);
      const mapBytes = createMapArray(0);

      gridTiles.forEach((g) => {
        const xTile = g.px[0]/16;
        const yTile = g.px[1]/16;

        tileBytes[(yTile*2*tileWidth)+(xTile*2)] = g.t

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
        g.t >= 128 && g.t <= 164 // Entity: TODO: Lots of these
          ? 33 + ((g.t-128)/4)
          : g.t >= 56 && g.t <= 60  // Generator: Which type of monster?
            ? 20 + g.t-56
            : g.t >= 40 && g.t <= 42
              ? 7 // Door
              : 5; // Guy
    });
  }

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
