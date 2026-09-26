# MultiCell BMS Hardware Peripheral Drivers Suite

**Target Platform**: NXP S32K144 (ARM Cortex-M4F) / MATLAB & Simulink MBDT  
**Standards Compliance**: AUTOSAR architecture conventions, MISRA-C:2012, ISO 26262 ASIL-B alignment

---

## 1. Driver Directory Matrix

This folder contains production-grade automotive peripheral drivers engineered specifically for Model-Based Design (MBD) with Simulink C Function blocks:

| Peripheral | Interface / Bus | Driver Directory | README & Documentation | Simulink Demo Model |
| :--- | :--- | :--- | :--- | :--- |
| **ST M24C04** | I2C (`0x50` / `0x51`) | [`drivers/eeprom_m24c04/`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/eeprom_m24c04/) | [EEPROM README](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/eeprom_m24c04/README.md) | [`models/EEPROM_M24C04_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/EEPROM_M24C04_Demo.slx) |
| **Maxim DS3231** | I2C (`0x68`) | [`drivers/rtc_ds3231/`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/) | [RTC README](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/README.md) | [`models/RTC_DS3231_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/RTC_DS3231_Demo.slx) |
| **SPI SD Card** | SPI Mode 0 | [`drivers/sd_card_spi/`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/sd_card_spi/) | [SD Card README](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/sd_card_spi/README.md) | [`models/SD_Card_SPI_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/SD_Card_SPI_Demo.slx) |

---

## 2. Combined S32K144 Pinout & Bus Allocation

To prevent pin conflicts on the S32K144 microcontroller, peripherals are organized cleanly across dedicated hardware buses:

```
                                  +-------------------+
                                  |   NXP S32K144     |
                                  |  ARM Cortex-M4F   |
                                  +---------+---------+
                                            |
         +----------------------------------+----------------------------------+
         |                                                                     |
         v                                                                     v
  [ LPI2C0 Bus ]                                                        [ LPSPI0 Bus ]
  PTA2 (SDA), PTA3 (SCL)                                                PTB2 (SCK), PTB4 (MOSI),
         |                                                              PTB3 (MISO), PTB5 (CS)
         +-----------------------+                                             |
         |                       |                                             v
         v                       v                                      [ SPI MicroSD ]
  [ ST M24C04 EEPROM ]   [ Maxim DS3231 RTC ]                           512-Byte Raw Physical
  Address: 0x50 / 0x51   Address: 0x68                                  Sectors (Blackbox Log)
  Non-Volatile Calib/DTC Real-Time Clock & TCXO
```

| Bus / Function | S32K144 Pins | Connected Peripherals | Operating Voltage |
| :--- | :--- | :--- | :--- |
| **LPI2C0** | `PTA2` (SDA), `PTA3` (SCL) | M24C04 EEPROM (`0x50/0x51`), DS3231 RTC (`0x68`) | 3.3V (with 4.7 k$\Omega$ pull-ups) |
| **LPSPI0** | `PTB2` (SCK), `PTB4` (MOSI), `PTB3` (MISO), `PTB5` (CS) | SPI MicroSD Card (Raw 512B sectors) | 3.3V Only |
| **LPUART1** | `PTC7` (TX), `PTC6` (RX) | Diagnostic Serial Console / Telemetry stream | 3.3V (115200 baud) |

---

## 3. Extracting and Reading SD Card Logs on Your System

To read raw sector logs on your PC without filesystem corruption:
1. **MATLAB GUI / Script**: Run [`read_sd_card('demo')`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/scripts/read_sd_card.m) or `read_sd_card('\\.\PhysicalDrive1')`.
2. **Python CLI**: Run [`python scripts/sd_card_dump.py --demo`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/scripts/sd_card_dump.py) or `python scripts/sd_card_dump.py --drive \\.\PhysicalDrive1 --csv trip.csv`.
3. Opens cleanly in Microsoft Excel or MATLAB Workspace.
