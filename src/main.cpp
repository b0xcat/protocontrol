#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LITTLEFS.h>
#include <stdint.h>

// #define FASTLED_ESP32_I2S
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
#include "scene/modifiers/blink.h"
#include "scene/modifiers/coloroverride.h"

#include "bitmaps.h"
#include "displays/fastled/fastleddisplay.h"
#include "displays/fastled/fastledstring.h"
#include "displays/fastled/fastledmatrix.h"
#include "displays/max7219/max7219display.h"

#include "crgbcanvas.h"

#include "remotereceiver.h"

// Debug enable
bool debug = false;
bool rainbowEnabled = false;
CRGB overrideColor{0, 255, 0};

// For synchronization of update and drawing loop
SemaphoreHandle_t xBinarySemaphore;

TaskHandle_t DrawingTask;
TaskHandle_t UpdateTask;

// Define the layout of our physical display
// FastLEDDisplay display {
//     new FastLEDString{
//         new WS2812<16, RGB>, {
//             new FastLEDMatrix{16, 8, 0, 0, 0},
//             new FastLEDMatrix{32, 8, 0, 16, 0},
//             new FastLEDMatrix{8, 8, 32, 8, 0},
//         }},
//     new FastLEDString{
//         new WS2812<17, RGB>, {
//             new FastLEDMatrix{16, 8, 64, 0, 2},
//             new FastLEDMatrix{32, 8, 48, 16, 2},
//             new FastLEDMatrix{8, 8, 40, 8, 2},
//         }},
//     new FastLEDString{
//         new WS2812B<21,RGB>, {
//             new FastLEDMatrix{8, 8, 24, 8, 0}
//         }
//     },
//     new FastLEDString{
//         new WS2812B<22,RGB>, {
//             new FastLEDMatrix{8, 8, 56, 8, 0}
//         }
//     }
// };

#define CS_PIN 19
#define CLK_PIN 18
#define MISO_PIN 2 // we do not use this pin just fill to match constructor
#define MOSI_PIN 23
#define NUMBER_OF_DEVICES 14
SPIClass hspi(HSPI);

Max72xxPanel panel{
    CLK_PIN,
    MISO_PIN,
    MOSI_PIN,
    CS_PIN,
    &hspi,
    NUMBER_OF_DEVICES,
    1};

Max7219display display{panel};

#define NUM_LEDS_SIDEPANEL 15 + 8 + 16 + 12
// WS2812<21, GRB> sidePanelLeft;
// WS2812<22, GRB> sidePanelRight;
// CRGB sidePanelFramebuffer[NUM_LEDS_SIDEPANEL];

ProtoControl::BitmapManager<256> bitmapManager;

BlinkController blinkController(false, 250, 8000, 2000);

Scene scene{
    new Rainbow<TargetFollowerElement> {"eye_r_follower", 16, 8, 0, 0, {
        new Blink<MirrorHorizontal<BitmapElement>>{blinkController, "eye_r"},
    }},
    new Rainbow<TargetFollowerElement> {"mouth_r_follower", 32, 8, 16, 0, {
        new MirrorHorizontal<BitmapElement>{"mouth_r"},
    }},
    new Rainbow<TargetFollowerElement> {"nose_r_follower", 8, 8, 48, 0, {
        new MirrorHorizontal<BitmapElement>{"nose_r"},
    }},

    new Rainbow<TargetFollowerElement> {"nose_l_follower", 8, 8, 56, 0, {
        new BitmapElement{"nose_l"},
    }},
    new Rainbow<TargetFollowerElement> {"mouth_l_follower", 32, 8, 64, 0, {
        new BitmapElement{"mouth_l"},
    }},
    new Rainbow<TargetFollowerElement> {"eye_l_follower", 16, 8, 96, 0, {
        new Blink<BitmapElement>{blinkController, "eye_l"},
    }},

    // new Rainbow<ColorOverride<BitmapElement>>{"ear_r", 24, 8},
    // new Rainbow<ColorOverride<BitmapElement>>{"ear_l", 56, 8}
    
};

volatile bool flipped = false;
CRGBCanvas *framebuffer[]{
    new CRGBCanvas(112, 8),
    new CRGBCanvas(112, 8)};

// // Used to draw to the display
ElementDrawer drawer[]{
    ElementDrawer(*framebuffer[0]),
    ElementDrawer(*framebuffer[1])};

// // Used to set bitmaps in the scene
ElementRGBBitmapSetter normalfacesetter;
ElementRGBBitmapSetter angryfacesetter;
ElementRGBBitmapSetter nwnfacesetter;
ElementRGBBitmapSetter uwufacesetter;
ElementRGBBitmapSetter owofacesetter;
ElementRGBBitmapSetter heartfacesetter;
ElementRGBBitmapSetter arrowfacesetter;

