#ifndef _COLORCONVERSION_H_
#define _COLORCONVERSION_H_

#include <stdint.h>
#include <FastLED.h>

#ifdef __AVR
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #ifndef PROGMEM
  #define PROGMEM
 #endif
#endif

static const uint8_t PROGMEM
  gamma5[] = {
    0x00,0x01,0x02,0x03,0x05,0x07,0x09,0x0b,
    0x0e,0x11,0x14,0x18,0x1d,0x22,0x28,0x2e,
    0x36,0x3d,0x46,0x4f,0x59,0x64,0x6f,0x7c,
    0x89,0x97,0xa6,0xb6,0xc7,0xd9,0xeb,0xff },
  gamma6[] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x08,
    0x09,0x0a,0x0b,0x0d,0x0e,0x10,0x12,0x13,
    0x15,0x17,0x19,0x1b,0x1d,0x20,0x22,0x25,
    0x27,0x2a,0x2d,0x30,0x33,0x37,0x3a,0x3e,
    0x41,0x45,0x49,0x4d,0x52,0x56,0x5b,0x5f,
    0x64,0x69,0x6e,0x74,0x79,0x7f,0x85,0x8b,
    0x91,0x97,0x9d,0xa4,0xab,0xb2,0xb9,0xc0,
    0xc7,0xcf,0xd6,0xde,0xe6,0xee,0xf7,0xff };

// Expand 16-bit input color (Adafruit_GFX colorspace) to 24-bit (NeoPixel)
// (w/gamma adjustment)
inline constexpr uint32_t expandColor(uint16_t color) {
return ((uint32_t)pgm_read_byte(&gamma5[ color >> 11       ]) << 16) |
        ((uint32_t)pgm_read_byte(&gamma6[(color >> 5) & 0x3F]) <<  8) |
                    pgm_read_byte(&gamma5[ color       & 0x1F]);
}

// Simple conversion to 565 from 8bit per channel rgb
inline constexpr uint16_t fromRGB( uint8_t r, uint8_t g, uint8_t b) {
  return ( ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3) );
}

inline uint16_t convertCRGBto565(CRGB in) {
  return fromRGB(in.r, in.g, in.b);
}

inline void convert565toCRGB(uint16_t in, CRGB& out) {
  uint32_t tmp = expandColor(in);
  char* tmp_bytes = static_cast<char*>(static_cast<void*>(&tmp));
  out.r = tmp_bytes[2];
  out.g = tmp_bytes[0];
  out.b = tmp_bytes[1];
}



// void RGBtoHSV (byte rgb[], double hsv[]) {
//   byte xMin = rgb[0];
//   if (rgb[1]<xMin) {
//     xMin = rgb[1];
//   }
//   if (rgb[2]<xMin) {     xMin = rgb[2];   }   byte xMax = rgb[0];   if (rgb[1]>xMax) {
//     xMax = rgb[1];
//   }
//   if (rgb[2]>xMax) {
//     xMax = rgb[2];
//   }
//   hsv[2] = xMax;
 
//   byte delta = xMax-xMin;
 
//   if (xMax!=0) {
//     hsv[1] = (int)(delta)*255/xMax;
//   } else {
//     hsv[0] = 0;
//     hsv[1] = 0;
//     return;
//   }
 
//   if (rgb[0]==xMax) {
//     hsv[0] = (rgb[1]-rgb[2])*60/delta;
//   } else if (rgb[1]==xMax) {
//     hsv[0] = 120+(rgb[2]-rgb[0])*60/delta;
//   } else {
//     hsv[0] = 240+(rgb[0]-rgb[1])*60/delta;
//   }
//   if (hsv[0]<0) {
//     hsv[0] += 360;
//   }
// }

// void HSVtoRGB (double hsv[], byte rgb[]) {
//   double h = hsv[0];
//   double s = hsv[1]/255.0;
//   double v = hsv[2]/255.0;
//   double c = v*s;
//   double tmp = h/60.0;
//   double tmp2 = tmp-2*floor(tmp/2);
//   double x = c*(1-abs(tmp2-1));
//   double m = v-c;
//   double r,g,b;
//   int i = floor(tmp);

//   switch (i) {
//     case 0:
//       r = c;
//       g = x;
//       b = 0;
//       break;
//     case 1:
//       r = x;
//       g = c;
//       b = 0;
//       break;
//     case 2: 
//       r = 0;
//       g = c;
//       b = x;
//       break;
//     case 3:
//       r = 0;
//       g = x;
//       b = c;
//       break;
//     case 4:
//       r = x;
//       g = 0;
//       b = c;
//       break;
//     case 5:
//       r = c;
//       g = 0;
//       b = x;
//       break;
//   }
//   rgb[0] = constrain((int)255*(r+m),0,255);
//   rgb[1] = constrain((int)255*(g+m),0,255);
//   rgb[2] = constrain((int)255*(b+m),0,255);
// }


#endif // _COLORCONVERSION_H_