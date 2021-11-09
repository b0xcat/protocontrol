#ifndef __FACESTORAGE__
#define __FACESTORAGE__

#include <FS.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include <Arduino.h>

class FaceStorage
{
private:
    fs::FS &fs;
    size_t facesize;

public:
    FaceStorage(fs::FS &fs, size_t face_size) : fs(fs)
    {
        this->facesize = face_size;
    }

    int loadface(String facename, uint8_t *face)
    {
        if (!fs.exists(String("/f/") + facename))
        {
            Serial.println(String("Face ") + facename + " does not exist");
            return -1;
        }

        File facefile = fs.open(String("/f/") + facename, "rb");
        facefile.readBytes((char *)face, facesize);
        return 0;
    }

    bool exists(String facename) {
        return fs.exists(String("/f/") + facename);
    }

    int saveface(String facename, uint8_t *face) {
        File file = fs.open(String("/f/") + facename, "wb");
        size_t written = file.write((byte*)face, facesize);

        return written == facesize ? 0 : -1;
    }

    int newface(String facename)
    {
        String facepath = String("/f/") + facename;

        // Check if face exists
        if (fs.exists(facepath)){
            return -1;
        }
        
        uint8_t facedata[facesize];

        for (uint i = 0; i < facesize; i++)
        {
            facedata[i] = 0;
        }

        File file = fs.open(facepath, "w");
        size_t written = file.write((byte *)&facedata, sizeof(facedata));
        
        return written;
    }

    void listfaces(JsonArray &array)
    {
        File root = fs.open("/f");
        File file = root.openNextFile();
        while (file)
        {
            if (!file.isDirectory())
            {
                array.add(String(file.name()).substring(3));
                file = root.openNextFile();
            }
        }
    }
};

#endif