#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include "sensor_board.h"
#include "web_mqtt.h"
#include "memory_op.h"
#include <ArduinoJson.h>

const char mqtt_err[10][48]{
  "MQTT_CONNECTION_TIMEOUT",
  "MQTT_CONNECTION_LOST",
  "MQTT_CONNECT_FAILED",
  "MQTT_DISCONNECTED",
  "MQTT_CONNECTED",
  "MQTT_CONNECT_BAD_PROTOCOL",
  "MQTT_CONNECT_BAD_CLIENT_ID",
  "MQTT_CONNECT_UNAVAILABLE",
  "MQTT_CONNECT_BAD_CREDENTIALS",
  "MQTT_CONNECT_UNAUTHORIZED"
};

// Here are the possible return values of client.state() and their meanings:
// -4 (MQTT_CONNECTION_TIMEOUT):
// The server didn't respond within the specified time.
// -3 (MQTT_CONNECTION_LOST):
// The connection to the server was lost.
// -2 (MQTT_CONNECT_FAILED):
// The network connection failed.
// -1 (MQTT_DISCONNECTED):
// The client is disconnected.
// 0 (MQTT_CONNECTED):
// The client is connected to the broker.
// 1 (MQTT_CONNECT_BAD_PROTOCOL):
// The server doesn't support the requested version of the MQTT protocol.
// 2 (MQTT_CONNECT_BAD_CLIENT_ID):
// The server rejected the client identifier.
// 3 (MQTT_CONNECT_UNAVAILABLE):
// The server is unavailable.
// 4 (MQTT_CONNECT_BAD_CREDENTIALS):
// The username/password provided by the client were rejected by the server.
// 5 (MQTT_CONNECT_UNAUTHORIZED):
// The client was not authorized to connect.

int loop_file=0;

