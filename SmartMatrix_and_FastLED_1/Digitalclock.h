/*
 * Adapted by 3tawi
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef Digitalclock_H
#define Digitalclock_H

#include <DS1307ESPTime.h>
#include <esp32DHT.h>
DS1307ESPTime rtc;
DHT11 sensor;

String Message = "Congratulations on the wind and the wonderful Morocco's qualification to the round of 16 of the World Cup.";
bool flasher = false;
bool flashSe = false;
bool Mode24h = false;
int NewRTCm = 60;
int xps = 64, msg = 0;
float Temp, Humi;
float NewTemp, NewHumi;
String textmsg, textip, text;
String datee, datetxt, ddate;


void Readclock() {
  sensor.setup(13);  // pin 13 is DATA
  sensor.onData([](float humidity, float temperature) {
  NewHumi = humidity;
  NewTemp = temperature;
  }); 
  rtc.DSbegin();
  rtc.DSgetdatime();
  setenv("TZ", "UTC-1", 1);
  tzset();
}

void getmesg() {
  xps = 128;
  switch(msg) {
    case 0 :
        textmsg = Message;
        msg++;
        break;
    case 1 :
        textmsg = "Temperature : ";
        textmsg += Temp;
        textmsg += (char)127;
        msg++;
        break;
    case 2 :
        textmsg = "Humidity : ";
        textmsg += Humi;
        textmsg += "H%";
        msg++;
        break;
    case 3 :
        textmsg = rtc.getDate(true);
        msg = 0;
        break;
    case 4 :
        textmsg = "Time Saved";
        msg = 3;
        break;
    }
}

void redtemphumi() {
  sensor.read();
  if (Temp != NewTemp || Humi != NewHumi) {
    Temp = NewTemp;
    Humi = NewHumi;
  }
}

void readtime() {
  if(millis() - lastTime >= 1000) {
    flasher = !flasher;
    co0 = random(0, 91);
    lastTime = millis();
  }
  if (NewRTCm != rtc.getMinute()) {
    redtemphumi();
    flashSe = !flashSe;
    co1 = random(0, 91);
    NewRTCm = rtc.getMinute();
  }
}

void drawMesg(String text, uint8_t* font_gp, int len, int ym) {
  uint16_t text_length = text.length();
  effects.setFont(font_gp); 
  effects.setCursor(xps, ym);
  effects.printStr(text);
  xps -= 1;
  if (xps < -(text_length * len)) {
    getmesg();
  }
}

void drawMesg1(String text, uint8_t* font_gp, int len, int ym) {
  uint16_t text_length = text.length();
  effects.setFont(font_gp); 
  effects.setCursor(xps, ym);
  effects.printStr(text, co1);
  xps -= 1;
  if (xps < -(text_length * len)) {
    xps = 128;
  }
}

void drawampm(uint8_t* font_gp, int ym) {
  effects.setFont(font_gp); 
  effects.setCursor(96, ym);
  String textAmPm = Mode24h ? " " : rtc.getAmPm();
  effects.printStr(textAmPm, co1+6);
}
    
void drawHMTime(int yy) {
  readtime();
  String text1 = flasher ? rtc.getTime("%H %M") : rtc.getTime("%H:%M");
  String text2 = flasher ? rtc.getTime("%I %M") : rtc.getTime("%I:%M");
  String text = Mode24h ? text1 : text2;
  backgroundLayer.setFont(&atawi19x11c);
  backgroundLayer.setTextSize(2);
  backgroundLayer.setCursor(0, yy);
  effects.print(text, co1);
  text = rtc.getTime("%S");
  backgroundLayer.setTextSize(1);
  backgroundLayer.setCursor(104, yy);
  effects.print(text, co0);
  int xx = rtc.getSecond() * 2;
  xx = flashSe ? xx+4 : 123-xx;
  effects.fillEllipse(xx, yy-4, 4, 2, MyColor[co0]);
    }

#endif
