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
port 0x84 and 0x86 are backed at memory addresses (IX+1) and (IX+4):

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

### Z80 CTC Channels

The CTC channels are used to generate the stereo audio signal and 
for measuring the time between serial keyboard impulses:

- **CTC 0**: configured as timer, generates the audio frequency
for one audio channel
- **CTC 1**: configured as timer, generates the audio frequency for the 
other audio channel, and for the cassette tape save audio signal
- **CTC 2**: usually used as timer to generate an interrupt for sound length,
also controls the video foreground color blinking frequency
- **CTC 3**: used by the keyboard interrupt handler to measure the length
between two serial impulses from the keyboard

The interrupt vectors for CTC channel 0 to 3 start at 0x01E8:

- **0x01E8**: CTC-0 interrupt vector (unused)
- **0x01EA**: CTC-1 interrupt vector (cassette tape output)
- **0x01EC**: CTC-2 interrupt vector (sound duration)
- **0x01EE**: CTC-3 interrupt vector (keyboard input)

### Color Buffer Byte Structure

A byte in color buffer uses 3 bits for the background color,
4 bits for the foreground color, and 1 bit for blinking:

~~~
 7  6  5  4  3  2  1  0
+--+--+--+--+--+--+--+--+
|B |FX|FG|FR|FB|BG|BR|BB|
+--+--+--+--+--+--+--+--+
~~~

- **B**: foreground color blink bit
- **FX**: foreground color intensity
- **FG**: foreground color green
- **FR**: foreground color red
- **FB**: foreground color blue
- **BG**: background color green
- **BR**: background color red
- **BB**: background color blue

### KC85/3 Video Memory Layout

The KC85/3 video memory consists of a single pixel buffer at address 
0x8000 followed by a lower-resolution color buffer at address 0xA800. 

A single pixel buffer byte represents a horizontal 8-pixel pattern, and 
one color buffer byte describes the foreground and background color
for a block of 8x4 pixel.

The display resolution is 320x256 pixels, which results in a pixel buffer
size of 10KByte and a color buffer size of 2.5KByte. The remaining memory
in the video memory bank is used for a 40x32 ASCII code backing buffer,
operating system variables and cassette loading/saving buffers.

The pixel buffer layout is not linear, there is a 256x256 pixel area
on the left side, and 64x256 pixel area on the right side.

Vertical lines are not sequential but interleaved. The next line is reached by
adding 0x80 to the current address, 

##### KC85/3 Pixel buffer memory layout:

Each block is 8x1 pixels, or 1 byte:

~~~
 0                                32      40
+--------------------------------+--------+
|8000                            |A000    |
|8080                            |A080    |
|8100                            |A100    |
|8180                            |A180    |
|8020                            |A020    |
|80A0                            |A0A0    |
|8120                            |A120    |
|81A0                            |A1A0    |
|...
~~~

##### KC85/3 Color buffer memory layout:

Each block is 8x4 pixels, or 1 byte:

~~~
 0                                32      40
+--------------------------------+--------+
|A800                            |B000    |
|A820                            |B020    |
|...
~~~

### KC85/4 Video Memory Layout

On the KC85/4, the pixel resolution is the same, each pixel buffer byte
describes 8x1 pixels.

The color resolution has increased to be the same as the pixel resolution,
each byte in the color buffer describes the foreground and background
color for 8x1 pixels.

Since the pixel and color buffer no longer fit into one 16KByte memory bank,
they have been split into two separate memory banks, this means that
a bank switch is necessary to write pixels and colors.

The KC85/4 also has a second set of pixel- and color-buffers, and it is
possible to select a different set for displaying and CPU access, making
hardware-double-buffering possible.

The pixel- and color-buffer layout has been rotated by 90 degrees. Writing
bytes sequentially in pixel- or color-buffer fills vertical columns on screen,
which drastically simplifies video memory addressing. The separate left
and right area that had to be addressed differently are gone.

Since pixel- and color-buffers have the same resolution, the address 
computation for both is identical.

~~~
+----------------------------------------+
|8000                                    |
|8001                                    |
|8002                                    |
|...                                     |
~~~

### KC85/4 Hicolor Mode

(TODO)





