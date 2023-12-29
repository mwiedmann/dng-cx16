const fs = require("fs");

const filebase = process.argv[2];
const imageName = `gfx/${filebase}.data`
const palName = `gfx/${filebase}.data.pal`
const palOutputFilename = `build/${filebase.toUpperCase()}.PAL`
const l0OutputFilename = `build/L0${filebase.toUpperCase()}.BIN`
const l1OutputFilename = `build/L1${filebase.toUpperCase()}.BIN`

console.log(
  `imageName: ${imageName} palName ${palName}`
);

const palData = fs.readFileSync(palName);
const imageData = [...fs.readFileSync(imageName)];

const finalPal = []

const adjustColor = (c) => c>>4

// The raw data is just a long array of R,G and B bytes
// Convert them to G+B and R (4 bytes each)
for (let i = 0; i < palData.length; i+=3) {
    finalPal.push((adjustColor(palData[i+1])<<4) + adjustColor(palData[i+2]))
    finalPal.push(adjustColor(palData[i]))
}

const l0FlattenedTiles = [];
const l1FlattenedTiles = [];
let frameWidth = 16
let frameHeight = 16
let xTiles = 8
let yTiles = 21

let ty, tx, y, x, start, pixelIdx;
const l0Size = xTiles * yTiles * frameWidth * frameHeight
const l0ImageData = imageData.slice(0, l0Size)
const l1ImageData = imageData.slice(l0Size)

// L0
for (ty = 0; ty < yTiles; ty++) {
  for (tx = 0; tx < xTiles; tx++) {
    for (y = 0; y < frameHeight; y++) {
      start =
        ty * xTiles * frameWidth * frameHeight +
        tx * frameWidth +
        y * xTiles * frameWidth;
      for (x = 0; x < frameWidth; x++) {
        pixelIdx = start + x;
        l0FlattenedTiles.push(l0ImageData[pixelIdx]);
      }
    }
  }
}

// L1
frameWidth = 8
frameHeight = 8
xTiles = 16
yTiles = 6

for (ty = 0; ty < yTiles; ty++) {
  for (tx = 0; tx < xTiles; tx++) {
    for (y = 0; y < frameHeight; y++) {
      start =
        ty * xTiles * frameWidth * frameHeight +
        tx * frameWidth +
        y * xTiles * frameWidth;
      for (x = 0; x < frameWidth; x++) {
        pixelIdx = start + x;
        l1FlattenedTiles.push(l1ImageData[pixelIdx]);
      }
    }
  }
}

let output = new Uint8Array(finalPal);
fs.writeFileSync(palOutputFilename, output, "binary");

output = new Uint8Array([...l0FlattenedTiles]);
fs.writeFileSync(l0OutputFilename, output, "binary");

output = new Uint8Array([...l1FlattenedTiles]);
fs.writeFileSync(l1OutputFilename, output, "binary");

console.log(`Generated files ${palOutputFilename} ${l0OutputFilename} ${l1OutputFilename}`);
