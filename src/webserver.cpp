#include "webserver.h"

void notFound(AsyncWebServerRequest *request)
{
    Serial.print("Not found: ");
    Serial.print(request->url());
    Serial.println(request->contentType());
    request->send(404, "text/plain", "Not found");
}

WebServer::WebServer(AsyncWebServer &awebserver,
                     FaceStorage &fs, MatrixManager &mm, uint fbsize_, uint8_t *fbuf) : server(awebserver), facestorage(fs), matrixmanager(mm)
{
    // TODO: automate getting proper fbsize
    fbsize = fbsize_;
    tmp_framebuffer = new uint8_t[fbsize];
    framebuffer = fbuf;
}

void WebServer::init(uint8_t *fbuf)
{
    framebuffer = fbuf;
    // Serve static files from SPIFFS
    server.serveStatic("/", SPIFFS, "/w/").setDefaultFile("index.html");

    // vvv API Handlers vvv

    // Set face
    server.on(
        "/api/setface", HTTP_POST, [&](AsyncWebServerRequest *request) {}, NULL,
        [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            // Make sure we don't write too far, pls no h4xx our beeper boi
            if (total > fbsize)
            {
                Serial.println("ERROR fb overflow");
                request->send(500);
                return;
            }

            // Copy the incoming face data to the framebuffer
            for (size_t i = index; i < len + index; i++)
            {
                framebuffer[i] = data[i];
            }

            matrixmanager.setFrame(framebuffer);

            request->send(200);
        });

    // // Get face
    server.on("^\\/api\\/getface\\/(.+)$", HTTP_GET, [&](AsyncWebServerRequest *request) {
        String facename = request->pathArg(0);

        Serial.println("Start face request");

        facestorage.loadface(facename, tmp_framebuffer);

        Serial.println("Trying to send face");
        // request->send_P(200, "application/octet-stream", buf);
        AsyncWebServerResponse *response = request->beginResponse_P(200,
                                                                    "application/octet-stream", tmp_framebuffer, fbsize);
        request->send(response);

        Serial.println("Succesfully sent face");
    });

    server.on("^\\/api\\/downloadface\\/(.+)$", HTTP_GET, [&](AsyncWebServerRequest *request) {
        String facename = request->pathArg(0);

        if (facestorage.exists(facename))
        {
            request->send(SPIFFS, String("/f/") + facename, String(), true);
        }
        else
        {
            request->send(404);
        }
    });

    server.on(
        "/api/uploadface", HTTP_POST, [&](AsyncWebServerRequest *request) {},
        [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            // Make sure we don't write too far, pls no h4xx our beeper boi
            if (index + len > fbsize)
            {
                Serial.println("ERROR fb overflow");
                request->send(500);
                return;
            }

            // Copy the incoming face data to the framebuffer
            for (size_t i = 0; i < len; i++)
            {
                tmp_framebuffer[index + i] = data[i];
            }

            if (final)
            {
                int result = facestorage.saveface(filename, tmp_framebuffer);
                if (result == 0)
                {
                    request->send(200);
                }
                else
                {
                    request->send(500);
                }
            }

            matrixmanager.setFrame(framebuffer);

            request->send(200);
        },
        NULL);

    server.on(
        "^\\/api\\/saveface\\/(.+)$", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            String facename = request->pathArg(0);

            // Make sure we don't write too far, pls no h4xx our beeper boi
            if (total > fbsize)
            {
                Serial.println("ERROR fb overflow");
                request->send(500);
                return;
            }

            // Copy the incoming face data to the framebuffer
            for (size_t i = index; i < len + index; i++)
            {
                tmp_framebuffer[i] = data[i];
            }

            // Once we are completely done copying chunks, save to FS
            if (index + len == total)
            {
                int result = facestorage.saveface(facename, tmp_framebuffer);
                if (result == 0)
                {
                    request->send(200);
                }
                else
                {
                    request->send(500);
                }
            }
        });

    // List faces
    server.on("/api/listfaces", HTTP_GET, [&](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(4096);
        JsonArray array = doc.to<JsonArray>();

        facestorage.listfaces(array);

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(doc, *response);
        request->send(response);
    });

    // Create new face
    AsyncCallbackJsonWebHandler *newhandler = new AsyncCallbackJsonWebHandler(
        "/api/newface", [&](AsyncWebServerRequest *request, JsonVariant &json) {
            const JsonObject &jsonObj = json.as<JsonObject>();

            const String name = jsonObj["name"].as<String>();

            int result = facestorage.newface(name);
            if (result == -1)
            {
                request->send(409);
            }
            else
            {
                request->send(200);
            }
        },
        4096);
    server.addHandler(newhandler);

    server.onNotFound(notFound);

    server.begin();
}