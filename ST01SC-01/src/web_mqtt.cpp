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
        <!-- <link rel="stylesheet" href="css/fonts-googleapis-raleway.css"> -->
        <!-- <link rel="stylesheet" href="css/bootstrap.min.css"> -->
        <link rel="stylesheet" href="/css/iotadd.css">
        <!-- <script src="https://kit.fontawesome.com/a076d05399.js" crossorigin="anonymous"></script> -->
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
        <!-- <div class="w3-container">
          <h5>Dashboard</h5>
        </div> -->
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
            <button class="tablinks" onclick="openTab(event, 'Tab1')">Mqtt config</button>
            <button class="tablinks" onclick="openTab(event, 'Tab2')">CA1.pem</button>
            <button class="tablinks" onclick="openTab(event, 'Tab3')">Certificate.pem</button>
            <button class="tablinks" onclick="openTab(event, 'Tab4')">Private.pem.key</button>
      </div>
      <div id="Tab1" class="tabcontent">
        <fieldset>
        <form action="/mqtt_config" method="post">
            <div class="w3-twothird ">
                <h5 class="table-with-margin  w3-blue" >MQTT Service</h5>
                <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
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
                        <td style="width:20%">Protocol</td>
                        <td>
                              <select id="mqtt_ssl" name="mqtt_ssl" onclick="updatemqtt_ssl();">
                                  <option value="1">mqtts/tls</option>
                                  <option value="0">mqll/tcp</option>
                              </select>
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
                    <tr>
                        <td style="width:20%">Login</td>
                        <td>
                            <input type="text" style="width:60%" id="user_mqtt" name="user_mqtt" value="" maxlength="64" placeholder="User name max 32 char">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">Password</td>
                        <td>
                            <input type="password" style="width:60%" id="pass_mqtt" name="pass_mqtt" value="" maxlength="64" placeholder="Password max 64 char">
                        </td>
                    </tr>
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

      <div id="Tab2" class="tabcontent">
        <fieldset>            
        <form action="/upload_cert_ca" method="post" enctype="multipart/form-data">
          <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
            <tr>
              <td style="width:20%">CERT_CA</td>
              <td> <input type="file" name="file" required>  </td>
            </tr>
            <tr>
                <td style="width:20%"></td>
                <td>
                  <input type="submit" value="Upload">
                </td>
            </tr> 
            <tr>
                <td style="width:20%"></td>
                <td style="width:50%">Name</td>
                <td style="width:40%">Status</td>
            </tr> 
            <tr>
                <td style="width:20%">Current file</td>
                <td>
                  <input type="text" style="width:50%" id="cert_ca_name" name="cert_ca_name" value="" readonly>
                </td>
                <td>
                <input type="text" style="width:40%" id="cert_ca_status" name="cert_ca_status" readonly>
                </td>
            </tr>
          </table>

        </form>
        </fieldset>
      </div>

      <div id="Tab3" class="tabcontent">
        <fieldset>            
        <form action="/upload_cert_crt" method="post" enctype="multipart/form-data">
          <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
            <tr>
              <td style="width:20%">CERT_CRT</td>
              <td> <input type="file" name="file" required>  </td>
            </tr>
            <tr>
                <td style="width:20%"></td>
                <td>
                  <input type="submit" value="Upload">
                </td>
            </tr>
            <tr>
                <td style="width:20%"></td>
                <td style="width:50%">Name</td>
                <td style="width:40%">Status</td>
            </tr>  
            <tr>
                <td style="width:20%">Current file</td>
                <td>
                  <input type="text" style="width:50%" id="cert_crt_name" name="cert_crt_name" value="" readonly>
                </td>
                <td>
                <input type="text" style="width:40%" id="cert_crt_status" name="cert_crt_status" readonly>
                </td>
            </tr>
          </table>
        </form>
        </fieldset>
      </div>

      <div id="Tab4" class="tabcontent">
        <fieldset>            
        <form action="/upload_cert_pri" method="post" enctype="multipart/form-data">
          <table class="w3-table w3-striped w3-white table-with-margin  w3-white " >
            <tr>
              <td style="width:20%">CERT_PRI</td>
              <td> <input type="file" name="file" required>  </td>
            </tr>
            <tr>
                <td style="width:20%"></td>
                <td>
                  <input type="submit" value="Upload">
                </td>
            </tr>
            <tr>
                <td style="width:20%"></td>
                <td style="width:50%">Name</td>
                <td style="width:40%">Status</td>
            </tr>  
            <tr>
                <td style="width:20%">Current file</td>
                <td>
                  <input type="text" style="width:50%" id="cert_pri_name" name="cert_pri_name" value="" readonly>
                </td>
                <td>
                <input type="text" style="width:40%" id="cert_pri_status" name="cert_pri_status" readonly>
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

document.getElementById("Tab1").style.display = "block";
function updatemqtt_ssl(){
  if(document.getElementById("mqtt_ssl").value==0)
  { // mqtt/tcp
  document.getElementById("user_mqtt").disabled = false;
  document.getElementById("pass_mqtt").disabled = false;
  }
  else
  { // mqtts/tls
  document.getElementById("user_mqtt").disabled = true;
  document.getElementById("pass_mqtt").disabled = true;
  }
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
          document.getElementById('cert_ca_status').value = json_data_set.cert_ca_status;
          document.getElementById('cert_crt_name').value = json_data_set.cert_crt_name;
          document.getElementById('cert_crt_status').value = json_data_set.cert_crt_status;
          document.getElementById('cert_pri_name').value = json_data_set.cert_pri_name;
          document.getElementById('cert_pri_status').value = json_data_set.cert_pri_status;
        }
      };
      xhr.open("GET", "/up_mqtt_set", true);
      xhr.send();
    }
    
window.onload = function () 
    {
    update_load_data_set(); 
    openTab(event, 'Tab1');
    updatemqtt_ssl();
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
    //--------------- Cert file CRT----------------//
    json_data_set += "\"cert_crt_name\":\"" + String(st01.mqtt.cert_crt_fs_name)+"\""+ ",";
    if(st01.mqtt.cert_crt_ready)
      json_data_set += "\"cert_crt_status\":\""+String("OK")+"\""+ ",";
    else
      json_data_set += "\"cert_crt_status\":\""+String("Not OK")+"\""+ ",";
    //--------------- Cert file PRI----------------//
    json_data_set += "\"cert_pri_name\":\"" + String(st01.mqtt.cert_pri_fs_name)+"\""+ ",";
    if(st01.mqtt.cert_pri_ready)
      json_data_set += "\"cert_pri_status\":\""+String("OK")+"\""+ ",";
    else
      json_data_set += "\"cert_pri_status\":\""+String("Not OK")+"\""+ ",";

    json_data_set += "\"pub_period\":\"" + String(st01.mqtt.pub_period)+"\"";
    json_data_set += "}";
    Serial.printf("\r\nup_mqtt_set json=%s ",json_data_set.c_str());
    request->send(200, "application/json", json_data_set);
  });
}

