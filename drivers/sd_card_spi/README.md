# SPI-Based SD Card Driver & BMS Blackbox Logger

**Target Platform**: NXP S32K144 (ARM Cortex-M4F) / MATLAB & Simulink MBD  
**Standards Alignment**: AUTOSAR conventions, MISRA-C:2012, SD Physical Layer Specification v2.00  
**Associated Simulink Model**: [`models/SD_Card_SPI_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/SD_Card_SPI_Demo.slx)  
**Driver Source Files**: [`sd_spi.h`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/sd_card_spi/sd_spi.h), [`sd_spi.c`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/sd_card_spi/sd_spi.c)

---

## 1. Overview & Architectural Philosophy

This driver provides a high-reliability, deterministic flash memory logging solution for automotive Battery Management Systems (BMS). It functions as the **Event Data Recorder (EDR) / Blackbox Trip Logger**, capturing high-frequency pack telemetry, thermal events, cell voltages, and fault flags.

### Why Raw Sector Storage?
Unlike consumer gadgets that use FAT32 or exFAT, mission-critical automotive ECUs log directly to **raw 512-byte physical sectors**:
1. **Crash & Power-Loss Immunity**: If the vehicle 12V battery is severed during an accident or thermal incident, raw sectors cannot become corrupted. At most, the last in-flight 512-byte block is lost, preserving all prior crash data.
2. **Strictly Deterministic Timing**: Sector writes complete within ~1 ms, cleanly fitting into 10 ms or 100 ms periodic BMS scheduling loops without filesystem garbage-collection pauses.
3. **Zero RAM/Heap Overhead**: No dynamic allocation or bulky filesystem directory tables.

---

## 2. Hardware Wiring & S32K144 Pinout

> [!WARNING]
> **Logic Level**: SD Cards operate strictly at **3.3V**. Ensure VDD and SPI logic lines (SCK, MOSI, CS) are powered from 3.3V. Connecting 5V directly to an SD card will permanently damage it.

### S32K144 Pin Mapping Table

| Signal | LPSPI0 (Default) | LPSPI1 | LPSPI2 | MicroSD Card Pin | Direction | Function |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **SCK** | `PTB2` (ALT3) or `PTE0` (ALT2) | `PTB14` (ALT3) | `PTC15` (ALT3) | **CLK** | Output | Serial Clock (Mode 0: CPOL=0, CPHA=0) |
| **MOSI** | `PTB4` (ALT3) or `PTE1` (ALT2) | `PTB16` (ALT3) | `PTC17` (ALT3) | **DI** | Output | Master Out $\rightarrow$ Card Data In |
| **MISO** | `PTB3` (ALT3) or `PTE2` (ALT2) | `PTB15` (ALT3) | `PTC16` (ALT3) | **DO** | Input | Card Data Out $\rightarrow$ Master In (add 10k pull-up) |
| **CS (SS)** | `PTB5` (ALT3 / GPIO) | `PTB17` (ALT3 / GPIO) | `PTC14` (ALT3 / GPIO) | **CS** | Output | Chip Select (Active LOW) |
| **VCC** | **3.3V Rail** | **3.3V Rail** | **3.3V Rail** | **VDD** | Power | Card Power Supply (3.3V $\pm$ 5%) |
| **GND** | Board GND | Board GND | Board GND | **VSS** | Power | Common Ground |

---

## 3. Data Storage & Layout

The SD card is treated as a continuous array of **512-byte physical sectors** ($0, 1, 2, \dots, N$):

```
+---------------------------------------------------------------------------------------+
|                                SD CARD FLASH MEMORY                                   |
+---------------------+---------------------+---------------------+---------------------+
| Sector 0 (512 B)    | Sector 1 (512 B)    | Sector 2 (512 B)    | Sector N (512 B)    |
| Device Header /     | Telemetry Records   | Telemetry Records   | Circular Ring       |
| Magic Signature     | #0 to #15           | #16 to #31          | Buffer Storage      |
+---------------------+---------------------+---------------------+---------------------+
```

### 32-Byte BMS Telemetry Packet Format
Each 512-byte sector holds exactly **16 BMS records** (16 $\times$ 32B = 512B):

| Byte Offset | Field Name | Data Type | Units / Scale | Description |
| :--- | :--- | :--- | :--- | :--- |
| `0..3` | `timestamp` | `uint32_t` | Seconds | Unix epoch timestamp (synchronized from DS3231 RTC) |
| `4..5` | `pack_voltage_mv` | `uint16_t` | mV ($0.001\text{ V}$) | Total battery pack voltage (e.g. `48200` = 48.20 V) |
| `6..7` | `pack_current_ca` | `int16_t` | cA ($0.01\text{ A}$) | Pack current (positive = charge, negative = discharge) |
| `8..9` | `soc_permille` | `uint16_t` | 0.1% | State of Charge (e.g. `950` = 95.0%) |
| `10` | `temp_max_c` | `int8_t` | °C | Maximum cell temperature across pack |
| `11` | `temp_min_c` | `int8_t` | °C | Minimum cell temperature across pack |
| `12..13` | `cell_v_max_mv` | `uint16_t` | mV | Maximum individual cell voltage |
| `14..15` | `cell_v_min_mv` | `uint16_t` | mV | Minimum individual cell voltage |
| `16..17` | `fault_flags` | `uint16_t` | Bitmask | BMS active fault flags (OV, UV, OT, OC) |
| `18..19` | `cycle_count` | `uint16_t` | Cycles | Battery charge/discharge cycle count |
| `20..27` | `reserved` | `uint8_t[8]` | - | Reserved for future expansion / ISO 26262 parity |
| `28..31` | `packet_footer` | `uint8_t[4]` | Sync bytes | `0xAA, 0x55, CRC8, 0x5A` |

---

## 4. How to Read Data on Your PC / Laptop

Because the SD card is formatted with raw physical sectors, Windows Explorer will not display drive letters like `E:\`. **Do not click "Format Disk" when Windows asks.**

Instead, use one of the two integrated extraction tools provided in this repository:

### Method A: MATLAB Extraction Tool ([`scripts/read_sd_card.m`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/scripts/read_sd_card.m))

Run directly inside MATLAB:

```matlab
% 1. Immediate Demo / Test Mode (No SD card needed)
dataTable = read_sd_card('demo');

% 2. Read directly from a connected USB SD card reader on Windows:
% (Run MATLAB as Administrator to allow raw physical disk access)
dataTable = read_sd_card('\\.\PhysicalDrive1', 'MaxSectors', 100);

% 3. Read from a binary dump file:
dataTable = read_sd_card('sd_dump.bin', 'ExportCSV', 'trip_log.csv');
```

**Features**:
* Decodes all 32-byte records across all sectors.
* Generates an interactive 3-panel figure: Pack Voltage/Current, Cell Imbalance spread, Thermal & SOC trends.
* Automatically exports to [`BMS_Trip_Log_Export.csv`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/BMS_Trip_Log_Export.csv) for opening in Microsoft Excel.

---

### Method B: Python Extraction Tool ([`scripts/sd_card_dump.py`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/scripts/sd_card_dump.py))

Run from Windows PowerShell or Command Prompt:

```bash
# 1. Identify which PhysicalDrive is your SD card
python scripts/sd_card_dump.py --list

# 2. Dump and decode directly to CSV (Run Command Prompt as Administrator)
python scripts/sd_card_dump.py --drive \\.\PhysicalDrive1 --sectors 100 --csv trip_log.csv --outbin sd_dump.bin

# 3. Test in demo mode
python scripts/sd_card_dump.py --demo
```

---

## 5. Simulink Model-Based Design Integration

The model [`models/SD_Card_SPI_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/SD_Card_SPI_Demo.slx) exposes 5 reusable C Function blocks:

