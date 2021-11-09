#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "matrix.h"
//#include "layermanager.h"
#include "matrixmanager.h"
#include "facestorage.h"

#include <FS.h>
#include <SPIFFS.h>

class WebServer {

private: 

    AsyncWebServer &server;
    FaceStorage &facestorage;
    //LayerManager &layermanager;
    MatrixManager &matrixmanager;

    // External framebuffer, contains what is displayed
    uint8_t *framebuffer;

    // Internal framebuffer, owned by server, do whatever you want with it
    uint8_t *tmp_framebuffer;

    uint fbsize;

public:

    WebServer(AsyncWebServer &awebserver,
        FaceStorage &fs, MatrixManager &mm,
        uint fbsize, uint8_t *fbuf);

    void init(uint8_t *fbuf);

};

#endif