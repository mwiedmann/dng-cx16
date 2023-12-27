/*
Read the .ldtk file and turn it into a series of .bin files (one per level)
The bin file is formatted for the C data structs so they can be malloc'd and loaded in
*/
const fs = require("fs");

const rawText = fs.readFileSync("gfx/dng.ldtk");
const d = JSON.parse(rawText);

const tileWidth = 90
const tileMax = 128

const createLevelCode = (levelNum, level) => {
  const getMapIdFromTile = (tileId) => {
    if (tileId >= 2 && tileId <= 17) { // Wall
      return 3;
    } else {
      return 2; // Floor
    }
  }

  const getTileData = (gridTiles, override0) => {
      const tileBytes = [];
      const mapBytes = [];
      let i=0;

      const addDummyBytes = () => {
        for (let i = 90; i < 128; i++) {
          tileBytes.push(0);
          tileBytes.push(0);
        }
      };

      gridTiles.forEach((g) => {
        tileBytes.push(g.t);
        tileBytes.push(0); // No flipping in this set

        mapBytes.push(getMapIdFromTile(g.t));

        i++;
        if (i == 90) {
          addDummyBytes()
          i = 0;
        }
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
      currentMapData[(y * 90) + x] = g.t == 62 ? 4 /*Entity*/ : g.t==48 ? 6 /*Generator*/ : 5 /* Guy*/;
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
