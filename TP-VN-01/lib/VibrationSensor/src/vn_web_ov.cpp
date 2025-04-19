
#include "vn_web_ov.h"
#include "web_home.h"
#include "vn_web_module.h"
#include "mems.h"
#include <ArduinoJson.h>
StaticJsonDocument<2500> doc;

vn_WebPage::vn_WebPage(unsigned long tkinterval):_interval(tkinterval),
server(80),
Mqttclient(wifiSecureClient),
mqttClient2(ethClient)
{
    
}


String vn_WebPage::formatBytes(size_t bytes)
{
    if (bytes < 1024)
    {
      return String(bytes) + "B";
    }
    else if (bytes < (1024 * 1024))
    {
      return String(bytes / 1024.0) + "KB";
    }
    else if (bytes < (1024 * 1024 * 1024))
    {
      return String(bytes / 1024.0 / 1024.0) + "MB";
    }
    else
    {
      return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

uint8_t vn_WebPage::deleteFile(const char* filename) {
    if (SPIFFS.exists(filename)) {
      if (SPIFFS.remove(filename)) {
        Serial.println("File deleted successfully");
        return 0;
      } else {
        Serial.println("Error deleting file");
        return 1;
      }
    } else {
      Serial.println("File does not exist");
      return 2;
    }
  }

void vn_WebPage::listDir()
  {
    root = SPIFFS.open("/");
    file = root.openNextFile();
    size_t fileSize;
    String fileName;
    size_t bytesRead;
    while (file)
    {
      fileName = file.name();
      fileSize = file.size();
      Serial.printf("FS File: %s, size: %s path:%s\n", fileName.c_str(), formatBytes(fileSize).c_str(),file.path() );
      file = root.openNextFile();
    }
  }


void vn_WebPage::onUnauthorized(AsyncWebServerRequest *request) {
    if (!request->authenticate("admin", "admin"))
      return request->requestAuthentication();
  }

void vn_WebPage::starting_web()
{
    server.on("/css/w3.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/css/w3.css", "text/css");
      });
    
    server.on("/css/iotadd.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/css/iotadd.css", "text/css");
      });

      server.on("/assets/bars-solid.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/assets/bars-solid.svg", "image/svg+xml");
      });
    
      server.on("/assets/Fiot.png", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/assets/Fiot.png", "image/png");
      });
    
      server.on("/assets/Fiot2.png", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/assets/Fiot2.png", "image/png");
      });
    
      server.on("/css/chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/css/chart.js", "application/javascript");
      });
      
    // Route for root / web page
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    this->onUnauthorized(request);
    web_active=true;
    request->send_P(200, "text/html", web_overview);
  });
  server.on("/index.html", HTTP_GET, [this](AsyncWebServerRequest *request){
    this->onUnauthorized(request);
    web_active=true;
    request->send_P(200, "text/html", web_overview);
  });
  
  //============================================//
  server.on("/gSampling", HTTP_GET, [this](AsyncWebServerRequest *request){
    // StaticJsonDocument<2000> doc;  // May need to increase if content too large
    // StaticJsonDocument<2048> doc;

    web_active=true;
    // delay(1500);
    JsonArray xArray = doc["x"].to<JsonArray>();
    JsonArray yArray = doc["y"].to<JsonArray>();
    JsonArray zArray = doc["z"].to<JsonArray>();

    for (int i = 0; i < NUM_SAMPLES; i++) {
      xArray.add(roundf(vn_mems.pmems_data->x.gRaw[i] * 10000.0f) / 10000.0f);
      yArray.add(roundf(vn_mems.pmems_data->y.gRaw[i] * 10000.0f) / 10000.0f);
      zArray.add(roundf(vn_mems.pmems_data->z.gRaw[i] * 10000.0f) / 10000.0f);
    }

    String jsonStr;
    serializeJson(doc, jsonStr);
    
    request->send(200, "application/json", jsonStr);
  });

  server.on("/gFFT", HTTP_GET, [this](AsyncWebServerRequest *request){
    // StaticJsonDocument<2000> doc;  // May need to increase if content too large
    
    web_active=true;
    // delay(1500);
    JsonArray xArray = doc["x"].to<JsonArray>();
    JsonArray yArray = doc["y"].to<JsonArray>();
    JsonArray zArray = doc["z"].to<JsonArray>();

    for (int i = 0; i < NUM_SAMPLES/2; i++) {
      xArray.add(roundf(vn_mems.pmems_data->x.gFFT[i] * 10000.0f) / 10000.0f);
      yArray.add(roundf(vn_mems.pmems_data->y.gFFT[i] * 10000.0f) / 10000.0f);
      zArray.add(roundf(vn_mems.pmems_data->z.gFFT[i] * 10000.0f) / 10000.0f);
    }

    String jsonStr;
    serializeJson(doc, jsonStr);
    
    request->send(200, "application/json", jsonStr);
  });

  server.on("/gFFTpeak", HTTP_GET, [this](AsyncWebServerRequest *request){
    // StaticJsonDocument<2000> doc;  // May need to increase if content too large
    
    web_active=true;
    // delay(1500);
    JsonArray xArray = doc["x"].to<JsonArray>();
    JsonArray yArray = doc["y"].to<JsonArray>();
    JsonArray zArray = doc["z"].to<JsonArray>();

    for (int i = 0; i < 10; i++) {
      JsonObject xObj = xArray.createNestedObject();
      xObj["freq"] = roundf(vn_mems.pmems_data->x.peakFreq[i] * 10000.0f) / 10000.0f;
      xObj["mag"]  = roundf(vn_mems.pmems_data->x.peakValue[i] * 10000.0f) / 10000.0f;

      JsonObject yObj = yArray.createNestedObject();
      yObj["freq"] = roundf(vn_mems.pmems_data->y.peakFreq[i] * 10000.0f) / 10000.0f;
      yObj["mag"] = roundf(vn_mems.pmems_data->y.peakValue[i] * 10000.0f) / 10000.0f;

      JsonObject zObj = zArray.createNestedObject();
      zObj["freq"] = roundf(vn_mems.pmems_data->z.peakFreq[i] * 10000.0f) / 10000.0f;
      zObj["mag"] = roundf(vn_mems.pmems_data->z.peakValue[i] * 10000.0f) / 10000.0f;
    }

    String jsonStr;
    serializeJson(doc, jsonStr);
    
    request->send(200, "application/json", jsonStr);
  });

  server.on("/adxl", HTTP_GET, [this](AsyncWebServerRequest *request){
    // StaticJsonDocument<2000> doc;  // May need to increase if content too large
    web_active=true;
    vn_mems.motionDetected=true;
    request->send(200, "application/json", "{\"ADXL\":\"updated\"}");
  });
  //============================================//

  server.begin();
}