const char web_mqtt_setting[] PROGMEM = R"rawliteral(
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
<body class="w3-light-grey">
    <div class="w3-bar w3-top w3-black w3-large" style="z-index:4">
        <button class="w3-bar-item w3-button w3-hide-large w3-hover-none w3-hover-text-light-grey" onclick="w3_open();"><i class="fa fa-bars"></i>  Menu</button>
       <span class="w3-bar-item w3-right"><br></span>
    </div>
    <nav class="w3-sidebar w3-collapse w3-white w3-animate-left" style="z-index:3;width:300px;" id="mySidebar"><br>
        <div class="w3-container w3-row">
          <div class="w3-col s4">
            <img src="/assets/Fiot.png" class="w3-circle w3-margin-right" style="width:80px">
          </div>

        </div>
        <hr>

        <div class="w3-bar-block">
          <a href="#" class="w3-bar-item w3-button w3-padding-16 w3-hide-large w3-dark-grey w3-hover-black" onclick="w3_close()" title="close menu"><i class="fa fa-remove fa-fw"></i>  Close Menu</a>
          <a href="/index.html" class="w3-bar-item w3-button w3-padding "><i class="fa fa-users fa-fw"></i>  Overview</a>
          <!-- <a href="#" class="w3-bar-item w3-button w3-padding"><i class="fa fa-users fa-fw"></i>  Traffic</a> -->
          <a href="/mqtt_setting.html" class="w3-bar-item w3-button w3-padding w3-blue"><i class="fa fa-connectdevelop " aria-hidden="true"></i>  Mqtt setting</a>
          <a href="/network_setting.html" class="w3-bar-item w3-button w3-padding "><i class="fa fa-cog fa-fw"></i>  Network Settings</a><br><br>
        </div>
      </nav>

      <!-- Overlay effect when opening sidebar on small screens -->
    <div class="w3-overlay w3-hide-large w3-animate-opacity" onclick="w3_close()" style="cursor:pointer" title="close side menu" id="myOverlay"></div>
    <!-- !PAGE CONTENT! -->
    <div class="w3-main " style="margin-left:300px;margin-top:43px;">
      <div class="tab">
            <button class="tablinks" id="tab1Button" data-tab-content-id="Tab1" onclick="openTab(event, 'Tab1')">Mqtt Config (1)</button>
            <button class="tablinks" id="tab2Button" data-tab-content-id="Tab2" onclick="openTab(event, 'Tab2')">Root CA1 (2)</button>
            <button class="tablinks" id="tab3Button" data-tab-content-id="Tab3" onclick="openTab(event, 'Tab3')">Device Certificate (3)</button>
            <button class="tablinks" id="tab4Button" data-tab-content-id="Tab4" onclick="openTab(event, 'Tab4')">Device Private key (4)</button>
      </div>
      
      <div id="Tab1" class="tabcontent">
        <fieldset>
        <form action="/mqtt_config" method="post">
            <div class="w3-twothird ">
                <h5 class="table-with-margin  w3-blue" >MQTT Service</h5>
                <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
                    <tr>
                      <td style="width:20%">Protocol</td>
                        <td>
                          <select id="mqtt_ssl" name="mqtt_ssl" onclick="updatemqtt_ssl();">
                              <option value="1">mqtts/tls</option>
                              <option value="0">mqll/tcp</option>
                          </select>
                        </td>
                      </tr>
                    <tr>
                        <td style="width:20%">Host</td>
                        <td>
                            <input type="text" style="width:80%" id="h_name" name="h_name" value="local" maxlength="64" placeholder="Host max 64 char">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">Port</td>
                        <td>
                            <input type="number"  id="port_mqtt" name="port_mqtt" value="8000" maxlength="5" placeholder="Port number max 5 char">
                        </td>
                    </tr>
                    
                    <tr>
                        <td style="width:20%">Device Name</td>
                        <td>
                            <input type="text" style="width:60%" id="client_name" name="client_name" maxlength="32" value="" placeholder="Host max 32 char">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">Client Id</td>
                        <td>
                            <input type="text" style="width:60%" id="id_mqtt" name="id_mqtt" value="" maxlength="64" placeholder="MQTT Client Id max 128 char">
                        </td>
                    </tr>
                    <!-- ===================== -->
                      <tr id="log_info1" class="hidden" >
                        <td style="width:20%">Login</td>
                        <td>
                            <input type="text" style="width:60%" id="user_mqtt" name="user_mqtt" value="" maxlength="64" placeholder="User name max 32 char">
                        </td>
                    </tr>
                    <tr id="log_info2" class="hidden" >
                        <td style="width:20%">Password</td>
                        <td>
                            <input type="password" style="width:60%" id="pass_mqtt" name="pass_mqtt" value="" maxlength="64" placeholder="Password max 64 char">
                        </td>
                    </tr>
                    <!-- ================ -->
                    <tr>
                        <td style="width:20%">Pub Topic</td>
                        <td>
                            <input type="text" style="width:60%" id="pub_mqtt" name="pub_mqtt" value="" maxlength="64" placeholder="MQTT Client Id max 128 char">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">Sub Topic</td>
                        <td>
                            <input type="text" style="width:60%" id="sub_mqtt" name="sub_mqtt" value="" maxlength="64" placeholder="MQTT Client Id max 128 char">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">Pub period (s)</td>
                        <td>
                            <input type="number" style="width:60%" id="pub_period" name="pub_period" value="10" max="99999" min="5">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%"></td>
                        <td>
                            <p><input type="submit" class="sm" value = "Apply config" /></p>
                        </td>
                    </tr>
                </table>
            </div>
        </form>
        </fieldset>  
      </div>

      <div id="Tab2" class="tabcontent hidden">
        <fieldset>            
        <form action="/upload_cert_ca" method="post" enctype="multipart/form-data">
          <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
            <tr>
              <td style="width:10%">AWS Root CA1</td>
              <td> <input type="file" name="file" required>  </td>
            </tr>
            <tr>
                <td style="width:10%"></td>
                <td>
                  <input type="submit" value="Upload">
                </td>
            </tr> 
            <tr>
                <td style="width:10%"></td>
                <td style="width:50%">Name</td>
            </tr> 
            <tr>
                <td style="width:10%">Current file</td>
                <td>
                  <input type="text" style="width:50%" id="cert_ca_name" name="cert_ca_name" value="" readonly>
                </td>
            </tr>
            <tr>
              <td style="width:10%">File Content:</td>
              <td style="width:60%">
                <textarea id="cert_ca_content" name="cert_ca_content" rows="12" style="width:80%" readonly></textarea>
              </td>
            </tr>
          </table>
        </form>
        </fieldset>
      </div>

      <div id="Tab3" class="tabcontent hidden">
        <fieldset>            
        <form action="/upload_cert_crt" method="post" enctype="multipart/form-data">
          <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
            <tr>
              <td style="width:10%">AWS Device Certificate</td>
              <td> <input type="file" name="file" required>  </td>
            </tr>
            <tr>
                <td style="width:10%"></td>
                <td>
                  <input type="submit" value="Upload">
                </td>
            </tr>
            <tr>
                <td style="width:10%"></td>
                <td style="width:50%">Name</td>
            </tr>  
            <tr>
                <td style="width:10%">Current file</td>
                <td>
                  <input type="text" style="width:50%" id="cert_crt_name" name="cert_crt_name" value="" readonly>
                </td>
            </tr>
            <tr>
              <td style="width:10%">File Content:</td>
              <td style="width:60%">
                <textarea id="cert_crt_content" name="cert_crt_content" rows="12" style="width:80%" readonly></textarea>
              </td>
            </tr>
          </table>
        </form>

        </fieldset>
      </div>

      <div id="Tab4" class="tabcontent hidden">
        <fieldset>            
        <form action="/upload_cert_pri" method="post" enctype="multipart/form-data">
          <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
            <tr>
              <td style="width:10%">AWS Device Private key</td>
              <td> <input type="file" name="file" required>  </td>
            </tr>
            <tr>
                <td style="width:10%"></td>
                <td>
                  <input type="submit" value="Upload">
                </td>
            </tr>
            <tr>
                <td style="width:10%"></td>
                <td style="width:50%">Name</td>
                <!-- <td style="width:40%">Status</td> -->
            </tr>  
            <tr>
                <td style="width:10%">Current file</td>
                <td>
                  <input type="text" style="width:50%" id="cert_pri_name" name="cert_pri_name" value="" readonly>
                </td>
            </tr>
            <tr>
              <td style="width:10%">File Content:</td>
              <td style="width:60%">
                <textarea id="cert_pri_content" name="cert_pri_content" rows="12" style="width:80%" readonly></textarea>
              </td>
            </tr>
          </table>
        </form>
        
        </fieldset>
      </div>
    </div>
