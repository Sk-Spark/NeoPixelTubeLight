
var activeButton = null;
var colorCanvas = null;
//just for local dev
var mock_modes = "<li><a href='#'>Static</a></li><li><a href='#'>Blink</a></li><li><a href='#'>Breath</a></li><li><a href='#'>Color Wipe</a></li><li><a href='#'>Color Wipe Inverse</a></li><li><a href='#'>Color Wipe Reverse</a></li><li><a href='#'>Color Wipe Reverse Inverse</a></li><li><a href='#'>Color Wipe Random</a></li><li><a href='#'>Random Color</a></li><li><a href='#'>Single Dynamic</a></li><li><a href='#'>Multi Dynamic</a></li><li><a href='#'>Rainbow</a></li><li><a href='#'>Rainbow Cycle</a></li><li><a href='#'>Scan</a></li><li><a href='#'>Dual Scan</a></li><li><a href='#'>Fade</a></li><li><a href='#'>Theater Chase</a></li><li><a href='#'>Theater Chase Rainbow</a></li><li><a href='#'>Running Lights</a></li><li><a href='#'>Twinkle</a></li><li><a href='#'>Twinkle Random</a></li><li><a href='#'>Twinkle Fade</a></li><li><a href='#'>Twinkle Fade Random</a></li><li><a href='#'>Sparkle</a></li><li><a href='#'>Flash Sparkle</a></li><li><a href='#'>Hyper Sparkle</a></li><li><a href='#'>Strobe</a></li><li><a href='#'>Strobe Rainbow</a></li><li><a href='#'>Multi Strobe</a></li><li><a href='#'>Blink Rainbow</a></li><li><a href='#'>Chase White</a></li><li><a href='#'>Chase Color</a></li><li><a href='#'>Chase Random</a></li><li><a href='#'>Chase Rainbow</a></li><li><a href='#'>Chase Flash</a></li><li><a href='#'>Chase Flash Random</a></li><li><a href='#'>Chase Rainbow White</a></li><li><a href='#'>Chase Blackout</a></li><li><a href='#'>Chase Blackout Rainbow</a></li><li><a href='#'>Color Sweep Random</a></li><li><a href='#'>Running Color</a></li><li><a href='#'>Running Red Blue</a></li><li><a href='#'>Running Random</a></li><li><a href='#'>Larson Scanner</a></li><li><a href='#'>Comet</a></li><li><a href='#'>Fireworks</a></li><li><a href='#'>Fireworks Random</a></li><li><a href='#'>Merry Christmas</a></li><li><a href='#'>Fire Flicker</a></li><li><a href='#'>Fire Flicker (soft)</a></li><li><a href='#'>Fire Flicker (intense)</a></li><li><a href='#'>Circus Combustus</a></li><li><a href='#'>Halloween</a></li><li><a href='#'>Bicolor Chase</a></li><li><a href='#'>Tricolor Chase</a></li><li><a href='#'>TwinkleFOX</a></li><li><a href='#'>Rain</a></li><li><a href='#'>Custom 0</a></li><li><a href='#'>Custom 1</a></li><li><a href='#'>Custom 2</a></li><li><a href='#'>Custom 3</a></li><li><a href='#'>Custom 4</a></li><li><a href='#'>Custom 5</a></li><li><a href='#'>Custom 6</a></li><li><a href='#'>Custom 7</a></li>";

window.addEventListener('DOMContentLoaded', (event) => {
  // init the canvas color picker
  colorCanvas = document.getElementById('color-canvas');
  var colorctx = colorCanvas.getContext('2d');

  // Create color gradient
  var gradient = colorctx.createLinearGradient(0, 0, colorCanvas.width - 1, 0);
  gradient.addColorStop(0,    "rgb(255,   0,   0)");
  gradient.addColorStop(0.16, "rgb(255,   0, 255)");
  gradient.addColorStop(0.33, "rgb(0,     0, 255)");
  gradient.addColorStop(0.49, "rgb(0,   255, 255)");
  gradient.addColorStop(0.66, "rgb(0,   255,   0)");
  gradient.addColorStop(0.82, "rgb(255, 255,   0)");
  gradient.addColorStop(1,    "rgb(255,   0,   0)");

  // Apply gradient to canvas
  colorctx.fillStyle = gradient;
  colorctx.fillRect(0, 0, colorCanvas.width - 1, colorCanvas.height - 1);

  // Create semi transparent gradient (white -> transparent -> black)
  gradient = colorctx.createLinearGradient(0, 0, 0, colorCanvas.height - 1);
  gradient.addColorStop(0,    "rgba(255, 255, 255, 1)");
  gradient.addColorStop(0.48, "rgba(255, 255, 255, 0)");
  gradient.addColorStop(0.52, "rgba(0,     0,   0, 0)");
  gradient.addColorStop(1,    "rgba(0,     0,   0, 1)");

  // Apply gradient to canvas
  colorctx.fillStyle = gradient;
  colorctx.fillRect(0, 0, colorCanvas.width - 1, colorCanvas.height - 1);

  // setup the canvas click listener
  colorCanvas.addEventListener('click', (event) => {
    var imageData = colorCanvas.getContext('2d').getImageData(event.offsetX, event.offsetY, 1, 1);

    var selectedColor = 'rgb(' + imageData.data[0] + ',' + imageData.data[1] + ',' + imageData.data[2] + ')'; 
    console.log('click: ' + event.offsetX + ', ' + event.offsetY + ', ' + selectedColor);
    document.getElementById('color-value').value = selectedColor;

    selectedColor = imageData.data[0] * 65536 + imageData.data[1] * 256 + imageData.data[2];
    console.log("selectedColor:",selectedColor);
    submitVal('c', selectedColor);
  });

  // get list of modes from ESP
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
   if (xhttp.readyState == 4 && xhttp.status == 200) {
     document.getElementById('modes').innerHTML = xhttp.responseText;
     modes = document.querySelectorAll('ul#modes li a');
     modes.forEach(initMode);
    }
    else{
      //for local mock
      document.getElementById('modes').innerHTML = mock_modes;
      modes = document.querySelectorAll('ul#modes li a');
      modes.forEach(initMode);
    }
  };
  xhttp.open('GET', 'modes', true);
  xhttp.send();
});

function initMode(mode, index) {
  mode.addEventListener('click', (event) => onMode(event, index));
}

function onColor(event, color) {
  console.log("onColor:",co);
  event.preventDefault();
  var match = color.match(/rgb\(([0-9]*),([0-9]*),([0-9]*)\)/);
  if(match) {
    var colorValue = Number(match[1]) * 65536 + Number(match[2]) * 256 + Number(match[3]);
    console.log('onColor:' + match[1] + "," + match[2] + "," + match[3] + "," + colorValue);
    submitVal('c', colorValue);
  }
}

function onMode(event, mode) {
  event.preventDefault();
  if(activeButton) activeButton.classList.remove('active')
  activeButton = event.target;
  activeButton.classList.add('active');
  submitVal('m', mode);
}

function onBrightness(event, dir) {
  event.preventDefault();
  submitVal('b', dir);
}

function onSpeed(event, dir) {
  event.preventDefault();
  submitVal('s', dir);
}

function onAuto(event, dir) {
  event.preventDefault();
  submitVal('a', dir);
}

function submitVal(name, val) {
  var xhttp = new XMLHttpRequest();
  xhttp.open('GET', 'set?' + name + '=' + val, true);
  xhttp.send();
}