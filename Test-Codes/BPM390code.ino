#include <Wire.h>
#include <Adafruit_BMP3XX.h>

Adafruit_BMP3XX bmp;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin(21, 22);

  if (!bmp.begin_I2C(0x77, &Wire)) {  // 0x77 = SDO high
    Serial.println("BMP390 not found! Check wiring.");
    while (1) delay(100);
  }

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  Serial.println("BMP390 ready!");
}

void loop() {
  if (!bmp.performReading()) {
    Serial.println("Read failed");
    return;
  }

  Serial.printf("Temp:     %.2f °C\n",  bmp.temperature);
  Serial.printf("Pressure: %.2f hPa\n", bmp.pressure / 100.0);
  Serial.printf("Altitude: %.2f m\n\n", bmp.readAltitude(1013.25));

  delay(2000);
}