#include <WiFiManager.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

WiFiClient  client;
HTTPClient  http;
WiFiManager wm;

int   httpCode = 0;
float voltage  = 0;

// Основной таймер
unsigned long mainLoopTimer  = 0; 
unsigned long mainLoopPeriod = 1000;

// Таймер проверки сети
unsigned long networkCheckTimer  = 0; 
unsigned long networkCheckPeriod = 2000;

// Получение значения напряжения с АЦП
float getVoltage() {
    voltage = analogRead(A0) * (34.4 / 1023.0);
    return voltage;
}

// Отправка данных на сервер
void sendData() {
    voltage = getVoltage();
    http.begin(client, "http://endpoint.ru/api.php?device=wemos_d1_mini&voltage=" + String(voltage));
    http.addHeader("Content-Type", "text/plain");
    httpCode = http.GET();
}

void setup() {
    Serial.begin(115200);
    wm.setConfigPortalTimeout(60);
    
    bool res;
    res = wm.autoConnect("Wemos_Voltage_Tester");

    if(!res) {
        ESP.restart();
    }
}

void loop() {

    if (millis() - mainLoopTimer >= mainLoopPeriod) {
        mainLoopTimer = millis();
        getVoltage();
        sendData();
        // plotter
        Serial.println(voltage);  
    }

    if ((WiFi.status() != WL_CONNECTED) && millis() - networkCheckTimer >= networkCheckPeriod) {
        networkCheckTimer = millis();
        Serial.println("No network, restart board...");
        delay(100);
        ESP.restart();
    }
}
