
/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from "Funky Clouds" by Stefan Petrick: https://gist.github.com/anonymous/876f908333cd95315c35
 * Portions of this code are adapted from "NoiseSmearing" by Stefan Petrick: https://gist.github.com/StefanPetrick/9ee2f677dbff64e3ba7a
 * Copyright (c) 2014 Stefan Petrick
 * http://www.stefan-petrick.de/wordpress_beta
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef Effects_H
#define Effects_H

/* ---------------------------- GLOBAL CONSTANTS ----------------------------- */

#define fontbyte(x) Font[x] 

#define SWAPint(X,Y) { \
        int temp = X ; \
        X = Y ; \
        Y = temp ; \
    }
    
uint16_t XY( uint8_t x, uint8_t y) 
{
    if( x >= kMatrixWidth || x < 0) return 0;
    if( y >= kMatrixHeight || y < 0) return 0;  
  
    return (y * kMatrixWidth) + x; // everything offset by one to capute out of bounds stuff - never displayed by ShowFrame()
    
}

const int  CENTER_X = 64;
const int  CENTER_Y = 32;
const uint16_t NUM_LEDS = kMatrixWidth * kMatrixHeight; // one led spare to capture out of bounds

    

uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatcos = cos8(beat + phase_offset);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapsin8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatsin = sin8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatsin, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapcos8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatcos = cos8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}


uint32_t noise_x;
uint32_t noise_y;
uint32_t noise_z;
uint32_t noise_scale_x;
uint32_t noise_scale_y;
uint8_t noise;

uint8_t noisesmoothing;

class Effects {
public:
  
  /* The only 'framebuffer' we have is what is contained in the leds and leds2 variables.
   * We don't store what the color a particular pixel might be, other than when it's turned
   * into raw electrical signal output gobbly-gook (i.e. the DMA matrix buffer), but this * is not reversible.
   * 
   * As such, any time these effects want to write a pixel color, we first have to update
   * the leds or leds2 array, and THEN write it to the RGB panel. This enables us to 'look up' the array to see what a pixel color was previously, each drawFrame().
   */
  void drawRGBPixel(int16_t x, int16_t y, uint8_t colorIndex) {
    if( x >= kMatrixWidth || x < 0) return;
    if( y >= kMatrixHeight || y < 0) return;  
    rgb24 temp = ColorFromMesgPalette(colorIndex);
    backgroundLayer.drawPixel(x, y, temp);
  }
  
  // write one pixel with the specified color from the current palette to coordinates
  void Pixel(int x, int y, rgb24 color) {
    if( x >= kMatrixWidth || x < 0) return;
    if( y >= kMatrixHeight || y < 0) return;  
    backgroundLayer.drawPixel(x, y, color);
  }
  
  // write one pixel with the specified color from the current palette to coordinates
  void drawLine(int x0, int y0, int x1, int y1, uint8_t colorIndex) {
    rgb24 temp = ColorFromMesgPalette(colorIndex);
    backgroundLayer.drawLine(x0, y0, x1, y1, temp);
  }
  // write one pixel with the specified color from the current palette to coordinates
  void drawLine(int x0, int y0, int x1, int y1, rgb24 color) {
    backgroundLayer.drawLine(x0, y0, x1, y1, color);
  }
  
 void PrepareFrame() {
   // leds = (CRGB*) backgroundLayer.backBuffer();
  }

  void ShowFrame() {
    backgroundLayer.swapBuffers();
  }

  // scale the brightness of the screenbuffer down
  void DimAll(byte value)
  {
  rgb24 *buffer = backgroundLayer.backBuffer();
  for (int i = 0; i < NUM_LEDS; i++) {
    CRGB color = CRGB(buffer[i].red, buffer[i].green, buffer[i].blue);
    color.nscale8(value);
    buffer[i] = (rgb24)color;
  }
  }  

  void ClearFrame()
  {
    backgroundLayer.fillScreen({0, 0, 0});
  }

  // palettes
  static const int paletteCount = 12;
  int paletteIndex = -1;
  TBlendType currentBlendType = NOBLEND;
  CRGBPalette16 MesgPalette;

  void Setup() {
    RandomPalette();
    NoiseVariablesSetup();
  }

  void CyclePalette(int offset = 1) {
    paletteIndex += offset;
    if (paletteIndex >= paletteCount)
      paletteIndex = 0;
    else if (paletteIndex < 0)
      paletteIndex = paletteCount - 1;
      setupMesgPalette(paletteIndex);
  }

  void RandomPalette() {
    setupMesgPalette(random(0, paletteCount - 1));
  }