</body>    

<script>
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

function openTab(evt, tabName) {
  var i, tabcontent, tablinks;

  // Hide all tab content
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }

  // Remove the "active" class from all tab buttons
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }

  // Show the specific tab content
  document.getElementById(tabName).style.display = "block";

  // Add the "active" class to the button that opened the tab
  evt.currentTarget.className += " active";
}

function updatemqtt_ssl(){

  var mqttSslValue = document.getElementById("mqtt_ssl").value;
  var tabsToHide = ["Tab2", "Tab3", "Tab4"];
  var tabButtonsToHide = ["tab2Button", "tab3Button", "tab4Button"];

  if (mqttSslValue == "0") {
    tabsToHide.forEach(function(tab) {
      var element = document.getElementById(tab);
      if (element) {
        element.classList.add("hidden");
      }
    });
    tabButtonsToHide.forEach(function(button) {
      var element = document.getElementById(button);
      if (element) {
        element.style.display = "none";
      }
    });
  } else {
    tabsToHide.forEach(function(tab) {
      var element = document.getElementById(tab);
      if (element) {
        element.classList.remove("hidden");
      }
    });
    tabButtonsToHide.forEach(function(button) {
      var element = document.getElementById(button);
      if (element) {
        element.style.display = "inline-block";
      }
    });
  }

  if(document.getElementById("mqtt_ssl").value==0)
  { // mqtt/tcp
  document.getElementById("user_mqtt").disabled = false;
  document.getElementById("pass_mqtt").disabled = false;
  
  document.getElementById("user_mqtt").classList.remove("hidden");
  document.getElementById("pass_mqtt").classList.remove("hidden");
  document.getElementById("log_info1").classList.remove("hidden");
  document.getElementById("log_info2").classList.remove("hidden");
  }
  else
  { // mqtts/tls
  document.getElementById("user_mqtt").disabled = true;
  document.getElementById("pass_mqtt").disabled = true;

  document.getElementById("user_mqtt").classList.add("hidden");
  document.getElementById("pass_mqtt").classList.add("hidden");
  document.getElementById("log_info1").classList.add("hidden");
  document.getElementById("log_info2").classList.add("hidden");
  }
}

function editCertCA() {
  document.getElementById('cert_ca_content').readOnly = false;
  document.querySelector('#Tab2 button:nth-of-type(1)').style.display = 'none';
  document.querySelector('#Tab2 button:nth-of-type(2)').style.display = 'inline-block';
}

