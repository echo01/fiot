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
// #include "web_overview.h"
#include "web_netsetting.h"
#include "memory_op.h"

const char web_net_setting[] PROGMEM = R"rawliteral(
<!DOCTYPE html>

<html>
    <head>
        <title>FIoT</title>
        <link rel="icon" href="/assets/Fiot2.png" type="image/x-icon">
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="stylesheet" href="/css/w3.css">
        <!-- <link rel="stylesheet" href="css/font-awesome-min.css"> -->
        <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css">
        <!-- <link rel="stylesheet" href="css/fonts-googleapis-raleway.css"> -->
        <!-- <link rel="stylesheet" href="css/bootstrap.min.css"> -->
        <link rel="stylesheet" href="/css/iotadd.css">
        <!-- <script src="https://kit.fontawesome.com/a076d05399.js" crossorigin="anonymous"></script> -->
        <!-- <link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css"> -->
        <!-- <link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Raleway"> -->
        <!-- <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css"> -->
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
          <a href="/mqtt_setting.html" class="w3-bar-item w3-button w3-padding"><i class="fa fa-connectdevelop" aria-hidden="true"></i>  Mqtt setting</a>
          <a href="/network_setting.html" class="w3-bar-item w3-button w3-padding w3-blue"><i class="fa fa-cog fa-fw"></i>  Network Settings</a><br><br>
        </div>
      </nav>

      <!-- Overlay effect when opening sidebar on small screens -->
    <div class="w3-overlay w3-hide-large w3-animate-opacity" onclick="w3_close()" style="cursor:pointer" title="close side menu" id="myOverlay"></div>

    <!-- !PAGE CONTENT! -->
    <div class="w3-main" style="margin-left:300px;margin-top:43px;">
        <div class="w3-twothird w3-striped w3-white ">
            <div class="tab w3-twothird w3-striped w3-white">
                <button class="tablinks" onclick="openTab(event, 'Tab1')">SSID</button>
                <button class="tablinks" onclick="openTab(event, 'Tab2')">Netconfig</button>
                <button class="tablinks" onclick="openTab(event, 'Tab3')">Web admin</button>
            </div>
        </div>

        <div id="Tab1" class="tabcontent">
            <br>
            <br>
            <fieldset>
                <table class="w3-table w3-striped w3-white table-with-margin w3-card-4 w3-white">
                    <tr>
                        <td>
                        <h5 class="table-with-margin w3-card-4 w3-blue" style="margin-top: 10px;">Wi-Fi Scan</h5>
                        </td>
                    </tr>
                    <tr>
                        <td>
                        <button onclick="scanWiFi()" class="w3-button w3-blue">Scan Wi-Fi</button>
                        </td>
                    </tr>
                </table>
            </fieldse>
            <fieldset>
                <form action="/ssid_set" method="post">
                    <div class="w3-twothird">
                    <h5 class="table-with-margin  w3-orange" >STA SSID Config</h5>
                        <table class="w3-table w3-striped w3-white table-with-margin w3-card-4 w3-white">
                            <tr>
                                <td style="width:20%">STA WIFI</td>
                                <td><input type="checkbox" align="left" id="ssid_en" name="ssid_en" onclick="update_en_sta_mode();"></td>
                            </tr>
                            <tr>
                                <td style="width:20%">List</td>
                                <td>
                                <select id="wifiNetworks" onchange="updateSSIDField()" >
                                    <option value="Wifi1 a">Wifi1</option>
                                    <option value="Wifi2 b">Wifi2</option>
                                    <option value="Wifi3 c">Wifi3</option>
                                </select>
                                </td>
                            </tr>
                            <tr>
                                <td style="width:20%">SSID</td>
                                <td>
                                    <input type="text" id="ssid_name" name="ssid_name" value="" maxlength="36" placeholder="ssid">
                                </td>
                            </tr>
                            <tr>
                                <td style="width:20%">Password</td>
                                <td>
                                    <input type="password" id="ssid_pass" name="ssid_pass" value="" maxlength="36" placeholder="Password">
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
            <fieldset>
                 <form action="/ap_ssid_set" method="post">
                    <div class="w3-twothird">
                    <h5 class="table-with-margin  w3-orange" >AP SSID Network Config</h5>
                    <table class="w3-table w3-striped w3-white table-with-margin w3-card-4 w3-white">
                            <tr>
                                <td style="width:20%">AP WIFI</td>
                                <td><input type="checkbox" align="left" id="ap_ssid_en" name="ap_ssid_en" onclick="update_en_ap_mode();"></td>
                            </tr>
                            <tr>
                                <td style="width:20%">SSID</td>
                                <td>
                                    <input type="text" id="apssid_name" name="apssid_name" value="" maxlength="36" placeholder="ssid">
                                </td>
                            </tr>
                            <tr>
                                <td style="width:20%">Password</td>
                                <td>
                                    <input type="password" id="apssid_pass" name="apssid_pass" value="" maxlength="36" placeholder="Password">
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
        <br>
        <br>
        <fieldset>
        <form action="/net_config" method="post">
            <div class="w3-twothird">
                <h5 class="table-with-margin  w3-orange" >Device Network Config</h5>
                <table class="w3-table w3-striped w3-white table-with-margin  w3-white" >
                    <tr>
                        <td style="width:20%">Mac Address</td>
                        <td>
                            <input type="text" id="mac_0" name="mac_0" style="width:50%" maxlength="2" readonly disabled > 
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">DHCP Client</td>
                        <td><input type="checkbox" align="left" id="dhcp" name="dhcp" onclick="updatedhcp();"></td>
                    </tr>
                    <tr>
                        <!-- <td><i class="fa fa-user w3-text-blue w3-large"></i></td> -->
                        <td style="width:20%">Device IP</td>
                        <td>
                            <input type="number" id="ip_0" name="ip_0" style="width:20%" min="0" max="255">
                            <input type="number" id="ip_1"name="ip_1"  style="width:20%" min="0" max="255">
                            <input type="number" id="ip_2" name="ip_2" style="width:20%" min="0" max="255">
                            <input type="number" id="ip_3" name="ip_3" style="width:20%" min="0" max="255">
                        </td>
                    </tr>
                    <tr>
                        <!-- <td><i class="fa fa-user w3-text-blue w3-large"></i></td> -->
                        <td style="width:20%">Subnet mask</td>
                        <td>
                            <input type="number" id="nm_0" name="nm_0" style="width:20%" min="0" max="255">
                            <input type="number" id="nm_1"name="nm_1"  style="width:20%" min="0" max="255">
                            <input type="number" id="nm_2" name="nm_2" style="width:20%" min="0" max="255">
                            <input type="number" id="nm_3" name="nm_3" style="width:20%" min="0" max="255">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">Gateway</td>
                        <td>
                            <input type="number" id="gw_0" name="gw_0" style="width:20%" min="0" max="255">
                            <input type="number" id="gw_1"name="gw_1"  style="width:20%" min="0" max="255">
                            <input type="number" id="gw_2" name="gw_2" style="width:20%" min="0" max="255">
                            <input type="number" id="gw_3" name="gw_3" style="width:20%" min="0" max="255">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">dns</td>
                        <td>
                            <input type="number" id="dns1_0" name="dns1_0" style="width:20%" min="0" max="255">
                            <input type="number" id="dns1_1"name="dns1_1"  style="width:20%" min="0" max="255">
                            <input type="number" id="dns1_2" name="dns1_2" style="width:20%" min="0" max="255">
                            <input type="number" id="dns1_3" name="dns1_3" style="width:20%" min="0" max="255">
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

        <div id="Tab3" class="tabcontent">
        <br>
        <br>
        <fieldset>
        <form action="/admin_config" method="post">
            <div class="w3-twothird">
                <h5 class="table-with-margin w3-card-4 w3-yellow" style="margin-top: 10px;">Web admin config</h5>
                <table class="w3-table w3-striped w3-white table-with-margin w3-card-4 w3-white">
                    <tr>
                        <td style="width:20%">User</td>
                        <td>
                            <input type="text" id="web_user" name="web_user" value="" maxlength="8" placeholder="Username">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%">Password</td>
                        <td>
                            <input type="password" id="web_pass" name="web_pass" value="" maxlength="8" placeholder="Password">
                        </td>
                    </tr>
                    <tr>
                        <td style="width:20%"></td>
                        <td>
                            <p><input type="submit" class="sm" value = "Apply" /></p>
                        </td>
                    </tr>
                </table>
            </div>
        </form> 
        </fieldset>
        </div>
    </div>
