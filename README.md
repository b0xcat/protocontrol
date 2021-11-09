# ProtoControl

Open this project in VSCode with platformio installed.

Edit the include/wifi_credentials.h file with the SSID and password of your wifi network

Connect an esp32, and type `pio run -t uploadfs` in the terminal. This will build the code and upload it, as well as the files needed for the webserver.

Then, watch the serial console for the IP assigned to your esp32, and navigate to it in your webbrowser, e.g. http://*your.ip.here.pls*:4000/

This will open the web interface where you can draw your protogen faces!