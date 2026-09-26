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

| Reg Addr | Name | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 | Range / Function |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| `0x00` | Seconds | 0 | 10 Seconds | Single Seconds | `00`..`59` (BCD) |
| `0x01` | Minutes | 0 | 10 Minutes | Single Minutes | `00`..`59` (BCD) |
| `0x02` | Hours | 0 | 12/24 | 10 Hours | Single Hours | `00`..`23` (24h BCD) |
| `0x03` | Day of Week | 0 | 0 | 0 | 0 | 0 | Day of Week | `1`..`7` (1=Sun) |
| `0x04` | Date | 0 | 0 | 10 Date | Single Date | `01`..`31` (BCD) |
| `0x05` | Month / Century | Century | 0 | 0 | 10 Month | Single Month | `01`..`12` (BCD) |
| `0x06` | Year | 10 Year | Single Year | `00`..`99` (BCD) |
| `0x07` | Alarm 1 Seconds | **A1M1** | 10 Seconds | Single Seconds | Alarm 1 Sec |
| `0x08` | Alarm 1 Minutes | **A1M2** | 10 Minutes | Single Minutes | Alarm 1 Min |
| `0x09` | Alarm 1 Hours | **A1M3** | 12/24 | 10 Hours | Single Hours | Alarm 1 Hours |
| `0x0A` | Alarm 1 Day/Date | **A1M4** | **DY/DT** | 10 Date | Day / Date | Alarm 1 Day/Date |
| `0x0B` | Alarm 2 Minutes | **A2M2** | 10 Minutes | Single Minutes | Alarm 2 Min |
| `0x0C` | Alarm 2 Hours | **A2M3** | 12/24 | 10 Hours | Single Hours | Alarm 2 Hours |
| `0x0D` | Alarm 2 Day/Date | **A2M4** | **DY/DT** | 10 Date | Day / Date | Alarm 2 Day/Date |
| `0x0E` | Control Register | **EOSC** | **BBSQW** | **CONV** | **RS2** | **RS1** | **INTCN** | **A2IE** | **A1IE** | Clock & Alarm Control |
| `0x0F` | Status Register | **OSF** | 0 | 0 | 0 | **EN32kHz**| **BSY** | **A2F** | **A1F** | Status Flags |
| `0x10` | Aging Offset | Sign | $2^6$ | $2^5$ | $2^4$ | $2^3$ | $2^2$ | $2^1$ | $2^0$ | Crystal Trim ($\pm 127$) |
| `0x11` | Temp MSB | Sign | $2^6$ | $2^5$ | $2^4$ | $2^3$ | $2^2$ | $2^1$ | $2^0$ | Integer °C |
| `0x12` | Temp LSB | $2^{-1}$ | $2^{-2}$ | 0 | 0 | 0 | 0 | 0 | 0 | Fraction ($0.25^\circ\text{C}$) |

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

---

## 6. How to Set an Alarm in DS3231

The DS3231 contains **two independent, programmable alarms**:
* **Alarm 1**: Full 1-second resolution (matches Seconds, Minutes, Hours, and Day/Date).
* **Alarm 2**: 1-minute resolution (matches Minutes, Hours, and Day/Date).

### Step 1: Alarm 1 Register Map & Mask Bits

Alarm 1 uses registers `0x07` through `0x0A`. Bit 7 of each register is a **mask bit** (`A1M1`..`A1M4`):

| A1M4 (Reg 0x0A, bit 7) | A1M3 (Reg 0x09, bit 7) | A1M2 (Reg 0x08, bit 7) | A1M1 (Reg 0x07, bit 7) | Trigger Condition / Rate |
| :---: | :---: | :---: | :---: | :--- |
| `1` | `1` | `1` | `1` | **Alarm once per second** (`DS3231_ALARM1_EVERY_SEC`) |
| `1` | `1` | `1` | `0` | **Alarm when seconds match** (once per minute, e.g. at second 00) |
| `1` | `1` | `0` | `0` | **Alarm when minutes and seconds match** (once per hour, e.g. at 15:00) |
| `1` | `0` | `0` | `0` | **Daily Alarm** (when hours, minutes, and seconds match, e.g. 08:30:00) |
| `0` | `0` | `0` | `0` | **Monthly Alarm** (when date, hours, minutes, and seconds match) |

### Step 2: Enabling the Physical Interrupt (`INT/SQW` Pin)

To make the physical `INT/SQW` pin on the DS3231 pull LOW when an alarm fires:
1. **Control Register (`0x0E`)**:
   - Set bit 2 (`INTCN = 1`): Routes the alarm interrupt to the physical `INT/SQW` pin (instead of outputting a square wave).
   - Set bit 0 (`A1IE = 1`): Enables the Alarm 1 interrupt.
2. **Status Register (`0x0F`)**:
   - Bit 0 is `A1F` (Alarm 1 Flag). The hardware sets this to `1` when the match occurs.
   - ⚠️ **CRITICAL RULE**: The MCU must clear `A1F` by writing `0` to it, or else the `INT/SQW` pin will stay pulled LOW and never trigger another interrupt!

