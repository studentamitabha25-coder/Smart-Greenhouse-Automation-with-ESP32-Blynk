#define BLYNK_TEMPLATE_ID "Your template id" //After creating a device in Blynk, you will find this three information, copy them and replace them in your code.
#define BLYNK_TEMPLATE_NAME "Your template name"
#define BLYNK_AUTH_TOKEN "Your Auth token"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <EEPROM.h>

// === Pin Definitions ===
#define DHTPIN 15
#define DHTTYPE DHT11 //In this line replace DHT11 with DHT22 if you are using DHT22.
#define MOISTURE_PIN 34
#define PUMP_PIN 14
#define HUMIDIFIER_PIN 12

// === Virtual Pin Definitions ===
#define VP_TEMP          V0
#define VP_HUMIDITY      V1
#define VP_SOIL_MOISTURE V2
#define VP_PUMP_STATUS   V3
#define VP_HUMIDIFIER    V4
#define VP_THRESHOLD     V5
#define VP_MANUAL_PUMP   V6
#define VP_PUMP_DURATION V8
#define VP_DELAY_H       V10
#define VP_DELAY_M       V11
#define VP_DELAY_S       V12
#define VP_PUMP_COUNT    V13
#define VP_PUMP_TIME     V14

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "Your wifi name";//Write your wifi name here.
char pass[] = "your wifi password";//write your wifi password here.

// === System Variables ===
int humidityThreshold = 80;
unsigned long pumpDuration = 2000;
unsigned long pumpDelay = 60000;
unsigned long lastPumpTime = 0;
bool pumpOn = false;
unsigned long pumpStartTime = 0;

// EEPROM-backed variables
unsigned long pumpTotalTimeSec = 0;
int pumpCount = 0;

// EEPROM addresses
#define EEPROM_SIZE 64
#define ADDR_PUMP_COUNT      0
#define ADDR_PUMP_TIME_SEC   10

// Delay input from Blynk
int delayHours = 0, delayMinutes = 0, delaySeconds = 0;

// === Humidity Correction Scale ===
const float HUMIDITY_SCALE = 0.8;

// === BLYNK WRITE HANDLERS ===
BLYNK_WRITE(VP_THRESHOLD) {
  humidityThreshold = param.asInt();
}

BLYNK_WRITE(VP_MANUAL_PUMP) {
  if (param.asInt() == 1 && !pumpOn) {
    digitalWrite(PUMP_PIN, HIGH);
    pumpOn = true;
    pumpStartTime = millis();
    lastPumpTime = millis();
    pumpCount++;
    Blynk.virtualWrite(VP_PUMP_STATUS, 1);
  }
}

BLYNK_WRITE(VP_PUMP_DURATION) {
  int sec = param.asInt();
  if (sec >= 1 && sec <= 30) {
    pumpDuration = sec * 1000UL;
  }
}

BLYNK_WRITE(VP_DELAY_H) {
  delayHours = constrain(param.asInt(), 0, 23);
  updatePumpDelay();
}

BLYNK_WRITE(VP_DELAY_M) {
  delayMinutes = constrain(param.asInt(), 0, 59);
  updatePumpDelay();
}

BLYNK_WRITE(VP_DELAY_S) {
  delaySeconds = constrain(param.asInt(), 0, 59);
  updatePumpDelay();
}

void updatePumpDelay() {
  pumpDelay = ((unsigned long)delayHours * 3600UL +
               (unsigned long)delayMinutes * 60UL +
               (unsigned long)delaySeconds) * 1000UL;
  Serial.printf("Pump Delay set to %02d:%02d:%02d (%lu ms)\n", delayHours, delayMinutes, delaySeconds, pumpDelay);
}

// === SENSOR DATA TO BLYNK ===
void sendSensorData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp)) Blynk.virtualWrite(VP_TEMP, temp);

  if (!isnan(hum)) {
    hum *= HUMIDITY_SCALE;
    hum = constrain(hum, 0, 100);
    Blynk.virtualWrite(VP_HUMIDITY, hum);

    bool humidifierOn = hum < humidityThreshold;
    digitalWrite(HUMIDIFIER_PIN, humidifierOn);
    Blynk.virtualWrite(VP_HUMIDIFIER, humidifierOn);
  }

  int raw = analogRead(MOISTURE_PIN);
  int moist = map(raw, 3500, 1200, 0, 100);
  moist = constrain(moist, 0, 100);
  Blynk.virtualWrite(VP_SOIL_MOISTURE, moist);

  Blynk.virtualWrite(VP_PUMP_COUNT, pumpCount);
  Blynk.virtualWrite(VP_PUMP_TIME, pumpTotalTimeSec);
}

// === AUTO PUMP LOGIC ===
void autoPumpControl() {
  unsigned long now = millis();

  if (pumpOn) {
    if (now - pumpStartTime >= pumpDuration) {
      digitalWrite(PUMP_PIN, LOW);
      pumpOn = false;
      Blynk.virtualWrite(VP_PUMP_STATUS, 0);

      unsigned long runSec = (now - pumpStartTime) / 1000;
      pumpTotalTimeSec += runSec;
      lastPumpTime = now;

      savePumpData();  // save to EEPROM
    }
  } else {
    if (now - lastPumpTime >= pumpDelay) {
      int raw = analogRead(MOISTURE_PIN);
      int moist = map(raw, 3500, 1200, 0, 100);
      moist = constrain(moist, 0, 100);

      if (moist < 30) {
        digitalWrite(PUMP_PIN, HIGH);
        pumpOn = true;
        pumpStartTime = now;
        pumpCount++;
        Blynk.virtualWrite(VP_PUMP_STATUS, 1);
      }
    }
  }
}

// === EEPROM HANDLING ===
void loadPumpData() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(ADDR_PUMP_COUNT, pumpCount);
  EEPROM.get(ADDR_PUMP_TIME_SEC, pumpTotalTimeSec);
  if (pumpCount < 0 || pumpCount > 10000) pumpCount = 0;
  if (pumpTotalTimeSec > 3600000) pumpTotalTimeSec = 0;
}

void savePumpData() {
  EEPROM.put(ADDR_PUMP_COUNT, pumpCount);
  EEPROM.put(ADDR_PUMP_TIME_SEC, pumpTotalTimeSec);
  EEPROM.commit();
}

// === SETUP ===
void setup() {
  Serial.begin(115200);

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(HUMIDIFIER_PIN, LOW);

  dht.begin();
  EEPROM.begin(EEPROM_SIZE);
  loadPumpData();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.syncVirtual(VP_THRESHOLD, VP_PUMP_DURATION, VP_DELAY_H, VP_DELAY_M, VP_DELAY_S);
}

// === MAIN LOOP ===
void loop() {
  Blynk.run();
  static unsigned long lastSensorUpdate = 0;
  unsigned long now = millis();

  if (now - lastSensorUpdate >= 2000) {
    sendSensorData();
    lastSensorUpdate = now;
  }

  autoPumpControl();
}
