# Quickstart

Open this project in VSCode with platformio installed.

Depending on what display type you are using, go into the `platformio.ini` file and comment out either `-DUSE_MAX7219` or `-DUSEWS2812Matrix`. If you are using MAX7219 displays it should already be configured correctly for you.

Connect an esp32, and type `pio run -t upload && pio run -t uploadfs` in the terminal. This will build and upload the code and the filesystem image containing the expression data. 
Alternatively, you can use the platformio menu and upload the code and data from there.

