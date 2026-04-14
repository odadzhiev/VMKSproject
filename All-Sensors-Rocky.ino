#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BMP3XX.h>
#include <TinyGPS++.h>

MPU9250_asukiaaa mpu;
Adafruit_BMP280 bmp;
Adafruit_BMP3XX bmp390;
TinyGPSPlus gps;

bool launched = false;
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  
  Serial2.begin(115200, SERIAL_8N1, 44, 43); 

  Wire.begin(5, 6);

  mpu.setWire(&Wire);
  mpu.beginAccel();
  mpu.beginGyro();

  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 error!");
  }

  // === SETUP ОТ ТВОЯ BMP390 ===
  if (!bmp390.begin_I2C(0x77, &Wire)) {
    Serial.println("BMP390 not found! Check wiring.");
    while (1) delay(100);
  }

  bmp390.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp390.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp390.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp390.setOutputDataRate(BMP3_ODR_50_HZ);

  Serial.println("System ready 🚀");
}

void loop() {
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  mpu.accelUpdate();
  mpu.gyroUpdate();

  float ax = mpu.accelX();
  float ay = mpu.accelY();
  float az = mpu.accelZ();

  float gx = mpu.gyroX();
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);

  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;

  float altitude390 = 0;
  if (bmp390.performReading()) {
    altitude390 = bmp390.readAltitude(1013.25);
  }



  if (millis() - lastPrint > 500) { 
    lastPrint = millis();

    Serial.print("Accel: ");
    Serial.print(totalAccel);

    Serial.print(" | Gyro X: ");
    Serial.print(gx);

    Serial.print(" | Temp: ");
    Serial.print(temp);

    Serial.print(" C | Pressure: ");
    Serial.print(pressure);
    Serial.print(" hPa");

    Serial.print(" | Altitude: ");
    Serial.print(altitude390);
    Serial.print(" m");

    // GPS част
    Serial.print(" | Satellites: ");
    Serial.print(gps.satellites.value());
    
    if (gps.location.isValid()) {
      Serial.print(" | Lat: "); Serial.print(gps.location.lat(), 6);
      Serial.print(" | Lon: "); Serial.print(gps.location.lng(), 6);
      Serial.print(" | Speed: "); Serial.print(gps.speed.kmph()); Serial.print(" km/h");
    } else {
      Serial.print(" | GPS: Waiting...");
    }
    Serial.println();
  }


  if (!launched && totalAccel > 2.5) {
    launched = true;
    Serial.println("🚀 LAUNCH DETECTED!");
  }
}
