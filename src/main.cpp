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

// #include "displays/ws2812/ws2812display.h"
// #include "displays/ws2812/ws2812matrix.h"
// #include "displays/ws2812/ws2812string.h"

#include "scene/scene.h"
#include "scene/elements/bitmapelement.h"
// #include "scene/elements/adagfxelement.h"
#include "scene/elements/targetfollowerelement.h"

// #include "scene/visitors/elementprinter.h"
#include "scene/visitors/elementrgbbitmapsetter.h"
#include "scene/visitors/elementdrawer.h"
#include "scene/visitors/elementupdater.h"

#include "scene/modifiers/mirror.h"
#include "scene/modifiers/rainbow.h"

#include "bitmaps.h"
#include "displays/fastled/fastleddisplay.h"
#include "displays/fastled/fastledstring.h"
#include "displays/fastled/fastledmatrix.h"

#include "crgbcanvas.h"

// For synchronization of update and drawing loop
SemaphoreHandle_t xBinarySemaphore;

TaskHandle_t DrawingTask;
TaskHandle_t UpdateTask;

// Define the layout of our physical display
FastLEDDisplay display {
    new FastLEDString{
        new WS2812<16, GRB>, {
            new FastLEDMatrix{16, 8, 0, 0, 0},
            new FastLEDMatrix{32, 8, 0, 16, 0},
            new FastLEDMatrix{8, 8, 32, 8, 0},
        }},
    new FastLEDString{
        new WS2812<17, GRB>, {
            new FastLEDMatrix{16, 8, 64, 0, 2},
            new FastLEDMatrix{32, 8, 48, 16, 2},
            new FastLEDMatrix{8, 8, 40, 8, 2},
        }}
};

#define NUM_LEDS_SIDEPANEL 15 + 8 + 16 + 12

ProtoControl::BitmapManager<256> bitmapManager;

Scene scene{

    new TargetFollowerElement{"eye_l_follower", 16, 8, 0, 0, {
        new MirrorHorizontal<BitmapElement>{"eye_l"},
    }},
    new TargetFollowerElement{"eye_r_follower", 16, 8, 64, 0, {
        new BitmapElement{"eye_r"},
    }},

    new TargetFollowerElement {"nose_l_follower", 8, 8, 32, 8, {
        new MirrorHorizontal<BitmapElement>{"nose_l"},
    }},
    new TargetFollowerElement {"nose_r_follower", 8, 8, 40, 8, {
        new BitmapElement{"nose_r"},
    }},
    

    new TargetFollowerElement {"mouth_l_follower", 32, 8, 0, 16, {
        new MirrorHorizontal<BitmapElement>{"mouth_l"},
    }},
    new TargetFollowerElement {"mouth_r_follower", 32, 8, 48, 16, {
        new BitmapElement{"mouth_r"},
    }}
    
};


volatile bool flipped = false;
CRGBCanvas framebuffer[] {
    CRGBCanvas(80, 24),
    CRGBCanvas(80, 24)
};

// // Used to draw to the display
ElementDrawer drawer[] {
    ElementDrawer(framebuffer[0]),
    ElementDrawer(framebuffer[1])
};

// // Used to set bitmaps in the scene
ElementRGBBitmapSetter bmpsetter;
ElementRGBBitmapSetter eyesclosedsetter;

// // Used to print a text representation of the scene
// ElementPrinter ep(Serial);

// // Used to update the elements in the scene
ElementUpdater updater;

unsigned long last = 0;
void updateLoop(void * params) {

    // For benchmarking
    unsigned long before;
    unsigned long after;
    unsigned long delta;

    while (true) {

        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        before = micros();

        // Swap buffers
        flipped = !flipped;

        // Serial.printf("Clearing %d \n", flipped);
        framebuffer[flipped].clear();

        xSemaphoreGive(xBinarySemaphore);

        unsigned long now = micros();
        delta = now - last;
        
        // Serial.printf("Loop took %d us\n", delta);

        last = micros();

        updater.setTimeDelta(micros());
        updater.visit(&scene);

        drawer[flipped].visit(&scene);

        if (now / 200000 % 2)
        {
            eyesclosedsetter.visit(&scene);
        }
        else
        {
            bmpsetter.visit(&scene);
        }

        after = micros();
        delta = after - before;

        // Serial.printf("Updating took %d us\n", delta);

    }
}

