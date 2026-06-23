#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2C_SDA 21
#define I2C_SCL 22

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define SOIL_PIN 34

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

BH1750 lightMeter;
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Calibrate these later
int SOIL_DRY_VALUE = 3000;
int SOIL_WET_VALUE = 1200;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.begin(I2C_SDA, I2C_SCL);

  dht.begin();

  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 not found");
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Sensors ready");
  display.display();

  delay(1000);
}

void loop() {
  float lux = lightMeter.readLightLevel();

  float dhtTemp = dht.readTemperature();
  float dhtHum = dht.readHumidity();

  int soilRaw = analogRead(SOIL_PIN);
  int soilPercent = map(soilRaw, SOIL_DRY_VALUE, SOIL_WET_VALUE, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  Serial.print("TEMP=");
  Serial.print(dhtTemp);
  Serial.print(",HUM=");
  Serial.print(dhtHum);
  Serial.print(",LUX=");
  Serial.print(lux);
  Serial.print(",SOIL_RAW=");
  Serial.print(soilRaw);
  Serial.print(",SOIL=");
  Serial.println(soilPercent);

  // Decide face/status
  String status = "HAPPY";

  if (dhtTemp > 28) {
    status = "HOT";
  } else if (dhtTemp < 18) {
    status = "COLD";
  } else if (soilPercent < 30) {
    status = "DRY";
  } else if (lux > 2000) {
    status = "BRIGHT";
  } else if (lux < 300) {
    status = "DARK";
  }

  // Draw OLED
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Face area
  display.setTextSize(1);

  if (status == "HOT") {
    display.setCursor(42, 0);
    display.println("TOO HOT");
    display.setCursor(42, 16);
    display.println("( o_o )");
    display.setCursor(49, 26);
    display.println(" /|\\");
    display.setCursor(88, 18);
    display.println("'");
    display.setCursor(92, 24);
    display.println("'");
  } 
  else if (status == "COLD") {
    display.setCursor(40, 0);
    display.println("TOO COLD");
    display.setCursor(42, 16);
    display.println("( >_< )");
    display.setCursor(34, 28);
    display.println("*     *");
  } 
  else if (status == "DRY") {
    display.setCursor(20, 0);
    display.println("NEEDS WATER");
    display.setCursor(42, 16);
    display.println("( -_- )");
    display.setCursor(48, 28);
    display.println("\\___/");
  } 
  else if (status == "BRIGHT") {
    display.setCursor(18, 0);
    display.println("TOO BRIGHT");
    display.setCursor(42, 16);
    display.println("( >:| )");
    display.setCursor(88, 8);
    display.println("\\|/");
    display.setCursor(88, 16);
    display.println("-O-");
    display.setCursor(88, 24);
    display.println("/|\\");
  } 
  else if (status == "DARK") {
    display.setCursor(24, 0);
    display.println("TOO DARK");
    display.setCursor(42, 16);
    display.println("( -.- )");
    display.setCursor(52, 28);
    display.println("zzz");
  } 
  else {
    display.setCursor(42, 0);
    display.println("HAPPY");
    display.setCursor(42, 16);
    display.println("( ^_^ )");
    display.setCursor(50, 28);
    display.println("\\___/");
  }

  // Bottom readings
  display.setTextSize(1);

  display.setCursor(0, 48);
  display.print("T:");
  display.print(dhtTemp, 0);
  display.print("C H:");
  display.print(dhtHum, 0);
  display.print("% L:");
  display.print((int)lux);

  display.setCursor(0, 56);
  display.print("Soil:");
  display.print(soilPercent);
  display.print("% Raw:");
  display.print(soilRaw);

  display.display();

  delay(15000); // 15 seconds
  // use delay(900000); for 15 minutes
}