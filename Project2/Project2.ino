// Include the DHT11 library for interfacing with the temperature/humidity sensor.
#include <DHT11.h>
DHT11 dht11(12);  //temperature/humidity sensor

#include <LiquidCrystal_I2C.h>       // Library for LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 column and 2 rows

//Moisture sensor values
#define moisturePin A0
int dryValue = 330;
int wetValue = 650;
int percentage;

int moisture;

//Temp and humidity values
int temperature;
int humidity;

///Buzzer
//int buzzer = 13;

//Buttons
int tempH = 2;
int moistureBtn = 3;

volatile bool home = true;
//Button ISR booleans
volatile bool tempHMenu = false;
volatile bool moistureMenu = false;

void tempHISR() {
  tempHMenu = true;
  moistureMenu = false;
}

void moistureISR() {
  moistureMenu = true;
  tempHMenu = false;
}

void setup() {
  // Initialize serial communication to allow debugging and data readout.
  // Using a baud rate of 9600 bps.
  Serial.begin(9600);

  //pinMode(buzzer, OUTPUT);
  pinMode(tempH, INPUT_PULLUP);
  pinMode(moistureBtn, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(tempH), tempHISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(moistureBtn), moistureISR, FALLING);

  //Declare data headers
  Serial.print("Temperature(C), ");
  Serial.print("Humidity(%), ");
  Serial.print("Moisture(%)");
  Serial.println();

  lcd.init();       //initialize the lcd
  lcd.backlight();  //open the backlight
}

void loop() {
  //Allow data to be fed in
  soilMoisture();
  tempHumidity();

  //Serial readings of data from sensors
  Serial.print(temperature);  //Tempearture
  Serial.print(" ");
  Serial.print(humidity);  //Humidity
  Serial.print(" ");
  Serial.print(percentage);  //Moisture
  Serial.print(" ");
  // Serial.print(moisture);  //Moisture
  // Serial.print(" ");
  Serial.println();
  //Where the LCD code lives
  stateHandler();
  delay(1000);
}

void menu() {    
  lcd.clear();           // clear display
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  lcd.print("PlantMate");  // print message at (0, 0)
  lcd.setCursor(0, 1);         // move cursor to   (2, 1)
  lcd.print("Smart Irrigation");     // print message at (2, 1)
}

//Logic functions for Temprature, Humidity and Soil Moisture
void soilMoisture() {
  moisture = analogRead(moisturePin);
  percentage = map(moisture, dryValue, wetValue, 0, 100);
  //ChatGPT code for keeping percentage within 0 to 100%
  //Prompt: how to keep a percentage range from 0 to 100% within range
  percentage = constrain(percentage, 0, 100); //Constrain percentage within 0 to 100%
}

void tempHumidity() {
  // Attempt to read the temperature and humidity values from the DHT11 sensor.
  int result = dht11.readTemperatureHumidity(temperature, humidity);
}

//Print functions for Temprature, Humidity and Soil Moisture
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
  if (percentage < 20){
    lcd.setCursor(0, 1);
    lcd.print("Plant is sad:(");
  }else if (percentage > 20 && percentage < 70){
    lcd.setCursor(0, 1);
    lcd.print("Plant is happy:)");
  }else if (percentage > 70){
    lcd.setCursor(0, 1);
    lcd.print("Plant is wet:)");
  }
}

void stateHandler() {
  if (home){
    menu();
  }
  if (tempHMenu) {
    home = false;
    moistureMenu = false;
    Serial.println("Temperature and humidity button pressed");
    printTH();     // Show temperature and humidity
  } else if (moistureMenu) {
    home = false;
    tempHMenu = false;
    Serial.println("Soil moisture button pressed");
    printMoisture();  // Show soil moisture only
  }
}