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

int red, green, blue;
int lastRed, lastGreen, lastBlue;

int setColor(String command) // dim to off and then dim to new color
{
        lastRed = red;
        lastGreen = green;
        lastBlue = blue;

        int fadeRed, fadeGreen, fadeBlue;

        String redString = command.substring(0,3);
        red = redString.toInt();

        String greenString = command.substring(4,7);
        green = greenString.toInt();

        String blueString = command.substring(8,11);
        blue = blueString.toInt();

        for (int i = 0; i <=  max(max(lastRed,lastGreen),lastBlue); i++)
        {
                fadeRed = constrain(lastRed - i, 0, 255);
                fadeGreen = constrain(lastGreen - i, 0, 255);
                fadeBlue = constrain(lastBlue - i, 0, 255);

                for (int i=0; i<strip.numPixels(); i++)
                {
                        strip.setPixelColor(i, fadeRed, fadeGreen, fadeBlue);
                }
                strip.show();
                delay(5);
        }

        for (int i = max(max(red,green),blue); i>=0; i--)
        {

                fadeRed = constrain(red - i, 0, red);
                fadeGreen = constrain(green - i, 0, green);
                fadeBlue = constrain(blue - i, 0, blue);

                for (int i=0; i<strip.numPixels(); i++) // fade in
                {
                        strip.setPixelColor(i, fadeRed, fadeGreen, fadeBlue);
                }
                strip.show();
                delay(5);
        }
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

void loop() // Put code here to loop forever
{
        // strobe();

        // breathe();

        // circle();
}