</body>    

<script>
    function updatedhcp() {
    document.getElementById("ip_0").disabled = document.getElementById("dhcp").checked;
    document.getElementById("ip_1").disabled = document.getElementById("dhcp").checked;
    document.getElementById("ip_2").disabled = document.getElementById("dhcp").checked;
    document.getElementById("ip_3").disabled = document.getElementById("dhcp").checked;
  
    document.getElementById("nm_0").disabled = document.getElementById("dhcp").checked;
    document.getElementById("nm_1").disabled = document.getElementById("dhcp").checked;
    document.getElementById("nm_2").disabled = document.getElementById("dhcp").checked;
    document.getElementById("nm_3").disabled = document.getElementById("dhcp").checked;
  
    document.getElementById("gw_0").disabled = document.getElementById("dhcp").checked;
    document.getElementById("gw_1").disabled = document.getElementById("dhcp").checked;
    document.getElementById("gw_2").disabled = document.getElementById("dhcp").checked;
    document.getElementById("gw_3").disabled = document.getElementById("dhcp").checked;

    }

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

function update_load_data_set() {
      // ใช้ AJAX เพื่อดึงข้อมูลจาก '/getADC'
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
          // เมื่อข้อมูลถูกดึงมา, อัปเดตค่าบน HTML
          var json_data_set = JSON.parse(xhr.responseText);
          document.getElementById('mac_0').value = json_data_set.mac_0;
          document.getElementById('dhcp').value = json_data_set.dhcp;

          document.getElementById('ip_0').value = json_data_set.ip_0;
          document.getElementById('ip_1').value = json_data_set.ip_1;
          document.getElementById('ip_2').value = json_data_set.ip_2;
          document.getElementById('ip_3').value = json_data_set.ip_3;

          document.getElementById('nm_0').value = json_data_set.nm_0;
          document.getElementById('nm_1').value = json_data_set.nm_1;
          document.getElementById('nm_2').value = json_data_set.nm_2;
          document.getElementById('nm_3').value = json_data_set.nm_3;

          document.getElementById('gw_0').value = json_data_set.gw_0;
          document.getElementById('gw_1').value = json_data_set.gw_1;
          document.getElementById('gw_2').value = json_data_set.gw_2;
          document.getElementById('gw_3').value = json_data_set.gw_3;

          document.getElementById('dns1_0').value = json_data_set.dns1_0;
          document.getElementById('dns1_1').value = json_data_set.dns1_1;
          document.getElementById('dns1_2').value = json_data_set.dns1_2;
          document.getElementById('dns1_3').value = json_data_set.dns1_3;

          document.getElementById('ssid_name').value = json_data_set.ssid_name;
          document.getElementById('ssid_pass').value = json_data_set.ssid_pass;
          document.getElementById('apssid_name').value = json_data_set.apssid_name;
          document.getElementById('apssid_pass').value = json_data_set.apssid_pass;
          if(json_data_set.ssid_en==1)
            document.getElementById('ssid_en').checked=true;
          else
            document.getElementById('ssid_en').checked=false;

          document.getElementById('ssid_en').value = json_data_set.ssid_en;
          document.getElementById('ap_ssid_en').value = json_data_set.ap_ssid_en;
          if(json_data_set.ap_ssid_en==1)
            document.getElementById('ap_ssid_en').checked=true;
          else
            document.getElementById('ap_ssid_en').checked=false;
        }
      };
      xhr.open("GET", "/up_net_set", true);
      xhr.send();
    }

