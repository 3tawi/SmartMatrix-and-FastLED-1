/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from LedEffects Snake by Robert Atkins: https://bitbucket.org/ratkins/ledeffects/src/26ed3c51912af6fac5f1304629c7b4ab7ac8ca4b/Snake.cpp?at=default
 * Copyright (c) 2013 Robert Atkins
 *
 */

#ifndef PatternDigitalClock_H
#define PatternDigitalClock_H

class PatternDigitalClock : public Drawable {
private:
    static const byte SNAKE_LENGTH = 32;
    CRGB colors[SNAKE_LENGTH];
    uint8_t initialHue;
    enum Direction {
        UP, DOWN, LEFT, RIGHT
    };//
    struct Pixel {
        uint8_t x;
        uint8_t y;
    };
    struct Snake {
        Pixel pixels[SNAKE_LENGTH];
        Direction direction;
        void newDirection() {
            switch (direction) {
                case UP:
                case DOWN:
                    direction = random(0, 2) == 1 ? RIGHT : LEFT;
                    break;
                case LEFT:
                case RIGHT:
                    direction = random(0, 2) == 1 ? DOWN : UP;
                default:
                    break;
            }
        }
        void shuffleDown() {
            for (byte i = SNAKE_LENGTH - 1; i > 0; i--) {
                pixels[i] = pixels[i - 1];
            }
        }
        void reset() {
            direction = UP;
            for (int i = 0; i < SNAKE_LENGTH; i++) {
                pixels[i].x = 0;
                pixels[i].y = 0;
            }
        }
        void move() {
            switch (direction) {
                case UP:
                    pixels[0].y = (pixels[0].y + 1) % kMatrixHeight;
                    break;
                case LEFT:
                    pixels[0].x = (pixels[0].x + 1) % kMatrixWidth;
                    break;
                case DOWN:
                    pixels[0].y = pixels[0].y == 0 ? kMatrixHeight - 1 : pixels[0].y - 1;
                    break;
                case RIGHT:
                    pixels[0].x = pixels[0].x == 0 ? kMatrixWidth - 1 : pixels[0].x - 1;
                    break;
            }
        }
        void draw(CRGB colors[SNAKE_LENGTH]) {
            for (byte i = 0; i < SNAKE_LENGTH; i++) {
              colors[i] %= (255 - i * (255 / SNAKE_LENGTH));
              effects.Pixel(pixels[i].x, pixels[i].y, colors[i]);
            }
        }
    };
    static const int snakeCount = 16;
    Snake snakes[snakeCount];

public:
    PatternDigitalClock() {
        name = (char *)"DigitalClock";
    }

    void start() {
        for (int i = 0; i < snakeCount; i++) {
            Snake* snake = &snakes[i];
            snake->reset();
        }
    }    

    void drawsnake() {
     fill_palette(colors, SNAKE_LENGTH, initialHue++, 5, effects.MesgPalette, 255, LINEARBLEND);
     for (int i = 0; i < snakeCount; i++) {
      
      Snake* snake = &snakes[i];
      snake->shuffleDown();
      if (random(10) > 7) {
        snake->newDirection();
      }
      snake->move();
      snake->draw(colors);
    }
  }

    unsigned int drawFrame() {
      effects.ClearFrame();
      drawsnake();
      drawHMTime(25);
      drawampm(GPFont16x16, 47);
      drawMesg(textmsg, GPFont16x16, 16, 2);
      effects.ShowFrame();
      EVERY_N_SECONDS(10) {
        effects.CyclePalette(1);
      }       
        return 0;
    }
};

#endif
