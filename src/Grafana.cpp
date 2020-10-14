#include "grafana.h"
#include "config.h"

void Grafana::update()
{
    unsigned long now = millis();
    if (lastUpdate + 10000 > now)
    {
        return;
    }
    lastUpdate = now;

    String response;

    http.begin(GRAFANA_BASE "?db=ruuvi&q=SELECT%20last(%22temperature%22),%20last(%22humidity%22)%20FROM%20%22ruuvi_measurements%22%20WHERE%20(%22name%22%20%3D%20%27Outdoors%27)%20AND%20time%20%3E%3D%20now()%20-%201h");
    int httpCode = http.GET();
    yield();
    if (httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK)
        {
            response = http.getString();
            int this_index = response.indexOf("values");
            this_index = response.indexOf(",", this_index);
            int next_index = response.indexOf(",", this_index + 1);
            outdoorTemperature = response.substring(this_index + 1, next_index).toFloat();
            int end_index = response.indexOf("]", next_index + 1);
            outdoorHumidity = response.substring(next_index + 1, end_index).toFloat();
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}
