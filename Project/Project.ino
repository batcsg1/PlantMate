// Include the DHT22 library for interfacing with the temperature/humidity sensor.
#include "DHT.h"
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);  //temperature/humidity sensor

#include <LiquidCrystal_I2C.h>       // Library for LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 column and 2 rows

//Infrared receiver
#include <IRremote.hpp>

//Moisture sensor values
#define moisturePin A0
int dry = 560;
int wet = 280;
int percentage;
int moisture;

//Temp and humidity values
float temperature;
float humidity;

///Buzzer
int buzzer = 13;

//Light sensor values
#define lightPin A1
int light;

//Water pump
#define waterPump 5

//Water level sensor
#include <HCSR04.h>
#define trig_pin 2
#define echo_pin 3
HCSR04 hc(trig_pin, echo_pin);  //initialisation class HCSR04 (trig pin , echo pin)

//Menu state enum
enum MenuState { HOME,
                 TEMP_HUMIDITY,
                 MOISTURE,
                 LIGHT };

MenuState currentMenu = HOME;

void setup() {
  Serial.begin(9600);

  //Setup DHT22
  dht.begin();

  //IR Receiver
  IrReceiver.begin(6);

  pinMode(buzzer, OUTPUT);
  pinMode(waterPump, OUTPUT);

  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);

  //Declare data headers
  Serial.print("Temperature(C), ");
  Serial.print("Humidity(%), ");
  Serial.print("Moisture(%), ");
  Serial.print("Light(%), ");
  Serial.print("Water Level(cm)");
  Serial.println();

  lcd.init();       //initialize the lcd
  lcd.backlight();  //open the backlight
}

void loop() {
  //Allow data to be fed in
  menuState();
  soilMoisture();
  tempHumidity();
  lightSensor();

  //Serial readings of data from sensors
  Serial.print(temperature);  //Tempearture
  Serial.print(" ");
  Serial.print(humidity);  //Humidity
  Serial.print(" ");
  Serial.print(moisture);  //Moisture
  Serial.print(" ");
  Serial.print(light);
  Serial.print(" ");
  Serial.print(hc.dist());
  Serial.println();


  switch (currentMenu) {
    case HOME:
      menu();
      break;
    case TEMP_HUMIDITY:
      printTH();
      break;
    case MOISTURE:
      printMoisture();
      break;
    case LIGHT:
      printLight();
      break;
  }
  delay(1500);
}

void menuState() {
  if (IrReceiver.decode()) {
    uint32_t irValue = IrReceiver.decodedIRData.decodedRawData;

    // Print the value in HEX format
    Serial.print("Hex Code: 0x");
    Serial.println(irValue, HEX);

    switch (irValue) {
      case 0xF30CFF00: currentMenu = HOME; break;
      case 0xE718FF00: currentMenu = TEMP_HUMIDITY; break;
      case 0xA15EFF00: currentMenu = MOISTURE; break;
      case 0xF708FF00: currentMenu = LIGHT; break;
    }

    IrReceiver.resume();
  }
}

void beep() {
  tone(buzzer, 1000);  // Send 1KHz sound signal...
  delay(1000);         // ...for 1 sec
  noTone(buzzer);      // Stop sound...
  delay(1000);         // ...for 1sec
}

void soilMoisture() {
  //ChatGPT code for keeping percentage within 0 to 100%
  //Prompt: how to keep a percentage range from 0 to 100% within range
  int rawMoisture = analogRead(moisturePin);
  percentage = map(rawMoisture, wet, dry, 100, 0);  //Constrain percentage within 0 to 100%
  moisture = constrain(percentage, 0, 100);
  // if (percentage == 0) {
  digitalWrite(waterPump, HIGH);  // Relay ON
  // } else {
  // digitalWrite(waterPump, HIGH);  // Relay OFF
  // }
}

void tempHumidity() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}

void lightSensor() {
  light = analogRead(lightPin);
}

//-- LCD Functions
void menu() {
  lcd.clear();                    // clear display
  lcd.setCursor(0, 0);            // move cursor to   (0, 0)
  lcd.print("PlantMate");         // print message at (0, 0)
  lcd.setCursor(0, 1);            // move cursor to   (2, 1)
  lcd.print("Smart Irrigation");  // print message at (2, 1)
}

//-- Print functions for Temprature, Humidity and Soil Moisture
void printTH() {
  //LCD Code will go here
  lcd.clear();  // clear display
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temperature) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: " + String(humidity) + "%");
}

void printMoisture() {
  //LCD Code will go here
  lcd.clear();  // clear display
  lcd.setCursor(0, 0);
  lcd.print("Moisture: " + String(percentage) + "%");
  if (percentage < 20) {
    lcd.setCursor(0, 1);
    lcd.print("Plant is sad:(");
  } else if (percentage > 20 && percentage < 70) {
    lcd.setCursor(0, 1);
    lcd.print("Plant is happy:)");
  } else if (percentage > 70) {
    lcd.setCursor(0, 1);
    lcd.print("Plant is wet:)");
  }
}

void printLight() {
  //LCD Code will go here
  lcd.clear();  // clear display
  lcd.setCursor(0, 0);
  lcd.print("Light: " + String(light) + "%");
}
