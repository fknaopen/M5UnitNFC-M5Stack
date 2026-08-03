# M5UnitNFC-M5Stack

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

## Overview

This repository provides a targeted port of the [M5Unit-NFC](https://github.com/m5stack/M5Unit-NFC) library. While the original library is designed for the modern `M5Unified` ecosystem, this port is specifically tailored to operate within the legacy **`M5Stack`** environment.

The primary goal of this project is to allow developers maintaining or building applications on the traditional `M5Stack` framework to integrate M5Stack's [Unit-NFC (SKU:U216)](https://docs.m5stack.com/ja/unit/Unit_NFC) without the overhead or necessity of migrating their entire codebase to `M5Unified`.

## Features and Limitations

To keep the library lightweight and focused on the most common use cases, this is a **partial port**.

**Supported Operations:**
* **NFC-A (ISO 14443A):** Detect / Identify
* **NFC-F (FeliCa):** Detect / Identify

**Not Supported:**
* NFC-B, NFC-V, and other PICC types
* Raw Read/Write operations
* Emulation modes
* Dependencies on `M5UnitUnified` and `M5HAL` (These have been stripped or adapted for the classic `M5Stack` core).

## Getting Started

### Installation
1. Clone or download this repository.
2. Place the `M5UnitNFC-M5Stack` directory into your Arduino `libraries` folder.
3. Include the library in your project alongside `<M5Stack.h>`.

### Examples
Check the `examples/` directory for ready-to-use sketches:
* `NFCA_Detect`: Detect and identify NFC-A tags.
* `NFCF_Detect`: Detect and identify NFC-F (FeliCa) tags.
* `NFCAF_Detect`: Detect and identify both NFC-A and NFC-F tags.

## References
* [Unit-NFC Official Documentation](https://docs.m5stack.com/ja/unit/Unit_NFC)

## Acknowledgment
This project is a fork and port of the official [M5Unit-NFC](https://github.com/m5stack/M5Unit-NFC) library by M5Stack.

## License
MIT License. See [LICENSE](LICENSE) for details.
