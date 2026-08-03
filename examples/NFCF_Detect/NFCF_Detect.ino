/*
  M5UnitNFC-M5Stack Library Example
  NFC-F (FeliCa) IDm Reader Example
*/

#include <M5Stack.h>
#include <Wire.h>
#include <M5UnitUnifiedNFC.h>

namespace {
m5::unit::UnitNFC nfcUnit{};
m5::nfc::NFCLayerF nfcF{nfcUnit};
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

    Serial.println("\n=== M5UnitNFC: NFC-F (FeliCa) IDm Reader ===");

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("NFC-F Reader");
    M5.Lcd.println("Initializing...");

    // Configure unit for NFC-F
    auto cfg = nfcUnit.config();
    cfg.mode = m5::nfc::NFC::F;
    nfcUnit.config(cfg);

    if (!nfcUnit.begin()) {
        Serial.println("[ERROR] nfcUnit.begin() Failed!");
        M5.Lcd.setTextColor(RED);
        M5.Lcd.println("Init Failed!");
        while (1) delay(1000);
    }

    Serial.println("[NFC-F Reader] Unit ST25R3916 initialized successfully!");
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
        M5.Lcd.println("NFC-F (FeliCa) Reader");

        M5.Lcd.setCursor(20, 110);
        M5.Lcd.setTextColor(YELLOW);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("Waiting for Card...");
        M5.Lcd.setCursor(20, 140);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(1);
        M5.Lcd.println("Touch Suica/PASMO/FeliCa card to Unit");
        cleared = true;
    }
}

void loop() {
    M5.update();
    nfcUnit.update();

    m5::nfc::f::PICC picc{};

    bool detected = nfcF.polling(
        picc, 
        m5::nfc::f::system_code_wildcard, 
        m5::nfc::f::RequestCode::None, 
        m5::nfc::f::TimeSlot::Slot1
    );

    if (detected) {
        Serial.println("\n------------------------------------------");
        Serial.println(" [SUCCESS] NFC-F PICC Card Detected!");
        Serial.printf("   IDm: %s\n", picc.idmAsString().c_str());
        Serial.printf("   PMm: %s\n", picc.pmmAsString().c_str());
        Serial.println("------------------------------------------");

        // M5Stack LCD UI Render
        M5.Lcd.fillScreen(BLACK);
        
        // Top Header
        M5.Lcd.fillRect(0, 0, 320, 35, GREEN);
        M5.Lcd.setCursor(15, 8);
        M5.Lcd.setTextColor(BLACK);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("NFC-F CARD DETECTED!");

        // IDm Display Block
        M5.Lcd.drawRect(5, 45, 310, 85, YELLOW);
        M5.Lcd.setCursor(15, 55);
        M5.Lcd.setTextColor(YELLOW);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("IDm (ID):");
        
        M5.Lcd.setCursor(15, 85);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(3);
        M5.Lcd.println(picc.idmAsString().c_str());

        // PMm Display Block
        M5.Lcd.drawRect(5, 140, 310, 85, CYAN);
        M5.Lcd.setCursor(15, 150);
        M5.Lcd.setTextColor(CYAN);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("PMm (Parameter):");
        
        M5.Lcd.setCursor(15, 180);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println(picc.pmmAsString().c_str());

        delay(2000);
        drawStandbyScreen();
    } else {
        drawStandbyScreen();
    }

    delay(200);
}
