//Custom library for analogue sensors
#include <Analogue.h>

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
Analogue moisture(moisturePin);

//Temp and humidity values
float temperature;
float humidity;

///Buzzer
int buzzer = 13;

//Light sensor values
#define lightPin A1
Analogue light(lightPin);

//Water pump
#define waterPump 5
bool isPump = false;

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
  Serial.print(moisture.digital);  //Moisture
  Serial.print(" ");
  Serial.print(light.digital);
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

    digitalWrite(waterPump, isPump ? HIGH : LOW);

    switch (irValue) {
      case 0xF30CFF00: currentMenu = HOME; break;
      case 0xE718FF00: currentMenu = TEMP_HUMIDITY; break;
      case 0xA15EFF00: currentMenu = MOISTURE; break;
      case 0xF708FF00: currentMenu = LIGHT; break;
      case 0xEA15FF00: isPump = true; break;
      case 0xF807FF00: isPump = false; break;
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
  moisture.begin(280, 560);

  if (moisture.digital <= 30) {
    digitalWrite(waterPump, HIGH);  // Relay ON
  } else {
    digitalWrite(waterPump, LOW);  // Relay OFF
  }
}

void tempHumidity() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}

void lightSensor() {
  light.begin(1000, 150);
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
  lcd.print("Moisture: " + String(moisture.digital) + "%");
  if (moisture.digital < 20) {
    lcd.setCursor(0, 1);
    lcd.print("Plant is sad:(");
  } else if (moisture.digital > 20 && moisture.digital < 70) {
    lcd.setCursor(0, 1);
    lcd.print("Plant is happy:)");
  } else if (moisture.digital > 70) {
    lcd.setCursor(0, 1);
    lcd.print("Plant is wet:)");
  }
}

void printLight() {
  //LCD Code will go here
  lcd.clear();  // clear display
  lcd.setCursor(0, 0);
  lcd.print("Light: " + String(light.digital) + "%");
}
