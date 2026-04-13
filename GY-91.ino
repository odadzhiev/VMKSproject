#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>

MPU9250_asukiaaa mpu;
Adafruit_BMP280 bmp;

bool launched = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // MPU
  mpu.setWire(&Wire);
  mpu.beginAccel();
  mpu.beginGyro();

  // BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 error!");
  }

  Serial.println("System ready 🚀");
}

void loop() {
  // === MPU ===
  mpu.accelUpdate();
  mpu.gyroUpdate();

  float ax = mpu.accelX();
  float ay = mpu.accelY();
  float az = mpu.accelZ();

  float gx = mpu.gyroX();

  // общо ускорение
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);

  // === BMP ===
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;

  // === PRINT ===
  Serial.print("Accel: ");
  Serial.print(totalAccel);

  Serial.print(" | Gyro X: ");
  Serial.print(gx);

  Serial.print(" | Temp: ");
  Serial.print(temp);

  Serial.print(" C | Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  if (!launched && totalAccel > 2.5) {
    launched = true;
    Serial.println("🚀 LAUNCH DETECTED!");
  }

  delay(100);
}