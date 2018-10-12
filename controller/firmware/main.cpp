#include "Adafruit_DHT/Adafruit_DHT.h"
#include "neopixel/neopixel.h"
#include "Particle.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

#define PIXEL_PIN D0
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812B

char message[256];

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

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

int lockFunction(String args)
{
        int lockOption = args.toInt();
        lock.write(lockOption);
        return lockOption;
}

void setup() // Put setup code here to run once
{
        strip.begin();
        strip.show(); // Initialize all pixels to 'off'
        dht.begin();

        lock.attach(D2);

        Serial.begin(115200);

        pinMode(D7, OUTPUT);
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
