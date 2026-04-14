#include <esp_now.h>
#include <WiFi.h>

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

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes: ");
    Serial.println(len);
    Serial.print("Temp: ");
    Serial.println(myData.temperature);
    Serial.print("Pressure: ");
    Serial.println(myData.pressure);
    Serial.print("Alt: ");
    Serial.println(myData.altitude);
    Serial.print("Acc: ");
    Serial.println(myData.totalAccel);
    Serial.print("Gyro: ");
    Serial.println(myData.gyroX);
    Serial.print("Latitude: ");
    Serial.println(myData.lat);
    Serial.print("Longtitute: ");
    Serial.println(myData.lng);
    Serial.print("Speed: ");
    Serial.println(myData.speed);
    Serial.print("Sat: ");
    Serial.println(myData.satellites);
    
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

}