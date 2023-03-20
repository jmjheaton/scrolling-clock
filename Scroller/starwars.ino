#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const uint16_t PANEL_RES_X = 64;
const uint16_t PANEL_RES_Y = 64;
const uint16_t PANEL_CHAIN = 1;

const char* ssid = "NETGEAR04";
const char* password = "icypiano224";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 1 * 60 * 60, 60000);

MatrixPanel_I2S_DMA *dma_display = nullptr;

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi.");

  // Set up NTP client to get time from the internet
  timeClient.begin();

  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,
    PANEL_RES_Y,
    PANEL_CHAIN
  );
  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
}

int16_t scrollY = PANEL_RES_Y;

void loop() {
  timeClient.update(); // Update the time from the internet
  int currentHour = (timeClient.getHours() + 24 - 8) % 12; // Convert UTC to Pacific Standard Time
  int currentMinute = timeClient.getMinutes();

  // Clear the display
  dma_display->fillScreen(0);

  // Define clock colors
  uint16_t colors[] = {
    dma_display->color565(255, 0, 0),
    dma_display->color565(0, 255, 0),
    dma_display->color565(0, 0, 255),
    dma_display->color565(255, 0, 255)
  };

  // Draw the digital clock displays
  dma_display->setTextSize(2);
  for (int i = 0; i < 4; i++) {
    int clockIndex = (i + (scrollY / 18)) % 4;
    int yOffset = i * 18 - (scrollY % 18);
    dma_display->setTextColor(colors[clockIndex]);
    dma_display->setCursor(PANEL_RES_X / 2 - 30, yOffset);
    dma_display->printf("%02d:%02d", currentHour, currentMinute);
  }

  // Update the scroll position
  scrollY += 1;
  if (scrollY >= 4 * 18) {
    scrollY = 0;
  }

  // Refresh the display
  dma_display->flipDMABuffer();
  delay(100);
}