function scanWiFi() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var networks = JSON.parse(this.responseText);
            var select = document.getElementById("wifiNetworks");
            select.innerHTML = ""; // Clear previous options
            for (var i = 0; i < networks.length; i++) {
                var option = document.createElement("option");
                option.text = networks[i].ssid;
                option.value = networks[i].ssid;
                select.appendChild(option);
            }
        }
    };
    xhttp.open("GET", "/scan_wifi", true);
    xhttp.send();
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

function updateSSIDField() {
    var select = document.getElementById("wifiNetworks");
    var ssidField = document.getElementById("ssid_name");
    var selectedSSID = select.value;
    ssidField.value = selectedSSID;
}

function update_en_sta_mode(){
    document.getElementById("ssid_name").disabled = true;
    document.getElementById("ssid_pass").disabled = !document.getElementById("ssid_en").checked;
    if(document.getElementById("ssid_en").checked)
        document.getElementById("ssid_en").value=1;
    else
        document.getElementById("ssid_en").value=0;
}

function update_en_ap_mode(){
    document.getElementById("apssid_name").disabled = !document.getElementById("ap_ssid_en").checked;
    document.getElementById("apssid_pass").disabled = !document.getElementById("ap_ssid_en").checked;
    if(document.getElementById("ap_ssid_en").checked)
        document.getElementById("ap_ssid_en").value=1;
    else
        document.getElementById("ap_ssid_en").value=0;
}

