#include <Inkplate.h>
#include <TwiFi.h>
#include <VariPass.h>

#include "Configuration.h"
#include "ConfigurationWiFi.h"
#include "ConfigurationVariPass.h"

Inkplate display(INKPLATE_1BIT);
String nameWiFi;
int counter = UPDATE_INTERVAL;

String getGraphUrl(String key, String id, int width, int height);
void drawGraph(String key, String id, int x, int y, int width, int height);

void connectAttempt(int idEntry, int attempt);
void connectSuccess(int idEntry);
void connectFail(int idEntry);

String getGraphUrl(String key, String id, int width, int height) {
    String url = "https://api.varipass.org/?action=sgraph&eink=true&thick=true";
    if (key != NULL)
        url += "&key=" + key;
    url += "&id=" + id;
    url += "&width=" + String(width);
    url += "&height=" + String(height);

    return url;
}

void drawGraph(String key, String id, int x, int y, int width, int height) {
    String url = getGraphUrl(key, id, width, height);
    char buf[256];
    url.toCharArray(buf, url.length()+1);
    display.drawBitmapFromWeb(buf, x, y);    
}

void connectAttempt(int idEntry, int attempt) {
    if (attempt == 1) {
        display.print("Connecting to WiFi: " + String(wifis[idEntry].ssid) + "...");
        display.partialUpdate();
    }
    else {
        display.print(".");
        display.partialUpdate();
    }
}

void connectSuccess(int idEntry) {
    nameWiFi = String(wifis[idEntry].ssid);
    display.println("\nConnected!");
    display.partialUpdate();
}

void connectFail(int idEntry) {
    display.println("\nFailed. Trying next.");
    display.partialUpdate();
}

void setup() {
    display.begin();
    display.setTextSize(2);
    display.clearDisplay();
    display.display();

    twifiInit(
        wifis,
        WIFI_COUNT,
        WIFI_HOST,
        WIFI_TIMEOUT,
        &connectAttempt,
        &connectSuccess,
        &connectFail,
        WIFI_DEBUG
        );
    twifiConnect(true);

    display.clearDisplay();
    display.setCursor(2, 2);
    display.println("WiFI: " + String(nameWiFi));
    display.display();
}

void loop() {
    if (counter >= UPDATE_INTERVAL - 1) {
        counter = 0;

        display.fillRect(340, 2, 140, 16, WHITE);
        display.setTextSize(2);
        display.setCursor(340, 2);
        display.println("Loading...");
        display.drawRect(696, 2, 102, 16, BLACK);
        display.fillRect(697, 3, 100, 14, BLACK);
        display.partialUpdate();

        drawGraph(VARIPASS_KEY, VARIPASS_ID_TEMPERATURE, 0,   20, 400, 580);
        drawGraph(VARIPASS_KEY, VARIPASS_ID_HUMIDITY,    400, 20, 400, 580);

        display.fillRect(340, 2, 140, 16, WHITE);
        int result;
        double bat = varipassReadFloat(VARIPASS_KEY, VARIPASS_ID_BATTERY, &result);
        if (result == VARIPASS_RESULT_SUCCESS) {
            display.setCursor(340, 2);
            display.println("Bat: " + String(bat, 1) + "%");
        }

        display.drawRect(696, 2, 102, 16, BLACK);
        display.fillRect(697, 3, 100, 14, WHITE);
        display.display();
    }
    else {
        counter++;

        int perc = ((float)counter / UPDATE_INTERVAL) * 100;

        display.drawRect(696, 2, 102, 16, BLACK);
        display.fillRect(697, 3, perc, 14, BLACK);
        display.fillRect(697+perc, 3, 100-perc, 14, WHITE);
        display.partialUpdate();
    }
    delay (1000);
}