### Step 3: Using the C Driver API

The driver [`ds3231.h`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/ds3231.h) provides a one-call setup function that handles the registers, mask bits, and interrupt enables automatically:

```c
/* Example 1: Set a daily alarm at 08:30:00 */
uint8_t status = DS3231_SetAlarm1(8U, 30U, 0U, DS3231_ALARM1_MATCH_HR_MIN_SEC);

/* Example 2: Set an hourly wakeup alarm (fires at minute 00, second 00) */
uint8_t status = DS3231_SetAlarm1(0U, 0U, 0U, DS3231_ALARM1_MATCH_MIN_SEC);

/* Check if alarm fired and clear the flag */
bool alarm_fired = false;
DS3231_CheckAlarm1(&alarm_fired, true); /* true = automatically clear flag in hardware */
if (alarm_fired) {
    /* Perform BMS periodic health check / wakeup routine */
}
```

### Step 4: Automotive Low-Power Wakeup (S32K144 Integration)

```
[ DS3231 RTC ]                         [ NXP S32K144 MCU ]
  INT/SQW Pin (Active LOW) ---------->   PTA4 / PTD0 (External Interrupt / WUU)
                                                |
                                        1. MCU is in VLPS (Sleep)
                                        2. Alarm fires -> INT pulls LOW
                                        3. PORT interrupt ISR wakes MCU
                                        4. Read cell voltages & temperatures
                                        5. Call DS3231_ClearAlarm1()
                                        6. Return to VLPS (Sleep)
```

---

## 7. 100% Complete DS3231 Feature Coverage Matrix

Every single register and capability of the Maxim DS3231 silicon is fully implemented in [`ds3231.h`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/ds3231.h) & [`ds3231.c`](file:///c:/Users/Shreyas/Documents/MultiCell%20BMS%20Algorithum%20Develpment%20LAB/MultiCellBMS-Version3/drivers/rtc_ds3231/ds3231.c):

| Feature Category | Register Address | Hardware Bit / Field | C Driver API Function | Verification Status |
| :--- | :--- | :--- | :--- | :---: |
| **Real-Time Clock** | `0x00..0x02` | Sec, Min, Hours (24h/12h BCD) | `DS3231_GetTimeScalars()`, `DS3231_SetTimeScalars()` | ✅ Tested & Passed |
| **Calendar** | `0x03..0x06` | Day, Date, Month, Year BCD | `DS3231_GetDateScalars()`, `DS3231_SetDateScalars()` | ✅ Tested & Passed |
| **Combined Array** | `0x00..0x06` | 7-byte Time Array | `DS3231_GetTimeArray()`, `DS3231_SetTimeArray()` | ✅ Tested & Passed |
| **Alarm 1** | `0x07..0x0A` | Sec, Min, Hr, Day/Date + A1M1..4 | `DS3231_SetAlarm1()`, `DS3231_CheckAlarm1()`, `DS3231_ClearAlarm1()` | ✅ Tested & Passed |
| **Alarm 2** | `0x0B..0x0D` | Min, Hr, Day/Date + A2M2..4 | `DS3231_SetAlarm2()`, `DS3231_CheckAlarm2()`, `DS3231_ClearAlarm2()` | ✅ Tested & Passed |
| **Square-Wave Generator** | `0x0E` | `RS2`, `RS1`, `INTCN`, `BBSQW` | `DS3231_EnableSquareWave()` (1Hz, 1.024k, 4.096k, 8.192k) | ✅ Tested & Passed |
| **32.768 kHz Output** | `0x0F` | `EN32kHz` (bit 3) | `DS3231_Enable32kHzOutput()`, `DS3231_Is32kHzOutputEnabled()` | ✅ Tested & Passed |
| **Temperature Sensor** | `0x11..0x12` | 10-bit signed ($0.25^\circ\text{C}$ res) | `DS3231_GetTemperature()`, `DS3231_GetTemperatureFixed()` | ✅ Tested & Passed |
| **Forced Temp Conversion**| `0x0E`, `0x0F` | `CONV` (bit 5), `BSY` (bit 2) | `DS3231_TriggerTemperatureConversion()`, `DS3231_IsTemperatureBusy()`| ✅ Tested & Passed |
| **Crystal Aging Trim** | `0x10` | 8-bit signed ($\pm 0.1\text{ ppm}$/LSB) | `DS3231_SetAgingOffset()`, `DS3231_GetAgingOffset()` | ✅ Tested & Passed |
| **Power Loss Detect** | `0x0F` | `OSF` (Oscillator Stop Flag) | `DS3231_CheckOscillatorStopFlag()` | ✅ Tested & Passed |
| **Battery Life Control** | `0x0E` | `EOSC` (Enable Oscillator on Vbat) | `DS3231_SetOscillatorStopOnBattery()` | ✅ Tested & Passed |
| **Multi-Bus Selection** | Internal | S32K LPI2C0 / LPI2C1 / etc. | `DS3231_SetI2CInstance()`, `DS3231_GetI2CInstance()` | ✅ Tested & Passed |


