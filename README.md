# Readme

Here's protocontrol! It is a firmware for the esp32 for displaying and animating faces (or other things) on several types of led matrices in fursuits. 

<!-- Picture!! -->

It has several convenient features to make life easy:
- Define bitmaps with just png files (see `util/imageconverter/in), they will be converted for use on the esp32 automagically!
- Supports WS2812 or Max7219 led matrices
- Automatic transition animations when switching expressions
- Automatic blinking animations

<!-- Here it is in action -->

<!-- Video!! -->
<!-- [Max7219 video] [RGB video] -->

<!-- - get-started
- tweet me
- store link!!
- reference docs
- deep dive docs -->

# Get Started
To get started, if you have worked with platformio before, see the instructions in the next section. If you need more detailed instructions, please read on!

## Hardware
Coming soon

## Quickstart

Open this project in VSCode with platformio installed.

Depending on what display type you are using, go into the `platformio.ini` file and comment out either `-DUSE_MAX7219` or `-DUSEWS2812Matrix`. If you are using MAX7219 displays it should already be configured correctly for you.

Connect an esp32, and type `pio run -t upload && pio run -t uploadfs` in the terminal. This will build and upload the code and the filesystem image containing the expression data. 
Alternatively, you can use the platformio menu and upload the code and data from there.

<!-- [Kicad Diagram] -->

## Detailed instructions
(TODO)
1. Install [vscode](https://code.visualstudio.com/) and [platformio](https://platformio.org/)
2. Download [the code](https://github.com/b0xcat/protocontrol/archive/refs/heads/main.zip) from this repository
3. Open the project in vscode <!-- Expand on this -->
4. From the platformio menu on the left:
    1. Run `Upload Filesystem Image`
    2. Run `Upload`

<!-- [Fritzing diagram] -->



# Deep Dive

Check back here later for a more detailed explanation on how the code works 
