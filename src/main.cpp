#include <ESP32-RGB64x32MatrixPanel-I2S-DMA.h>
#include <Wire.h> // https://community.platformio.org/t/adafruit-gfx-lib-will-not-build-any-more-pio-5/15776/6
#include <SPI.h>  // https://community.platformio.org/t/adafruit-gfx-lib-will-not-build-any-more-pio-5/15776/6
#include <WiFi.h>
#include <ezTime.h>
#include <Fonts/TomThumb.h>

#include "config.h"
#include "Grafana.h"

RGB64x32MatrixPanel_I2S_DMA panel(true);
Timezone tz;
Grafana grafana;

long long frame = 0;
unsigned long t = 0;
float fps = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting");
  panel.begin();
  panel.setFont(&TomThumb);
  panel.setTextWrap(false);

  // wifi
  panel.flipDMABuffer();
  panel.clearScreen();
  panel.setCursor(1, 6);
  panel.setTextColor(panel.color565(64, 0, 0));
  panel.print("Connecting");
  panel.showDMABuffer();

  WiFi.setHostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++)
  {
    delay(100);
  }

  // ntp
  panel.flipDMABuffer();
  panel.clearScreen();
  panel.setCursor(1, 6);
  panel.setTextColor(panel.color565(64, 0, 0));
  panel.print("Synchronizing NTP");
  panel.showDMABuffer();

  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  tz.setPosix("EET-2EEST,M3.5.0/3,M10.5.0/4");
  Serial.println("Local time: " + tz.dateTime());

  // initial update
  panel.flipDMABuffer();
  panel.clearScreen();
  panel.setCursor(1, 6);
  panel.setTextColor(panel.color565(64, 0, 0));
  panel.print("Loading data");
  panel.showDMABuffer();

  grafana.update();
}

void loop()
{
  unsigned long now = millis();
  if (t + 200 < now)
  {
    fps = frame * 1000.f / (now - t);
    t = now;
    frame = 0;
  }
  grafana.update();
  panel.flipDMABuffer();
  panel.clearScreen();
  panel.setTextColor(panel.color565(64, 0, 0));
  panel.setCursor(1, 6);
  panel.printf("%s", tz.dateTime("H:i:s").c_str());
  panel.setCursor(1, 13);
  panel.printf("%.2f C  %.2f %%", grafana.outdoorTemperature, grafana.outdoorHumidity);
  panel.setCursor(1, 31);
  panel.printf("%.2f fps", fps);
  panel.showDMABuffer();
  frame++;
}
