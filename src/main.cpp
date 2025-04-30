#include "WiFi.h"
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "ThingSpeak.h"

#define DHTPIN 27 
#define DHTTYPE DHT22
#define MQ2 35
#define BUZZER_PIN 12
#define LED 17

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
  pinMode(LED, OUTPUT);

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

  int gas = analogRead(MQ2);
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  // Temp
  LCD.clear();
  digitalWrite(LED, HIGH);
  if (temp > 30){
    Serial.print(temp);
    tone(BUZZER_PIN, 1000);          
    digitalWrite(LED, LOW);
    Serial.println("C°: - ALERTA: Temperatura alta detectada!");
    LCD.setCursor(0, 0);
    LCD.print("ALERT");
    LCD.setCursor(0, 1);
    LCD.print("HIGH TEMP");
    LCD.setCursor(0, 2);
    LCD.println("TEMP C:" + String(temp));
    noTone(BUZZER_PIN);
    delay(5000);
  }
  else if (temp >= 17 && temp <= 30){
    Serial.print(temp);
    Serial.println("C°: - Temperatura ambiente ideal");
  }
  else{
    Serial.print(temp);
    tone(BUZZER_PIN, 1000);             
    
    digitalWrite(LED, LOW);
    Serial.println("C°: - Temperatura abaixo do indicado");
    LCD.setCursor(0, 0);
    LCD.println("ALERT");
    LCD.setCursor(0, 1);
    LCD.println("LOW TEMP");
    LCD.setCursor(0, 2);
    LCD.println("TEMP C:" + String(temp));
    noTone(BUZZER_PIN);
    delay(5000);
  }

  // Gás
  LCD.clear();
  if (gas >= 3500) {
    tone(BUZZER_PIN, 1000);             
    digitalWrite(LED, LOW);
    Serial.print(gas);
    Serial.println("pp: Alerta! Nível elevado de gás. Contate o suporte.");
    LCD.setCursor(0, 0);
    LCD.println("ALERT");
    LCD.setCursor(0, 1);
    LCD.println("HIGH CONCE GAS");
    LCD.setCursor(0, 2);
    LCD.println("GAS pp:" + String(gas));
    noTone(BUZZER_PIN);
    delay(2000);
  } 
  else {
    Serial.print(gas);
    Serial.println("pp: Nenhum gás detectado.");
  }
  LCD.setCursor(0, 0);
  LCD.println("TEMP C:" + String(temp));
  LCD.setCursor(0, 1);
  LCD.println("UMID:" + String(humidity));
  LCD.setCursor(0, 2);
  LCD.println("GAS " + String(gas) + "pp");
 

  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, gas);
  int x = ThingSpeak.writeFields(channelID, Key);
  Serial.println("");
  delay(2000);
}