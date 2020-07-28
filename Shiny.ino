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
    String url = "https://api.varipass.org/?action=sgraph&eink=true&";
    if (key != NULL)
        url += "key=" + key;
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
    display.setTextSize(1);
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
    display.setCursor(1, 1);
    display.println("WiFI: " + String(nameWiFi));
    display.display();
}

void loop() {
    if (counter >= UPDATE_INTERVAL - 1) {
        counter = 0;

        display.fillRect(370, 1, 70, 8, WHITE);
        display.setCursor(370, 1);
        display.println("Loading...");
        display.drawRect(697, 1, 102, 8, BLACK);
        display.fillRect(698, 2, 100, 6, BLACK);
        display.partialUpdate();

        drawGraph(VARIPASS_KEY, VARIPASS_ID_TEMPERATURE, 0,   10, 400, 590);
        drawGraph(VARIPASS_KEY, VARIPASS_ID_HUMIDITY,    400, 10, 400, 590);

        display.fillRect(370, 1, 70, 8, WHITE);
        int result;
        double bat = varipassReadFloat(VARIPASS_KEY, VARIPASS_ID_BATTERY, &result);
        if (result == VARIPASS_RESULT_SUCCESS) {
            display.setCursor(370, 1);
            display.println("Bat: " + String(bat, 1) + "%");
        }

        display.drawRect(697, 1, 102, 8, BLACK);
        display.fillRect(698, 2, 100, 6, WHITE);
        display.display();
    }
    else {
        counter++;

        int perc = ((float)counter / UPDATE_INTERVAL) * 100;

        display.drawRect(697, 1, 102, 8, BLACK);
        display.fillRect(698, 2, perc, 6, BLACK);
        display.fillRect(698+perc, 2, 100-perc, 6, WHITE);
        display.partialUpdate();
    }
    delay (1000);
}
