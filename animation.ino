#include "graphics/animated_logo/animated_logo_1.h"
#include "graphics/animated_logo/animated_logo_2.h"
#include "graphics/animated_logo/animated_logo_3.h"
#include "graphics/animated_logo/animated_logo_4.h"
#include "graphics/animated_logo/animated_logo_5.h"
#include "graphics/animated_logo/animated_logo_6.h"
#include "graphics/animated_logo/animated_logo_7.h"
#include "graphics/animated_logo/animated_logo_8.h"
#include "graphics/animated_logo/animated_logo_9.h"
#include "graphics/animated_logo/animated_logo_10.h"
#include "graphics/animated_logo/animated_logo_11.h"
#include "graphics/animated_logo/animated_logo_12.h"
#include "graphics/animated_logo/animated_logo_13.h"
#include "graphics/animated_logo/animated_logo_14.h"
#include "graphics/animated_logo/animated_logo_15.h"
#include "graphics/animated_logo/animated_logo_16.h"

void drawFrame(int frame) {
  int x;
  int y;
  int width;
  int height;
  unsigned char *bits;
  
  switch(frame) {
    default:
      case 1: bits = animated_logo_1_bits; width = animated_logo_1_width; height = animated_logo_1_height; break;
      case 2: bits = animated_logo_2_bits; width = animated_logo_2_width; height = animated_logo_2_height; break;
      case 3: bits = animated_logo_3_bits; width = animated_logo_3_width; height = animated_logo_3_height; break;
      case 4: bits = animated_logo_4_bits; width = animated_logo_4_width; height = animated_logo_4_height; break;
      case 5: bits = animated_logo_5_bits; width = animated_logo_5_width; height = animated_logo_5_height; break;
      case 6: bits = animated_logo_6_bits; width = animated_logo_6_width; height = animated_logo_6_height; break;
      case 7: bits = animated_logo_7_bits; width = animated_logo_7_width; height = animated_logo_7_height; break;
      case 8: bits = animated_logo_8_bits; width = animated_logo_8_width; height = animated_logo_8_height; break;
      case 9: bits = animated_logo_9_bits; width = animated_logo_9_width; height = animated_logo_9_height; break;
      case 10: bits = animated_logo_10_bits; width = animated_logo_10_width; height = animated_logo_10_height; break;
      case 11: bits = animated_logo_11_bits; width = animated_logo_11_width; height = animated_logo_11_height; break;
      case 12: bits = animated_logo_12_bits; width = animated_logo_12_width; height = animated_logo_12_height; break;
      case 13: bits = animated_logo_13_bits; width = animated_logo_13_width; height = animated_logo_13_height; break;
      case 14: bits = animated_logo_14_bits; width = animated_logo_14_width; height = animated_logo_14_height; break;
      case 15: bits = animated_logo_15_bits; width = animated_logo_15_width; height = animated_logo_15_height; break;
      case 16: bits = animated_logo_16_bits; width = animated_logo_16_width; height = animated_logo_16_height; break;
  }
  
  x = 128 - width / 2;
  y = 32 - height / 2;
  u8g2.clearBuffer();
  u8g2.drawXBM(x, y, width, height, bits);
  u8g2.sendBuffer();
}

void drawBlackFrame() {
  u8g2.clearBuffer();
  u8g2.sendBuffer();  
}

void drawLogoIn(int frame_delay_ms=0) {
  for (int i=6; i<=16; i++) {
    drawFrame(i);
    delay(frame_delay_ms);
  }
}

void drawLogoOut(int frame_delay_ms=0) {
  for (int i=16; i>=6; i--) {
    drawFrame(i);
    delay(frame_delay_ms);
  }
}

void drawTextIn(int frame_delay_ms=0) {
  for (int i=5; i>=1; i--) {
    drawFrame(i);
    delay(frame_delay_ms);
  }
}

void drawTextOut(int frame_delay_ms=0) {
  for (int i=1; i<=5; i++) {
    drawFrame(i);
    delay(frame_delay_ms);
  }
}

void splashAnimation() {
  int frame_delay_ms = 5;   // milliseconds to pause between frames
  int logo_pause_ms = 2000; // milliseconds to pause on logo 
  int text_pause_ms = 2000; // milliseconds to pause on text
  int ending_pause_ms = 1000;  // milliseconds to pause on end (black frame)
  
  u8g2.setFlipMode(1);

  drawBlackFrame();
  delay(200);

  drawLogoIn(frame_delay_ms);
  delay(logo_pause_ms);

  drawLogoOut(frame_delay_ms);
  drawBlackFrame();
  delay(frame_delay_ms);

  drawTextIn(frame_delay_ms*2);
  delay(text_pause_ms);

  // JP text flips out [1 to 5]
  drawTextOut(frame_delay_ms*2);
  drawBlackFrame();
  delay(ending_pause_ms);
}
