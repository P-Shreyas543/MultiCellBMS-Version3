#!/usr/bin/env python3
"""
MultiCell BMS - Raw SD Card Reader & Telemetry Extraction Utility
=================================================================
Target: Automotive Blackbox, Trip Log Extraction, and Physical Block Decoding.

Usage:
    # 1. Test immediate demo decoding (No hardware required)
    python sd_card_dump.py --demo

    # 2. List physical disk drives on Windows
    python sd_card_dump.py --list

    # 3. Dump from physical SD card (Requires Administrator / Root)
    #    Windows:
    python sd_card_dump.py --drive \\.\PhysicalDrive1 --sectors 100 --csv trip_log.csv
    #    Linux:
    sudo python3 sd_card_dump.py --drive /dev/sdb --sectors 100 --csv trip_log.csv

    # 4. Decode an existing binary dump file
    python sd_card_dump.py --file sd_dump.bin --csv trip_log.csv
"""

import os
import sys
import struct
import argparse
import csv
import platform

SECTOR_SIZE = 512
RECORD_SIZE = 32
RECORDS_PER_SECTOR = SECTOR_SIZE // RECORD_SIZE  # 16 records / sector

def list_drives_windows():
    """List physical disk drives on Windows using PowerShell."""
    print("\n--- Detected Physical Disks on Windows ---")
    try:
        import subprocess
        cmd = ["powershell", "-Command", "Get-CimInstance -ClassName Win32_DiskDrive | Select-Object DeviceID, Model, @{Name='Size_GB';Expression={[math]::Round($_.Size/1GB, 2)}} | Format-Table -AutoSize"]
        res = subprocess.run(cmd, capture_output=True, text=True, check=True)
        print(res.stdout)
    except Exception as e:
        print(f"Could not query disks: {e}")
        print("Tip: Run PowerShell command 'Get-Disk' to identify your SD Card's DeviceID (e.g., \\\\.\\PhysicalDrive1).")

def generate_demo_data(num_sectors=8):
    """Generate in-memory simulated SD card bytes matching S32K BMS driver."""
    data = bytearray(num_sectors * SECTOR_SIZE)
    # Sector 0: Header
    header = b"MULTICELL_BMS_V3:TRIP_RECORDER_BLOCK_0"
    data[:len(header)] = header

    # Sectors 1..N: Telemetry Records
    rec_count = 0
    base_ts = 1727337600  # Unix timestamp
    for s in range(1, num_sectors):
        for r in range(RECORDS_PER_SECTOR):
            rec_count += 1
            offset = s * SECTOR_SIZE + r * RECORD_SIZE
            ts = base_ts + rec_count
            v_mv = int(48200 - 200 * (rec_count % 10))
            i_ca = int(-2500 + 300 * (rec_count % 5))
            soc_perm = int(950 - rec_count)
            t_max = 35 + (rec_count % 5)
            t_min = 28 + (rec_count % 4)
            cv_max = 4160 - (rec_count % 20)
            cv_min = 4120 - (rec_count % 20)
            faults = 0
            cycles = 142

            chunk = struct.pack(
                "<IHhHbbHHHH8s4s",
                ts, v_mv, i_ca, soc_perm,
                t_max, t_min, cv_max, cv_min,
                faults, cycles, b"\x00" * 8, b"\xAA\x55\x00\x5A"
            )
            data[offset:offset + RECORD_SIZE] = chunk
    return bytes(data)

