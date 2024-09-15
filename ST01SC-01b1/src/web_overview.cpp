#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include "sensor_board.h"
// #include "web_mqtt.h"
#include "web_overview.h"
#include "web_mqtt.h"
#include <ArduinoJson.h>

const char web_overview[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>FIoT</title>
<link rel="icon" href="/assets/Fiot2.png" type="image/x-icon">
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="/css/w3.css">
<link rel="stylesheet" href="css/fontawesome.css">
<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css">
<link rel="stylesheet" href="/css/iotadd.css">
<style>
html,body,h1,h2,h3,h4,h5 {font-family: "Raleway", sans-serif}
</style>
</head>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<body class="w3-light-grey">

<!-- Top container -->
<div class="w3-bar w3-top w3-black w3-large" style="z-index:4">
  <button class="w3-bar-item w3-button w3-hide-large w3-hover-none w3-hover-text-light-grey" onclick="w3_open();"><i class="fa fa-bars"></i>  Menu</button>
  <span class="w3-bar-item w3-right"><br></span>
</div>

<!-- Sidebar/menu -->
<nav class="w3-sidebar w3-collapse w3-white w3-animate-left" style="z-index:3;width:300px;" id="mySidebar"><br>
  <div class="w3-container w3-row">
    <div class="w3-col s4">
      <img src="/assets/Fiot.png" class="w3-circle w3-margin-right" style="width:80px">
    </div>
    <!-- <div class="w3-col s8 w3-bar">
      <span>Welcome, <strong>Mike</strong></span><br>
      <a href="#" class="w3-bar-item w3-button"><i class="fa fa-envelope"></i></a>
      <a href="#" class="w3-bar-item w3-button"><i class="fa fa-user"></i></a>
      <a href="#" class="w3-bar-item w3-button"><i class="fa fa-cog"></i></a>
    </div> -->
  </div>
  <hr>
  <div class="w3-bar-block">
    <a href="#" class="w3-bar-item w3-button w3-padding-16 w3-hide-large w3-dark-grey w3-hover-black" onclick="w3_close()" title="close menu"><i class="fa fa-remove fa-fw"></i>  Close Menu</a>
    <a href="/index.html" class="w3-bar-item w3-button w3-padding w3-blue"><i class="fa fa-users fa-fw"></i>  Overview</a>
    <a href="/mqtt_setting.html" class="w3-bar-item w3-button w3-padding"><i class="fa fa-connectdevelop" aria-hidden="true"></i>  Mqtt setting</a>
    <a href="/network_setting.html" class="w3-bar-item w3-button w3-padding"><i class="fa fa-cog fa-fw"></i>  Network Settings</a><br><br>
  </div>
</nav>


<!-- Overlay effect when opening sidebar on small screens -->
<div class="w3-overlay w3-hide-large w3-animate-opacity" onclick="w3_close()" style="cursor:pointer" title="close side menu" id="myOverlay"></div>

<!-- !PAGE CONTENT! -->
<div class="w3-main" style="margin-left:300px;margin-top:43px;">

  <!-- Header -->
  <header class="w3-container" style="padding-top:22px">
    <h5 class="w3-blue" ><b><i class="fa fa-dashboard"></i> FIoT Dashboard</b></h5>
  </header>

  <div class="w3-row-padding w3-margin-bottom ">

    <div class="w3-container">
      <div class="w3-row-padding w3-margin-top">
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-red w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-tachometer w3-xxxlarge" aria-hidden="true"></i></div>
            <div class="w3-right">
              <h2 id="node_t" name="node_t">99</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>x-axis rms virbration m/s</h4>
          </div>
        </div>
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-blue w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-tachometer w3-xxxlarge" aria-hidden="true"></i></div>
            <div class="w3-right">
              <h2 id="node_rh" name="node_rh">99</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>y-axis rms virbration m/s</h4>
          </div>
        </div>
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-teal w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-tachometer w3-xxxlarge"></i></div>
            <div class="w3-right">
              <h2 id="node_voc" name="node_voc">23</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>z-axis rms virbration m/s</h4>
          </div>
        </div>
      </div>

      <div class="w3-row-padding w3-margin-top">
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-red w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-tachometer w3-xxxlarge" aria-hidden="true"></i></div>
            <div class="w3-right">
              <h2 id="x_grms" name="x_grms">0</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>x-axis rms m&sup2/s</h4>
          </div>
        </div>
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-blue w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-tachometer w3-xxxlarge" aria-hidden="true"></i></div>
            <div class="w3-right">
              <h2 id="y_grms" name="y_grms">0</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>y-axis rms m&sup2/s</h4>
          </div>
        </div>
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-teal w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-tachometer w3-xxxlarge"></i></div>
            <div class="w3-right">
              <h2 id="z_grms" name="z_grms">0</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>z-axis rms m&sup2/s</h4>
          </div>
        </div>
      </div>
    
      <div class="w3-row-padding w3-margin-top">
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-teal w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-battery-4 w3-xxxlarge"></i></div>
            <div class="w3-right">
              <h2 id="node_battery" name="node_battery">23</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>Battery</h4>
          </div>
        </div>
        <div class="w3-third w3-margin-bottom">
          <div class="w3-container w3-green w3-padding-16 w3-card-4">
            <div class="w3-left"><i class="fa fa-wifi w3-xxxlarge"></i></div>
            <div class="w3-right">
              <h2 id="node_wifi" name="node_wifi">5</h2>
            </div>
            <div class="w3-clear"></div>
            <h4>Wi-Fi</h4>
          </div>
        </div>
        <!-- Add more cards here if needed -->
        
      </div>
      <div class="w3-row-padding w3-margin-top">
        <div class="w3-third">
           <div class="w3-container w3-light-grey w3-padding-small w3-card-4">
              <h4 class="w3-center w3-medium">3-Axis Rotation Plan<div class="w3-left"><i class="fa fa-compass w3-xxxlarge"></i></div></h4>
              <svg id="rotation-svg" width="150" height="150" viewBox="0 0 150 150">
              <!-- X-Axis Group -->
              <g id="x-group">
              <line id="x-axis" x1="75" y1="75" x2="125" y2="75" class="axis x"></line>
              <text x="130" y="80" class="rotation-step" id="x-angle">X: 0°</text>
              </g>
              <!-- Y-Axis Group -->
              <g id="y-group">
              <line id="y-axis" x1="75" y1="75" x2="75" y2="25" class="axis y"></line>
              <text x="80" y="20" class="rotation-step" id="y-angle">Y: 0°</text>
              </g>
              <!-- Z-Axis Group -->
              <g id="z-group">
              <line id="z-axis" x1="75" y1="75" x2="105" y2="105" class="axis z"></line>
              <text x="110" y="110" class="rotation-step" id="z-angle">Z: 0°</text>
              </g>
              </svg>
           </div>
        </div>
      </div>
    </div>


  </div>

  <div class="w3-container w3-padding-16">
  <h5>G-Values (X-Axis)</h5>
  <canvas id="gBarChart_X" width="400" height="100"></canvas>
  </div>
  <div class="w3-container w3-padding-16">
  <h5>G-Values (y-Axis)</h5>
  <canvas id="gBarChart_Y" width="400" height="100"></canvas>
  </div>
  <div class="w3-container w3-padding-16">
  <h5>G-Values (z-Axis)</h5>
  <canvas id="gBarChart_Z" width="400" height="100"></canvas>
  </div>
  

  <div class="w3-panel w3-padding-16">
    <!-- <div class="w3-row-padding w3-card-4" style="margin:0 -16px"> -->
    <div class="w3-row-padding w3-card-4">
      <!-- <div class="w3-third">
        <h5>Regions</h5>
        <img src="/w3images/region.jpg" style="width:100%" alt="Google Regional Map">
      </div> -->
      <div class="w3-twothird">
        <h5>Status</h5>
        <table class="w3-table w3-striped w3-white">
          <tr>
            <td><i class="fa fa-connectdevelop w3-text-blue w3-large"></i></td>
            <td>Mqtt host</td>
            <td><span id="host_status" name="host_status">Connected</span></td>
          </tr>
          <tr>
            <td><i class="fa fa-bell w3-text-red w3-large"></i></td>
            <td>Next Public (s)</td>
            <td><span id="pub_t" name="pub_t">15</span></td>
          </tr>
          <tr>
            <td><i class="fa fa-users w3-text-yellow w3-large"></i></td>
            <td>Public Count</td>
            <td><span id="pub_cnt" name="pub_cnt">15</span></td>
          </tr>
          <tr>
            <td><i class="fa fa-comment w3-text-red w3-large"></i></td>
            <td>Subscribe Cout</td>
            <td><span id="sub_cnt" name="sub_cnt">25</span></td>
          </tr>
        </table>
      </div>
      <div class="w3-container w3-padding-16">
        <h5>System Log</h5>
        <textarea id="logDisplay" name="logDisplay" rows="10" style="width:100%; max-height: 300px; overflow-y: auto;" readonly></textarea>
      </div>

    </div>
    
  </div>

  <footer class="w3-container w3-padding-16 w3-light-grey">
    <p>Powered by AKE  <a href="https://www.w3schools.com/w3css/default.asp" target="_blank">w3.css</a></p>
  </footer>
  <!-- End page content -->
</div>

<script>
// Dummy data: 256 random values between 0 and 100 for the 'g' on the x-axis
var gValues = Array.from({length: 256}, () => Math.floor(0));

var ctx = document.getElementById('gBarChart_X').getContext('2d');
  var gBarChart_X = new Chart(ctx, {
    type: 'bar',
    data: {
      labels: Array.from({length: 256}, (_, i) => ((i + 1)*1000/512).toFixed(2) ), // Labels from 1 to 256 for the x-axis
      datasets: [{
        label: 'm²/s',
        data: gValues, // The data to display
        backgroundColor: 'rgba(255, 50, 0, 0.5)',
        borderColor: 'rgba(255, 50, 0, 1)',
        borderWidth: 1
      }]
    },
    options: {
      scales: {
        y: {
          beginAtZero: true
        }
      }
    }
  });

var ctx = document.getElementById('gBarChart_Y').getContext('2d');
  var gBarChart_Y = new Chart(ctx, {
    type: 'bar',
    data: {
      labels: Array.from({length: 256}, (_, i) => ((i + 1)*1000/512).toFixed(2) ), // Labels from 1 to 256 for the x-axis
      datasets: [{
        label: 'm²/s',
        data: gValues, // The data to display
        backgroundColor: 'rgba(60, 179, 113, 0.5)',
        borderColor: 'rgba(60, 179, 113, 1)',
        borderWidth: 1
      }]
    },
    options: {
      scales: {
        y: {
          beginAtZero: true
        }
      }
    }
  });

var ctx = document.getElementById('gBarChart_Z').getContext('2d');
  var gBarChart_Z = new Chart(ctx, {
    type: 'bar',
    data: {
      labels: Array.from({length: 256}, (_, i) => ((i + 1)*1000/512).toFixed(2) ), // Labels from 1 to 256 for the x-axis
      datasets: [{
        label: 'm²/s',
        data: gValues, // The data to display
        backgroundColor: 'rgba(54, 162, 235, 0.5)',
        borderColor: 'rgba(54, 162, 235, 1)',
        borderWidth: 1
      }]
    },
    options: {
      scales: {
        y: {
          beginAtZero: true
        }
      }
    }
  });

// Get the Sidebar
var mySidebar = document.getElementById("mySidebar");
// Get the DIV with overlay effect
var overlayBg = document.getElementById("myOverlay");
// Toggle between showing and hiding the sidebar, and add overlay effect
function w3_open() {
  if (mySidebar.style.display === 'block') {
    mySidebar.style.display = 'none';
    overlayBg.style.display = "none";
  } else {
    mySidebar.style.display = 'block';
    overlayBg.style.display = "block";
  }
}

// Close the sidebar with the close button
function w3_close() {
  mySidebar.style.display = "none";
  overlayBg.style.display = "none";
}

function update_node_sensor() {
      // ใช้ AJAX เพื่อดึงข้อมูลจาก '/getADC'
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
          // เมื่อข้อมูลถูกดึงมา, อัปเดตค่าบน HTML
          var adcValues = JSON.parse(xhr.responseText);
          document.getElementById('node_t').innerText = adcValues.node_t;
          document.getElementById('node_rh').innerText = adcValues.node_rh;
          document.getElementById('node_voc').innerText = adcValues.node_voc;
          document.getElementById('x_grms').innerText = adcValues.x_grms;
          document.getElementById('y_grms').innerText = adcValues.y_grms;
          document.getElementById('z_grms').innerText = adcValues.z_grms;

          document.getElementById('node_battery').innerText = adcValues.node_battery+" V";
          document.getElementById('node_wifi').innerText = adcValues.node_wifi+" rssi";
          document.getElementById('host_status').innerText = adcValues.host_status;
          document.getElementById('pub_cnt').innerText = adcValues.pub_cnt;
          document.getElementById('sub_cnt').innerText = adcValues.sub_cnt;
          document.getElementById('pub_t').innerText = adcValues.pub_t;
          
          var logDisplay = document.getElementById('logDisplay');
          var logText = `Battery: ${adcValues.node_battery} V\n` +
                      `WiFi Signal: ${adcValues.node_wifi} rssi\n` +
                      `Host Status: ${adcValues.host_status}\n` +
                      `Published Count: ${adcValues.pub_cnt}\n` +
                      `Subscribed Count: ${adcValues.sub_cnt}\n` +
                      `Publish Time: ${adcValues.pub_t}\n`+
                      `mqtt_status: ${adcValues.err_code}\n\n`
                      ;

        // Append new log data to the existing content
        logDisplay.value += logText;

        // Scroll to the bottom of the textarea to show the latest entry
        logDisplay.scrollTop = logDisplay.scrollHeight;
        }
      };
      xhr.open("GET", "/get_node_sensor", true);
      xhr.send();
    }
  // Update ADC values every second
  
