#include <HTTPClient.h>

class Grafana
{
public:
    float outdoorTemperature;
    float outdoorHumidity;

    void update();

private:
    HTTPClient http;
    unsigned long lastUpdate = 0;
};