document.getElementById("Tab1").style.display = "block";
window.onload = function () 
    {
    document.getElementById("ssid_en").checked=true;
    document.getElementById("ssid_en").value=1;
    document.getElementById("ap_ssid_en").checked=true;
    document.getElementById("ap_ssid_en").value=1;
    update_load_data_set(); 
    openTab(event, 'Tab1'); 
    update_en_sta_mode();
    update_en_ap_mode();
    }
</script>

</html>
)rawliteral";


void scan_ssid()
{
  Serial.println("Scan start");

    //WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();
    st01.net.scan_num_ssid = n;
    Serial.println("Scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        if(n>7) n=7;
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            st01.net.scan_ssid[i]=WiFi.SSID(i).c_str();
            st01.net.scan_rssi[i]=WiFi.RSSI(i);
            st01.net.scan_encryptionType[i]=WiFi.encryptionType(i);
            st01.net.scan_ch[i]=WiFi.channel(i);

            Serial.printf("%2d",i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", st01.net.scan_ssid[i].c_str());
            Serial.print(" | ");
            Serial.printf("%4d", st01.net.scan_rssi[i]);
            Serial.print(" | ");
            Serial.printf("%2d", st01.net.scan_ch[i]);
            Serial.print(" | ");

            switch (st01.net.scan_encryptionType[i])
            {
            case WIFI_AUTH_OPEN:
                Serial.print("open");
                break;
            case WIFI_AUTH_WEP:
                Serial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Serial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Serial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Serial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Serial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Serial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Serial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Serial.print("WAPI");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();
            delay(10);
        }
    }
    Serial.println("");

    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();
}

void api_scan_setting()
{
    server.on("/scan_wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    scan_ssid();
    int i=0;
    // int numNetworks = WiFi.scanNetworks();
    String json_data_set = "[";

    if(st01.net.scan_num_ssid>0)
        {
        // for(i=0; i<st01.net.scan_num_ssid; i++)
        //     {
        //     if (i > 0) json_data_set += ",";
        //     json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[i] + "\",\"rssi\":" + String(st01.net.scan_rssi[i]) + "}";
        //     }
        json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[0] + "\",\"rssi\":" + String(st01.net.scan_rssi[0]) + "},";
        json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[1] + "\",\"rssi\":" + String(st01.net.scan_rssi[1]) + "},";
        json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[2] + "\",\"rssi\":" + String(st01.net.scan_rssi[2]) + "},";
        json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[3] + "\",\"rssi\":" + String(st01.net.scan_rssi[3]) + "},";
        json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[4] + "\",\"rssi\":" + String(st01.net.scan_rssi[4]) + "},";
        json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[5] + "\",\"rssi\":" + String(st01.net.scan_rssi[5]) + "},";
        json_data_set += "{\"ssid\":\"" + st01.net.scan_ssid[6] + "\",\"rssi\":" + String(st01.net.scan_rssi[6]) + "}";
        }
    else{
        json_data_set += "{\"ssid\":\"" + String("none") + "\",\"rssi\":" + String(-255) + "}";
        }
    json_data_set += "]";
    // WiFi.scanDelete();
    // Serial.printf("\r\nwifi scan json=%s ",json_data_set.c_str());
    request->send(200, "application/json", json_data_set);    
    });
}



