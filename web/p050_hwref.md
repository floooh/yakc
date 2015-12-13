---
layout: page
title: Hardware Reference
---

## Hardware Programming Reference

### Z80 PIO Registers

The PIO registers are mapped to the following IO ports:

- **0x88**: PIO-A data
- **0x89**: PIO-B data
- **0x8A**: PIO-A control
- **0x8B**: PIO-B control

The interrupt vectors are located at 0x01E4 and 0x01E6:

- **0x01E4**: PIO-A interrupt vector (used for cassette tape input)
- **0x01E6**: PIO-B interrupt vector (used for keyboard input)

The **PIO-A data register 0x88** is used to control internal memory banks, and
the tape status LED and tape monitor:

- **PIO-A bit 0**: CAOS ROM at address 0xE000 on/off
- **PIO-A bit 1**: RAM bank at address 0x0000 on/off
- **PIO-A bit 2**: video memory at address 0x8000 on/off
- **PIO-A bit 3**: write-protection for RAM bank at address 0x0000 (1: write-protection off)
- **PIO-A bit 4**: unused
- **PIO-A bit 5**: tape LED on/off
- **PIO-A bit 6**: tape motor on/off
- **PIO-A bit 7**: BASIC ROM at address 0xC000 on/off

The **PIO-B data register 0x89** is used to control sound volume and to 
globally enable or disable the foreground color blinking. On the KC85/4,
2 previously unused bits are used to control the RAM banks at address 0x8000:

- **KC85/3: PIO-B bits 0..4**: sound volume
- **KC85/4: PIO-B bit 0**: reset symmetry flip-flop for sound output
- **KC85/4: PIO-B bits 1..3**: sound volume
- **KC85/4: PIO-B bit 4**: unused
- **KC85/4: PIO-B bit 5**: RAM banks at address 0x8000 on/off
- **KC85/4: PIO-B bit 6**: write-protection for RAM banks at address 0x8000 (1: write-protection off)
- **PIO-B bit 7**: globally enable/disable foreground color blinking

### KC85/4 Ports 0x84 and 0x86

The KC85/4 has 2 additional IO ports which are not backed by the PIO and 
cannot be read-back with an IN instruction, instead the values for
port 0x84 and 0x86 are backed at memory addresses at (IX+1) and (IX+4):

- **0x84 bit 0**: display image 0 or 1
- **0x84 bit 1**: CPU access to pixel or color video memory bank (0: pixels, 1: color)
- **0x84 bit 2**: CPU access to image 0 or 1
- **0x84 bit 3**: hicolor mode (0: hicolor off, 1: hicolor on)
- **0x84 bit 4**: select RAM bank at address 0x8000 (0: bank0, 1: bank1)
- **0x84 bit 5**: block select for RAM at address 0x8000 (FIXME: no idea what that does?!)
- **0x84 bits 6..7**: reserved

- **0x86 bit 0**: RAM bank at address 0x4000 on/off
- **0x86 bit 1**: write-protection for RAM bank at address 0x4000 (1: write-protection off)
- **0x86 bits 2..6**: unused/reserved
- **0x86 bit 7**: CAOS ROM at address 0xC000 on/off

### Z80 CTC Registers

(FIXME!)


### KC85/3 Video Memory Layout

(FIXME!)

### KC85/4 Video Memory Layout

(FIXME!)