function saveCertCA() {
  document.getElementById('cert_ca_content').readOnly = true;
  document.querySelector('#Tab2 button:nth-of-type(1)').style.display = 'inline-block';
  document.querySelector('#Tab2 button:nth-of-type(2)').style.display = 'none';
}

function editCert_cert_crt() {
  document.getElementById('cert_crt_content').readOnly = false;
  document.querySelector('#Tab3 button:nth-of-type(1)').style.display = 'none';
  document.querySelector('#Tab3 button:nth-of-type(2)').style.display = 'inline-block';
}

function saveCert_cert_crt() {
  document.getElementById('cert_crt_content').readOnly = true;
  document.querySelector('#Tab3 button:nth-of-type(1)').style.display = 'inline-block';
  document.querySelector('#Tab3 button:nth-of-type(2)').style.display = 'none';
}

function editCert_cert_pri() {
  document.getElementById('cert_pri_content').readOnly = false;
  document.querySelector('#Tab4 button:nth-of-type(1)').style.display = 'none';
  document.querySelector('#Tab4 button:nth-of-type(2)').style.display = 'inline-block';
}

function saveCert_cert_pri() {
  document.getElementById('cert_pri_content').readOnly = true;
  document.querySelector('#Tab4 button:nth-of-type(1)').style.display = 'inline-block';
  document.querySelector('#Tab4 button:nth-of-type(2)').style.display = 'none';
}

function loadCertCAContent() {
  var dummyContent = "-----BEGIN CERTIFICATE-----\nMIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\nADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\nb24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n-----END CERTIFICATE-----";
  document.getElementById('cert_ca_content').value = dummyContent;
}

function update_load_data_set() {
      // ใช้ AJAX เพื่อดึงข้อมูลจาก '/getADC'
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
          // เมื่อข้อมูลถูกดึงมา, อัปเดตค่าบน HTML
          var json_data_set = JSON.parse(xhr.responseText);
          document.getElementById('h_name').value = json_data_set.h_name;
          document.getElementById('port_mqtt').value = json_data_set.port_mqtt;
          document.getElementById('client_name').value = json_data_set.client_name;
          document.getElementById('id_mqtt').value = json_data_set.id_mqtt;
          document.getElementById('user_mqtt').value = json_data_set.user_mqtt;
          document.getElementById('pass_mqtt').value = json_data_set.pass_mqtt;
          document.getElementById('pub_mqtt').value = json_data_set.pub_mqtt;
          document.getElementById('sub_mqtt').value = json_data_set.sub_mqtt;
          document.getElementById('pub_period').value = json_data_set.pub_period;

          document.getElementById('mqtt_ssl').value = json_data_set.mqtt_ssl;

          document.getElementById('cert_ca_name').value = json_data_set.cert_ca_name;
          document.getElementById('cert_crt_name').value = json_data_set.cert_crt_name;
          document.getElementById('cert_pri_name').value = json_data_set.cert_pri_name;

        }
      };
      xhr.open("GET", "/up_mqtt_set", true);
      xhr.send();
    }

function update_load_ca1_file() {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
          document.getElementById('cert_ca_content').value = xhr.responseText;
        }
      };
      xhr.open("GET", "/up_mqtt_ca1", true);
      xhr.send();
    }
function update_load_crt_file() {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
          document.getElementById('cert_crt_content').value = xhr.responseText;
        }
      };
      xhr.open("GET", "/up_mqtt_crt", true);
      xhr.send();
    }
function update_load_pri_file() {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
          document.getElementById('cert_pri_content').value = xhr.responseText;
        }
      };
      xhr.open("GET", "/up_mqtt_pri", true);
      xhr.send();
    }        

function saveCurrentTab(tabName) {
    localStorage.setItem('lastActiveTab', tabName);
}

function openTab(evt, tabName) {
  var i, tabcontent, tablinks;

  // Hide all tab content
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }

  // Remove the "active" class from all tab buttons
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }

  // Show the specific tab content
  document.getElementById(tabName).style.display = "block";

  // Add the "active" class to the button that opened the tab
  evt.currentTarget.className += " active";
  saveCurrentTab(tabName);
}

