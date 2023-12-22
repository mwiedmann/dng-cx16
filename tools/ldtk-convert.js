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
  // const addDummyBytes = (bytes) => {
  //   for (let i = tileWidth; i < tileMax; i++) {
  //     bytes.push(0);
  //     bytes.push(0);
  //   }
  // };

  // // { "px": [48,32], "src": [80,80], "f": 0, "t": 65, "d": [83] },
  // const addTiles = (gridTiles, override0) => {
  //   const bytes = [];
  //   let col = 0;
  //   gridTiles.forEach((g) => {
  //     bytes.push(g.t === override0 ? 0 : g.t + 1); // Add 1 because we have a blank tile before the loaded tiles

  //     const flip =
  //       ((g.f === 2 || g.f === 3 ? 1 : 0) << 3) +
  //       ((g.f === 1 || g.f === 3 ? 1 : 0) << 2);
  //     bytes.push(flip);
  //     col++;

  //     if (col == tileWidth) {
  //       addDummyBytes(bytes);
  //       col = 0;
  //     }
  //   });

  //   return bytes;
  // };

  const addMapData = (gridTiles, override0) => {
      const bytes = [];
      let col = 0;
      gridTiles.forEach((g) => {
        bytes.push(g.t);
      });
  
      return bytes;
    };

  level.layerInstances.forEach((li) => {
    let bytes = undefined;

    switch (li.__identifier) {
      case "Map":
        bytes = addMapData(li.gridTiles);
        break;
    }

    if (bytes) {
      let outputFilename;
      let output;

      outputFilename = `L${levelNum}.BIN`;
      output = new Uint8Array(bytes);
      
      fs.writeFileSync(`build/${outputFilename}`, output, "binary");
    }
  });
};

d.levels.forEach((l) => {
  const levelNum = l.identifier.split("_")[1];
  createLevelCode(levelNum, l);
});
