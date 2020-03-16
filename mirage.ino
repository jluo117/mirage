#include <FastLED.h>

#define NUM_LEDS 120
#define DATA_PIN 11
#define CLOCK_PIN 13
#define BRIGHTNESS 5

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(3000);
  Serial.println("Established USART arduino-computer connection");
  FastLED.addLeds<APA102,DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
  delay(3000); //sanity delay
  FastLED.setBrightness(BRIGHTNESS);
}
unsigned char incomingByte = 0;
int red = 225;
int green = 225;
int blue = 225;
int currColor = 0;

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    //calculateSound(incomingByte);
    FastLED.show();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }
  return c;
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  FastLED.show();
}

void calculateSound(unsigned char incomingByte) {
  if ((incomingByte < 200)) {
        FastLED.setBrightness(20);
      }
      else if ((incomingByte > 200)) {
        FastLED.setBrightness(10);
      }
}
void colorAdjust(unsigned char incomingByte) {
  if ((incomingByte < 20)) { //left
    if (currColor == 0) { //red
      red = 255;
      green = 0;
      blue = 0;
    }
    else if (currColor == 1) {//orange
      red = 225;
      green = 127;
      blue = 0;
    }
    else if (currColor == 2) { //yellow
      red = 255;
      green = 225;
      blue = 0;
    }
    else if (currColor == 3) { //green
      red = 0;
      green = 255;
      blue = 0;
    }
    else if (currColor == 4) { //blue
      red = 0;
      green =0;
      blue = 225;
    }
    else if (currColor == 5) { //purple
      red = 75;
      green = 0;
      blue = 255;
    }
    else if (currColor == 6) { //magenta
      red = 148;
      green = 0;
      blue = 211;
    }
    currColor = currColor + 1;
    currColor = currColor % 7;
  }
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = 0;
    incomingByte = Serial.read();
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC); //output microphone input
    }
    if (incomingByte != 0) {
      calculateSound(incomingByte);
      colorAdjust(incomingByte);
      //rainbowCycle(20);
      for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB(red, green, blue);
      FastLED.show();
    }
  delay(30);
}
