/*
   Copyright (C) 2019 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file ESP32-CAM.ino
  *
  * Main source with setup() and loop() 
  */

#include "esp_camera.h"
#include <WiFi.h>

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_M5STACK_PSRAM
#define CAMERA_MODEL_AI_THINKER


#include "Config.h"
#define USE_CONFIG_OVERRIDE //!< Switch to use ConfigOverride
#ifdef USE_CONFIG_OVERRIDE
  #include "ConfigOverride.h"
#endif

#if defined(CAMERA_MODEL_WROVER_KIT)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#else
#error "Camera model not selected"
#endif

void startCameraServer();

String lastRoverInfo;
String serialData;
bool   infoMode = true;

String getActionCmds(String cmds)
{
   if (cmds.indexOf('A') != -1) {
      sensor_t * s = esp_camera_sensor_get();
      
      infoMode = !infoMode; // toggle info mode on 'A' button
      if (s) {
         if (infoMode) {
            // smaller video on info mode.
            s->set_framesize(s, FRAMESIZE_HQVGA);
         } else {
            s->set_framesize(s, FRAMESIZE_QVGA);
         }
      }
   }
   // Send commands via Serial to the rover.
   // All Commands should start with '21' as cmd-identifier
   Serial.printf("21%s\n", cmds.c_str());
   // return last rover information
   // sample: 'X:[1.04] Y:[-0.34] Z:[0.33] T:[26.71] V:[8.60] LL:[976] LR:[977] S:[128] IM:[1]'
   if (infoMode) {
      return lastRoverInfo + "IM:[1] ";
   } else {
      return lastRoverInfo + "IM:[0] ";
   }
}

void setup() 
{
   delay(1000);
   
   Serial.begin(115200);
   
   camera_config_t config;
   config.ledc_channel = LEDC_CHANNEL_0;
   config.ledc_timer = LEDC_TIMER_0;
   config.pin_d0 = Y2_GPIO_NUM;
   config.pin_d1 = Y3_GPIO_NUM;
   config.pin_d2 = Y4_GPIO_NUM;
   config.pin_d3 = Y5_GPIO_NUM;
   config.pin_d4 = Y6_GPIO_NUM;
   config.pin_d5 = Y7_GPIO_NUM;
   config.pin_d6 = Y8_GPIO_NUM;
   config.pin_d7 = Y9_GPIO_NUM;
   config.pin_xclk = XCLK_GPIO_NUM;
   config.pin_pclk = PCLK_GPIO_NUM;
   config.pin_vsync = VSYNC_GPIO_NUM;
   config.pin_href = HREF_GPIO_NUM;
   config.pin_sscb_sda = SIOD_GPIO_NUM;
   config.pin_sscb_scl = SIOC_GPIO_NUM;
   config.pin_pwdn = PWDN_GPIO_NUM;
   config.pin_reset = RESET_GPIO_NUM;
   config.xclk_freq_hz = 20000000;
   config.pixel_format = PIXFORMAT_JPEG;
   
   //init with high specs to pre-allocate larger buffers
   if(psramFound()){
      config.frame_size = FRAMESIZE_UXGA;
      config.jpeg_quality = 10;
      config.fb_count = 2;
   } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.jpeg_quality = 12;
      config.fb_count = 1;
   }
   
   // camera init
   esp_err_t err = esp_camera_init(&config);
   if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
   }
   
   //drop down frame size for higher initial frame rate
   sensor_t * s = esp_camera_sensor_get();
   s->set_framesize(s, FRAMESIZE_HQVGA);
   
   WiFi.begin(WIFI_SID, WIFI_PW);
   
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   Serial.println("");
   Serial.println("WiFi connected");
   
   startCameraServer();
   
   Serial.print("Camera Ready! Use 'http://");
   Serial.print(WiFi.localIP());
   Serial.println("' to connect");

   serialData.reserve(255);
}

void loop() 
{
   while (Serial.available() > 0) {
      int readByte = Serial.read();

      // Serial.println("["+ String((char) readByte)+"]");
      if (readByte != -1 && readByte != '\r') {
         if (readByte != '\n') {
            serialData += (char) readByte;
         } else { // == '\n'
            lastRoverInfo = serialData;
            // Serial.println("xx: " + lastRoverInfo);
            serialData = "";           
         }
      }
   }
   delay(10);
}