def decode_sectors(raw_bytes, csv_filename="BMS_Trip_Log_Export.csv"):
    total_sectors = len(raw_bytes) // SECTOR_SIZE
    print(f"[+] Total Sectors Read: {total_sectors} ({len(raw_bytes):,} bytes)")

    # Sector 0 Header Inspection
    s0 = raw_bytes[:SECTOR_SIZE]
    clean_hdr = "".join(chr(b) for b in s0[:64] if 32 <= b <= 126)
    print(f"[+] Sector 0 Header: '{clean_hdr}'")

    rows = []
    rec_id = 0

    for s in range(1, total_sectors):
        s_start = s * SECTOR_SIZE
        s_end = s_start + SECTOR_SIZE
        sec_data = raw_bytes[s_start:s_end]

        for r in range(RECORDS_PER_SECTOR):
            offset = r * RECORD_SIZE
            chunk = sec_data[offset:offset + RECORD_SIZE]

            if chunk == b"\x00" * RECORD_SIZE or chunk == b"\xFF" * RECORD_SIZE:
                continue

            try:
                (ts, v_mv, i_ca, soc_perm,
                 t_max, t_min, cv_max, cv_min,
                 faults, cycles, _, _) = struct.unpack("<IHhHbbHHHH8s4s", chunk)

                rec_id += 1
                rows.append({
                    "RecordID": rec_id,
                    "SectorID": s,
                    "Slot": r,
                    "Timestamp": ts,
                    "PackVoltage_V": round(v_mv / 1000.0, 3),
                    "PackCurrent_A": round(i_ca / 100.0, 2),
                    "SOC_Percent": round(soc_perm / 10.0, 1),
                    "TempMax_C": t_max,
                    "TempMin_C": t_min,
                    "CellVMax_V": round(cv_max / 1000.0, 3),
                    "CellVMin_V": round(cv_min / 1000.0, 3),
                    "DeltaCell_mV": (cv_max - cv_min),
                    "FaultWord": f"0x{faults:04X}",
                    "CycleCount": cycles
                })
            except struct.error:
                continue

    print(f"[+] Successfully decoded {len(rows)} BMS telemetry records.")

    if rows and csv_filename:
        with open(csv_filename, mode="w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
            writer.writeheader()
            writer.writerows(rows)
        print(f"[+] Exported CSV file to: {os.path.abspath(csv_filename)}")

    # Preview first 3 rows
    if rows:
        print("\n--- Preview (First 3 Records) ---")
        for row in rows[:3]:
            print(f" Record #{row['RecordID']}: {row['Timestamp']}s | {row['PackVoltage_V']}V | {row['PackCurrent_A']}A | SOC {row['SOC_Percent']}% | Tmax {row['TempMax_C']}C | Cells [{row['CellVMin_V']}V..{row['CellVMax_V']}V]")

def main():
    parser = argparse.ArgumentParser(description="MultiCell BMS Raw SD Card Extraction Utility")
    parser.add_argument("--demo", action="store_true", help="Generate and decode simulated BMS memory")
    parser.add_argument("--list", action="store_true", help="List physical disks on system")
    parser.add_argument("--drive", type=str, help="Physical drive path (e.g. '\\\\.\\PhysicalDrive1' on Windows, '/dev/sdb' on Linux)")
    parser.add_argument("--file", type=str, help="Binary dump file path (e.g. 'sd_dump.bin')")
    parser.add_argument("--sectors", type=int, default=100, help="Number of sectors to dump (default: 100)")
    parser.add_argument("--csv", type=str, default="BMS_Trip_Log_Export.csv", help="Output CSV path")
    parser.add_argument("--outbin", type=str, default=None, help="Optional output path to save raw binary image (.bin)")

    args = parser.parse_args()

    print("=" * 60)
    print(" MultiCell BMS - SPI SD Card Raw Sector Extractor")
    print("=" * 60)

    if args.list:
        if platform.system() == "Windows":
            list_drives_windows()
        else:
            print("Run 'lsblk' on Linux or 'diskutil list' on macOS to see drive paths.")
        return

    if args.demo:
        print("[+] Running in DEMO mode with synthetic BMS data...")
        raw_data = generate_demo_data(8)
        if args.outbin:
            with open(args.outbin, "wb") as f:
                f.write(raw_data)
            print(f"[+] Saved raw binary to: {args.outbin}")
        decode_sectors(raw_data, args.csv)
        return

    raw_data = None

    if args.file:
        print(f"[+] Reading from file: {args.file}")
        with open(args.file, "rb") as f:
            raw_data = f.read(args.sectors * SECTOR_SIZE)

    elif args.drive:
        print(f"[+] Reading raw sectors from drive: {args.drive}")
        try:
            with open(args.drive, "rb") as f:
                raw_data = f.read(args.sectors * SECTOR_SIZE)
        except PermissionError:
            print("[!] Permission Denied! Direct raw drive access requires Administrator/Root privileges.")
            print("    Please run this terminal / command prompt as Administrator.")
            sys.exit(1)
        except Exception as e:
            print(f"[!] Error accessing drive: {e}")
            sys.exit(1)
    else:
        print("[!] No input specified. Defaulting to --demo mode.")
        raw_data = generate_demo_data(8)

    if args.outbin and raw_data:
        with open(args.outbin, "wb") as f:
            f.write(raw_data)
        print(f"[+] Saved raw binary to: {args.outbin}")

    if raw_data:
        decode_sectors(raw_data, args.csv)

if __name__ == "__main__":
    main()
