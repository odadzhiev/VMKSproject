#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BMP3XX.h>
#include <TinyGPS++.h>

uint8_t broadcastAddress[] = {0x10, 0xB4, 0x1D, 0xEB, 0x8B, 0x28};

typedef struct struct_message {
    float temperature;
    float pressure;
    float altitude;
    float totalAccel;
    float gyroX;
    float lat;
    float lng;
    float speed;
    int satellites;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

MPU9250_asukiaaa mpu;
Adafruit_BMP280 bmp;
Adafruit_BMP3XX bmp390;
TinyGPSPlus gps;

bool launched = false;
unsigned long lastSend = 0;

void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
    Serial.print("Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  }
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

    if (!bmp390.begin_I2C(0x77, &Wire)) {
        Serial.println("BMP390 not found! Check wiring.");
        while (1) delay(100);
    }
    bmp390.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp390.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp390.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp390.setOutputDataRate(BMP3_ODR_50_HZ);

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(OnDataSent);

    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    Serial.println("System Ready");
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

    if (!launched && totalAccel > 2.5) {
        launched = true;
    }

    if (bmp390.performReading()) {
        myData.temperature = bmp390.temperature;
        myData.pressure = bmp390.pressure / 100.0;
        myData.altitude = bmp390.readAltitude(1013.25);
    }

    myData.totalAccel = totalAccel;
    myData.gyroX = gx;
    myData.satellites = gps.satellites.value();
    
    if (gps.location.isValid()) {
        myData.lat = gps.location.lat();
        myData.lng = gps.location.lng();
        myData.speed = gps.speed.kmph();
    } else {
        myData.lat = 0;
        myData.lng = 0;
        myData.speed = 0;
    }

    if (millis() - lastSend > 1500) {
        lastSend = millis();
        
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

        Serial.printf("Sent -> Temp: %.2f  | Acc: %.2f | GyroX: %.2f | Alt: %.2f | Pres: %.2f | Lat: %.6f | Lon: %.6f | Spd: %.2f | Sat: %d\n", 
                      myData.temperature, myData.totalAccel, myData.gyroX, myData.altitude, myData.pressure, myData.lat, myData.lng, myData.speed, myData.satellites);
    }
}
