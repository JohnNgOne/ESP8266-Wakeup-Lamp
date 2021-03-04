/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-web-server-slider-pwm/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <time.h>

// Replace with your network credentials
const char* ssid = "Kowabungah!";
const char* password = "schnitzel4hire1738";

const int output = 13;

String sliderValue = "0";

int inputHours = 10;
int inputMinutes = 0;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML>
<html>
   <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Wake Up Lamp</title>
      <style>
         html {
         font-family: Arial;
         display: inline-block;
         text-align: center;
         }
         h2 {
         font-size: 2.3rem;
         }
         p {
         font-size: 1.9rem;
         }
         body {
         width: 90%;
         margin: 0px auto;
         padding-bottom: 25px;
         }
         .slider {
         -webkit-appearance: none;
         margin: 14px;
         width: 100%;
         height: 25px;
         background: #FFD65C;
         outline: none;
         -webkit-transition: .2s;
         transition: opacity .2s;
         }
         .slider::-webkit-slider-thumb {
         -webkit-appearance: none;
         appearance: none;
         width: 80px;
         height: 80px;
         background: #003249;
         cursor: pointer;
         }
         .slider::-moz-range-thumb {
         width: 80px;
         height: 80px;
         background: #003249;
         cursor: pointer;
         }
         .inputNum {
            text-align:center;
            line-height:5em;
         }
      </style>
   </head>
   <body>
      <h2>Wake Up Lamp</h2>
      <p>
        <span id="textSliderValue">%SLIDERVALUE%</span>
      </p>
      <p>
        <input type="range" oninput="updateSliderPWM(this)" id="pwmSlider" min="0" max="31.98" value="%SLIDERVALUE%" step="0.01" class="slider">
      </p>
      <p>
        <input type="number" onsubmit="updateTime()" id="inputHour" min="0" max="23" placeholder="HOUR (24)" class="inputNum">
        <input type="number" onsubmit="updateTime()" id="inputMinute" min="0" max="60" placeholder="MINUTE" class="inputNum">
        <br>
        <input type="submit" value="Update" onclick="updateTime()">
      </p>
      <script>
         function updateSliderPWM() {
           var sliderValue = Math.round(Math.pow((document.getElementById("pwmSlider").value),2));
           document.getElementById("textSliderValue").innerHTML = sliderValue;
           var xhr = new XMLHttpRequest();
           xhr.open("GET", "/get?slider="+sliderValue, true);
           xhr.send();
         }
         function updateTime() {
           var hourValue = document.getElementById("inputHour").value;
           var minuteValue = document.getElementById("inputMinute").value;
           var xhr = new XMLHttpRequest();
           xhr.open("GET", "/get?hour="+hourValue, true);
           xhr.send();
           var xhr = new XMLHttpRequest();
           xhr.open("GET", "/get?minute="+minuteValue, true);
           xhr.send();
         }
      </script>
   </body>
</html>



)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if (var == "SLIDERVALUE"){
    return sliderValue;
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  analogWriteFreq(1000);
  analogWrite(output, sliderValue.toInt());

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  //time config
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "EST5EDT", 1);
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?slider=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam("slider")) {
      inputMessage = request->getParam("slider")->value();
      sliderValue = inputMessage;
      analogWrite(output, sliderValue.toInt());
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam("hour")) {
      inputMessage = request->getParam("hour")->value();
      inputHours = inputMessage.toInt();
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam("minute")) {
      inputMessage = request->getParam("minute")->value();
      inputMinutes = inputMessage.toInt();
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  
  // Start server
  server.begin();
}
  
void loop() {
  // TURN LED ON
  time_t now = time(nullptr);
  Serial.println(ctime(&now));
  Serial.println(inputMinutes);
  
  Serial.println(inputHours);
  struct tm* p_tm = localtime(&now);
  if((p_tm->tm_hour == inputHours) && (p_tm->tm_min == inputMinutes)) {
      analogWrite(output,((p_tm->tm_sec)*17+20));
  }
  
  delay(2000);
}
