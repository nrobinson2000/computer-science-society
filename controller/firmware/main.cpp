#include "neopixel/neopixel.h"
#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D0
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

STARTUP(WiFi.selectAntenna(ANT_AUTO)); // continually switches at high speed between antennas

SerialLogHandler logHandler(LOG_LEVEL_ALL);

// Prototypes for local build, ok to leave in for Build IDE
void rainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);

int setColor(String command)
{
        Serial.println(command);

        String redString = command.substring(0,3);
        Serial.println(redString);
        int red = redString.toInt();

        String greenString = command.substring(4,7);
        Serial.println(greenString);
        int green = greenString.toInt();

        String blueString = command.substring(8,11);
        Serial.println(blueString);
        int blue = blueString.toInt();

        for (int i=0; i<strip.numPixels(); i++)
        {
                strip.setPixelColor(i, strip.Color(red,green,blue));
        }
        strip.show();

        return 0;
}


void setup() // Put setup code here to run once
{
        strip.begin();
        strip.show(); // Initialize all pixels to 'off'
        Serial.begin(115200);

        pinMode(D7, OUTPUT);

        Particle.function("setColor", setColor);

        Particle.connect();
}


void breathe()
{
        for (int j=0; j<=100; j++)
        {
                for (int i=0; i<strip.numPixels(); i++)
                {
                        strip.setPixelColor(i, strip.Color(j,j,j));
                }
                strip.show();
                delay(10);
        }
        delay(200);

        for (int j=100; j>=0; j--)
        {
                for (int i=0; i<strip.numPixels(); i++)
                {
                        strip.setPixelColor(i, strip.Color(j,j,j));
                }
                strip.show();
                delay(10);
        }
        delay(200);
}

void strobe()
{
        for (int i=0; i<strip.numPixels(); i++)
        {
                strip.setPixelColor(i, strip.Color(0,0,0));
        }
        strip.show();

        delay(20);

        for (int i=0; i<strip.numPixels(); i++)
        {
                strip.setPixelColor(i, strip.Color(100,100,100));
        }
        strip.show();

        delay(20);
}

void circle()
{

for (int k=0; k<256; k++)
{

  for (int i=0; i<strip.numPixels(); i++)
  {
          strip.setPixelColor(i, Wheel((i+k) & 255));

          if (i == 14)
          {
            strip.setPixelColor(i+1, Wheel((i+k) & 255));
            strip.setPixelColor(0, Wheel((i+k) & 255));
          }
          else if (i == 15)
          {
            strip.setPixelColor(0, Wheel((i+k) & 255));
            strip.setPixelColor(1, Wheel((i+k) & 255));
          }
          else
          {
            strip.setPixelColor(i+1, Wheel((i+k) & 255));
            strip.setPixelColor(i+2, Wheel((i+k) & 255));
          }

          if (i == 0){
            strip.setPixelColor(strip.numPixels()-1, strip.Color(0,0,0));}
          else {
            strip.setPixelColor(i-1, strip.Color(0,0,0));}

          strip.show();
          delay(45);
  }

}
}

void loop() // Put code here to loop forever
{
        // strobe();

        // breathe();

        // circle();
}


void rainbow(uint8_t wait) {
        uint16_t i, j;

        for(j=0; j<256; j++) {
                for(i=0; i<strip.numPixels(); i++) {
                        strip.setPixelColor(i, Wheel((i+j) & 255));
                }
                strip.show();
                delay(wait);
        }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
        if(WheelPos < 85) {
                return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        } else if(WheelPos < 170) {
                WheelPos -= 85;
                return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
        } else {
                WheelPos -= 170;
                return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
}
