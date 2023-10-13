#include <pgmspace.h>

/*
The tiny Javascript/canvas based color picker is based on the clever work of the folks
at Sparkbox. https://seesparkbox.com/foundry/how_i_built_a_canvas_color_picker
*/

char main_css[] PROGMEM = R"=====(
body {
  background: #1f232a;
  font-family: "Proxima Nova", sans-serif;
  color: #8b949a;
  letter-spacing: 0.05em;
  -webkit-transition: background 0.5s ease;
  transition: background 0.5s ease;
  display: -webkit-box;
  display: -ms-flexbox;
  display: flex;
  -webkit-box-pack: space-evenly;
      -ms-flex-pack: space-evenly;
          justify-content: space-evenly;
  min-width: 600px;
}

.color-picker-panel {
  background: #1f232a;
  width: 310px;
  border-radius: 8px;
  border: 2px solid #364347;
  -webkit-box-shadow: 0 4px 12px rgba(0, 0, 0, 0.4);
          box-shadow: 0 4px 12px rgba(0, 0, 0, 0.4);
  height: -webkit-fit-content;
  height: -moz-fit-content;
  height: fit-content;
}

.panel-row {
  position: relative;
  margin: 0 10px 10px;
}

.panel-row:first-child {
  margin-top: 10px;
  margin-bottom: 6px;
}

.panel-row:after {
  content: "";
  display: table;
  clear: both;
}

.panel-header {
  background: #15191c;
  padding: 8px;
  margin: 0 -10px 10px;
  text-align: center;
}

.swatch {
  display: inline-block;
  width: 32px;
  height: 32px;
  background: #ccc;
  border-radius: 4px;
  margin-left: 4px;
  margin-bottom: 4px;
  -webkit-box-sizing: border-box;
          box-sizing: border-box;
  border: 2px solid #364347;
  cursor: pointer;
}

.default-swatches {
  width: 212px;
}

.default-swatches .swatch:nth-child(6n + 1) {
  margin-left: 0;
}

.color-cursor {
  border-radius: 100%;
  background: #ccc;
  -webkit-box-sizing: border-box;
          box-sizing: border-box;
  position: absolute;
  pointer-events: none;
  z-index: 2;
  border: 2px solid #fff;
  -webkit-transition: all 0.2s ease;
  transition: all 0.2s ease;
}

.color-cursor.dragging {
  -webkit-transition: none;
  transition: none;
}

.color-cursor#spectrum-cursor {
  width: 30px;
  height: 30px;
  margin-left: -15px;
  margin-top: -15px;
  top: 0;
  left: 0;
}

.color-cursor#hue-cursor {
  top: 0;
  left: 50%;
  height: 20px;
  width: 20px;
  margin-top: -10px;
  margin-left: -10px;
  pointer-events: none;
}

.spectrum-map {
  position: relative;
  width: 212px;
  height: 200px;
  overflow: hidden;
}

#spectrum-canvas {
  position: absolute;
  width: 100%;
  height: 100%;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: #ccc;
}

.hue-map {
  position: absolute;
  top: 5px;
  bottom: 5px;
  right: 29px;
  width: 10px;
}

#hue-canvas {
  border-radius: 8px;
  position: absolute;
  top: 0;
  right: 0;
  bottom: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: #ccc;
}

.button {
  background: #2a3137;
  border: 0;
  border-radius: 4px;
  color: #8b949a;
  font-size: 1rem;
  -webkit-box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
          box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
  outline: none;
  cursor: pointer;
  padding: 4px;
}

.button:active {
  background: #262c31;
}

.button.eyedropper {
  position: absolute;
  right: 0;
  top: 0;
  width: 68px;
  height: 68px;
  display: block;
}

.button.add-swatch {
  display: block;
  padding: 6px;
  width: 200px;
  margin: 10px auto 0;
}

.button.mode-toggle {
  position: absolute;
  top: 0;
  right: 0;
  width: 68px;
  height: 40px;
}

.value-fields {
  display: none;
  -webkit-box-align: center;
      -ms-flex-align: center;
          align-items: center;
}

.value-fields.active {
  display: -webkit-box;
  display: -ms-flexbox;
  display: flex;
}

.value-fields .field-label {
  margin-right: 6px;
}

.value-fields .field-input {
  background: #15191c;
  border: 1px solid #364347;
  -webkit-box-sizing: border-box;
          box-sizing: border-box;
  border-radius: 2px;
  line-height: 38px;
  padding: 0 4px;
  text-align: center;
  color: #8b949a;
  font-size: 1rem;
  display: block;
}

.rgb-fields .field-group {
  display: -webkit-box;
  display: -ms-flexbox;
  display: flex;
  -webkit-box-align: center;
      -ms-flex-align: center;
          align-items: center;
}

.rgb-fields .field-input {
  width: 42px;
  margin-right: 10px;
}

.hex-field .field-input {
  width: 170px;
}

.color-indicator {
  display: inline-block;
  vertical-align: middle;
  margin-right: 10px;
  width: 20px;
  height: 20px;
  border-radius: 4px;
  background: #ccc;
}

input::-webkit-outer-spin-button,
input::-webkit-inner-spin-button {
  /* display: none; <- Crashes Chrome on hover */
  -webkit-appearance: none;
  margin: 0;
  /* <-- Apparently some margin are still there even though it's hidden */
}

.action-btn {
  width: 49%;
  font-size: xx-large;
}

.panel-controls {
  display: -webkit-box;
  display: -ms-flexbox;
  display: flex;
  -webkit-box-pack: justify;
      -ms-flex-pack: justify;
          justify-content: space-between;
}

ul {
  list-style-type: none;
}

ul li a {
  display: block;
  margin: 3px;
  padding: 10px;
  border: 2px solid #404040;
  border-radius: 5px;
  color: #909090;
  text-decoration: none;
}

ul#modes li a {
  min-width: 220px;
}

ul.control li a {
  min-width: 60px;
  min-height: 24px;
}

ul.control {
  display: -webkit-box;
  display: -ms-flexbox;
  display: flex;
  -webkit-box-orient: horizontal;
  -webkit-box-direction: normal;
      -ms-flex-direction: row;
          flex-direction: row;
  -webkit-box-pack: end;
      -ms-flex-pack: end;
          justify-content: flex-end;
  -webkit-box-align: center;
      -ms-flex-align: center;
          align-items: center;
  padding: 0px;
}

ul li a.active {
  border: 2px solid #909090;
}

.effects-header {
  display: -webkit-box;
  display: -ms-flexbox;
  display: flex;
  -ms-flex-pack: distribute;
      justify-content: space-around;
}
/*# sourceMappingURL=colorPicker.css.map */
)=====";
