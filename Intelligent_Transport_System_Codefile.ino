#define BLYNK_TEMPLATE_ID "TMPL3FkENj0uE"
#define BLYNK_TEMPLATE_NAME "Temperature"
#define BLYNK_AUTH_TOKEN "_avYwuqmczZV-jgcrLm352DUknU8fTl8"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <MFRC522.h>

#define RXD2 17
#define TXD2 16
#define GPS_BAUD 9600
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Hmm";
char pass[] = "29042004";

BlynkTimer timer;

// --- Sensor Pins ---
#define DHTPIN 27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define BUZZER_PIN 25
#define LIGHT_SENSOR_PIN 34
#define TRIG_PIN 32
#define ECHO_PIN 33
#define SOUND_SPEED 0.034

// --- RFID Pins ---
#define RST_PIN 22
#define SS_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- GPS Variables ---
String gpsData;
String latitude = "";
String longitude = "";
String utcTime = "";
String localTime = "";
const String defaultLatitude = "12.840871";
const String defaultLongitude = "80.153364";

// --- RFID Logic ---
bool rfidAccessGranted = false;
const int lightThreshold = 3800;

// --- Serial Print Timer ---
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 2000;

// --- Status Holders ---
String boxStatus = "Closed";
String buzzerStatus = "OFF";
float lastDistance = 0;
String objectPresence = "Object Present"; // For V8
bool errorShown = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, processSensors); // All sensors & Blynk update
}

void loop() {
  Blynk.run();
  timer.run();
  handleRFID();
}

// ========== RFID ==========
void handleRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  if (content == "D72C0CCA" || content == "9EBA2FAA") {
    Serial.println("🔐 RFID: Authorized");
    rfidAccessGranted = true;
    Blynk.virtualWrite(V2, "Authorized");
    beep(1, 300, 100);
  } else {
    Serial.println("🔒 RFID: Denied");
    rfidAccessGranted = false;
    Blynk.virtualWrite(V2, "Denied");
    beep(3, 200, 100);
  }

  delay(1000);
}

// ========== Sensor & Display Logic ==========
void processSensors() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float spd = gps.speed.kmph();

  // GPS parsing
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    if (c == '\n') {
      parseGPS(gpsData);
      gpsData = "";
    } else {
      gpsData += c;
    }
  }

  // Use fallback if no GPS lock
  String lat = latitude.length() > 0 ? latitude : defaultLatitude;
  String lon = longitude.length() > 0 ? longitude : defaultLongitude;
  String gpsInfo = "Lat: " + lat + " Lon: " + lon + " Time: " + (localTime.length() > 0 ? localTime : "Waiting...");

  // Default Status
  boxStatus = "Closed";
  buzzerStatus = "OFF";

  if (rfidAccessGranted) {
    boxStatus = "Open";
    buzzerStatus = "OFF";
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    handleLightSensor();       // updates boxStatus, buzzerStatus
    handleUltrasonicSensor();  // updates buzzerStatus, objectPresence
  }

  // -------- Error Handling --------
  if (isnan(h) || isnan(t)) {
    handleError("DHT Sensor Error!");
  } else if (t > 100) {
    handleError("Temp is Normal");
  } else if (spd > 80) {
    handleError("Speed Limit Exceeded!");
  } else {
    // Clear any previous error
    if (errorShown) {
      Blynk.virtualWrite(V6, "");
      errorShown = false;
    }
  }

  // Blynk Virtual Updates
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V3, buzzerStatus);
  Blynk.virtualWrite(V4, boxStatus);
  Blynk.virtualWrite(V5, gpsInfo);
  Blynk.virtualWrite(V7, spd);
  Blynk.virtualWrite(V8, objectPresence); // ultrasonic status

  // Serial Monitor Print (once every 2 sec)
  if (millis() - lastPrintTime > printInterval) {
    lastPrintTime = millis();
    Serial.println("===============================");
    Serial.print("🌡 Temp: "); Serial.print(t); Serial.print(" °C");
    Serial.print(" | 💧 Humidity: "); Serial.println(h);
    Serial.print("📦 Box Status: "); Serial.println(boxStatus);
    Serial.print("🔊 Buzzer: "); Serial.println(buzzerStatus);
    Serial.print("📍 GPS → "); Serial.println(gpsInfo);
    Serial.print("🚗 Speed: "); Serial.print(spd); Serial.println(" km/h");
    Serial.print("🔓 RFID Access: "); Serial.println(rfidAccessGranted ? "Authorized" : "Denied");
    Serial.print("📏 Distance: "); Serial.print(lastDistance); Serial.print(" cm → ");
    Serial.println(objectPresence);
  }
}

// ========== Light Sensor ==========
void handleLightSensor() {
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  if (lightValue < lightThreshold) {
    boxStatus = "Open";
    buzzerStatus = "ON";
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    boxStatus = "Closed";
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// ========== Ultrasonic Sensor ==========
void handleUltrasonicSensor() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * SOUND_SPEED / 2;
  lastDistance = distance;

  if (distance > 9) {
    objectPresence = "Object Absent";
    buzzerStatus = "ON";
    beep(2, 400, 200);
  } else {
    objectPresence = "Object Present";
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// ========== Error Handling ==========
void handleError(String msg) {
  Serial.println("⚠ " + msg);
  Blynk.virtualWrite(V6, msg);
  errorShown = true;
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerStatus = "ON";
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  buzzerStatus = "OFF";
}

void beep(int times, int onMs, int offMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.virtualWrite(V3, "ON");
    delay(onMs);
    digitalWrite(BUZZER_PIN, LOW);
    Blynk.virtualWrite(V3, "OFF");
    delay(offMs);
  }
}

// ========== GPS Parsing ==========
void parseGPS(String sentence) {
  if (sentence.startsWith("$GPRMC")) {
    int index = sentence.indexOf(',') + 1;
    utcTime = sentence.substring(index, index + 6);
    index = sentence.indexOf(',', index) + 1;
    String status = sentence.substring(index, index + 1);
    if (status == "A") {
      index = sentence.indexOf(',', index) + 1;
      latitude = formatCoordinates(sentence.substring(index, index + 10), sentence.charAt(index + 11));
      index = sentence.indexOf(',', index + 12) + 1;
      longitude = formatCoordinates(sentence.substring(index, index + 11), sentence.charAt(index + 12));
      localTime = convertToIST(utcTime);
    }
  }
}

String formatCoordinates(String rawCoord, char dir) {
  float deg = (dir == 'N' || dir == 'S') ? rawCoord.substring(0, 2).toFloat() : rawCoord.substring(0, 3).toFloat();
  float min = (dir == 'N' || dir == 'S') ? rawCoord.substring(2).toFloat() : rawCoord.substring(3).toFloat();
  float coord = deg + (min / 60.0);
  return String(coord, 6) + " " + dir;
}

String convertToIST(String utc) {
  if (utc.length() < 6) return "Invalid Time";
  int h = utc.substring(0, 2).toInt();
  int m = utc.substring(2, 4).toInt();
  int s = utc.substring(4, 6).toInt();
  h += 5; m += 30;
  if (m >= 60) { m -= 60; h++; }
  if (h >= 24) h -= 24;
  String period = (h >= 12) ? "PM" : "AM";
  if (h == 0) h = 12; else if (h > 12) h -= 12;
  char buf[12]; sprintf(buf, "%02d:%02d:%02d %s", h, m, s, period.c_str());
  return String(buf);
}