function loadLastActiveTab() {
    var lastActiveTab = localStorage.getItem('lastActiveTab');
    if (lastActiveTab) {
        var tablinks = document.getElementsByClassName('tablinks');
        for (var i = 0; i < tablinks.length; i++) {
            if (tablinks[i].getAttribute('data-tab-content-id') === lastActiveTab) {
                        tablinks[i].className += ' active';
                        var tabContent = document.getElementById(lastActiveTab);
                        tabContent.style.display = 'block';
                        break;
                    }
        }
    } else {
        // Default to the first tab if no tab is saved in local storage
        document.getElementsByClassName('tablinks')[0].className += ' active';
        document.getElementsByClassName('tabcontent')[0].style.display = 'block';
    }
}

window.onload = function () 
    {
    update_load_data_set(); 
    loadLastActiveTab();
    updatemqtt_ssl();
    update_load_ca1_file();
    update_load_crt_file();
    update_load_pri_file();
    }
  

</script>
</html>
)rawliteral";



void init_apt_web_mqtt_setting()
{
    server.on("/mqtt_config", HTTP_POST, [](AsyncWebServerRequest *request){
    // Loop through each input with name="number" and print its value
    Serial.println("POST web_mqtt_setting");
    String inputName ="h_name";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("h_name " + inputName + ": " + inputValue);
        inputValue.toCharArray(st01.mqtt.host_name,inputValue.length()+1);
        // line_bot.move_speed=inputValue.toInt();
      }

    inputName ="port_mqtt";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("port_mqtt " + inputName + ": " + inputValue);
        // line_bot.move_time = inputValue.toInt();
        st01.mqtt.port = inputValue.toInt();
      }

    inputName ="client_name";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("client_name " + inputName + ": " + inputValue);
        // line_bot.kp = inputValue.toInt();
        inputValue.toCharArray(st01.mqtt.device_name,inputValue.length()+1);
      }
    
    inputName ="id_mqtt";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("id_mqtt " + inputName + ": " + inputValue);
        // line_bot.ki = inputValue.toInt();
        inputValue.toCharArray(st01.mqtt.Client_id,inputValue.length()+1);
      }

    inputName ="user_mqtt";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("user_mqtt " + inputName + ": " + inputValue);
        // line_bot.kd = inputValue.toInt();
        inputValue.toCharArray(st01.mqtt.user,inputValue.length()+1);
      }
    inputName ="pass_mqtt";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("pass_mqtt " + inputName + ": " + inputValue);
        // line_bot.kd = inputValue.toInt();
        inputValue.toCharArray(st01.mqtt.pass,inputValue.length()+1);
      }
    inputName ="pub_mqtt";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("pub_mqtt" + inputName + ": " + inputValue);
        // line_bot.kd = inputValue.toInt();
        inputValue.toCharArray(st01.mqtt.pub,inputValue.length()+1);
      }
    inputName ="sub_mqtt";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("sub_mqtt " + inputName + ": " + inputValue);
        // line_bot.kd = inputValue.toInt();
        inputValue.toCharArray(st01.mqtt.sub,inputValue.length()+1);
      }

    inputName ="pub_period";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("pub_period " + inputName + ": " + inputValue);
        // line_bot.move_time = inputValue.toInt();
        st01.mqtt.pub_period = inputValue.toInt();
      }

    inputName ="mqtt_ssl";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        Serial.println("mqtt_ssl " + inputName + ": " + inputValue);
        // line_bot.move_time = inputValue.toInt();
        st01.mqtt.mqtt_ssl= inputValue.toInt();
      }
    WriteConfig();
    delay(10);
    request->send(200, "text/html",web_mqtt_setting);
  });
}

void api_get_web_mqtt_file_ca()
{
    server.on("/up_mqtt_ca1", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json_data_set;
    json_data_set =String(st01.mqtt.AWS_CERT_CA1);
    Serial.printf("\r\nCA = %s ",json_data_set.c_str());
    request->send(200, "text/plain", json_data_set);
    });
}

void api_get_web_mqtt_file_crt()
{
    server.on("/up_mqtt_crt", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json_data_set;
    json_data_set =String(st01.mqtt.AWS_CERT_CRT1);
    Serial.printf("\r\nCRT = %s ",json_data_set.c_str());
    request->send(200, "text/plain", json_data_set);
    });
}

