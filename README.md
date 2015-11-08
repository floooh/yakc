# yakc
Yet another KC emulator (WTF is a 'KC': https://en.wikipedia.org/wiki/KC_85)

[![Build Status](https://travis-ci.org/floooh/yakc.svg)](https://travis-ci.org/floooh/yakc)

Live Demo:

This is a 'mostly uptodate' emscripten-compiled version:

http://floooh.github.io/virtualkc/

Goals:
- small and portable emulator core in a simple C++
- emulator core has hooks for keyboard-input, audio/video-output, load/save blocks of memory
- Oryol as input/video/audio wrapper (https://github.com/floooh/oryol)
- imgui (https://github.com/ocornut/imgui) as simple debugger overlay (view and edit registers, memory, asm/disasm)
- emulate KC85/3 and KC85/4
- small and fast enough to feel good on browser and mobile 

Non-Goals:
- no complete hardware emulation ('display needling', I/O ports, modules, analog cassette interface, ...)
- no emulation of the tricky serial keyboard interface

Potential goals:
- CPU/memory state recording to allow rewind/replay in debugger (that would be cool!)
- support for other East-German 8-bitters (Z9001, Z1013, LC80)
- Speccy, CPC, KC-compact?
