#include <ESP8266WebServer.h>
#define WEB_SERVER ESP8266WebServer
#define ESP_RESET ESP.reset()

#include <WS2812FX.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h> // Include the mDNS library

extern const char index_html[];
extern const char main_js[];
extern const char tinycolor_js[];
extern const char main_css[];

// #define WIFI_SSID "SparkMI"
// #define WIFI_PASSWORD "123456789"

//#define STATIC_IP                       // uncomment for static IP, set IP below
#ifdef STATIC_IP
  IPAddress ip(192,168,0,123);
  IPAddress gateway(192,168,0,1);
  IPAddress subnet(255,255,255,0);
#endif

// QUICKFIX...See https://github.com/esp8266/Arduino/issues/263
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define LED_PIN 2                       // 0 = GPIO0, 2=GPIO2
#define LED_COUNT 16

#define WIFI_TIMEOUT 60000              // checks WiFi every ...ms. Reset after this time, if WiFi cannot reconnect.
#define HTTP_PORT 80

unsigned long auto_last_change = 0;
unsigned long last_wifi_check_time = 0;
String modes = "";
uint8_t myModes[] = {}; // *** optionally create a custom list of effect/mode numbers
bool auto_cycle = true;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WEB_SERVER server(HTTP_PORT);

// For Wifi Connection Counter
unsigned long now = millis();

String oldSSID = WiFi.SSID();
String oldPsk = WiFi.psk();
bool isSetup = true;
bool isHttpServerSet = false;

/*
 * Connect to WiFi. If no connection is made within WIFI_TIMEOUT, ESP gets resettet.
 */
void wifi_setup() {  
  Serial.printf("WiFi.ssid: %s\n",WiFi.SSID());
  Serial.printf("WiFi.pass: %s\n",WiFi.psk());

  // *** Trying to connecting to WiFi shelf ***
  if( !isSetup && oldSSID != "" && oldPsk != ""){
    Serial.println("Self Connecting to WiFi...");
    Serial.printf("old_ssid: %s\n",oldSSID);
    Serial.printf("old_pass: %s\n",oldPsk);
    wl_status_t res = WiFi.begin(oldSSID, oldPsk,0,NULL,true);
    WiFi.waitForConnectResult();
    if(res != WL_CONNECTED){
      Serial.println("[ERROR] WiFi connection res:");
    }
    else{
      Serial.println("Connect to Wifi !!!");
    }
    Serial.println(res);
  }

  // *** Wifi Manager ***
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  wifiManager.setConfigPortalTimeout(30); // Time out 40 seconds
  
  //set custom ip for portal
  //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("SmartTubeLight-AP");
  //or use this for auto generated name ESP + ChipID
  // wifiManager.autoConnect();
  
  //if you get here you have connected to the WiFi
  if(WiFi.status() == WL_CONNECTED) 
    Serial.println("connected...yeey :)");
  else
    Serial.println("Unable to Connect to WiFi.");

  // *** mDNS Setup ***
  if (!MDNS.begin("NightLamp"))
  { // Start the mDNS responder for esp8266.local
      Serial.println("Error setting up MDNS responder!");
  }
  else{
      Serial.println("mDNS responder started");
      // Add service to MDNS-SD
      MDNS.addService("http", "tcp", 80);
  }
 
}

/*
 * Build <li> string for all modes.
 */
void modes_setup() {
  modes = "";
  uint8_t num_modes = sizeof(myModes) > 0 ? sizeof(myModes) : ws2812fx.getModeCount();
  for(uint8_t i=0; i < num_modes; i++) {
    uint8_t m = sizeof(myModes) > 0 ? myModes[i] : i;
    modes += "<li><a href='#'>";
    modes += ws2812fx.getModeName(m);
    modes += "</a></li>";
  }
}

/* #####################################################
#  Webserver Functions
##################################################### */

void srv_handle_not_found() {
  server.send(404, "text/plain", "File Not Found");
}

void srv_handle_index_html() {
  server.send_P(200,"text/html", index_html);
}

void srv_handle_main_js() {
  server.send_P(200,"application/javascript", main_js);
}

void srv_handle_tinycolor_js() {
  server.send_P(200,"application/javascript", tinycolor_js);
}

void srv_handle_main_css() {
  server.send_P(200,"text/css", main_css);
}