// // Used to print a text representation of the scene
// ElementPrinter ep(Serial);

// // Used to update the elements in the scene
ElementUpdater updater;

unsigned long last = 0;
void updateLoop(void *params)
{

    // For benchmarking
    unsigned long before;
    unsigned long after;
    __attribute__((unused)) unsigned long delta;

    while (true)
    {

        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        before = micros();

        // Swap buffers
        flipped = !flipped;

        // Serial.printf("Clearing %d \n", flipped);
        framebuffer[flipped]->clear();

        xSemaphoreGive(xBinarySemaphore);

        unsigned long now = micros();
        delta = now - last;

        // Serial.printf("Loop took %d us\n", delta);

        last = micros();

        updater.setTimeDelta(micros());
        updater.visit(&scene);

        drawer[flipped].visit(&scene);

        after = micros();
        delta = after - before;

        // Serial.printf("Updating took %d us\n", delta);
    }
}

void drawLoop(void *params)
{
    __attribute__((unused)) unsigned long begin = 0;
    __attribute__((unused)) unsigned long end = 0;
    while (true)
    {
        begin = micros();

        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        display.clear();

        // Move framebuffer to display
        // Serial.printf("Drawing %d \n", !flipped);
        auto &cur_framebuffer = framebuffer[!flipped];
        for (uint32_t x = 0; x < cur_framebuffer->getWidth(); x++)
        {
            for (uint32_t y = 0; y < cur_framebuffer->getHeight(); y++)
            {
                display.setPixel(x, y, cur_framebuffer->getPixel(x, y));
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
    Serial.begin(115200);

    // Print mac address
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    // Set up esp now remote
    WiFi.mode(WIFI_STA);
    RemoteReceiver::init();

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

    // Define expressions
    normalfacesetter
        .add("eye_r", bitmapManager.get("/565/proto_eye"))
        .add("eye_l", bitmapManager.get("/565/proto_eye"))
        .add("nose_r", bitmapManager.get("/565/proto_nose"))
        .add("nose_l", bitmapManager.get("/565/proto_nose"))
        .add("mouth_r", bitmapManager.get("/565/proto_mouth"))
        .add("mouth_l", bitmapManager.get("/565/proto_mouth"))
        // .add("ear_l", bitmapManager.get("/565/proto_ear"))
        // .add("ear_r", bitmapManager.get("/565/proto_ear"))
        .visit(&scene);

    // angryfacesetter
    //     .add("eye_r", bitmapManager.get("/565/proto_eye_angry"))
    //     .add("eye_l", bitmapManager.get("/565/proto_eye_angry"))
    //     .add("nose_r", bitmapManager.get("/565/proto_nose_angry"))
    //     .add("nose_l", bitmapManager.get("/565/proto_nose_angry"))
    //     .add("mouth_r", bitmapManager.get("/565/proto_mouth_angry"))
    //     .add("mouth_l", bitmapManager.get("/565/proto_mouth_angry"))
    //     .add("ear_l", bitmapManager.get("/565/proto_ear"))
    //     .add("ear_r", bitmapManager.get("/565/proto_ear"));

    // nwnfacesetter
    //     .add("eye_r", bitmapManager.get("/565/proto_eye_n"))
    //     .add("eye_l", bitmapManager.get("/565/proto_eye_n"))
    //     .add("nose_r", bitmapManager.get("/565/proto_nose"))
    //     .add("nose_l", bitmapManager.get("/565/proto_nose"))
    //     .add("mouth_r", bitmapManager.get("/565/proto_mouth_w"))
    //     .add("mouth_l", bitmapManager.get("/565/proto_mouth_w"))
    //     .add("ear_l", bitmapManager.get("/565/proto_ear"))
    //     .add("ear_r", bitmapManager.get("/565/proto_ear"));

    // uwufacesetter
    //     .add("eye_r", bitmapManager.get("/565/proto_eye_u"))
    //     .add("eye_l", bitmapManager.get("/565/proto_eye_u"))
    //     .add("nose_r", bitmapManager.get("/565/proto_nose"))
    //     .add("nose_l", bitmapManager.get("/565/proto_nose"))
    //     .add("mouth_r", bitmapManager.get("/565/proto_mouth_w"))
    //     .add("mouth_l", bitmapManager.get("/565/proto_mouth_w"))
    //     .add("ear_l", bitmapManager.get("/565/proto_ear"))
    //     .add("ear_r", bitmapManager.get("/565/proto_ear"));

    // owofacesetter
    //     .add("eye_r", bitmapManager.get("/565/proto_eye_o"))
    //     .add("eye_l", bitmapManager.get("/565/proto_eye_o"))
    //     .add("nose_r", bitmapManager.get("/565/proto_nose"))
    //     .add("nose_l", bitmapManager.get("/565/proto_nose"))
    //     .add("mouth_r", bitmapManager.get("/565/proto_mouth_w"))
    //     .add("mouth_l", bitmapManager.get("/565/proto_mouth_w"))
    //     .add("ear_l", bitmapManager.get("/565/proto_ear"))
    //     .add("ear_r", bitmapManager.get("/565/proto_ear"));

    // heartfacesetter
    //     .add("eye_r", bitmapManager.get("/565/proto_eye_heart"))
    //     .add("eye_l", bitmapManager.get("/565/proto_eye_heart"))
    //     .add("nose_r", bitmapManager.get("/565/proto_nose"))
    //     .add("nose_l", bitmapManager.get("/565/proto_nose"))
    //     .add("mouth_r", bitmapManager.get("/565/proto_mouth"))
    //     .add("mouth_l", bitmapManager.get("/565/proto_mouth"))
    //     .add("ear_l", bitmapManager.get("/565/proto_ear"))
    //     .add("ear_r", bitmapManager.get("/565/proto_ear"));

    // arrowfacesetter
    //     .add("eye_r", bitmapManager.get("/565/proto_eye_arrow"))
    //     .add("eye_l", bitmapManager.get("/565/proto_eye_arrow"))
    //     .add("nose_r", bitmapManager.get("/565/proto_nose"))
    //     .add("nose_l", bitmapManager.get("/565/proto_nose"))
    //     .add("mouth_r", bitmapManager.get("/565/proto_mouth_angry"))
    //     .add("mouth_l", bitmapManager.get("/565/proto_mouth_angry"))
    //     .add("ear_l", bitmapManager.get("/565/proto_ear"))
    //     .add("ear_r", bitmapManager.get("/565/proto_ear"));

    // Setup button event handler
    auto buttonHandler = [](int button)
    {
        Serial.print("Received button: ");
        Serial.println(button);
        switch (button)
        {
        case 3:
            heartfacesetter.visit(&scene);
            break;
        case 4:
            uwufacesetter.visit(&scene);
            break;
        case 5:
            angryfacesetter.visit(&scene);
            break;
        case 6:
            owofacesetter.visit(&scene);
            break;
        case 7:
            nwnfacesetter.visit(&scene);
            break;
        case 8:
            arrowfacesetter.visit(&scene);
            break;
        case 9:
            normalfacesetter.visit(&scene);
            break;
        case 10:
            rainbowEnabled = !rainbowEnabled;
            break;
        }
    };
    etl::delegate<void(int)> buttonHandlerDelegate(buttonHandler);
    RemoteReceiver::setButtonHandler(buttonHandlerDelegate);

    // Face config
    for (uint i = 0; i < NUMBER_OF_DEVICES; i++){
        display.getPanel().setRotation(i, 1);
    }
    display.getPanel().setRotation(13, 3);
    display.getPanel().setRotation(12, 3);

    display.getPanel().setPosition(13, 12, 0);
    display.getPanel().setPosition(12, 13, 0);

    display.setBrightness(32);

    // Write settings
    display.getPanel().write();

    // Startup blink
    for (uint i = 0; i < 3; i++) {
        display.getPanel().fillScreen(1);
        display.getPanel().write();
        delay(100);
        display.getPanel().fillScreen(0);
        display.getPanel().write();
        delay(100);
    }

    // Startup scan
    for (uint i = 0; i < NUMBER_OF_DEVICES * 8; i++) {
        display.getPanel().fillScreen(0);
        display.getPanel().drawFastVLine(i, 0, 8, 1);
        display.getPanel().write();
        delay(10);
    }

    //   FastLED.addLeds((CLEDController*)&sidePanelLeft, (CRGB*)&sidePanelFramebuffer, NUM_LEDS_SIDEPANEL);
    //   FastLED.addLeds((CLEDController*)&sidePanelRight, (CRGB*)&sidePanelFramebuffer, NUM_LEDS_SIDEPANEL);

    //   fill_solid((CRGB*)&sidePanelFramebuffer, NUM_LEDS_SIDEPANEL, CRGB {255, 0, 0});
    //   FastLED.show();

    xBinarySemaphore = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(
        updateLoop,  /* Task function. */
        "Task1",     /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        1,           /* priority of the task */
        &UpdateTask, /* Task handle to keep track of created task */
        0);          /* pin task to core 1 */

    xTaskCreatePinnedToCore(
        drawLoop,     /* Task function. */
        "Task2",      /* name of task. */
        10000,        /* Stack size of task */
        NULL,         /* parameter of the task */
        1,            /* priority of the task */
        &DrawingTask, /* Task handle to keep track of created task */
        1);           /* pin task to core 1 */

    xSemaphoreGive(xBinarySemaphore);
}

unsigned long prev = 0;
void loop()
{
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
    delay(5000);
}