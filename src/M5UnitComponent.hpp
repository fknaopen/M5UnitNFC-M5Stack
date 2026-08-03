#ifndef M5_UNIT_COMPONENT_HPP
#define M5_UNIT_COMPONENT_HPP

#include <Arduino.h>
#include <Wire.h>

#define M5_UNIT_COMPONENT_HPP_BUILDER(cls, addr) \
    static constexpr uint8_t DEFAULT_ADDRESS = addr;

namespace m5 {
namespace unit {

namespace types {
    struct uid_t { uint32_t uid; uid_t(uint32_t u = 0) : uid(u) {} };
    enum attribute { AccessSPI = 1, AccessI2C = 2 };
    struct attr_t { attribute attr; attr_t(attribute a) : attr(a) {} };
}
struct transaction_guard { transaction_guard(void*) {} };
struct ComponentConfig {
    uint32_t clock = 400000;
};

class Component {
protected:
    uint8_t _addr;
    ComponentConfig _ccfg;

public:
    Component(uint8_t addr) : _addr(addr) {}
    virtual ~Component() {}

    virtual bool begin() { return true; }
    virtual void update(const bool force = false) {}

    ComponentConfig component_config() { return _ccfg; }
    void component_config(ComponentConfig cfg) { _ccfg = cfg; }

    bool read_register(uint8_t reg, uint8_t* buf, size_t len) {
        Wire.beginTransmission(_addr);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0) return false;
        Wire.requestFrom((uint16_t)_addr, (uint8_t)len, (uint8_t)true);
        for (size_t i = 0; i < len; i++) {
            if (Wire.available()) {
                buf[i] = Wire.read();
            } else {
                return false;
            }
        }
        return true;
    }

    bool write_register(uint8_t reg, const uint8_t* buf, size_t len) {
        Wire.beginTransmission(_addr);
        Wire.write(reg);
        for (size_t i = 0; i < len; i++) {
            Wire.write(buf[i]);
        }
        return Wire.endTransmission() == 0;
    }

    bool read_register8(uint8_t reg, uint8_t& val) {
        return read_register(reg, &val, 1);
    }

    bool write_register8(uint8_t reg, uint8_t val) {
        return write_register(reg, &val, 1);
    }

    bool read_register16(uint8_t reg, uint16_t& val) {
        uint8_t buf[2];
        if (!read_register(reg, buf, 2)) return false;
        val = (buf[0] << 8) | buf[1];
        return true;
    }

    bool write_register16(uint8_t reg, uint16_t val) {
        uint8_t buf[2] = { (uint8_t)(val >> 8), (uint8_t)val };
        return write_register(reg, buf, 2);
    }

    struct Adapter { enum class Type { I2C, SPI }; Type type() { return Type::I2C; } };
    Adapter* adapter() { static Adapter a; return &a; }
    bool readRegister(uint8_t reg, uint8_t* buf, size_t len, uint8_t, bool) { return read_register(reg, buf, len); }
    bool writeRegister(uint8_t reg, const uint8_t* buf, size_t len, bool) { return write_register(reg, buf, len); }
    bool readRegister8(uint8_t reg, uint8_t& val, uint8_t, bool) { return read_register8(reg, val); }
    bool writeRegister8(uint8_t reg, uint8_t val, bool) { return write_register8(reg, val); }
    bool readRegister16BE(uint8_t reg, uint16_t& val, uint8_t, bool) { return read_register16(reg, val); }
    bool writeRegister16BE(uint8_t reg, uint16_t val, bool) { return write_register16(reg, val); }
    bool readRegister32BE(uint8_t reg, uint32_t& val, uint8_t, bool) { return read_register32(reg, val); }
    bool writeRegister32BE(uint8_t reg, uint32_t val, bool) { return write_register32(reg, val); }
    bool read_register32(uint8_t reg, uint32_t& val) {
        uint8_t buf[4];
        if (!read_register(reg, buf, 4)) return false;
        val = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | buf[3];
        return true;
    }

    bool write_register32(uint8_t reg, uint32_t val) {
        uint8_t buf[4] = { (uint8_t)(val >> 24), (uint8_t)(val >> 16), (uint8_t)(val >> 8), (uint8_t)val };
        return write_register(reg, buf, 4);
    }
};

} // namespace unit
} // namespace m5

#endif
