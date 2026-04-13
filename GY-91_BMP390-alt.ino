#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BMP3XX.h>

MPU9250_asukiaaa mpu;
Adafruit_BMP280 bmp280;
Adafruit_BMP3XX bmp390;

bool launched = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  mpu.setWire(&Wire);
  mpu.beginAccel();
  mpu.beginGyro();

  if (!bmp280.begin(0x76)) {
    Serial.println("BMP280 error!");
  }

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
  // MPU (GY-91)
  mpu.accelUpdate();
  mpu.gyroUpdate();

  float ax = mpu.accelX();
  float ay = mpu.accelY();
  float az = mpu.accelZ();
  float gx = mpu.gyroX();
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);

  float temp = bmp280.readTemperature();
  float pressure = bmp280.readPressure() / 100.0;

  float altitude390 = 0;
  if (bmp390.performReading()) {
    altitude390 = bmp390.readAltitude(1013.25);
  }

  Serial.print("Accel: ");
  Serial.print(totalAccel);

  Serial.print(" | Gyro X: ");
  Serial.print(gx);

  Serial.print(" | Temp: ");
  Serial.print(temp);

  Serial.print(" C | Pressure: ");
  Serial.print(pressure);
  Serial.print(" hPa");

  // ТОВА Е САМО ОТ BMP390
  Serial.print(" | Altitude: ");
  Serial.print(altitude390);
  Serial.println(" m");

  // ЛОГИКА ИЗСТРЕЛВАНЕ
  if (!launched && totalAccel > 2.5) {
    launched = true;
    Serial.println("🚀 LAUNCH DETECTED!");
  }

  delay(100);
}
