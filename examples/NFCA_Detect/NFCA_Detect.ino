/*
  M5UnitNFC-M5Stack Library Example
  NFCA Detect Example
  
  Detects NFC-A (ISO14443A / MIFARE / NTAG) cards and prints UID and type.
*/

#include <M5Stack.h>
#include <Wire.h>
#include <M5UnitUnifiedNFC.h>

namespace {
m5::unit::UnitNFC nfcUnit{};
m5::nfc::NFCLayerA nfcA{nfcUnit};
}

void setup() {
    M5.begin();
    M5.Power.begin();

    // ---- I2C Bus Clear / Recovery Routine for SDA Stuck LOW ----
    pinMode(21, INPUT_PULLUP);
    pinMode(22, OUTPUT);
    for (int i = 0; i < 10; ++i) {
        digitalWrite(22, LOW);
        delayMicroseconds(10);
        digitalWrite(22, HIGH);
        delayMicroseconds(10);
    }
    pinMode(21, INPUT);
    pinMode(22, INPUT);
    // -------------------------------------------------------------

    Wire.begin(); // Standard I2C Grove Port (SDA:21, SCL:22)

    Serial.begin(115200);
    delay(500);
    Serial.println(CORE_DEBUG_LEVEL);

    Serial.println("\n=== M5UnitNFC: NFC-A (ISO14443A / MIFARE / NTAG) Reader ===");

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("NFC-A Reader");
    M5.Lcd.println("Initializing...");

    // Configure unit for NFC-A
    auto cfg = nfcUnit.config();
    cfg.mode = m5::nfc::NFC::A;
    nfcUnit.config(cfg);

    if (!nfcUnit.begin()) {
        Serial.println("[ERROR] Failed to initialize UnitNFC (ST25R3916)");
        M5.Lcd.setTextColor(RED);
        M5.Lcd.println("Init Failed!");
        while (1) delay(1000);
    }

    Serial.println("[NFC-A Reader] Unit ST25R3916 initialized successfully!");
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.println("Ready!");
}

void drawStandbyScreen() {
    static bool cleared = false;
    if (!cleared) {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.fillRect(0, 0, 320, 30, BLUE);
        M5.Lcd.setCursor(10, 6);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("NFC-A Reader");

        M5.Lcd.setCursor(20, 110);
        M5.Lcd.setTextColor(YELLOW);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("Waiting for Card...");
        M5.Lcd.setCursor(20, 140);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(1);
        M5.Lcd.println("Touch MIFARE / NTAG / NFC-A tag to Unit");
        cleared = true;
    }
}

void loop() {
    M5.update();
    nfcUnit.update();

    std::vector<m5::nfc::a::PICC> piccs;
    
    // Attempt detection of NFC-A PICC
    if (nfcA.detect(piccs, 500U)) {
        for (auto& picc : piccs) {
            Serial.println("\n------------------------------------------");
            Serial.printf(" [DETECTED] NFC-A Card Found!\n");
            Serial.printf("   UID:  %s\n", picc.uidAsString().c_str());
            Serial.printf("   ATQA: 0x%04X, SAK: 0x%02X\n", picc.atqa, picc.sak);

            if (nfcA.identify(picc)) {
                Serial.printf("   Type: %s\n", picc.typeAsString().c_str());
                Serial.printf("   User Memory: %u bytes\n", picc.userAreaSize());
            } else {
                Serial.printf("   Type: Unknown (SAK: 0x%02X)\n", picc.sak);
            }
            Serial.println("------------------------------------------");

            // M5Stack LCD UI Render
            M5.Lcd.fillScreen(BLACK);
            
            // Top Header
            M5.Lcd.fillRect(0, 0, 320, 35, GREEN);
            M5.Lcd.setCursor(15, 8);
            M5.Lcd.setTextColor(BLACK);
            M5.Lcd.setTextSize(2);
            M5.Lcd.println("NFC-A CARD DETECTED!");

            // UID Display Block
            M5.Lcd.drawRect(5, 45, 310, 85, YELLOW);
            M5.Lcd.setCursor(15, 55);
            M5.Lcd.setTextColor(YELLOW);
            M5.Lcd.setTextSize(2);
            M5.Lcd.println("UID:");
            
            M5.Lcd.setCursor(15, 85);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(3);
            M5.Lcd.println(picc.uidAsString().c_str());

            // Type Display Block
            M5.Lcd.drawRect(5, 140, 310, 85, CYAN);
            M5.Lcd.setCursor(15, 150);
            M5.Lcd.setTextColor(CYAN);
            M5.Lcd.setTextSize(2);
            M5.Lcd.println("Type / SAK:");
            
            M5.Lcd.setCursor(15, 180);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(2);
            if (nfcA.identify(picc)) {
                M5.Lcd.println(picc.typeAsString().c_str());
            } else {
                M5.Lcd.printf("SAK:0x%02X\n", picc.sak);
            }
        }

        // Deactivate field after enumeration
        nfcA.deactivate();
        delay(2000);
        drawStandbyScreen();
    } else {
        drawStandbyScreen();
    }

    delay(100);
}