  void setupMesgPalette(int Coun) {
   if( Coun == 0) MesgPalette = CRGBPalette16(CRGB::Red, CRGB::Green, CRGB::Green, CRGB::Red);
   if( Coun == 1) MesgPalette = CRGBPalette16(CRGB::Red, CRGB::Yellow, CRGB::DeepPink, CRGB::White);
   if( Coun == 2) MesgPalette = CRGBPalette16(CRGB::DeepPink, CRGB::Blue, CRGB::Violet, CRGB::White);
   if( Coun == 3) MesgPalette = CRGBPalette16(CRGB::Red, CRGB::GreenYellow, CRGB::Gold, CRGB::White);
   if( Coun == 4) MesgPalette = CRGBPalette16(CRGB::Orange, CRGB::DeepPink, CRGB::Aqua, CRGB::White);
   if( Coun == 5) MesgPalette = CRGBPalette16(CRGB::Red, CRGB::Purple, CRGB::Navy, CRGB::GreenYellow);
   if( Coun == 6) MesgPalette = CRGBPalette16(CRGB::Aqua, CRGB::Purple, CRGB::DeepPink, CRGB::Red);
   if( Coun == 7) MesgPalette = CRGBPalette16(CRGB::Red, CRGB::DeepPink, CRGB::Gold, CRGB::White);
   if( Coun == 8) MesgPalette = CRGBPalette16(CRGB::Violet, CRGB::Yellow, CRGB::Gold, CRGB::White);
   if( Coun == 9) MesgPalette = CRGBPalette16(CRGB::DeepPink, CRGB::Red, CRGB::Purple, CRGB::White);
   if( Coun == 10) MesgPalette = CRGBPalette16(CRGB::Red,  CRGB::Crimson,  CRGB::Orange,  CRGB::DarkOrange, CRGB::Yellow, CRGB::Gold, CRGB::Green,  CRGB::GreenYellow,
                                               CRGB::Lime,  CRGB::Blue,  CRGB::Aqua,  CRGB::DarkTurquoise, CRGB::Navy, CRGB::Purple, CRGB::Violet,  CRGB::DeepPink );
  }

  CRGB ColorFromMesgPalette(uint8_t index = 0, uint8_t brightness = 255) {
    return ColorFromPalette(MesgPalette, index, brightness, currentBlendType);
  }
  // Oscillators and Emitters

  // the oscillators: linear ramps 0-255
  byte osci[6];

  // sin8(osci) swinging between 0 to MATRIX_WIDTH - 1
  byte p[6];

  // set the speeds (and by that ratios) of the oscillators here
  void MoveOscillators() {
    osci[0] = osci[0] + 5;
    osci[1] = osci[1] + 2;
    osci[2] = osci[2] + 3;
    osci[3] = osci[3] + 4;
    osci[4] = osci[4] + 1;
    if (osci[4] % 2 == 0)
      osci[5] = osci[5] + 1; // .5
    for (int i = 0; i < 4; i++) {
      p[i] = map8(sin8(osci[i]), 0, kMatrixWidth - 1); //why? to keep the result in the range of 0-MATRIX_WIDTH (matrix size)
    }
  }

 
  // All the caleidoscope functions work directly within the screenbuffer (leds array).
  // Draw whatever you like in the area x(0-15) and y (0-15) and then copy it arround.

  // rotates the first 16x16 quadrant 3 times onto a 32x32 (+90 degrees rotation for each one)
  void Caleidoscope1() {
    for (int x = 0; x < kMatrixWidth/2; x++) {
      for (int y = 0; y < kMatrixHeight/2; y++) {
        rgb24 temp = backgroundLayer.readPixel(x, y);
        uint16_t xy = XY(x, y);
        backgroundLayer.drawPixel(kMatrixWidth - 1 - x, y, temp);
        backgroundLayer.drawPixel(x, kMatrixHeight - 1 - y, temp);
        backgroundLayer.drawPixel(kMatrixWidth - 1 - x, kMatrixHeight - 1 - y, temp);
      }
    }
  }


  // mirror the first 16x16 quadrant 3 times onto a 32x32
  void Caleidoscope2() {
    for (int x = 0; x < kMatrixWidth/2; x++) {
      for (int y = 0; y < kMatrixHeight; y++) {
        rgb24 temp = backgroundLayer.readPixel(x, y);
        backgroundLayer.drawPixel(kMatrixWidth - 1 - x, y, temp);
      }
    }
  }

  // copy one diagonal triangle into the other one within a 16x16
  void Caleidoscope3() {
    for (int x = 0; x <= kMatrixWidth/4; x++) {
      for (int y = 0; y <= x/2; y++) {
        rgb24 temp = backgroundLayer.readPixel(y, x);
        backgroundLayer.drawPixel(x, y, temp);
      }
    }
  }

  CRGB HsvToRgb(uint8_t h, uint8_t s, uint8_t v) {
    CHSV hsv = CHSV(h, s, v);
    CRGB rgb;
    hsv2rgb_spectrum(hsv, rgb);
    return rgb;
  }
  
