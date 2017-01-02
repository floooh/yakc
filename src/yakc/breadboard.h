#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::breadboard
    @brief houses all the common chips required by emulated systems
*/
#include "yakc/core.h"
#include "yakc/clock.h"
#include "yakc/z80.h"
#include "yakc/z80dbg.h"
#include "yakc/z80pio.h"
#include "yakc/z80ctc.h"
#include "yakc/i8255.h"
#include "yakc/mc6845.h"
#include "yakc/sound_beeper.h"
#include "yakc/sound_speaker.h"
#include "yakc/sound_ay8910.h"
#include "yakc/crt.h"

namespace YAKC {

class breadboard {
public:
    clock clck;
    z80 cpu;
    class z80pio z80pio;
    class z80pio z80pio2;
    class z80ctc z80ctc;
    class i8255 i8255;
    class mc6845 mc6845;
    sound_beeper beeper;
    sound_speaker speaker;
    sound_ay8910 ay8910;
    z80dbg dbg;
    class crt crt;          // this is not a chip, but a cathode-ray-tube emulation
    ubyte ram[8][0x4000];
};

} // namespace YAKC
