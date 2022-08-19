#ifndef ESP_NOW_REMOTERECEIVER_H
#define ESP_NOW_REMOTERECEIVER_H

#include <Arduino.h>
#include <stdint.h>
#include <esp_now.h>
#include <etl/delegate.h>

typedef struct __attribute__((packed)) {
    uint32_t button_number;
    bool button_pushed;
} esp_now_msg;


class RemoteReceiver {
private:
    static esp_now_msg myMsg;
    static uint32_t lastButtonPressed;
    static etl::delegate<void(int)> buttonEventHandler;

    static void onDataRecv (const uint8_t * mac, const uint8_t *incomingData, int len) {
        memcpy(&myMsg, incomingData, sizeof(myMsg));

        // Serial.print("Bytes received: ");
        // Serial.println(len);

        if (myMsg.button_number == 0) {
            return;
        }

        if (myMsg.button_number == lastButtonPressed) {
            return;
        }

        lastButtonPressed = myMsg.button_number;

        Serial.print("Button number: ");
        Serial.println(myMsg.button_number);
        Serial.println();

        // Call the delegate function if it is valid
        buttonEventHandler.call_if(lastButtonPressed);

    }

public:
    static void init() {
        lastButtonPressed = 0;

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK) {
            Serial.println("Error initializing ESP-NOW");
            return;
        }
        esp_now_register_recv_cb(onDataRecv);
    }

    static void setButtonHandler(etl::delegate<void(int)> handler) {
        buttonEventHandler = handler;
    }
};

esp_now_msg RemoteReceiver::myMsg;
uint32_t RemoteReceiver::lastButtonPressed;
etl::delegate<void(int)> RemoteReceiver::buttonEventHandler;

#endif