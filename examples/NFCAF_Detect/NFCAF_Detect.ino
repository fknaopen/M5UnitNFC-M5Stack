/*
  M5UnitNFC-M5Stack Library Example
  NFC-F (FeliCa) & NFC-A (MIFARE/NTAG) Dual Reader Example
  
  Detects NFC-F first, then NFC-A sequentially.
*/

#include <M5Stack.h>
#include <Wire.h>
#include <M5UnitUnifiedNFC.h>

namespace {
m5::unit::UnitNFC nfcUnit{};
m5::nfc::NFCLayerF nfcF{nfcUnit};
m5::nfc::NFCLayerA nfcA{nfcUnit};
}

bool needsClear = true; // Flag to update screen drawing

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

    Serial.println("\n=== M5UnitNFC: NFC-F & NFC-A Dual Reader ===");

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("NFC Dual Reader");
    M5.Lcd.println("Initializing...");

    if (!nfcUnit.begin()) {
        Serial.println("[ERROR] nfcUnit.begin() Failed!");
        M5.Lcd.setTextColor(RED);
        M5.Lcd.println("Init Failed!");
        while (1) delay(1000);
    }

    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.println("Ready!");
    delay(1000);
}

void drawStandbyScreen() {
    if (needsClear) {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.fillRect(0, 0, 320, 30, BLUE);
        M5.Lcd.setCursor(10, 6);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("NFC-F & NFC-A Reader");

        M5.Lcd.setCursor(20, 110);
        M5.Lcd.setTextColor(YELLOW);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("Waiting for Card...");
        M5.Lcd.setCursor(20, 140);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(1);
        M5.Lcd.println("Touch Suica/FeliCa or MIFARE/NTAG");
        needsClear = false;
    }
}

// Helper function to switch modes and initialize
bool switchMode(m5::nfc::NFC mode) {
    if (nfcUnit.isNFCMode(mode)) {
        return true; // Do nothing if already in the same mode
    }
    
    // Turn off RF field properly before switching modes
    // This prevents the "Already tx_en" error when returning from Mode A
    nfcUnit.disableField();

    // Update config to match the new mode
    auto cfg = nfcUnit.config();
    cfg.mode = mode;
    nfcUnit.config(cfg);
    
    // Fast switch without full hardware reset
    bool ok = nfcUnit.configureNFCMode(mode);
    if (ok) {
        // Provide Guard Time for cards to boot up (FeliCa requires at least 20ms of unmodulated field)
        delay(25);
    }
    return ok;
}

void loop() {
    M5.update();
    nfcUnit.update();

    bool cardFound = false;

    // ==========================================
    // 1. NFC-F (FeliCa) Detection
    // ==========================================
    if (switchMode(m5::nfc::NFC::F)) {
        m5::nfc::f::PICC piccF{};
        if (nfcF.polling(piccF, m5::nfc::f::system_code_wildcard, m5::nfc::f::RequestCode::None, m5::nfc::f::TimeSlot::Slot1)) {
            cardFound = true;
            Serial.println("\n------------------------------------------");
            Serial.println(" [DETECTED] NFC-F (FeliCa) Card Found!");
            Serial.printf("   IDm: %s\n", piccF.idmAsString().c_str());
            Serial.printf("   PMm: %s\n", piccF.pmmAsString().c_str());
            Serial.println("------------------------------------------");

            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.fillRect(0, 0, 320, 35, GREEN);
            M5.Lcd.setCursor(15, 8);
            M5.Lcd.setTextColor(BLACK);
            M5.Lcd.setTextSize(2);
            M5.Lcd.println("NFC-F CARD DETECTED!");

            M5.Lcd.drawRect(5, 45, 310, 85, YELLOW);
            M5.Lcd.setCursor(15, 55);
            M5.Lcd.setTextColor(YELLOW);
            M5.Lcd.setTextSize(2);
            M5.Lcd.println("IDm:");
            
            M5.Lcd.setCursor(15, 85);
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(3);
            M5.Lcd.println(piccF.idmAsString().c_str());
        }
    }

    // ==========================================
    // 2. NFC-A (MIFARE/NTAG) Detection (Only if NFC-F is not found)
    // ==========================================
    if (!cardFound && switchMode(m5::nfc::NFC::A)) {
        std::vector<m5::nfc::a::PICC> piccsA;
        // Set a short timeout (200ms) for quicker switching back to F
        if (nfcA.detect(piccsA, 200U)) { 
            for (auto& picc : piccsA) {
                cardFound = true;
                Serial.println("\n------------------------------------------");
                Serial.printf(" [DETECTED] NFC-A Card Found!\n");
                Serial.printf("   UID:  %s\n", picc.uidAsString().c_str());
                Serial.printf("   ATQA: 0x%04X, SAK: 0x%02X\n", picc.atqa, picc.sak);

                if (nfcA.identify(picc)) {
                    Serial.printf("   Type: %s\n", picc.typeAsString().c_str());
                } else {
                    Serial.printf("   Type: Unknown\n");
                }
                Serial.println("------------------------------------------");

                M5.Lcd.fillScreen(BLACK);
                M5.Lcd.fillRect(0, 0, 320, 35, GREEN);
                M5.Lcd.setCursor(15, 8);
                M5.Lcd.setTextColor(BLACK);
                M5.Lcd.setTextSize(2);
                M5.Lcd.println("NFC-A CARD DETECTED!");

                M5.Lcd.drawRect(5, 45, 310, 85, YELLOW);
                M5.Lcd.setCursor(15, 55);
                M5.Lcd.setTextColor(YELLOW);
                M5.Lcd.setTextSize(2);
                M5.Lcd.println("UID:");
                
                M5.Lcd.setCursor(15, 85);
                M5.Lcd.setTextColor(WHITE);
                M5.Lcd.setTextSize(3);
                M5.Lcd.println(picc.uidAsString().c_str());

                break; // Show the first card and break
            }
            nfcA.deactivate(); // Ensure deactivation after detection
        }
    }

    // ==========================================
    // Display Update and Wait
    // ==========================================
    if (cardFound) {
        delay(3000); // Show result for 3 seconds
        needsClear = true; // Request standby screen redraw
    } else {
        drawStandbyScreen();
    }

    delay(50); // Polling interval
}