| Block Name | Inputs | Outputs | Description |
| :--- | :--- | :--- | :--- |
| **`SD_Init`** | None | `card_type` (`uint8`), `status` (`uint8`) | Initializes card, enters SPI mode, verifies voltage and card capacity. |
| **`SD_ReadSector`** | `sector_id` (`uint32`) | `data_512` (`uint8[512]`), `status` (`uint8`) | Reads full 512-byte raw physical block (`CMD17`). |
| **`SD_WriteSector`** | `sector_id` (`uint32`), `data_512` (`uint8[512]`) | `status` (`uint8`) | Writes full 512-byte raw physical block (`CMD24`). |
| **`SD_ReadPayload`** | `sector_id` (`uint32`), `offset` (`uint16`), `length` (`uint16`) | `payload_32` (`uint8[32]`), `status` (`uint8`) | Reads a compact 32-byte BMS telemetry slice without bulky 512B signals. |
| **`SD_SetInstance`** | `instance` (`uint32`) | None | Selects LPSPI peripheral instance (`0` = LPSPI0, `1` = LPSPI1, `2` = LPSPI2). |

---

## 6. Dual-Mode Operation (Simulation vs. Hardware Target)

* **Host / Desktop Simulation (`#if SD_IS_SIMULATION`)**:
  - Automatically activates when compiling under Windows / MATLAB host.
  - Allocates an in-RAM virtual disk (8 sectors $\times$ 512 bytes = 4096 bytes).
  - Pre-populates Sector 0 with `"MULTICELL_BMS_V3:TRIP_RECORDER_BLOCK_0"`.
  - Enables full MBD model simulation with zero hardware connected.
* **Target Hardware (`#else`)**:
  - Automatically compiles when building with Embedded Coder / S32 Design Studio for ARM Cortex-M4.
  - Calls `LPSPI_DRV_MasterTransferBlocking()` and manages hardware GPIO chip select lines.
