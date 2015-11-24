# yakc
Yet another KC emulator (WTF is a 'KC': https://en.wikipedia.org/wiki/KC_85)

[![Build Status](https://travis-ci.org/floooh/yakc.svg)](https://travis-ci.org/floooh/yakc)

** WORK IN PROGRESS!! **

### Live Demo:

A 'mostly uptodate' emscripten-compiled version is here:

http://floooh.github.io/virtualkc/

### How to Build:

```bash
> python --version
Python 2.7.10
> cmake --version
cmake version 3.3.2
> git clone https://github.com/floooh/yakc
> cd yakc
> ./fips gen
> ./fips make yakc_app
> ./fips run yakc_app
```

This should work on OSX, Linux and Windows, but only OSX is really tested
so far.

### Screenshots

![Overlay UI](screenshots/debug_ui.png)

![Pengo Intro Screen](screenshots/pengo_menu.png)

![Pengo Ingame](screenshots/pengo_ingame.png)

![Cave](screenshots/cave.png)

![House Intro Screen](screenshots/house_menu.png)

![House Ingame](screenshots/house_ingame.png)

### Goals:
- small and portable emulator core in a simple C++
- emulator core has hooks for keyboard-input, audio/video-output, load/save blocks of memory
- Oryol as input/video/audio wrapper (https://github.com/floooh/oryol)
- imgui (https://github.com/ocornut/imgui) as simple debugger overlay (view and edit registers, memory, asm/disasm)
- emulate KC85/3 and KC85/4
- small and fast enough to feel good on browser and mobile 

### Non-Goals:
- no complete hardware emulation ('display needling', I/O ports, modules, analog cassette interface, ...)
- no emulation of the tricky serial keyboard interface

### Potential goals:
- CPU/memory state recording to allow rewind/replay in debugger (that would be cool!)
- support for other East-German 8-bitters (Z9001, Z1013, LC80)
- Speccy, CPC, KC-compact?

### Use of external code:

- for the tricky undocumented CPU flag computations I have used portions of the
MAME z80 implementation: https://github.com/mamedev/mame/blob/master/src/devices/cpu/z80/z80.c
- the debugger's disassembler is also ripped from MAME: 
https://github.com/mamedev/mame/blob/master/src/devices/cpu/z80/z80dasm.c
- the wrapper application uses Oryol as portability wrapper:
https://github.com/floooh/oryol
- the debugger UI is implemented on top of IMGUI:
https://github.com/ocornut/imgui
- the ZEXDOC/ZEXALL conformance tests by Frank D. Cringle in yakc_test 
for testing Z80 compatibility is licensed under the GPL2, the source code 
is also located in yakc_test
