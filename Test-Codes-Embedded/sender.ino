#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP3XX.h>
#include <EspNowCam.h>

uint8_t broadcastAddress[] = {0x10, 0xB4, 0x1D, 0xEB, 0x8B, 0x28};

typedef struct struct_message {
    float temperature;
    float pressure;
    float altitude;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;
Adafruit_BMP3XX bmp;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);

    if (!bmp.begin_I2C(0x77, &Wire)) {
        Serial.println("BMP390 not found! Check wiring.");
        while (1) delay(100);
    }

    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
    Serial.println("BMP390 ready!");


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
}

void loop() {
    if (!bmp.performReading()) {
        Serial.println("BMP390 read failed");
        return;
    }

    myData.temperature = bmp.temperature;
    myData.pressure    = bmp.pressure / 100.0;
    myData.altitude    = bmp.readAltitude(1013.25);

    Serial.printf("Temp: %.2f °C | Pressure: %.2f hPa | Altitude: %.2f m\n",
                  myData.temperature, myData.pressure, myData.altitude);

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK) {
        Serial.println("Sent with success");
    } else {
        Serial.println("Error sending the data");
    }

    delay(2000);
}