void srv_handle_modes() {
  server.send(200,"text/plain", modes);
}

void srv_handle_set() {
  for (uint8_t i=0; i < server.args(); i++){
    if(server.argName(i) == "c") {
      uint32_t tmp = (uint32_t) strtol(server.arg(i).c_str(), NULL, 10);
      if(tmp <= 0xFFFFFF) {
        ws2812fx.setColor(tmp);
      }
    }

    if(server.argName(i) == "m") {
      uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
      uint8_t new_mode = sizeof(myModes) > 0 ? myModes[tmp % sizeof(myModes)] : tmp % ws2812fx.getModeCount();
      ws2812fx.setMode(new_mode);
      auto_cycle = false;
      Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    }

    if(server.argName(i) == "b") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.setBrightness(ws2812fx.getBrightness() * 0.8);
      } else if(server.arg(i)[0] == ' ') {
        ws2812fx.setBrightness(min(max(ws2812fx.getBrightness(), 5) * 1.2, 255));
      } else { // set brightness directly
        uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
        ws2812fx.setBrightness(tmp);
      }
      Serial.print("brightness is "); Serial.println(ws2812fx.getBrightness());
    }

    if(server.argName(i) == "s") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.setSpeed(max(ws2812fx.getSpeed(), 5) * 1.2);
      } else if(server.arg(i)[0] == ' ') {
        ws2812fx.setSpeed(ws2812fx.getSpeed() * 0.8);
      } else {
        uint16_t tmp = (uint16_t) strtol(server.arg(i).c_str(), NULL, 10);
        ws2812fx.setSpeed(tmp);
      }
      Serial.print("speed is "); Serial.println(ws2812fx.getSpeed());
    }

    if(server.argName(i) == "a") {
      if(server.arg(i)[0] == '-') {
        auto_cycle = false;
      } else { // ?a=+
        auto_cycle = true;
        auto_last_change = 0;
      }
    }
  }
  server.send(200, "text/plain", "OK");
}

void CheckWifiConnection(){
  now = millis();
  if(now - last_wifi_check_time > WIFI_TIMEOUT) {
    Serial.print("Checking WiFi... ");
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi connection lost. Reconnecting...");     
      wifi_setup();
    } else {
      Serial.println("OK | ip :"+ WiFi.localIP().toString());
    }
    last_wifi_check_time = now;
  }
}

void setupHttpServer() {
  Serial.println("HTTP server setup");
  server.on("/", srv_handle_index_html);
  server.on("/main.js", srv_handle_main_js);
  server.on("/tinycolor.js", srv_handle_tinycolor_js);
  server.on("/main.css", srv_handle_main_css);
  server.on("/modes", srv_handle_modes);
  server.on("/set", srv_handle_set);
  server.onNotFound(srv_handle_not_found);
  server.begin();
  Serial.println("HTTP server started.");
  server.handleClient();
}

void setup(){
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\nStarting...");

  modes.reserve(5000);
  modes_setup();

  Serial.println("WS2812FX setup");
  ws2812fx.init();
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.setColor(0xFF5900);
  ws2812fx.setSpeed(1000);
  ws2812fx.setBrightness(128);
  ws2812fx.start();

  Serial.println("Wifi setup");
  wifi_setup();  

  Serial.println("ready!");
  isSetup = false;
}


void loop() {
  MDNS.update();
  ws2812fx.service(); 

  if(WiFi.status() == WL_CONNECTED && !isHttpServerSet){
    setupHttpServer();
    isHttpServerSet = true;
  }
  else if(WiFi.status() != WL_CONNECTED ) {
    isHttpServerSet = false;
  }
  server.handleClient();

  CheckWifiConnection();

  if(auto_cycle && (now - auto_last_change > 10000)) { // cycle effect mode every 10 seconds
    uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();
    if(sizeof(myModes) > 0) { // if custom list of modes exists
      for(uint8_t i=0; i < sizeof(myModes); i++) {
        if(myModes[i] == ws2812fx.getMode()) {
          next_mode = ((i + 1) < sizeof(myModes)) ? myModes[i + 1] : myModes[0];
          break;
        }
      }
    }
    ws2812fx.setMode(next_mode);
    Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    auto_last_change = now;
  }
}


