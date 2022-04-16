#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <stdint.h>
#include <FastLED.h>
// #include "AsyncJson.h"
// #include "ArduinoJson.h"
// #include "wifi_credentials.h"
// #include "facestorage.h"
// #include "webserver.h"
// #include "matrix.h"
// #include "matrixmanager.h"
// #include "max7219painter.h"
#include "displays/ws2812/ws2812display.h"
#include "displays/ws2812/ws2812matrix.h"
#include "displays/ws2812/ws2812string.h"

#define DELTA_E 0.5
#define FACE_COLS 16 + 16 + 8 + 8 + 32 + 32
#define FACE_ROWS 8
#define FBSIZE FACE_COLS
#define N_LAYERS 4

// Define the layout of our physical display
WS2812Display display {
    new WS2812StringPin<16> {
            WS2812Matrix(16, 8, 0),
            WS2812Matrix(32, 8, 0),
            WS2812Matrix(8, 8, 0),
    },
    new WS2812StringPin<17> {
            WS2812Matrix(16, 8, 0),
            WS2812Matrix(32, 8, 0),
            WS2812Matrix(8, 8, 0),
    }
};
// WS2812Display *display;

// WS2812Matrix<17> testmatrix(16, 8, 0);

// // Internal, hardware agnostic representation of the matrices
// // Basically just specialized bitmaps with some convenience functions
// // and animation stuff built in
// MatrixManager matrixmanager;
// Matrix eye_r("eye_r", 16, 8, N_LAYERS);
// Matrix eye_l("eye_l", 16, 8, N_LAYERS);
// Matrix nose_r("nose_r", 8, 8, N_LAYERS);
// Matrix nose_l("nose_l", 8, 8, N_LAYERS);
// Matrix mouth_r("mouth_r", 32, 8, N_LAYERS);
// Matrix mouth_l("mouth_l", 32, 8, N_LAYERS);

// // The face that is actually drawn
// // TODO: refactor stuff that accesses this pointer
// uint8_t *framebuffer;// = matrixmanager.data; (can't set here yet since it's still uninitialized)

// // Loaded from wifi_credentials.h
// extern const char *ssid;
// extern const char *password;

// // Manages access to SPI flash for loading and storing faces
// FaceStorage facestorage(SPIFFS, FBSIZE);

// // WebServer wraps the async server for convenience and cleaner setup
// AsyncWebServer asyncserver(4000);
// WebServer server(asyncserver, facestorage, matrixmanager, FBSIZE, framebuffer);

// void setupWiFI()
// {
//   WiFi.begin(ssid, password);

//   if (WiFi.waitForConnectResult() != WL_CONNECTED)
//   {
//     Serial.println("Connection failed");
//     delay(1000);
//     return;
//   }

//   Serial.print("Connected: ");
//   Serial.println(WiFi.localIP());

//   delay(1000);
// }

void setup()
{
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);
  Serial.println("IN SETUP");

  // display = new WS2812Display { 
  //     WS2812String {
  //         WS2812Matrix(16, 8, 0),
  //         WS2812Matrix(32, 8, 0),
  //         WS2812Matrix(8, 8, 0),
  //     }
  // };

  Serial.println("RETURN FROM CONSTRUCTOR");

  // // Init Storage
  // if (!SPIFFS.begin())
  // {
  //   Serial.setTimeout(100000);
  //   Serial.println("Storage Mount Failed. Format? [y/n]");
  //   while(true) {
  //     if (Serial.available()) {
  //       char response = Serial.read();
  //       if (response == 'y') {
  //         SPIFFS.format();
  //         Serial.println("Storage Formatted");
  //         ESP.restart();
  //       } else {
  //         Serial.println("Rebooting...");
  //         ESP.restart();
  //       }
  //     }
  //   }
  // }

  // // Register & configure matrices
  // matrixmanager.add(&eye_r);
  // matrixmanager.add(&eye_l);
  // matrixmanager.add(&nose_r);
  // matrixmanager.add(&nose_l);
  // matrixmanager.add(&mouth_r);
  // matrixmanager.add(&mouth_l);
  // matrixmanager.setMatrixBlink("eye_r", true);
  // matrixmanager.setMatrixBlink("eye_l", true);
  // matrixmanager.init(); // DO NOT FORGET omg
  // framebuffer = matrixmanager.data;

  // // Set the drawing positions of the matrices on the screen
  // painter.setNameMapping(matrixPositions);

  // setupWiFI();

  // Serial.println("Setting up server");
  // server.init(framebuffer);
  // Serial.println("Server setup complete");

  // // Load default face
  // Serial.println("Loading default face");
  // int result = facestorage.loadface("boi", framebuffer);
  // if (result != 0)
  // {
  //   Serial.println("Failed loading default face");
  // } else {
  //   Serial.println("Setting default face");
  // }

  // matrixmanager.setFrame(framebuffer);

  // Add the matrix strings from the display to fastled
  // for (auto tuple : display.getBuffers())
  // {
  //   CRGB *buf = std::get<0>(tuple);
  //   ssize_t buf_len = std::get<1>(tuple);
  //   const uint8_t data_pin = std::get<2>(tuple);
  //   LEDS.addLeds<WS2812, data_pin, GRB>(buf, buf_len);
  // }
  
  // auto tuples = display.getBuffers();
  // LEDS.addLeds<WS2812, 16, GRB>(std::get<0>(tuples[0]), std::get<1>(tuples[0]));
  // LEDS.addLeds<WS2812, 17, GRB>(std::get<0>(tuples[1]), std::get<1>(tuples[1]));
  // LEDS.addLeds<WS2812, 17, GRB>(testmatrix.fastled_mem, 16*8);
  display.setBrightness(16);

  Serial.println("Setup complete");
}

// uint32_t last = 0;
// uint32_t last_blink = 0;
// uint32_t blink_interval = 10000;
// int blink_randomness;
// bool blinking = false;
// uint32_t last_debug_print = 0;
// int last_dtime = 0;

void loop()
{
  Serial.println("IN LOOP");
  display.fillScreen(fromRGB(255, 0, 0));
  display.show();
  delay(1000);
  display.clear();
  display.show();
  delay(1000);
  // uint32_t now = millis();

  // // Handle overflow, update next loop
  // if (now < last)
  // {
  //   last = 0;
  //   return;
  // }

  // // Time delta in milliseconds
  // uint32_t deltatime = now - last;
  // last = now;

  // // Handle blinking
  // last_blink += deltatime;
  // if (last_blink > blink_interval)
  // {
  //   if (!blinking)
  //   {
  //     blinking = true;
  //     matrixmanager.blink();
  //   }
  //   if (blinking)
  //   {
  //     if (last_blink > blink_interval + 100)
  //     {
  //       last_blink = 0;
  //       blink_interval = random(7000, 13000);
  //       blinking = false;
  //       matrixmanager.clearBlink();
  //     }
  //   }
  // }

  // // Animate face
  // matrixmanager.update(deltatime);

  // // Draw face to screen/matrices/whatever
  // matrixmanager.paint(painter);

  // // Debug prints
  // if (deltatime > last_dtime + 1 || deltatime < last_dtime - 1)
  // {
  //   Serial.print("dtime: ");
  //   Serial.println(deltatime);

  //   last_dtime = deltatime;
  // }
}