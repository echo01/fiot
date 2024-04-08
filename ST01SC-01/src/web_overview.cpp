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

const char web_overview[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
<html>
<head>
<title>FIoT</title>
<link rel="icon" href="/assets/Fiot2.png" type="image/x-icon">
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="/css/w3.css">
<!-- <link rel="stylesheet" href="css/font-awesome.min.css"> -->
<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css">
<!-- <link rel="stylesheet" href="css/fonts-googleapis-raleway.css"> -->
<!-- <link rel="stylesheet" href="css/bootstrap.min.css"> -->
<!-- <script src="https://kit.fontawesome.com/a076d05399.js" crossorigin="anonymous"></script> -->
<style>
html,body,h1,h2,h3,h4,h5 {font-family: "Raleway", sans-serif}
</style>
</head>
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
  <!-- <div class="w3-container w3-padding w3-blue ">
    <h5>Dashboard</h5>
  </div> -->
  <div class="w3-bar-block">
    <a href="#" class="w3-bar-item w3-button w3-padding-16 w3-hide-large w3-dark-grey w3-hover-black" onclick="w3_close()" title="close menu"><i class="fa fa-remove fa-fw"></i>  Close Menu</a>
    <a href="/index.html" class="w3-bar-item w3-button w3-padding w3-blue"><i class="fa fa-users fa-fw"></i>  Overview</a>
    <!-- <a href="#" class="w3-bar-item w3-button w3-padding"><i class="fa fa-users fa-fw"></i>  Traffic</a> -->
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
    <div class="w3-quarter">
      <div class="w3-container w3-red w3-padding-16 w3-card-4">
        <div class="w3-left"><i class="fa fa-thermometer-full w3-xxxlarge" aria-hidden="true"></i></div>
        <div class="w3-right">
          <h2 id="node_t" name="node_t">99</h2>
        </div>
        <div class="w3-clear"></div>
        <h4>temperature (C)</h4>
      </div>
    </div>
    <div class="w3-quarter">
      <div class="w3-container w3-blue w3-padding-16 w3-card-4">
        <div class="w3-left"><i class="fa fa-tint w3-xxxlarge" aria-hidden="true"></i></div>
        <div class="w3-right">
          <h2 id="node_rh" name="node_rh">99</h2>
        </div>
        <div class="w3-clear"></div>
        <h4>Views</h4>
      </div>
    </div>
    <div class="w3-quarter">
      <div class="w3-container w3-teal w3-padding-16 w3-card-4">
        <div class="w3-left"><i class="fa fa-leaf w3-xxxlarge"></i></div>
        <div class="w3-right">
          <h2 id="node_voc" name="node_voc">23</h2>
        </div>
        <div class="w3-clear"></div>
        <h4>Air VOC</h4>
      </div>
    </div>
  </div>

  <div class="w3-panel w3-padding-16">
    <div class="w3-row-padding w3-card-4" style="margin:0 -16px">
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
            <td><i>connected</i></td>
          </tr>
          <tr>
            <td><i class="fa fa-bell w3-text-red w3-large"></i></td>
            <td>last update</td>
            <td><i>15 mins</i></td>
          </tr>
          <tr>
            <td><i class="fa fa-users w3-text-yellow w3-large"></i></td>
            <td>Public Count</td>
            <td><i>15</i></td>
          </tr>
          <tr>
            <td><i class="fa fa-comment w3-text-red w3-large"></i></td>
            <td>Subscribe Cout</td>
            <td><i>25</i></td>
          </tr>
        </table>
      </div>
    </div>
  </div>
  <hr>
  

  <!-- Footer -->
  <footer class="w3-container w3-padding-16 w3-light-grey">
    <!-- <h4>FOOTER</h4> -->
    <p>Powered by AKE  <a href="https://www.w3schools.com/w3css/default.asp" target="_blank">w3.css</a></p>
  </footer>

  <!-- End page content -->
</div>

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
        }
      };
      xhr.open("GET", "/get_node_sensor", true);
      xhr.send();
    }
  // Update ADC values every second
  
  window.onload = function () 
    {
    update_node_sensor(); 
    setInterval(update_node_sensor, 3000);
    }
  // Initial update
</script>

</body>
</html>
)rawliteral";

