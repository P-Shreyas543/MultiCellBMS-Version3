# Maxim DS3231 High-Precision I2C Real-Time Clock (RTC) Driver

**Target Platform**: NXP S32K144 (ARM Cortex-M4F) / MATLAB & Simulink MBD  
**Standards Alignment**: AUTOSAR conventions, MISRA-C:2012  
**Associated Simulink Model**: [`models/RTC_DS3231_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/RTC_DS3231_Demo.slx)  
**Driver Source Files**: [`ds3231.h`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/ds3231.h), [`ds3231.c`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/ds3231.c)

---

## 1. Overview & Automotive Application

The **DS3231** is an extremely accurate, temperature-compensated real-time clock (TCXO) with an integrated 32.768 kHz quartz crystal. In automotive Battery Management Systems, accurate timekeeping is critical for:
1. **Timestamping SD Card Blackbox Logs**: Providing legally certifiable, millisecond-accurate time tags for trip logging and crash investigation.
2. **Warranty & Battery Aging Validation**: Tracking exact calendar aging vs. operational cycle aging.
3. **Internal Diagnostic Monitoring**: Reading the internal TCXO temperature sensor to assess ECU enclosure thermal conditions.

---

## 2. Hardware Wiring & S32K144 Pinout

* **I2C Slave Address**: `0x68` (7-bit address `0b1101000`).
* **Operating Voltage**: 2.3V to 5.5V (typically connected to 3.3V power domain).

### S32K144 Pin Mapping Table

| Signal | LPI2C0 (Default) | LPI2C1 | DS3231 Module Pin | Description |
| :--- | :--- | :--- | :--- | :--- |
| **SCL** | `PTA3` (ALT3) | `PTD3` (ALT2) | **SCL** | Serial Clock line |
| **SDA** | `PTA2` (ALT3) | `PTD2` (ALT2) | **SDA** | Serial Data line (4.7 k$\Omega$ pull-up to 3.3V) |
| **VCC** | 3.3V Rail | 3.3V Rail | **VCC** | Power Supply |
| **GND** | Board GND | Board GND | **GND** | Ground Reference |
| **SQW / INT** | Optional (GPIO input) | Optional | **SQW / INT** | Square wave / Alarm interrupt output |
| **VBAT** | CR2032 Cell | CR2032 Cell | **VBAT** | 3.0V Coin Cell backup power |

---

## 3. Register Map & Internal BCD Data Format

The DS3231 stores time and date in **Binary Coded Decimal (BCD)**:

| Reg Addr | Name | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 | Range |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| `0x00` | Seconds | 0 | 10 Seconds | Single Seconds | `00`..`59` |
| `0x01` | Minutes | 0 | 10 Minutes | Single Minutes | `00`..`59` |
| `0x02` | Hours | 0 | 12/24 | 10 Hours | Single Hours | `00`..`23` (24h) |
| `0x03` | Day of Week | 0 | 0 | 0 | 0 | 0 | Day of Week | `1`..`7` |
| `0x04` | Date | 0 | 0 | 10 Date | Single Date | `01`..`31` |
| `0x05` | Month / Century | Century | 0 | 0 | 10 Month | Single Month | `01`..`12` |
| `0x06` | Year | 10 Year | Single Year | `00`..`99` |
| `0x0F` | Status Register | **OSF** | 0 | 0 | 0 | EN32kHz | BSY | A2F | A1F | Flags |
| `0x11` | MSB Temp | Sign | $2^6$ | $2^5$ | $2^4$ | $2^3$ | $2^2$ | $2^1$ | $2^0$ | Integer °C |
| `0x12` | LSB Temp | $2^{-1}$ | $2^{-2}$ | 0 | 0 | 0 | 0 | 0 | 0 | Fraction ($0.25^\circ\text{C}$) |

> [!NOTE]
> The C driver [`ds3231.c`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/ds3231.c) automatically performs all BCD-to-Decimal and Decimal-to-BCD conversions internally, so Simulink signals are clean, standard integers.

---

## 4. Simulink C Function Blocks

In [`models/RTC_DS3231_Demo.slx`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/models/RTC_DS3231_Demo.slx), modular blocks provide clean, separate interfaces:

| Block Name | Inputs | Outputs | Description |
| :--- | :--- | :--- | :--- |
| **`RTC_GetTime`** | None | `Hours` (`uint8`), `Minutes` (`uint8`), `Seconds` (`uint8`), `Status` (`uint8`) | Reads 24-hour time scalars directly from register `0x00`. |
| **`RTC_GetDate`** | None | `Date` (`uint8`), `Month` (`uint8`), `Year` (`uint8`), `DayOfWeek` (`uint8`), `Status` (`uint8`) | Reads calendar scalars directly from register `0x03`. |
| **`RTC_SetTime`** | `Hours`, `Minutes`, `Seconds` | `Status` (`uint8`) | Sets the RTC clock time in 24h format. |
| **`RTC_SetDate`** | `Date`, `Month`, `Year`, `DayOfWeek` | `Status` (`uint8`) | Sets the calendar date. |
| **`RTC_GetTemperature`**| None | `Die_Temperature_C` (`single`), `Status` (`uint8`) | Reads the internal TCXO temperature sensor with $0.25^\circ\text{C}$ resolution. |
| **`RTC_SetI2CInstance`** | `Instance` (`uint32`) | None | Selects LPI2C instance (`0` = LPI2C0, `1` = LPI2C1). |

---

## 5. Integrating DS3231 with the SD Card Blackbox

To timestamp your SD Card blackbox trip logs:
1. Connect `RTC_GetTime` and `RTC_GetDate` to a simple Unix timestamp converter function (or pack `Year`, `Month`, `Day`, `Hour`, `Min`, `Sec` into 6 bytes).
2. Wire the timestamp into the 32-byte payload input of the `SD_WritePayload` block (Bytes 0..3).
3. Every log entry written to the SD card will carry an exact, battery-backed timestamp that persists even across vehicle shutdowns.
