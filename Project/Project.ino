// Include the DHT11 library for interfacing with the temperature/humidity sensor.
#include <DHT22.h>
#define pinDATA 12
DHT22 dht22(pinDATA);  //temperature/humidity sensor

#include <LiquidCrystal_I2C.h>       // Library for LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 column and 2 rows

//Moisture sensor values
#define moisturePin A0
#define dryValue 330;
#define wetValue 650;
#define percentage;
#define moisture;

//Temp and humidity values
int temperature;
int humidity;

///Buzzer
int buzzer = 13;

//Light sensor values
#define lightPin A1
int light;

//Buttons
#define tempH = 2;
#define moistureBtn = 3;
#define lightBtn = 4;

//Water pump
#define waterPump = 5;

//Menu state enum
enum MenuState { HOME,
                 TEMP_HUMIDITY,
                 MOISTURE,
                 LIGHT };
                
MenuState currentMenu = HOME;

void setup() {
  // Initialize serial communication to allow debugging and data readout.
  // Using a baud rate of 9600 bps.
  Serial.begin(9600);

  pinMode(buzzer, OUTPUT);
  pinMode(tempH, INPUT_PULLUP);
  pinMode(moistureBtn, INPUT_PULLUP);
  pinMode(lightBtn, INPUT_PULLUP);
  pinMode(waterPump, OUTPUT);

  //Declare data headers
  Serial.print("Temperature(C), ");
  Serial.print("Humidity(%), ");
  Serial.print("Moisture(%), ");
  Serial.print("Light(%)");
  Serial.println();

  lcd.init();       //initialize the lcd
  lcd.backlight();  //open the backlight
}

void loop() {

  //Allow data to be fed in
  soilMoisture();
  tempHumidity();
  lightSensor();

  //Serial readings of data from sensors
  Serial.print(temperature);  //Tempearture
  Serial.print(" ");
  Serial.print(humidity);  //Humidity
  Serial.print(" ");
  Serial.print(percentage);  //Moisture
  Serial.print(" ");
  Serial.print(light);
  Serial.println();

  checkButtons();
  delay(1000);
}

void beep() {
  tone(buzzer, 1000);  // Send 1KHz sound signal...
  delay(1000);         // ...for 1 sec
  noTone(buzzer);      // Stop sound...
  delay(1000);         // ...for 1sec
}

//-- Logic functions for Temprature, Humidity, Soil Moisture and Light
void soilMoisture() {
  moisture = analogRead(moisturePin);
  percentage = map(moisture, dryValue, wetValue, 0, 100);
  //ChatGPT code for keeping percentage within 0 to 100%
  //Prompt: how to keep a percentage range from 0 to 100% within range
  percentage = constrain(percentage, 0, 100);  //Constrain percentage within 0 to 100%

  if (percentage == 0) {
    digitalWrite(waterPump, LOW);  // Relay ON
  } else {
    digitalWrite(waterPump, HIGH); // Relay OFF
  }
}

void tempHumidity() {
  // Attempt to read the temperature and humidity values from the DHT11 sensor.
  temperature = dht22.getTemperature();
  humidity = dht22.getHumidity();
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

// Menu and button logic

void checkButtons() {
  delay(50);
  if (digitalRead(tempH) == LOW) {
    currentMenu = TEMP_HUMIDITY;
  }

  if (digitalRead(moistureBtn) == LOW) {
    currentMenu = MOISTURE;
  }

  if (digitalRead(lightBtn) == LOW) {
    currentMenu = LIGHT;
  }
  menuState();
}

void menuState() {
  if (currentMenu == HOME) {
    menu();
  } else if (currentMenu == TEMP_HUMIDITY) {
    printTH();
  } else if (currentMenu == MOISTURE) {
    printMoisture();
  } else if (currentMenu == LIGHT) {
    printLight();
  }
}