function fetchAngles() {
      fetch('/angles')
        .then(response => response.json())
        .then(data => {
          document.getElementById('x-angle').textContent = `X: ${data.angleX}°`;
          document.getElementById('y-angle').textContent = `Y: ${data.angleY}°`;
          document.getElementById('z-angle').textContent = `Z: ${data.angleZ}°`;

          const xGroup = document.getElementById('x-group');
          const yGroup = document.getElementById('y-group');
          const zGroup = document.getElementById('z-group');

          xGroup.setAttribute('transform', `rotate(${data.angleX}, 75, 75)`);
          yGroup.setAttribute('transform', `rotate(${data.angleY}, 75, 75)`);
          zGroup.setAttribute('transform', `rotate(${data.angleZ}, 75, 75)`);
        });
    }
function updateGBarChart(newGValues) {
    gBarChart_X.data.datasets[0].data = newGValues;
    gBarChart_x.update();

    gBarChart_Y.data.datasets[0].data = newGValues;
    gBarChart_Y.update();

    gBarChart_Z.data.datasets[0].data = newGValues;
    gBarChart_Z.update();
  }

function fetchGValues_x() {
      fetch('/getx_g_values') // Replace <ESP32_IP_ADDRESS> with your ESP32's IP
        .then(response => response.json())
        .then(data => {
          // Update chart data
          gBarChart_X.data.datasets[0].data = data;
          gBarChart_X.update();
        })
        .catch(error => console.error('Error fetching g-values x:', error));
    }