void api_get_web_mqtt_file_private()
{
    server.on("/up_mqtt_pri", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json_data_set;
    json_data_set =String(st01.mqtt.AWS_CERT_PRIVATE1);
    Serial.printf("\r\nPrivate = %s ",json_data_set.c_str());
    request->send(200, "text/plain", json_data_set);
    });
}

void api_get_web_mqtt_setting()
{
    server.on("/up_mqtt_set", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json_data_set = "{";
    json_data_set += "\"h_name\":\"" + String(st01.mqtt.host_name)+"\"" + ",";
    json_data_set += "\"port_mqtt\":" + String(st01.mqtt.port)+ ",";
    json_data_set += "\"client_name\":\"" + String(st01.mqtt.device_name)+"\"" + ",";
    json_data_set += "\"id_mqtt\":\"" + String(st01.mqtt.Client_id)+"\"" + ",";
    json_data_set += "\"user_mqtt\":\"" + String(st01.mqtt.user)+"\"" + ",";
    json_data_set += "\"pass_mqtt\":\"" + String(st01.mqtt.pass)+"\"" + ",";
    json_data_set += "\"pub_mqtt\":\"" + String(st01.mqtt.pub)+"\""+ ",";
    json_data_set += "\"sub_mqtt\":\"" + String(st01.mqtt.sub)+"\""+ ",";
    json_data_set += "\"mqtt_ssl\":\"" + String(st01.mqtt.mqtt_ssl)+"\""+ ",";
    // json_data_set += "\"pub_period\":" + String(st01.mqtt.pub_period)+",";
    // json_data_set += "\"cert_ca_text\":\"" + String(st01.mqtt.AWS_CERT_CA1)+"\""+",";
    // json_data_set += "\"cert_crt_text\":\"" + String(st01.mqtt.AWS_CERT_CRT1)+"\""+",";
    // json_data_set += "\"cert_pri_text\":\"" + String(st01.mqtt.AWS_CERT_PRIVATE1)+"\"";
    //--------------- Cert file CA----------------//
    json_data_set += "\"cert_ca_name\":\"" + String(st01.mqtt.cert_ca_fs_name)+"\""+ ",";
    if(st01.mqtt.cert_ca_ready)
      json_data_set += "\"cert_ca_status\":\""+String("OK")+"\""+ ",";
    else
      json_data_set += "\"cert_ca_status\":\""+String("Not OK")+"\""+ ",";
      // json_data_set += "\"cert_ca_content\":\"" + String(st01.mqtt.AWS_CERT_CA1)+"\""+ ",";
    //--------------- Cert file CRT----------------//
    json_data_set += "\"cert_crt_name\":\"" + String(st01.mqtt.cert_crt_fs_name)+"\""+ ",";
    if(st01.mqtt.cert_crt_ready)
      json_data_set += "\"cert_crt_status\":\""+String("OK")+"\""+ ",";
    else
      json_data_set += "\"cert_crt_status\":\""+String("Not OK")+"\""+ ",";
    // json_data_set += "\"cert_crt_content\":\"" + String(st01.mqtt.AWS_CERT_CRT1)+"\""+ ",";
    //--------------- Cert file PRI----------------//
    json_data_set += "\"cert_pri_name\":\"" + String(st01.mqtt.cert_pri_fs_name)+"\""+ ",";
    if(st01.mqtt.cert_pri_ready)
      json_data_set += "\"cert_pri_status\":\""+String("OK")+"\""+ ",";
    else
      json_data_set += "\"cert_pri_status\":\""+String("Not OK")+"\""+ ",";
    // json_data_set += "\"cert_pri_content\":\"" + String(st01.mqtt.AWS_CERT_PRIVATE1)+"\""+ ",";
  //-----------------------------------------------------------------------------------------
    json_data_set += "\"pub_period\":\"" + String(st01.mqtt.pub_period)+"\"";
    json_data_set += "}";
    Serial.printf("\r\nup_mqtt_set json=%s ",json_data_set.c_str());
    request->send(200, "application/json", json_data_set);

  //------------------------------------------------//
  });
}

