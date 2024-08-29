#include <Wire.h>
#include "HackPublisher.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_AM2320 am2320 = Adafruit_AM2320();

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins for sensors
const int gasSensorAnalog = A1;
const int gasSensorOutput = A3;

// Internet settings
// Replace as needed
const char *ssid = "";
const char *password = "";

HackPublisher publisher("J.Y.M.", true);

void setup() {
  Serial.begin(115200);
  am2320.begin();
  pinMode(gasSensorAnalog, INPUT);

  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Start publisher
  publisher.begin();

  Serial.println("OK");
}

void loop() {
  float humidity = am2320.readHumidity();
  delay(500);
  float temperature = am2320.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  int gasSensorValue = analogRead(gasSensorAnalog); // Get gas sensor value
  Serial.print("Gas: ");
  Serial.println(gasSensorValue);
  
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.display();
  oled.setTextColor(WHITE);
  oled.setTextSize(2);
  oled.setTextWrap(true);
  oled.print("Temp:");
  oled.print(temperature, 1);
  oled.println("C");
  oled.print("Hum:");
  oled.print(humidity, 1);
  oled.println("%");
  oled.print("Gas:");
  oled.println(gasSensorValue);
  displayDanger(gasSensorValue, temperature, humidity);

  publisher.store("temperature", temperature);
  publisher.store("theGasData", gasSensorValue);
  publisher.store("humid", humidity);
  
  publisher.send();
  Serial.println("Sent data...");
  
  oled.display();
  delay(1000);
}



// CHECK THE VALUES HERE
void displayDanger(int gas,float temp,float humid){
  if(gas > 100 || temp > 24 || humid < 45){
    oled.print("DNG:");
    if(gas > 380){ // CHANGE!!
      oled.print("G ");
    }
    if(temp > 24){
      oled.print("T ");
    }
    if(humid < 100){
      oled.print("H");
    }
  }
  
}