void drawLoop(void * params) {
    unsigned long begin = 0;
    unsigned long end = 0;
    while (true) {
        begin = micros();

        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        display.clear();

        // Move framebuffer to display
        // Serial.printf("Drawing %d \n", !flipped);
        auto& cur_framebuffer = framebuffer[!flipped];
        for (uint32_t x = 0; x < cur_framebuffer.getWidth(); x++) {
            for (uint32_t y = 0; y < cur_framebuffer.getHeight(); y++) {
                display.setPixel(x, y, cur_framebuffer.getPixel(x, y));
            }
        }

        display.show();

        // delay(1000);

        xSemaphoreGive(xBinarySemaphore);
        delayMicroseconds(100);

        end = micros();

        // Serial.printf("drawloop took %d us\n", end - begin);

        
    }
} 

void setup()
{
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(921600);

  // Set up littleFS
  if (!LITTLEFS.begin(true))
  {
    Serial.println("LittleFS Mount Failed");
    while (true)
    {
    }
  }

  // Collect and load the bitmaps in the LITTLEFS /565 folder
  bitmapManager.gather(LITTLEFS, "/565");

  // ep.visit(&scene);

  bmpsetter
      .add("eye_r", bitmapManager.get("/565/proto_eye"))
      .add("eye_l", bitmapManager.get("/565/proto_eye"))
      .add("nose_r", bitmapManager.get("/565/proto_nose"))
      .add("nose_l", bitmapManager.get("/565/proto_nose"))
      .add("mouth_r", bitmapManager.get("/565/proto_mouth"))
      .add("mouth_l", bitmapManager.get("/565/proto_mouth"))
      .add("ear_l", bitmapManager.get("/565/proto_ear"))
      .add("ear_r", bitmapManager.get("/565/proto_ear"))
      .visit(&scene);

  eyesclosedsetter
      .add("eye_r", bitmapManager.get("/565/proto_eye_closed"))
      .add("eye_l", bitmapManager.get("/565/proto_eye_closed"));

  display.setBrightness(32);

  xBinarySemaphore = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(
                    updateLoop,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &UpdateTask,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 1 */

    xTaskCreatePinnedToCore(
                drawLoop,   /* Task function. */
                "Task2",     /* name of task. */
                10000,       /* Stack size of task */
                NULL,        /* parameter of the task */
                1,           /* priority of the task */
                &DrawingTask,      /* Task handle to keep track of created task */
                1);          /* pin task to core 1 */

    xSemaphoreGive(xBinarySemaphore);
}


unsigned long prev = 0;
void loop()
{
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  delay(10000);
  // // For updating
//   unsigned long now = micros();
//   // unsigned long updateDelta = now - prev;
//   // unsigned long prev = now;

//   // For benchmarking
//   unsigned long before;
//   unsigned long after;
//   unsigned long delta;

//   display.clear();

//   before = micros();

//   updater.setTimeDelta(micros());
//   updater.visit(&scene);

//   drawer.visit(&scene);

//   after = micros();
//   delta = after - before;

//   Serial.printf("Updating took %d us\n", delta);

//   display.show();

//   // // // Update
//   // // before = micros();

//   if (now / 200000 % 2)
//   {
//     eyesclosedsetter.visit(&scene);
//   }
//   else
//   {
//     bmpsetter.visit(&scene);
//   }

  // after = micros();

  // delta = after - before;
  // Serial.print("Updating took ");
  // Serial.print(delta);
  // Serial.println(" us");

  // // Clear
  // before = micros();
  // display.clear();
  // after = micros();

  // delta = after - before;
  // Serial.print("Clearing took ");
  // Serial.print(delta);
  // Serial.println(" us");

  // // Draw
  // before = micros();
  // drawer.visit(&scene);
  // after = micros();

  // delta = after - before;
  // Serial.print("Drawing took ");
  // Serial.print(delta);
  // Serial.println(" us");

  // display.show();
}