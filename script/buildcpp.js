// Include fs module
const fs = require('fs');
const copyFromDir = __dirname + '/../src/view';
const files = ['main.js', 'index.html', 'main.css'];
// const copyToDir = __dirname+'/../../'+'buildCpp';
const copyToDir = __dirname + '/../src';

files.forEach((file) => {
  // Use fs.readFile() method to read the file
  fs.readFile(copyFromDir + '/' + file, 'utf8', function (err, data) {
    // console.log(data);
    // const filename = file.replace(/\./g, '_');
    const filename = file;
    const headerFilename = filename + '_H';
    const payload = `
      #include <pgmspace.h>
      char ${filename.replace(/\./g, '_')}[] PROGMEM = R"rawliteral(
          ${data}
          )rawliteral";
      `;
    if (!fs.existsSync(copyToDir)) {
      fs.mkdirSync(copyToDir, () => {});
    }
    fs.writeFile(copyToDir + '/' + filename + '.cpp', payload, (err) => {
      if (err) console.log(err);
      else console.log('Copied file: ', copyToDir + '/' + filename);
    });
  });
});