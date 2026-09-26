# ST M24C04 I2C EEPROM Driver

**Target Platform**: NXP S32K144 (ARM Cortex-M4F) / MATLAB & Simulink MBD  
**Standards Alignment**: AUTOSAR conventions, MISRA-C:2012, SAE J1850 CRC-8  
**Associated Simulink Models**: [`models/EEPROM_M24C04_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/EEPROM_M24C04_Demo.slx), [`models/EEPROM_SmartWheels.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/EEPROM_SmartWheels.slx)  
**Driver Source Files**: [`m24c04.h`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/eeprom_m24c04/m24c04.h), [`m24c04.c`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/eeprom_m24c04/m24c04.c)

---

## 1. Overview & Memory Architecture

The STMicroelectronics **M24C04** is a 4-Kbit (512-byte) electrically erasable programmable memory organized as two 256-byte blocks. In automotive BMS applications, it is utilized for **persistent, non-volatile state storage** that survives battery disconnects and ECU reboots.

### Memory Organization & I2C Addressing

| Memory Block | Byte Address Range | 7-Bit I2C Slave Address | Device Select Code |
| :--- | :--- | :--- | :--- |
| **Block 0** | `0x0000` to `0x00FF` (Bytes 0..255) | `0x50` (`0b1010000`) | Address bit A8 = 0 |
| **Block 1** | `0x0100` to `0x01FF` (Bytes 256..511) | `0x51` (`0b1010001`) | Address bit A8 = 1 |

* **Physical Page Size**: **16 Bytes**.
* **Page Boundary Constraint**: If you attempt to write across a 16-byte physical boundary without proper pagination, internal addressing rolls over to the beginning of the same page, overwriting previous bytes!
* **Write Cycle Time ($t_W$)**: **5 to 6 ms** internal programming delay required between page write operations.
* **Built-in Driver Feature**: The [`m24c04.c`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/eeprom_m24c04/m24c04.c) driver automatically splits arbitrary-length transfers into compliant 16-byte page segments and manages $t_W$ delays transparently.

---

## 2. Hardware Wiring & S32K144 Pinout

| Signal | LPI2C0 (Default) | LPI2C1 | EEPROM Pin | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **SCL** | `PTA3` (ALT3) | `PTD3` (ALT2) | **SCL (Pin 6)** | Serial Clock (Standard 100 kHz, Fast 400 kHz) |
| **SDA** | `PTA2` (ALT3) | `PTD2` (ALT2) | **SDA (Pin 5)** | Serial Data (Requires 4.7 k$\Omega$ external pull-up) |
| **WP** | Board GND | Board GND | **WP (Pin 7)** | Write Protect (Tie to GND for normal Read/Write) |
| **VCC** | 3.3V or 5.0V | 3.3V or 5.0V | **VCC (Pin 8)** | Power supply |
| **GND** | Board GND | Board GND | **VSS (Pin 4)** | Ground reference |

---

## 3. How Data is Stored for MultiCell BMS

In the MultiCell BMS architecture, the 512 bytes are mapped into structured non-volatile records:

```
+---------------------------------------------------------------------------------------+
|                         M24C04 EEPROM MEMORY MAP (512 BYTES)                         |
+---------------------------------------------------------------------------------------+
| Address Range     | Size    | Parameter Description                                   |
+-------------------+---------+---------------------------------------------------------+
| 0x0000 .. 0x000F  | 16 B    | Pack Identification, Serial Number & Firmware Version   |
| 0x0010 .. 0x001F  | 16 B    | Accumulated Ah Counter, Total Discharge Energy (kWh)    |
| 0x0020 .. 0x002F  | 16 B    | Life Cycle Count, SOH (State of Health) Parameter       |
| 0x0030 .. 0x003F  | 16 B    | SD Card Blackbox Write Pointer (Current Sector Index)   |
| 0x0040 .. 0x007F  | 64 B    | Historical Diagnostic Trouble Codes (DTC Fault Log)     |
| 0x0100 .. 0x013F  | 64 B    | Cell Voltage / Temperature Sensor Calibration Offsets   |
| 0x0140 .. 0x01FF  | 192 B   | User / Customer Configurable Parameters                 |
+-------------------+---------+---------------------------------------------------------+
```

---

## 4. Simulink C Function Blocks