function fetchGValues_y() {
      fetch('/gety_g_values') // Replace <ESP32_IP_ADDRESS> with your ESP32's IP
        .then(response => response.json())
        .then(data => {
          // Update chart data
          gBarChart_Y.data.datasets[0].data = data;
          gBarChart_Y.update();
        })
        .catch(error => console.error('Error fetching g-values y:', error));
    }
function fetchGValues_z() {
      fetch('/getz_g_values') // Replace <ESP32_IP_ADDRESS> with your ESP32's IP
        .then(response => response.json())
        .then(data => {
          // Update chart data
          gBarChart_Z.data.datasets[0].data = data;
          gBarChart_Z.update();
        })
        .catch(error => console.error('Error fetching g-values z:', error));
    }        

  window.onload = function () 
    {
    update_node_sensor(); 
    //fetchGValues_x();
    //fetchGValues_y();
    //fetchGValues_z();
    setInterval(update_node_sensor, 3000);
    setInterval(fetchAngles, 3000); // Fetch angles every second

    setInterval(fetchGValues_x, 4000);
    setInterval(fetchGValues_y, 4100);
    setInterval(fetchGValues_z, 4200);
    }
  // Initial update
</script>

</body>
</html>
)rawliteral";

void api_get_overview()
{
  server.on("/get_node_sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    char ValueString[10]; 
    int16_t err_code=st01.err_log.mqtt_Fail_rc_err+4;
    String errlog;
    if(err_code>=10) err_code=9;
    errlog="\""+String("mqt_status=")+String(mqtt_err[err_code])+"  err_code="+String(st01.err_log.mqtt_Fail_rc_err)+"\"";
    String adcValues = "{";
    dtostrf(mems_data.x.RmsVelocity*1000, 5, 0, ValueString);
    adcValues += "\"node_t\":" + String(ValueString) + ",";
    dtostrf(mems_data.y.RmsVelocity*1000, 5, 0, ValueString);
    adcValues += "\"node_rh\":" + String(ValueString) + ",";
    dtostrf(mems_data.z.RmsVelocity*1000, 5, 0, ValueString);
    adcValues += "\"node_voc\":" + String(ValueString)+ ",";

    dtostrf(mems_data.x.RmsG, 5, 3, ValueString);
    adcValues += "\"x_grms\":" + String(ValueString) + ",";
    dtostrf(mems_data.y.RmsG, 5, 3, ValueString);
    adcValues += "\"y_grms\":" + String(ValueString) + ",";
    dtostrf(mems_data.z.RmsG, 5, 3, ValueString);
    adcValues += "\"z_grms\":" + String(ValueString)+ ",";

    adcValues += "\"node_battery\":" + String(st01.Vbat)+ ",";
    adcValues += "\"node_wifi\":" + String(st01.net.rssi)+ ",";
    if(st01.mqtt.host_status)
      adcValues += "\"host_status\":\"" + String("Connect")+"\""+ ",";
    else
      adcValues += "\"host_status\":\"" + String("Disconnect")+"\""+ ",";
    adcValues += "\"pub_cnt\":" + String(st01.mqtt.pub_cnt)+ ",";
    adcValues += "\"sub_cnt\":" + String(st01.mqtt.sub_cnt)+ ",";  
    adcValues += "\"pub_t\":" + String(st01.mqtt.pub_period-tick_Aws_pub)+ ","; 
    adcValues += "\"err_code\":" + errlog; 

    adcValues += "}";
    Serial.println(adcValues.c_str());
    request->send(200, "application/json", adcValues);
  });

}

