#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LITTLEFS.h>
#include <stdint.h>

//#define FASTLED_ESP32_I2S

#include <FastLED.h>
// #include "AsyncJson.h"
// #include "ArduinoJson.h"
// #include "wifi_credentials.h"
#include "facestorage.h"
// #include "webserver.h"
#include "matrix.h"
#include "matrixmanager.h"
// #include "max7219painter.h"

#include "displays/ws2812/ws2812display.h"
#include "displays/ws2812/ws2812matrix.h"
#include "displays/ws2812/ws2812string.h"

#include "scene/scene.h"
#include "scene/elements/bitmapelement.h"
#include "scene/elements/adagfxelement.h"
#include "scene/visitors/elementprinter.h"
#include "scene/visitors/elementrgbbitmapsetter.h"
#include "scene/visitors/elementdrawer.h"

#include "bitmaps.h"

#define DELTA_E 0.5
#define FACE_COLS 16 + 16 + 8 + 8 + 32 + 32
#define FACE_ROWS 8
#define FBSIZE FACE_COLS
#define N_LAYERS 4

// ProtoControl::FSBacked565Bitmap eye(LITTLEFS, "565/proto_eye");
// ProtoControl::FSBacked565Bitmap mouth(LITTLEFS, "565/proto_mouth");
// ProtoControl::FSBacked565Bitmap nose(LITTLEFS, "565/proto_nose");

ProtoControl::BitmapManager<256> bitmapManager;

// Define the layout of our physical display
// The Strings need to be pointers to prevent slicing,
// since we treat them as their base class internally.
WS2812Display display {
    new WS2812StringPin<16> {
            WS2812Matrix(16, 8, 0),
            WS2812Matrix(32, 8, 0),
            WS2812Matrix(8, 8, 0),
    },
    new WS2812StringPin<17> {
            WS2812Matrix(16, 8, 2),
            WS2812Matrix(32, 8, 2),
            WS2812Matrix(8, 8, 2),
    }
};

// Define the elements of our scene
// in this case, these are all the parts of the face we want to draw
Scene scene {       // name     w x h   x   y
  new AdafruitGFXElement {"eye_r",   16, 8,  0,  0},
  new AdafruitGFXElement {"mouth_r", 32, 8,  16, 0},
  new AdafruitGFXElement {"nose_r",  8,  8,  48, 0},
  new AdafruitGFXElement {"eye_l",   16, 8,  56, 0},
  new AdafruitGFXElement {"mouth_l", 32, 8,  72, 0},
  new AdafruitGFXElement {"nose_l",  8,  8,  104,0},
};

// Used to draw to the display
ElementDrawer drawer(display);

// Used to set bitmaps in the scene
ElementRGBBitmapSetter bmpsetter;

// Used to print a text representation of the scene
ElementPrinter ep(Serial);

void setup()
{
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);

  // Set up littleFS
  if(!LITTLEFS.begin(true)){
      Serial.println("LittleFS Mount Failed");
      while(true) {}
  }

  // Collect and load the bitmaps in the LITTLEFS /565 folder
  bitmapManager.gather(LITTLEFS, "/565");

  ep.visit(&scene);

  bmpsetter
    .add("eye_r", new ProtoControl::Static565Bitmap((uint16_t*)epd_bitmap_eye, 16, 8))
    .add("eye_l", new ProtoControl::Static565Bitmap((uint16_t*)epd_bitmap_eye, 16, 8))
    .add("nose_r", new ProtoControl::Static565Bitmap((uint16_t*)epd_bitmap_nose, 8, 8))
    .add("nose_l", new ProtoControl::Static565Bitmap((uint16_t*)epd_bitmap_nose, 8, 8))
    .add("mouth_r", new ProtoControl::Static565Bitmap((uint16_t*)epd_bitmap_mouth, 32, 8))
    .add("mouth_l", new ProtoControl::Static565Bitmap((uint16_t*)epd_bitmap_mouth, 32, 8))
    .visit(&scene);

  drawer.visit(&scene);
}


void loop()
{
  unsigned long before;
  unsigned long after;
  unsigned long delta;

  // Update
  before = micros();
  bmpsetter
    .visit(&scene);
  after = micros();

  delta = after - before;
  // Serial.print("Updating took ");
  // Serial.print(delta);
  // Serial.println(" us");

  // Clear
  before = micros();
  display.clear();
  after = micros();

  delta = after - before;
  // Serial.print("Clearing took ");
  // Serial.print(delta);
  // Serial.println(" us");

  // Draw
  before = micros();
  drawer.visit(&scene);
  after = micros();

  delta = after - before;
  // Serial.print("Drawing took ");
  // Serial.print(delta);
  // Serial.println(" us");

  display.show();
}