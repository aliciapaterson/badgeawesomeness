#include "config.h"
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


#define PIN 14
int totalPixels = 43;
int pixelCnt = 0;
int ring1End = 19;
int command;
int currentMode = 0;
int loopCnt = 0;
int ringLoop=0;
int pixelColor = 0;
bool wipeOn = true;
int ring2start = 1;
int ring3start = 7;
int ring4start = 19;
int ring = 0;
uint32_t color;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring1 = Adafruit_NeoPixel(19, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring2 = Adafruit_NeoPixel(24, PIN, NEO_GRBW + NEO_KHZ800);

AdafruitIO_Feed *modeFeed  = io.feed("badgeawesome.mode");
AdafruitIO_Feed *colorFeed = io.feed("badgeawesome.color");

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();  
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
    // we are connected
  Serial.println();
  Serial.println(io.statusText());

  modeFeed->onMessage(setMode);
  colorFeed->onMessage(setColor);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  ring1.begin();
  ring1.show(); // Initialize all pixels to 'off'
  ring2.begin();
  ring2.show(); // Initialize all pixels to 'off' 

  ring1.setBrightness(100);
  ring2.setBrightness(100);  

  color = strip.Color(0,255,0); //set to green by default
}

void loop() {

  io.run();
  
  switch (currentMode) {
    case 1:
      colorWipe(color, 0); 
      break;

    case 2:
      theaterChase(color, 0);
      break;

    case 3:
      oneringtobringthemall(color, 10000);
      break;

    case 4:
      theaterChaseRainbow(0);
      break; 

    case 5:
      rainbowCycle(0);
      break; 

    case 6:
      rainbow(0);
      break; 
      
    default:
      ringsoff();
      break;
  }
}

void oneringtobringthemall(uint32_t c, uint8_t wait) {
  uint16_t i;
  if (ring>3) {
    ring=0;
  }
  if (ring==3) {
    ringsoff();
    for (i=0; i<(totalPixels-ring1End); i++) {
      ring2.setPixelColor(i,c);
    }
  }
  if (ring==2) {
    ringsoff();
    for (i=ring3start; i<ring4start; i++) {
      ring1.setPixelColor(i,c);
    }
  }
  if (ring==1) {
    ringsoff();
    for (i=ring2start; i<ring3start; i++) {
      ring1.setPixelColor(i,c);
    }
  }
  if (ring==0) {
    //turn them all off
    ringsoff();
    //turn the middle one on
    ring1.setPixelColor(0, c);
  }

  ring1.show();
  ring2.show();
  ring++;
  delay(wait);
}

void ringsoff() {
  uint16_t i,j =0;
  for (i=0; i<totalPixels; i++) {
    if (i<ring1End) {
      ring1.setPixelColor(i,0);
    } else {
      ring2.setPixelColor(j,0);
      j++;
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  if (loopCnt==totalPixels) {
    wipeOn = false;
  } else if (loopCnt<0) {
    wipeOn = true;
  }
  if (wipeOn) {
    if (loopCnt<ring1End) {
      ring1.setPixelColor(loopCnt, c);
      pixelCnt = 0;
    } else {
      ring2.setPixelColor(pixelCnt, c);
      pixelCnt++;
    }
  } else {
    if (loopCnt<ring1End) {
      ring1.setPixelColor(loopCnt, 0);
      pixelCnt = 24;
    } else {
      ring2.setPixelColor(pixelCnt, 0);
      pixelCnt--;
    }
  }
  ring1.show();
  ring2.show();
  if (wipeOn) {
    loopCnt++;
  } else {
    loopCnt--;
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  if (pixelColor>256) {
    pixelColor=0;
  }
    for(i=0; i<totalPixels; i++) {
      if (i<ring1End) {
        ring1.setPixelColor(i, Wheel((i+pixelColor) & 255));
        pixelCnt=0;
      } else {
        ring2.setPixelColor(pixelCnt, Wheel((i+pixelColor) & 255));
        pixelCnt++;
      }
    }
    ring1.show();
    ring2.show();
    delay(wait);
  pixelColor++;
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< totalPixels; i++) {
      if (i<ring1End) {
        ring1.setPixelColor(i, Wheel(((i * 256 / totalPixels) + j) & 255));
        pixelCnt=0;
      } else {
        ring2.setPixelColor(pixelCnt, Wheel(((i * 256 / totalPixels) + j) & 255));
        pixelCnt++;
      }
    }
    ring1.show();
    ring2.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < totalPixels; i=i+3) {
        if (i<ring1End) {
          ring1.setPixelColor(i+q, c);    //turn every third pixel on
          pixelCnt=0;
        } else {
          ring2.setPixelColor(pixelCnt+q, c);    //turn every third pixel on
          pixelCnt+=3;
        }
      }
      ring1.show();
      ring2.show();

      delay(wait);

      for (uint16_t i=0; i < totalPixels; i=i+3) {
        if (i<ring1End) {
          ring1.setPixelColor(i+q, 0);        //turn every third pixel off
          pixelCnt=0;
        } else {
          ring2.setPixelColor(pixelCnt+q, 0);        //turn every third pixel off
          pixelCnt+=3;
        }
      }
    }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  if (pixelColor>256) {
    pixelColor=0;
  }
  if (ringLoop>3) {
    ringLoop = 0;
    pixelColor++;
  }
  for (uint16_t i=0; i < totalPixels; i=i+3) {
    if (i<ring1End) {
      ring1.setPixelColor(i+ringLoop, Wheel( (i+pixelColor) % 255));    //turn every third pixel on
      pixelCnt=0;
    } else {
      ring2.setPixelColor(pixelCnt+ringLoop, Wheel( (i+pixelColor) % 255));    //turn every third pixel on
      pixelCnt+=3;
    }
  }
  ring1.show();
  ring2.show();
  delay(wait);

  for (uint16_t i=0; i < totalPixels; i=i+3) {
    if (i<ring1End) {
      ring1.setPixelColor(i+ringLoop, 0);        //turn every third pixel off
      pixelCnt=0;
    } else {
      ring2.setPixelColor(pixelCnt+ringLoop, 0);        //turn every third pixel off
      pixelCnt+=3;
    }
  }
  ringLoop++;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setMode(AdafruitIO_Data *data) {
  command = data->toInt();
   //action commands
  switch (command) {
    case 1:
      //pattern 1
      loopCnt=0;
      pixelColor=0;
      currentMode=1;
      break;

    case 2:
      //pattern 2
      loopCnt=0;
      pixelColor=0;
      currentMode=2;
      break;

    case 3:
      //pattern 3
      loopCnt=0;
      pixelColor=0;
      currentMode=3;
      break;

    case 4:
      //pattern 4
      loopCnt=0;
      pixelColor=0;
      currentMode=4;
      break;

    case 5:
      //pattern 5
      loopCnt=0;
      pixelColor=0;
      currentMode=5;
      break;
      
    case 6:
      //pattern 6
      loopCnt=0;
      pixelColor=0;
      currentMode=6;
      break;
  }
}

void setColor(AdafruitIO_Data *data) {
  int r,g,b;
  // print RGB values and hex value
  Serial.println("Received:");  
  Serial.print("  - R: ");
  Serial.println(data->toRed());
  Serial.print("  - G: ");
  Serial.println(data->toGreen());
  Serial.print("  - B: ");
  Serial.println(data->toBlue());
  Serial.print("  - HEX: ");
  Serial.println(data->value());

  r = data->toRed();
  g = data->toGreen();
  b = data->toBlue();
  color = strip.Color(r, g, b);
  
}
