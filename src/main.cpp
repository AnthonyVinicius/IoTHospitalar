#include "WiFi.h"
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ThingSpeak.h"

#define DHTPIN 27 
#define DHTTYPE DHT22
#define MQ2_ANALOG_PIN 34
#define MQ2_DIGITAL_PIN 21
#define BUZZER_PIN 12

const char* WIFI_NAME = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const int channelID  = 2937121;
const char* Key = "GIGRY4R6KISWYIUG";

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 20, 4);
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE); 

void setup()
{
  Serial.begin(115200);

  LCD.init();
  LCD.backlight();
  LCD.setCursor(0, 1);
  LCD.print("Connecting to ");
  LCD.setCursor(0, 2);
  LCD.print("WiFi ");

  dht.begin();
  pinMode(MQ2_DIGITAL_PIN, INPUT);
  pinMode(MQ2_ANALOG_PIN, INPUT);

  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Wifi not connected");
  }
  Serial.println("Wifi connected !");
  Serial.println("Local IP: " + String(WiFi.localIP()));
  WiFi.mode(WIFI_STA);
  pinMode(BUZZER_PIN, OUTPUT);
  LCD.clear();
  LCD.setCursor(0, 1);
  LCD.println("Online");
  LCD.setCursor(0, 2);
  LCD.println("Local IP: " + String(WiFi.localIP()));
  delay(2000);
  
  LCD.setCursor(0, 1);
  ThingSpeak.begin(client);
  Serial.println("Setup done!");
}

void loop(){

  int conce = digitalRead(MQ2_DIGITAL_PIN);
  int gas = analogRead(MQ2_ANALOG_PIN);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  // Temp
  LCD.clear();
  if (temp > 30){
    Serial.print(temp);
    tone(BUZZER_PIN, 1000);
    delay(2000);             
    noTone(BUZZER_PIN);
    Serial.println("C°: - ALERTA: Temperatura alta detectada!");
  }
  else if (temp >= 17 && temp <= 30){
    Serial.print(temp);
    Serial.println("C°: - Temperatura ambiente ideal");
  }
  else{
    Serial.print(temp);
    tone(BUZZER_PIN, 1000);
    delay(2000);             
    noTone(BUZZER_PIN);
    Serial.println("C°: - Temperatura abaixo do indicado");
  }
  LCD.setCursor(0, 0);
  LCD.println("Temp C:" + String(temp));
  LCD.setCursor(0, 1);
  LCD.println("Umid:" + String(humidity));
  

  // Gás
  if (gas > 300) {
    Serial.print(gas);
    tone(BUZZER_PIN, 1000);
    delay(2000);             
    noTone(BUZZER_PIN);
    Serial.println("pp: ALERTA! Gás detectado no ambiente.");
  } 
  else if (gas >= 500) {
    Serial.print(gas);
    Serial.println("pp: Alerta! Nível elevado de gás. Contate o suporte.");
  } 
  else {
    Serial.print(gas);
    Serial.println("pp: Nenhum gás detectado.");
  }
  LCD.setCursor(0, 2);
  LCD.println("Gas " + String(temp) + "pp");
 

  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, gas);
  int x = ThingSpeak.writeFields(channelID, Key);
  Serial.println("");
  delay(2000);
}