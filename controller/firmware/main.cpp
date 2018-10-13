#include "Adafruit_DHT/Adafruit_DHT.h"
#include "neopixel/neopixel.h"
#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

#define PIXEL_PIN D0
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812B

char message[256];

Adafruit_NeoPixel ring(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

STARTUP(WiFi.selectAntenna(ANT_AUTO)); // continually switches at high speed between antennas

SerialLogHandler logHandler(LOG_LEVEL_ALL);

Servo lock;

void softDelay(uint32_t msDelay) {
        for (uint32_t ms = millis(); millis() - ms < msDelay; Particle.process());
}

int brightness;
double humidity, lastHumidity;
double temperature, lastTemperature;

#define DHTPIN A2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void getReadings()
{
        brightness = analogRead(A0);

        lastHumidity = humidity;
        lastTemperature = temperature;

        humidity = dht.getHumidity();
        temperature = dht.getTempFarenheit();

        if (isnan(humidity) || isnan(temperature)) {
                Serial.println("Failed to read from DHT sensor!");
                humidity = lastHumidity;
                temperature = lastTemperature;
        }

        snprintf(message, sizeof(message), "{\"t\":\"%.2f\",\"h\":\"%.2f\",\"b\":\"%d\"}", temperature, humidity, brightness);
        Particle.publish("room_status", message, PRIVATE);
}

int red, green, blue;
int lastRed, lastGreen, lastBlue;

int setColor(const char* args) // dim to off and then dim to new color
{
        int fadeRed, fadeGreen, fadeBlue;

        lastRed = red;
        lastGreen = green;
        lastBlue = blue;

        // parse arguments to get red,green,blue
        if (sscanf(args, "%d,%d,%d", &red, &green, &blue) < 3) {
                return -1; // bail if not all 3 colors are provided
        }

        for (int i = 0; i <= max(max(lastRed,lastGreen),lastBlue); i++) // fade out
        {
                fadeRed = constrain(lastRed - i, 0, 255);
                fadeGreen = constrain(lastGreen - i, 0, 255);
                fadeBlue = constrain(lastBlue - i, 0, 255);

                for (int i=0; i<ring.numPixels(); i++)
                {
                        ring.setPixelColor(i, fadeRed, fadeGreen, fadeBlue);
                }
                ring.show();
                delay(5);
        }

        for (int i = max(max(red,green),blue); i>=0; i--) // fade in
        {
                fadeRed = constrain(red - i, 0, red);
                fadeGreen = constrain(green - i, 0, green);
                fadeBlue = constrain(blue - i, 0, blue);

                for (int i=0; i<ring.numPixels(); i++)
                {
                        ring.setPixelColor(i, fadeRed, fadeGreen, fadeBlue);
                }
                ring.show();
                delay(5);
        }
        return red*1000000 + green*1000 + blue;
}

int lockFunction(const char* args)
{
        int lockOption = atoi(args);
        lock.write(lockOption);
        return lockOption;
}

void setup() // Put setup code here to run once
{
        ring.begin();
        ring.show(); // Initialize all pixels to 'off'
        dht.begin();
        lock.attach(D2);
        Serial.begin(115200);

        pinMode(A5, OUTPUT);
        pinMode(A0, INPUT);

        digitalWrite(A5, HIGH);

        Particle.function("setColor", setColor);
        Particle.function("lock", lockFunction);

        Particle.connect();
}

void loop() // Put code here to loop forever
{
        if (Particle.connected())
        {
                getReadings();
        }
        softDelay(5000);
}
