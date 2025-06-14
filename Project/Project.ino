//Custom library for analogue sensors
#include <Analogue.h>

// Include the DHT22 library for interfacing with the temperature/humidity sensor.
#include "DHT.h"
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);  //temperature/humidity sensor

#include <LiquidCrystal_I2C.h>  
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Infrared receiver
#include <IRremote.hpp>

//Moisture sensor values
#define moisturePin A0
Analogue moisture(moisturePin);

//Temp and humidity values
float temperature;
float humidity;

//Light sensor values
#define lightPin A1
Analogue light(lightPin);

//Water pump
#define waterPump 5

//Water level sensor
#include <HCSR04.h>
#define trig_pin 2
#define echo_pin 3
HCSR04 hc(trig_pin, echo_pin);  //initialisation class HCSR04 (trig pin , echo pin)

// Menu state enum
enum MenuState { HOME,
                 TEMP_HUMIDITY,
                 MOISTURE,
                 LIGHT,
                 WATER };

MenuState currentMenu = HOME;

void setup() {
  Serial.begin(9600);

  //Setup DHT22
  dht.begin();

  //IR Receiver
  IrReceiver.begin(6);

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
  soilMoisture();
  tempHumidity();
  lightSensor();

  // Menu method
  menuState();

  //Serial readings of data from sensors
  Serial.print(temperature);  //Tempearture
  Serial.print(" ");
  Serial.print(humidity);  //Humidity
  Serial.print(" ");
  Serial.print(moisture.digital);  //Moisture
  Serial.print(" ");
  Serial.print(light.digital); // Light
  Serial.print(" ");
  Serial.print(hc.dist()); // Water level
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
    case WATER:
      printWater();
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

    // Switch statement for button values
    switch (irValue) {
      case 0xF30CFF00: currentMenu = HOME; break;
      case 0xE718FF00: currentMenu = TEMP_HUMIDITY; break;
      case 0xA15EFF00: currentMenu = MOISTURE; break;
      case 0xF708FF00: currentMenu = LIGHT; break;
      case 0xE31CFF00: currentMenu = WATER; break;
    }

    IrReceiver.resume();
  }
}

// ## Logic functions ##

void soilMoisture() {
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

// ## LCD Functions ##

void menu() {
  lcd.clear();            
  lcd.setCursor(0, 0);       
  lcd.print("PlantMate");  
        
  lcd.setCursor(0, 1);          
  lcd.print("Smart Irrigation"); 
}

void printWater() {
  lcd.clear();            
  lcd.setCursor(0, 0);       
  lcd.print("Water level:");  
        
  lcd.setCursor(0, 1);          
  lcd.print(String(hc.dist()) + "cm");
}

void printTH() {
  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temperature) + "C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: " + String(humidity) + "%");
}

void printMoisture() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture: " + String(moisture.digital) + "%");

  lcd.setCursor(0, 1);
  if (moisture.digital < 20) {
    lcd.print("Plant is sad:(");
  } else if (moisture.digital > 20 && moisture.digital < 70) {
    lcd.print("Plant is happy:)");
  } else if (moisture.digital > 70) {
    lcd.print("Plant is wet:)");
  }
}

void printLight() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light: " + String(light.digital) + "%");

  lcd.setCursor(0, 1);
  if (light.digital < 30) {
    lcd.print("Too dark");
  } else if (light.digital >= 30 && light.digital < 70) {
    lcd.print("Adequate light");
  } else if (light.digital >= 70 && light.digital <= 100) {
    lcd.print("Plenty of light");
  } else {
    lcd.print("Sensor error?");
  }
}
