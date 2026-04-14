#include "WebServer.h"
#include "WiFi.h"
#include "esp32cam.h"

const char* WIFI_SSID = "Knet";
const char* WIFI_PASS = "19062008";
const char* URL = "/stream";
const auto RESOLUTION = esp32cam::Resolution::find(800, 600);
const int FRAMERATE = 10;

WebServer server(80);

void handleStream() {
  static char head[128];
  WiFiClient client = server.client();

  server.sendContent("HTTP/1.1 200 OK\r\n"
                     "Content-Type: multipart/x-mixed-replace; "
                     "boundary=frame\r\n\r\n");

  while (client.connected()) {
    auto frame = esp32cam::capture();
    if (frame) {
      sprintf(head,
              "--frame\r\n"
              "Content-Type: image/jpeg\r\n"
              "Content-Length: %ul\r\n\r\n",
              frame->size());
      client.write(head, strlen(head));
      frame->writeTo(client);
      client.write("\r\n");
      delay(1000 / FRAMERATE);
    }
  }
}

void initCamera() {
  using namespace esp32cam;
  Config cfg;
  cfg.setPins(pins::XiaoSense);
  cfg.setResolution(RESOLUTION);
  cfg.setBufferCount(2);
  cfg.setJpeg(80);
  Camera.begin(cfg);
}

void initWifi() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.printf("Stream at: http://%s%s\n",
                WiFi.localIP().toString().c_str(), URL);
}

void initServer() {
  server.on(URL, handleStream);
  server.begin();
}

void setup() {
  Serial.begin(115200);
  initWifi();
  initCamera();
  initServer();
}

void loop() {
  server.handleClient();
}