The demo model [`models/EEPROM_M24C04_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/EEPROM_M24C04_Demo.slx) contains:

| Block Name | Inputs | Outputs | Description |
| :--- | :--- | :--- | :--- |
| **`EEPROM_Write`** | `addr` (`uint16`), `data_in` (`uint8[32]`), `len` (`uint16`) | `status` (`uint8`) | Writes up to 32 bytes into EEPROM with automatic 16-byte page segmentation. Returns `0x00` on success. |
| **`EEPROM_Read`** | `addr` (`uint16`), `len` (`uint16`) | `data_out` (`uint8[32]`), `status` (`uint8`) | Reads up to 32 bytes from EEPROM memory. **Port 1 is status, Port 2 is data**. |
| **`M24C04_SetInstance`** | `instance` (`uint32`) | None | Selects LPI2C instance (`0` = LPI2C0, `1` = LPI2C1). |

---

## 5. Critical Checklist & Troubleshooting Rules

1. **Port Inversion Bug**:
   - In Simulink C Function blocks, **Port 1 is always the scalar C function return value (`status`)**, and **Port 2 is the pointer argument (`data_out`)**.
   - *Never wire Port 1 to a transmitter or data display.*
2. **Buffer Zeroing**:
   - Always clear only the requested length: `memset(data, 0x00, len);`. Zeroing `M24C04_MAX_BUFFER_SIZE` when `data` points to a smaller buffer causes silent stack/heap corruption.
3. **Execution Sequencing**:
   - EEPROM writes require 5–6 ms to program into physical EEPROM cells. Guard writes with state machine transitions or one-shot triggers; do not trigger writes unconditionally at high sample rates.

---

## 6. 100% Complete M24C04 Feature Coverage Matrix

Every single capability specified in the ST M24C04 datasheet is fully implemented in [`m24c04.h`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/eeprom_m24c04/m24c04.h) & [`m24c04.c`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/eeprom_m24c04/m24c04.c):

| Feature Category | Hardware Specification | C Driver API Function | Automotive BMS Purpose | Verification |
| :--- | :--- | :--- | :--- | :---: |
| **Byte Write** | Single byte write (`0x000..0x1FF`) | `M24C04_WriteByte(addr, val)` | Calibration scalars, single-byte flags | ✅ Verified |
| **Byte Read** | Single byte random read | `M24C04_ReadByte(addr, *val)` | Reading status / fault triggers | ✅ Verified |
| **Page Write (16B)** | 16-byte physical page boundary | `M24C04_Write(addr, data, len)` | Standard 32B Simulink payload blocks | ✅ Verified |
| **Sequential Read (32B)**| Random address sequential read | `M24C04_Read(addr, data, len)` | Standard 32B Simulink payload blocks | ✅ Verified |
| **Arbitrary Multi-Byte Write** | Writes up to 512B spanning pages | `M24C04_WriteMulti(addr, data, len)` | Bulk parameter tables, pack configuration | ✅ Verified |
| **Arbitrary Multi-Byte Read** | Reads up to 512B across blocks | `M24C04_ReadMulti(addr, data, len)` | Reading full 512B EEPROM memory image | ✅ Verified |
| **CRC-Protected Write** | Appends SAE J1850 CRC-8 checksum | `M24C04_WriteWithCRC(addr, data, len)`| ISO 26262 ASIL safety data integrity | ✅ Verified |
| **CRC-Verified Read** | Validates data against stored CRC | `M24C04_ReadWithCRC(addr, data, len)` | Returns CRC error on bitflip / aging | ✅ Verified |
| **Range Erase** | Overwrites range with fill pattern | `M24C04_EraseRange(addr, len, 0xFF)` | Clearing DTC fault logs / reset params | ✅ Verified |
| **Bulk Chip Erase** | Overwrites entire 512B array | `M24C04_EraseAll(0xFF)` | Factory re-flashing to virgin blank state | ✅ Verified |
| **Device Presence Check** | I2C ACK check on address `0x50` | `M24C04_IsDeviceReady()` | Startup diagnostic hardware check | ✅ Verified |
| **Multi-Bus Selection** | `LPI2C0` vs `LPI2C1` runtime switch | `M24C04_SetI2CInstance(instance)` | Port pin multiplexing & multi-bus support | ✅ Verified |

