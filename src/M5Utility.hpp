#ifndef M5_UTILITY_HPP
#define M5_UTILITY_HPP

#include <Arduino.h>
#include <array>
#include <m5_utility/stl/extension.hpp>

#ifndef CORE_DEBUG_LEVEL
#define CORE_DEBUG_LEVEL 0
#endif

#if CORE_DEBUG_LEVEL >= 1
#define M5_LIB_LOGE(fmt, ...) Serial.printf("[NFC LIB ERR] " fmt "\n", ##__VA_ARGS__)
#define M5_DUMPE(buf, len) m5::utility::log::dump(buf, len)
#else
#define M5_LIB_LOGE(fmt, ...)
#define M5_DUMPE(buf, len)
#endif

#if CORE_DEBUG_LEVEL >= 2
#define M5_LIB_LOGW(fmt, ...) Serial.printf("[NFC LIB WRN] " fmt "\n", ##__VA_ARGS__)
#define M5_DUMPW(buf, len) m5::utility::log::dump(buf, len)
#else
#define M5_LIB_LOGW(fmt, ...)
#define M5_DUMPW(buf, len)
#endif

#if CORE_DEBUG_LEVEL >= 3
#define M5_LIB_LOGI(fmt, ...) Serial.printf("[NFC LIB INF] " fmt "\n", ##__VA_ARGS__)
#define M5_DUMPI(buf, len) m5::utility::log::dump(buf, len)
#else
#define M5_LIB_LOGI(fmt, ...)
#define M5_DUMPI(buf, len)
#endif

#if CORE_DEBUG_LEVEL >= 4
#define M5_LIB_LOGD(fmt, ...) Serial.printf("[NFC LIB DBG] " fmt "\n", ##__VA_ARGS__)
#define M5_DUMPD(buf, len) m5::utility::log::dump(buf, len)
#else
#define M5_LIB_LOGD(fmt, ...)
#define M5_DUMPD(buf, len)
#endif

#if CORE_DEBUG_LEVEL >= 5
#define M5_LIB_LOGV(fmt, ...) Serial.printf("[NFC LIB VRB] " fmt "\n", ##__VA_ARGS__)
#define M5_DUMPV(buf, len) m5::utility::log::dump(buf, len)
#else
#define M5_LIB_LOGV(fmt, ...)
#define M5_DUMPV(buf, len)
#endif

namespace m5 {

namespace stl {
    template <typename T>
    inline T byteswap(T value) { return value; }
}

namespace utility {

namespace log {
    inline void dump(const uint8_t* buf, size_t len, bool = false) {
        if (!buf || !len) return;
        Serial.print("[NFC DUMP] ");
        for (size_t i = 0; i < len; ++i) {
            Serial.printf("%02X ", buf[i]);
        }
        Serial.println();
    }
}

inline uint32_t millis() {
    return ::millis();
}

inline void delay(uint32_t ms) {
    ::delay(ms);
}

inline void delayMicroseconds(uint32_t us) {
    ::delayMicroseconds(us);
}

namespace mmh3 {
    constexpr uint32_t operator"" _mmh3(const char* str, size_t len) {
        return 0; // Dummy implementation
    }
}

namespace crypto {
    class TripleDES {
    public:
        using Key24 = std::array<uint8_t, 24>;
        using Key16 = std::array<uint8_t, 16>;
        enum class Mode { CBC };
        enum class Padding { None };
        TripleDES(Mode, Padding, const uint8_t* iv = nullptr) {}
        int encrypt(uint8_t*, const uint8_t*, uint32_t len, const Key24&) { return len; }
        int encrypt(uint8_t*, const uint8_t*, uint32_t len, const Key16&) { return len; }
        int decrypt(uint8_t*, const uint8_t*, uint32_t len, const Key24&) { return len; }
        int decrypt(uint8_t*, const uint8_t*, uint32_t len, const Key16&) { return len; }
    };
}

class CRC16 {
    uint16_t _poly;
    uint16_t _init;
    bool _refIn;
    bool _refOut;
    uint16_t _xorOut;
public:
    CRC16(uint16_t poly, uint16_t init, bool refIn, bool refOut, uint16_t xorOut) 
        : _poly(poly), _init(init), _refIn(refIn), _refOut(refOut), _xorOut(xorOut) {}

    uint16_t range(const uint8_t* c, size_t len) { return calc(c, len); }
    uint16_t calc(const uint8_t* data, size_t len) {
        uint16_t wCrc = _init;
        for (size_t i = 0; i < len; i++) {
            uint8_t d = data[i];
            if (_refIn) {
                d = (d >> 4 | d << 4);
                d = ((d & 0xCC) >> 2) | ((d & 0x33) << 2);
                d = ((d & 0xAA) >> 1) | ((d & 0x55) << 1);
            }
            wCrc ^= (uint16_t)(d << 8);
            for (int j = 0; j < 8; j++) {
                if (wCrc & 0x8000) {
                    wCrc = (wCrc << 1) ^ _poly;
                } else {
                    wCrc = wCrc << 1;
                }
            }
        }
        if (_refOut) {
            uint16_t out = wCrc;
            out = (out >> 8 | out << 8);
            out = ((out & 0xCCCC) >> 2) | ((out & 0x3333) << 2);
            out = ((out & 0xAAAA) >> 1) | ((out & 0x5555) << 1);
            wCrc = out;
        }
        return wCrc ^ _xorOut;
    }
};

template <size_t Bits, size_t... Taps>
class FibonacciLFSR_Right {
protected:
    std::array<uint8_t, 8> _state;
public:
    using state_type_t = std::array<uint8_t, 8>;
    FibonacciLFSR_Right(uint64_t seed = 1) { _state = state_type_t{}; _state[0] = seed & 0xFF; }
    uint8_t get() { return 0; }
    const state_type_t& state() const { return _state; }
    bool step() { return false; }
    uint32_t next32() { return 0; }
};

template <size_t Bits, size_t... Taps>
class FibonacciLFSR_Left {
protected:
    std::array<uint8_t, 8> _state;
public:
    using state_type_t = std::array<uint8_t, 8>;
    FibonacciLFSR_Left(uint64_t seed = 1) { _state = state_type_t{}; _state[0] = seed & 0xFF; }
    uint8_t get() { return 0; }
    const state_type_t& state() const { return _state; }
    bool step() { return false; }
    uint32_t next32() { return 0; }
};

} // namespace utility
} // namespace m5

#endif
