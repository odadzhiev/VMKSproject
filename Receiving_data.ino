#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
    float temperature;
    float pressure;
    float altitude;
} struct_message;

struct_message myData;

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Temperature: ");
    Serial.println(myData.temperature);
    Serial.print("Pressure: ");
    Serial.println(myData.pressure);
    Serial.print("Altitude: ");
    Serial.println(myData.altitude);
    
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