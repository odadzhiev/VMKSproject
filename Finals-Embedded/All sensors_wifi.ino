#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BMP3XX.h>
#include <TinyGPS++.h>
#include <math.h>

const char* ssid = "<Ime na WiFi>";
const char* password = "<Parola na WiFi>";

const char* serverUrl = "http://<res_laptop ip add>:3000/data";

MPU9250_asukiaaa mpu;
Adafruit_BMP280 bmp;
Adafruit_BMP3XX bmp390;
TinyGPSPlus gps;

typedef struct {
    float temperature;
    float pressure;
    float altitude;
    float totalAccel;
    float gyroX;
    float lat;
    float lng;
    float speed;
    int satellites;
} Data;

Data myData;

unsigned long lastSend = 0;
bool launched = false;

void sendToServer() {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(3000);

    String json = "{";
    json += "\"temperature\":" + String(myData.temperature) + ",";
    json += "\"pressure\":" + String(myData.pressure) + ",";
    json += "\"altitude\":" + String(myData.altitude) + ",";
    json += "\"accel\":" + String(myData.totalAccel) + ",";
    json += "\"gyroX\":" + String(myData.gyroX) + ",";
    json += "\"lat\":" + String(myData.lat, 6) + ",";
    json += "\"lng\":" + String(myData.lng, 6) + ",";
    json += "\"speed\":" + String(myData.speed) + ",";
    json += "\"satellites\":" + String(myData.satellites);
    json += "}";

    int code = http.POST(json);

    Serial.print("HTTP Code: ");
    Serial.println(code);

    http.end();
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, 44, 43); 

    Wire.begin(5, 6);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected");
    Serial.println(WiFi.localIP());

    mpu.setWire(&Wire);
    mpu.beginAccel();
    mpu.beginGyro();

    if (!bmp.begin(0x76)) {
        Serial.println("BMP390 failed");
    }

    if (!bmp390.begin_I2C(0x77, &Wire)) {
        Serial.println("BMP390 failed");
        while (1);
    }

    bmp390.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp390.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp390.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp390.setOutputDataRate(BMP3_ODR_50_HZ);

    Serial.println("System Ready");
}

void loop() {

    while (Serial2.available()) {
        gps.encode(Serial2.read());
    }

    mpu.accelUpdate();
    mpu.gyroUpdate();

    float ax = mpu.accelX();
    float ay = mpu.accelY();
    float az = mpu.accelZ();
    float gx = mpu.gyroX();

    myData.totalAccel = sqrt(ax * ax + ay * ay + az * az);
    myData.gyroX = gx;

    if (bmp390.performReading()) {
        myData.temperature = bmp390.temperature;
        myData.pressure = bmp390.pressure / 100.0;
        myData.altitude = bmp390.readAltitude(1013.25);
    }

    if (gps.location.isValid()) {
        myData.lat = gps.location.lat();
        myData.lng = gps.location.lng();
        myData.speed = gps.speed.kmph();
        myData.satellites = gps.satellites.value();
    } else {
        myData.lat = 0;
        myData.lng = 0;
        myData.speed = 0;
        myData.satellites = 0;
    }

    if (millis() - lastSend > 2000) {
        lastSend = millis();

        sendToServer();

        Serial.printf(
            "Temp:%.2f Pressure:%.2f Alt:%.2f Acc:%.2f Gyro:%.2f Lat:%.6f Lng:%.6f Speed:%.2f Sat:%d\n",
            myData.temperature,
            myData.pressure,
            myData.altitude,
            myData.totalAccel,
            myData.gyroX,
            myData.lat,
            myData.lng,
            myData.speed,
            myData.satellites
        );
    }
}