void api_get_angle()
{
  server.on("/angles", HTTP_GET, [](AsyncWebServerRequest *request){
    String json;
    StaticJsonDocument<200> doc;
    doc["angleX"] = mems_data.roll;
    doc["angleY"] = mems_data.pitch;
    doc["angleZ"] = mems_data.yaw;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });
}

void api_get_fft_chart()
{
  server.on("/getx_g_values", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<2048> doc;
    
    // Populate the JSON array
    JsonArray array = doc.to<JsonArray>();
    // mems_data.x.gReal[0]=0;
    // mems_data.x.gReal[1]=0;
    for (int i = 0; i < 256; i++) {
      array.add(mems_data.x.gFFT[i]*0.001);
      // array.add(mems_data.x.gReal[i]*0.001);
    }
    // Send the JSON response
    String json;
    serializeJson(doc, json);
     request->send(200, "application/json", json);
  });
  server.on("/gety_g_values", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<2048> doc;
    
    // Populate the JSON array
    JsonArray array = doc.to<JsonArray>();
    // mems_data.y.gReal[0]=0;
    // mems_data.y.gReal[1]=0;
    for (int i = 0; i < 256; i++) {
      array.add(mems_data.y.gFFT[i]*0.001);
      // array.add(mems_data.y.gReal[i]*0.001);
    }
    // Send the JSON response
    String json;
    serializeJson(doc, json);
     request->send(200, "application/json", json);
  });
  server.on("/getz_g_values", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<2048> doc;
    
    // Populate the JSON array
    JsonArray array = doc.to<JsonArray>();
    // mems_data.z.gReal[0]=0;
    // mems_data.z.gReal[1]=0;
    for (int i = 0; i < 256; i++) {
      array.add(mems_data.z.gFFT[i]*0.001);
      // array.add(mems_data.z.gReal[i]*0.001);
    }
    // Send the JSON response
    String json;
    serializeJson(doc, json);
     request->send(200, "application/json", json);
  });
}