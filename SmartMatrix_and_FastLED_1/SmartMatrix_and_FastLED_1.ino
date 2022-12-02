// REQUIRES the following Arduino libraries:
// - Lien vidéo: https://youtu.be/AAcYCvuQDJY
// - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
// - SmartMatrix Library: https://github.com/pixelmatix/SmartMatrix
// - FastLED Library: https://github.com/FastLED/FastLED
// - DHT Sensor Library: https://github.com/3tawi/esp32DHT 
// - DS1307ESP Library: https://github.com/3tawi/DS1307ESP
// - RGB LED Matrix with an ESP32 - How to get started: 
// - Getting Started ESP32 : https://www.youtube.com/watch?v=9b0Txt-yF7E 
// Find All "Great Projects" Videos : https://www.youtube.com/c/GreatProjects


#define USE_ADAFRUIT_GFX_LAYERS
#include <MatrixHardware_ESP32_V0.h>  
#include <SmartMatrix.h>
#include <FastLED.h>
#include "atawi19x11c.h"
#include "GPFont.h"

#define COLOR_DEPTH 24                  // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)
const uint16_t kMatrixWidth = 128;       // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 64;      // Set to the height of your display
const uint8_t kRefreshDepth = 24;       // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SM_PANELTYPE_HUB75_64ROW_MOD32SCAN;   // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_NONE);        // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);


int co0, co1;
unsigned long currentTime  = 0;
unsigned long lastTime = 0;
unsigned long prevTime = 0;
uint16_t MyColor[92] = { 0xF800, 0xF880, 0xF900, 0xF980, 0xFA20, 0xFAA0, 0xFB20, 0xFBA0, 0xFC40, 0xFCC0, 0xFD40, 0xFDC0,
                         0xFDC0, 0xFE60, 0xFEE0, 0xFF60, 0xFFE0, 0xEFE0, 0xDFE0, 0xCFE0, 0xBFE0, 0xAFE0, 0x9FE0, 0x8FE0,
                         0x77E0, 0x67E0, 0x57E0, 0x47E0, 0x37E0, 0x27E0, 0x17E0, 0x07E0, 0x07E2, 0x07E4, 0x07E6, 0x07E8,
                         0x07EA, 0x07EC, 0x07EE, 0x07F1, 0x07F3, 0x07F5, 0x07F7, 0x07F9, 0x07FB, 0x07FD, 0x07FF, 0x077F,
                         0x06FF, 0x067F, 0x05DF, 0x055F, 0x04DF, 0x045F, 0x03BF, 0x033F, 0x02BF, 0x023F, 0x019F, 0x011F,
                         0x009F, 0x001F, 0x101F, 0x201F, 0x301F, 0x401F, 0x501F, 0x601F, 0x701F, 0x881F, 0x981F, 0xA81F,
                         0xB81F, 0xC81F, 0xD81F, 0xE81F, 0xF81F, 0xF81D, 0xF81B, 0xF819, 0xF817, 0xF815, 0xF813, 0xF811,
                         0xF80E, 0xF80C, 0xF80A, 0xF808, 0xF806, 0xF804, 0xF802, 0xF800 };

#include "Effects.h"
Effects effects;
#include "Digitalclock.h"
#include "Drawable.h"
#include "Playlist.h"
#include "Patterns.h"
Patterns patterns;

void setup() {
  Serial.begin(115200);
  matrix.addLayer(&backgroundLayer); 
  matrix.begin();
  delay(2000);
  backgroundLayer.setBrightness(255);
  backgroundLayer.enableColorCorrection(true);
  Readclock();
  effects.Setup();
  Serial.println("Effects being loaded: ");
  patterns.listPatterns();
  patterns.setPattern(0); //   // simple noise
  patterns.start(); 
  Serial.print("Starting with pattern: ");
  Serial.println(patterns.getCurrentPatternName());
  //rtc.setTime(30, 58, 19, 1, 1, 2022);  // 1th Jan 2022 19:58:30
  //rtc.DSsetdatime(); 
}

void loop()
{
    currentTime = millis();
    if ( prevTime < currentTime)
    {
      prevTime = patterns.drawFrame() + currentTime;
    }
}