void upload_cert_ca_file()
{
  server.on("/upload_cert_ca", HTTP_POST, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200);
        response->addHeader("Connection", "close");
        request->send(response);
      },
      [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index) {
        loop_file=len;
          memcpy(st01.mqtt.AWS_CERT_CA1,data,len);
        }
        if(index>0)
        {
          memcpy(st01.mqtt.AWS_CERT_CA1+index,data,len);
        }

        if (final) {
          deleteFile(st01.mqtt.cert_ca_fs_name);
          Serial.printf("File upload started: %s\n", filename.c_str());
          Serial.printf("File len: %d index %d\n", len,index);
          sprintf(st01.mqtt.cert_ca_fs_name,"/keys/%s",filename.c_str());
          if (!SPIFFS.exists("/keys")) {
            SPIFFS.mkdir("/keys");
          }
          File file = SPIFFS.open(st01.mqtt.cert_ca_fs_name, FILE_WRITE);
          if (!file) {
            Serial.println("Failed to open file for writing");
            return;
          }
          file.write((uint8_t* )st01.mqtt.AWS_CERT_CA1, index + len);
          file.close();
          Serial.printf("File upload completed: %s, %u bytes\n", st01.mqtt.cert_ca_fs_name, index + len);
          Write_cert_fileName();
          request->redirect("/mqtt_setting.html");
        }
      });
}

void upload_file_private_file()
{
  server.on("/upload_cert_pri", HTTP_POST, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200);
    delay(50);
    response->addHeader("Connection", "close");
    // request->send(response);
    request->redirect("/mqtt_setting.html"); // Redirect to /mqtt_setting.html
    loop_file=0;
}, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    
    if (!index) {
        loop_file=len;
        memcpy(st01.mqtt.AWS_CERT_PRIVATE1,data,len);
        }
    if(index>0)
      {
        memcpy(st01.mqtt.AWS_CERT_PRIVATE1+index,data,len);
      }

    if (final) {
        deleteFile(st01.mqtt.cert_pri_fs_name);
        Serial.printf("File upload started: %s\n", filename.c_str());
        sprintf(st01.mqtt.cert_pri_fs_name, "/keys/%s", filename.c_str());
        Serial.printf("File len: %u index %u\n", len, index);
        // Check if directory exists, create if not
        if (!SPIFFS.exists("/keys")) {
            SPIFFS.mkdir("/keys");
        }
        // Open the file in write mode
        file = SPIFFS.open(st01.mqtt.cert_pri_fs_name, FILE_WRITE);
        // file = SPIFFS.open(f_private_key,FILE_WRITE);
        if (!file) {
            Serial.println("Failed to open file for writing");
            return;
        }
        //  Serial.printf("%s size %d\n", String(st01.mqtt.AWS_CERT_PRIVATE1).c_str(), len);
        // Write data to the file
        file.write((uint8_t* )st01.mqtt.AWS_CERT_PRIVATE1, index + len);
        file.close(); // Close the file after writing
        // Printing the size of data received in this chunk
        Serial.printf("File upload completed: %s, %u bytes\n", st01.mqtt.cert_pri_fs_name, index + len);
        Write_cert_fileName();
        request->redirect("/mqtt_setting.html");
    }

});

}



void upload_cert_crt_file()
{
  server.on("/upload_cert_crt", HTTP_POST, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200);
        response->addHeader("Connection", "close");
        // request->send(response);
        // request->send(200, "text/html",web_mqtt_setting);
        request->redirect("/mqtt_setting.html"); // Redirect to /mqtt_setting.html
      },
      [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index) {
        loop_file=len;
          memcpy(st01.mqtt.AWS_CERT_CRT1,data,len);
        }
        if(index>0)
        {
          memcpy(st01.mqtt.AWS_CERT_CRT1+index,data,len);
        }
        if (final) {
          deleteFile(st01.mqtt.cert_crt_fs_name);
          Serial.printf("File upload started: %s\n", filename.c_str());
          sprintf(st01.mqtt.cert_crt_fs_name,"/keys/%s",filename.c_str());
          if (!SPIFFS.exists("/keys")) {
            SPIFFS.mkdir("/keys");
          }
          File file = SPIFFS.open(st01.mqtt.cert_crt_fs_name, FILE_WRITE);
          if (!file) {
            Serial.println("Failed to open file for writing");
            return;
          }
          file.write((uint8_t* )st01.mqtt.AWS_CERT_CRT1, index + len);
          file.close();
          Serial.printf("File upload completed: %s, %u bytes\n", st01.mqtt.cert_crt_fs_name, index + len);
          Write_cert_fileName();
          request->redirect("/mqtt_setting.html");
        }
      });
}