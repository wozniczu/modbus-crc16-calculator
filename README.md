# Modbus CRC-16 Calculator

A simple desktop application with a graphical interface for computing the **Modbus RTU CRC-16** checksum of an arbitrary byte frame. Built in C++ with [SFML](https://www.sfml-dev.org/).

## Features

- Input a frame as space-separated hex bytes (e.g. `01 03 00 00 00 02`)
- Computes the CRC-16 using precomputed lookup tables
- Multithreaded benchmark: repeats the computation N times to measure total and per-iteration time
- Clipboard paste support (Ctrl+V) with automatic input filtering

## Requirements

- Windows
- SFML 2.x (runtime DLLs and `Arial.ttf` are bundled for convenience)
- C++17 compiler (e.g. MSVC, MinGW-w64)

## Build

Link against `sfml-graphics`, `sfml-window`, and `sfml-system`.

```bash
g++ main.cpp -o modbus-crc16-gui -lsfml-graphics -lsfml-window -lsfml-system
```

## Run

Build as above, then run the executable from this folder so it can load `Arial.ttf` and the SFML DLLs next to it.

## Usage

1. Enter the frame bytes in the first field.
2. Set the number of repetitions (default: 1 000 000).
3. Click **Oblicz CRC** - the CRC, total time, and per-iteration time are displayed.