void api_get_web_net_setting()
{
    server.on("/up_net_set", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json_data_set = "{";
    WiFi.macAddress().toCharArray(st01.msg,st01.mac.length()+1);
    json_data_set += "\"mac_0\":\"" + String(st01.msg)+"\""+ ",";
    json_data_set += "\"dhcp\":" + String(st01.net.dhcp)+ ",";

    json_data_set += "\"ip_0\":" + String(st01.net.localIp[0])+ ",";
    json_data_set += "\"ip_1\":" + String(st01.net.localIp[1])+ ",";
    json_data_set += "\"ip_2\":" + String(st01.net.localIp[2])+ ",";
    json_data_set += "\"ip_3\":" + String(st01.net.localIp[3])+ ",";

    json_data_set += "\"nm_0\":" + String(st01.net.localSubnet[0])+ ",";
    json_data_set += "\"nm_1\":" + String(st01.net.localSubnet[1])+ ",";
    json_data_set += "\"nm_2\":" + String(st01.net.localSubnet[2])+ ",";
    json_data_set += "\"nm_3\":" + String(st01.net.localSubnet[3])+ ",";

    json_data_set += "\"gw_0\":" + String(st01.net.localGateWay[0])+ ",";
    json_data_set += "\"gw_1\":" + String(st01.net.localGateWay[1])+ ",";
    json_data_set += "\"gw_2\":" + String(st01.net.localGateWay[2])+ ",";
    json_data_set += "\"gw_3\":" + String(st01.net.localGateWay[3])+ ",";

    json_data_set += "\"dns1_0\":" + String(st01.net.localDns[0])+ ",";
    json_data_set += "\"dns1_1\":" + String(st01.net.localDns[1])+ ",";
    json_data_set += "\"dns1_2\":" + String(st01.net.localDns[2])+ ",";
    json_data_set += "\"dns1_3\":" + String(st01.net.localDns[3])+ ",";
    json_data_set += "\"ssid_name\":\"" + String(st01.net.ssid)+"\""+",";
    json_data_set += "\"ssid_pass\":\"" + String(st01.net.password)+"\""+",";
    json_data_set += "\"ssid_en\":" + String(st01.net.ssid_en)+",";
    // if(st01.net.ssid_en)
    //     json_data_set += "\"ssid_en\":\"" + String("on")+"\""+",";
    // else
    //     json_data_set += "\"ssid_en\":\"" + String("off")+"\""+",";

    json_data_set += "\"apssid_name\":\"" + String(st01.net.ap_ssid)+"\""+",";
    json_data_set += "\"apssid_pass\":\"" + String(st01.net.ap_password)+"\""+",";
    json_data_set += "\"ap_ssid_en\":" + String(st01.net.ap_ssid_en)+",";
    // if(st01.net.ap_ssid_en)
    //     json_data_set += "\"ap_ssid_en\":\"" + String("on")+"\""+",";
    // else
    //     json_data_set += "\"ap_ssid_en\":\"" + String("off")+"\""+",";

    json_data_set += "\"web_user\":\"" + String(st01.net.web_user)+"\""+",";
    json_data_set += "\"web_pass\":\"" + String(st01.net.web_pass)+"\"";
    json_data_set += "}";
    Serial.printf("\r\nup_net_set json=%s ",json_data_set.c_str());
    request->send(200, "application/json", json_data_set);
  });
}

void init_apt_web_ssid_setting()
{
    server.on("/ssid_set", HTTP_POST, [](AsyncWebServerRequest *request){
    // Loop through each input with name="number" and print its value
    Serial.println("POST web_ssid_setting");
    
    String inputName ="ssid_name";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        
        inputValue.toCharArray(st01.net.ssid,inputValue.length()+1);
        Serial.print("STA_Networks : ");
        Serial.println(st01.net.ssid);
      }

    inputName ="ssid_pass";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
    
        inputValue.toCharArray(st01.net.password,inputValue.length()+1);
        Serial.print("ssid_pass : ");
        Serial.println(st01.net.password);
      }
    inputName ="ssid_en";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        st01.net.ssid_en=inputValue.toInt();
        // if (inputValue == "checked") {
        //     st01.net.ssid_en = 1; // Set ssid_en to 1 if the checkbox is checked
        // } else {
        //     st01.net.ssid_en = 0; // Set ssid_en to 0 if the checkbox is unchecked
        // }
        Serial.printf("ssid_en : %d  ",st01.net.ssid_en);
        Serial.print("ssid_en :");
        Serial.println(inputValue);
      }

    WriteConfig();
    // request->send(200);
    request->send(200, "text/html",web_net_setting);
  });
}

void init_apt_ap_web_ssid_setting()
{
    server.on("/ap_ssid_set", HTTP_POST, [](AsyncWebServerRequest *request){
    // Loop through each input with name="number" and print its value
    Serial.println("POST web_ssid_setting");
    String inputName ="apssid_name";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        
        inputValue.toCharArray(st01.net.ap_ssid,inputValue.length()+1);
        Serial.print("apssid_name :");
        Serial.println(inputValue);
      }

    inputName ="apssid_pass";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
    
        inputValue.toCharArray(st01.net.ap_password,inputValue.length()+1);
        Serial.print("apssid_pass : ");
        Serial.println(inputValue );
      }

    inputName ="ap_ssid_en";
    if (request->hasParam(inputName, true)) {
        String inputValue = request->getParam(inputName, true)->value();
        st01.net.ap_ssid_en=inputValue.toInt();
        // if (inputValue == 1) {
        //     st01.net.ap_ssid_en = 1; // Set ssid_en to 1 if the checkbox is checked
        // } else {
        //     st01.net.ap_ssid_en = 0; // Set ssid_en to 0 if the checkbox is unchecked
        // }
        Serial.printf("ap_ssid_en : %d  ",st01.net.ap_ssid_en);
        Serial.println(inputValue);
      }
    WriteConfig();
    // request->send(200);
    request->send(200, "text/html",web_net_setting);
  });
}
