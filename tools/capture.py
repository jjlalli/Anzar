#!/usr/bin/env python3
"""Record windows from an Anzar board in datalog mode into a NanoEdge-ready file.

    python capture.py --port /dev/tty.usbmodem1103 --label leak --count 300

Each run appends to data/<label>.csv, one window per line.
"""
import argparse
import pathlib

import serial


def is_window(line, n_values):
    values = line.split(",")
    if len(values) != n_values:
        return False
    try:
        for v in values:
            int(v)
    except ValueError:
        return False
    return True


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--port", required=True, help="serial port of the Nucleo board")
    ap.add_argument("--label", required=True, help="no_flow, use or leak")
    ap.add_argument("--count", type=int, default=200, help="windows to record")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--values", type=int, default=512 * 3,
                    help="values per window: signal length x axes")
    ap.add_argument("--out", default="data", help="output folder")
    args = ap.parse_args()

    out = pathlib.Path(args.out)
    out.mkdir(exist_ok=True)
    path = out / f"{args.label}.csv"

    saved = 0
    with serial.Serial(args.port, args.baud, timeout=5) as port, path.open("a") as f:
        port.reset_input_buffer()
        port.readline()  # the first line is usually cut in half
        while saved < args.count:
            line = port.readline().decode("ascii", errors="ignore").strip()
            if not line or line.startswith("#") or not is_window(line, args.values):
                continue
            f.write(line + "\n")
            saved += 1
            print(f"\r{args.label}: {saved}/{args.count}", end="", flush=True)

    print(f"\nsaved to {path}")


if __name__ == "__main__":
    main()
