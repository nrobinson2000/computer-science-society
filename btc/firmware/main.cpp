#include "Particle.h"
#include "LiquidCrystal/LiquidCrystal.h"

/*******************************************************

Display the Bitcoin price in Dollars and the time on an LCD
Nathan Robinson, October 2016

Updated in October 2017, to use new API and accommodate new price

********************************************************/

// select the pins used on the LCD panel
LiquidCrystal lcd(A5, A4, D6, D0, D1, D7);

void initLCD() {
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0,0);
  lcd.print("Price $:   Time:"); // print
  Time.zone(-4);
  Serial.begin(9600);
}

STARTUP(initLCD());

void bitcoinHandler(const char *event, const char *data) {  // This is called when the Photon recives the webhook data.
 Serial.println(data);                                    // Print the data to serial for debugging
 String latest = String(data);                  // Get the latest price of Bitcoin
 Serial.println(latest);                                     // Print it to serial
 lcd.setCursor(0, 1);                                     // Set the cursor to the second line
 lcd.print(latest);                                          // Print the price of bitcoin
 lcd.print(" ");                                          // Print a dividing space after
}

void setup() {
  Particle.publish("bitcoin");  // Trigger bitcoin webhook
  Particle.subscribe("hook-response/bitcoin", bitcoinHandler, MY_DEVICES);  // Subscribe to the webhook response
}

void loop() {
  if (millis()/1000 % 10 == 0) // Every ten seconds
  {
    Particle.publish("bitcoin");  // Trigger bitcoin webhook
  }
  // Print the time
  lcd.setCursor(8,1);            // move cursor to second line "1" and 8 spaces over

  if (Time.hour() < 10) {
    lcd.print("0");
    lcd.print(Time.hour());
  }
  else {
    lcd.print(Time.hour());
  }

  lcd.print(":");

  if (Time.minute() < 10) {
    lcd.print("0");
    lcd.print(Time.minute());
  }
  else {
    lcd.print(Time.minute());
  }

  lcd.print(":");

  if (Time.second() < 10) {
    lcd.print("0");
    lcd.print(Time.second());
  }
  else {
    lcd.print(Time.second());
  }
}
