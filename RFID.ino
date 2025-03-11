#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5
#define RST_PIN 4

const char* ssid = "POCO F4"; 
const char* password = "masbro12";
const char* serverURL = "http://192.168.131.228/pjbl/connect_rfid.php";

MFRC522 mfrc522(SS_PIN, RST_PIN);
WiFiClient client;

void setup() {
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");

    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("RFID Scanner Ready...");
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid += String(mfrc522.uid.uidByte[i]);
    }
    Serial.println("UID: " + uid);

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(client, serverURL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String postData = "uid=" + uid;
        Serial.print("Mengirim data ke: ");
        Serial.println(serverURL);
        Serial.println("Data: " + postData);
        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Response: " + response);
        } else {
            Serial.print("WiFi Status: ");
            Serial.println(WiFi.status());

            Serial.print("Error: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi tidak terhubung!");
    }
}
