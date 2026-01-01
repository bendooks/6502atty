# 6502 Atty

A 6502 SBC with an ATMega as a contoller and monitor for the 6502.

This is an initial commit as PCBs are off for manufacture and not been tested. This includes the prototype firmware and the first cut of the firmware code.

![3D PCB view](docs/6502atty_3d1.png)

## PCB updates (v1 rev2)

- Added J12 to route ~WE signal to the PGM/~WE to allow SRAM
- Connected ROM's VPP signal to GND, if using SRAM, ties A14 to GND
- U9C's pins 9 and 10 swapped for routing
- U11's E1/E2 pins swapped for easier routing
- Possible transcription error from prototype, ROM_ACC should be from AND not NAND gate
  - Swao U8C for one of U10 spare gates

# Firmware

The firmware is currently in development. To build, you will need at-least avr-gcc and the relevant libraries and is currently only being tested on Linux (Debian). Once built, download to the ATMega using something like avrdude.

## Revision information

The original prototype version was built on a perfboard, and not release.

