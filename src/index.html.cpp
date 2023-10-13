#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <link rel="stylesheet" href="main.css" />
    <script type="text/javascript" src="tinycolor.js"></script>
    <title>Night Lamp</title>
  </head>
  <body>
    <div class="color-picker-panel">
      <div class="panel-row">
        <div class="swatches default-swatches"></div>
        <button class="button eyedropper">Get Color</button>
      </div>
      <div class="panel-row">
        <div class="spectrum-map">
          <button id="spectrum-cursor" class="color-cursor"></button>
          <canvas id="spectrum-canvas"></canvas>
        </div>
        <div class="hue-map">
          <button id="hue-cursor" class="color-cursor"></button>
          <canvas id="hue-canvas"></canvas>
        </div>
      </div>
      <div class="panel-row">
        <div id="rgb-fields" class="field-group value-fields rgb-fields active">
          <div class="field-group">
            <label for="" class="field-label">R:</label>
            <input
              type="number"
              max="255"
              min="0"
              id="red"
              class="field-input rgb-input"
            />
          </div>
          <div class="field-group">
            <label for="" class="field-label">G:</label>
            <input
              type="number"
              max="255"
              min="0"
              id="green"
              class="field-input rgb-input"
            />
          </div>
          <div class="field-group">
            <label for="" class="field-label">B:</label>
            <input
              type="number"
              max="255"
              min="0"
              id="blue"
              class="field-input rgb-input"
            />
          </div>
        </div>
        <div id="hex-field" class="field-group value-fields hex-field">
          <label for="" class="field-label">Hex:</label>
          <input type="text" id="hex" class="field-input" />
        </div>
        <button id="mode-toggle" class="button mode-toggle">Mode</button>
      </div>
      <div class="panel-row">
        <h2 class="panel-header">User Colors</h2>
        <div id="user-swatches" class="swatches custom-swatches"></div>
        <button id="add-swatch" class="button add-swatch">
          <span id="color-indicator" class="color-indicator"></span
          ><span>Add to Swatches</span>
        </button>
      </div>
      <div class="panel-row">
        <h2 class="panel-header">Brightness</h2>
        <div class="panel-controls">
          <button id="add-swatch" class="button action-btn" onclick="onBrightness(event, '-')">☼</button>
          <button id="add-swatch" class="button action-btn" onclick="onBrightness(event, '+')">☀</button>
        </div>
      </div>
      <div class="panel-row">
        <h2 class="panel-header">Speed</h2>
        <div class="panel-controls">
          <button id="add-swatch" class="button action-btn" onclick="onSpeed(event, '-')">-</button>
          <button id="add-swatch" class="button action-btn" onclick="onSpeed(event, '+')">+</button>
        </div>
      </div>
      <div class="panel-row">
        <h2 class="panel-header">Auto Cycle</h2>
        <div class="panel-controls">
          <button id="add-swatch" class="button action-btn" onclick="onAuto(event, '-')">■</button>
          <button id="add-swatch" class="button action-btn" onclick="onAuto(event, '+')">►</button>
        </div>
      </div>
    </div>
    <div>
      <h2 class="effects-header">Effects</h2>
      <ul id='modes' class='flex-row-wrap'>
    </div>
  </body>
  <script type="text/javascript" src="main.js"></script>
</html>

)=====";
