#include "camera.h"
#include "esp_camera.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"

int imageCount = 1;
bool camera_sign = false;
bool sd_sign = false;


void writeFile(fs::FS &fs, const char * path, uint8_t * data, size_t len){
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("File open failed");
        return;
    }

    if(file.write(data, len) != len){
        Serial.println("Write failed");
    }

    file.close();
}


void takePhoto() {
    if(!camera_sign || !sd_sign) return;

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return;
    }

    char filename[32];
    sprintf(filename, "/img_%d.jpg", imageCount++);

    writeFile(SD, filename, fb->buf, fb->len);

    Serial.printf("Saved: %s\n", filename);

    esp_camera_fb_return(fb);
}


void initCameraSD() {
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size = FRAMESIZE_VGA;   //  UXGA 
    config.jpeg_quality = 15;
    config.fb_count = 1;

    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("Camera init failed");
        return;
    }

    camera_sign = true;

    if(!SD.begin(21)){
        Serial.println("SD failed");
        return;
    }

    if(SD.cardType() == CARD_NONE){
        Serial.println("No SD card");
        return;
    }

    sd_sign = true;

    Serial.println("Camera + SD ready");
}