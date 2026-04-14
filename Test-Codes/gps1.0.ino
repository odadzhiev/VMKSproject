#include <TinyGPS++.h>

TinyGPSPlus gps;

void setup() {
  
  Serial.begin(115200);
  
  Serial2.begin(115200, SERIAL_8N1, 16, 17); 

  Serial.println("--- ТЕСТ С TINYGPS++ ---");
  Serial.println("Очаквам валидни данни от сателитите...");
}

void loop() {
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      displayInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("Грешка: Не получавам данни от GPS. Провери кабелите!");
    delay(5000);
  }
}

void displayInfo() {
  Serial.print("Локация: "); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(",");
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print("INVALID");
  }

  Serial.print("  Дата/Час: ");
  if (gps.date.isValid()) {
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.year());
  }

  Serial.print("  Скорост: ");
  Serial.print(gps.speed.kmph());
  Serial.print(" km/h");

  Serial.print("  Сателити: ");
  Serial.println(gps.satellites.value());
}