  void NoiseVariablesSetup() {
    noisesmoothing = 200;
    noise_x = random16();
    noise_y = random16();
    noise_z = random16();
    noise_scale_x = 8000;
    noise_scale_y = 6000;
  }


void xLine(int x0, int x1, int y, int16_t color)
  {
    if (x0 > x1)
    {
      int xb = x0;
      x0 = x1;
      x1 = xb;
    }
    if (x0 < 0)
      x0 = 0;
    if (x1 > kMatrixWidth)
      x1 = kMatrixWidth;
    for (int x = x0; x < x1; x++)
    backgroundLayer.drawPixel(x, y, color);
  }

void fillEllipse(int x, int y, int rx, int ry, int16_t color) {
    if(ry == 0)
      return;
    float f = float(rx) / ry;
    f *= f;   
    for(int i = 0; i < ry + 1; i++)
    {
      float s = rx * rx - i * i * f;
      int xr = (int)sqrt(s <= 0 ? 0 : s);
      xLine(x - xr, x + xr + 1, y + i, color);
      if(i) 
        xLine(x - xr, x + xr + 1, y - i, color);
    }
  }
  
void print(String st, int c)
{
  char buf[st.length()+1];
  st.toCharArray(buf, st.length()+1);
  printhue(buf, c);
}

void printhue(char *st, int c)
{
  int stl = strlen(st);
  for (int cnt=0; cnt<stl; cnt++){
    backgroundLayer.setTextColor(MyColor[c % 92]);  
    backgroundLayer.print(*st++);
    c += 20;
  }
}
  
void printStr(String st, int tc)
{
  char buf[st.length()+1];
  st.toCharArray(buf, st.length()+1);
  printStr(buf, tc);
}

void printStr(char *st, int rc)
{
  int x = cursor_x;
  int y = cursor_y;
  int stl = strlen(st);
  for (int cnt=0; cnt<stl; cnt++){
    int16_t color = MyColor[rc % 92];
    printchar(*st++, x + (cnt*(x_size)), y, color);
    rc += 6;
  }
}

void printchar(unsigned char c, int x, int y, int16_t color)
{
   int font_idx = ((c - offset)*((x_size*y_size/8)))+4;
    int cbyte=fontbyte(font_idx);
    int cbit=7;
    for (int cy=0; cy<y_size; cy++)
    {
      for (int cx=0; cx<x_size; cx++)
      {
        if ((cbyte & (1<<cbit)) != 0)
          backgroundLayer.drawPixel(x+cx, y+cy, color);
        cbit--;
        if (cbit<0)
        {
          cbit=7;
          font_idx++;
          cbyte=fontbyte(font_idx);
        }
      }
    }
}
  
void printStr(String st)
{
  char buf[st.length()+1];
  st.toCharArray(buf, st.length()+1);
  printStr(buf);
}

void printStr(char *st)
{
  int x = cursor_x;
  int y = cursor_y;
  int stl = strlen(st);
  for (int cnt=0; cnt<stl; cnt++){
    printchar(*st++, x + (cnt*(x_size)), y);
  }
}

void printchar(unsigned char c, int x, int y)
{
   int font_idx = ((c - offset)*((x_size*y_size/8)))+4;
    int cbyte=fontbyte(font_idx);
    int cbit=7;
    for (int cy=0; cy<y_size; cy++)
    {
      for (int cx=0; cx<x_size; cx++)
      {
        if ((cbyte & (1<<cbit)) != 0)
          printStrhue(x+cx, y+cy);
        cbit--;
        if (cbit<0)
        {
          cbit=7;
          font_idx++;
          cbyte=fontbyte(font_idx);
        }
      }
    }
}
  
void printStrhue(int x, int y)
{
  uint32_t ioffset = noise_scale_x * (x - kMatrixWidth);
  uint32_t joffset = noise_scale_y * (y - kMatrixHeight);
  byte data = inoise16(noise_x + ioffset, noise_y + joffset, noise_z) >> 8;
  uint8_t olddata = noise;
  uint8_t newdata = scale8(olddata, noisesmoothing) + scale8(data, 256 - noisesmoothing);
  data = newdata;
  noise = data;
  rgb24 pixel = ColorFromMesgPalette(255 - 4 * noise, olddata);
  backgroundLayer.drawPixel(x, y, pixel);
}

void setFont(uint8_t* font_c)
{
  Font=font_c;
  x_size=fontbyte(0);
  y_size=fontbyte(1);
  offset=fontbyte(2);
  numchars=fontbyte(3);
}

void setTextColor(int16_t color) {
  textcolor = color;
}

void setCursor(int16_t x, int16_t y) 
{
    cursor_x = x;
    cursor_y = y;
}  

protected:
  uint8_t* Font;
  uint8_t x_size;
  uint8_t y_size;
  uint8_t offset;
  uint8_t numchars;
  int16_t cursor_x;     ///< x location to start print()ing text
  int16_t cursor_y;     ///< y location to start print()ing text
  int16_t textcolor;   ///< 16-bit background color for print()

};

#endif
