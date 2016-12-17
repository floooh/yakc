//------------------------------------------------------------------------------
//  cpc.cc
//------------------------------------------------------------------------------
#include "cpc.h"
#include "roms/roms.h"

namespace YAKC {

//
// http://www.cpcwiki.eu/index.php/CPC_Palette
// http://www.grimware.org/doku.php/documentations/devices/gatearray
//
// index into this palette is the 'hardware color number' & 0x1F
// order is ABGR
//
uint32_t hw_palette[32] = {
    0xff6B7D6E,         // #40 white
    0xff6D7D6E,         // #41 white
    0xff6BF300,         // #42 sea grean
    0xff6DF3F3,         // #43 pastel yellow
    0xff6B0200,         // #44 blue
    0xff6802F0,         // #45 purple
    0xff687800,         // #46 cyan
    0xff6B7DF3,         // #47 pink
    0xff6802F3,         // #48 purple
    0xff6BF3F3,         // #49 pastel yellow
    0xff0DF3F3,         // #4A bright yellow
    0xffF9F3FF,         // #4B bright white
    0xff0605F3,         // #4C bright red
    0xffF402F3,         // #4D bright magenta
    0xff0D7DF3,         // #4E orange
    0xffF980FA,         // #4F pastel magenta
    0xff680200,         // #50 blue
    0xff6BF302,         // #51 sea green
    0xff01F002,         // #52 bright green
    0xffF2F30F,         // #53 bright cyan
    0xff010200,         // #54 black
    0xffF4020C,         // #55 bright blue
    0xff017802,         // #56 green
    0xffF47B0C,         // #57 sky blue
    0xff680269,         // #58 magenta
    0xff6BF371,         // #59 pastel green
    0xff04F571,         // #5A lime
    0xffF4F371,         // #5B pastel cyan
    0xff01026C,         // #5C red
    0xffF2026C,         // #5D mauve
    0xff017B6E,         // #5E yellow
    0xffF67B6E,         // #5F pastel blue
};

//------------------------------------------------------------------------------
void
cpc::init(breadboard* b) {
    this->board = b;
    clear(this->pens, sizeof(this->pens));
}

//------------------------------------------------------------------------------
void
cpc::init_memory_map() {
    z80& cpu = this->board->cpu;
    cpu.mem.unmap_all();

    if (device::cpc464 == this->cur_model) {
        // map 32 KByte RAM from 4000 to BFFF
        cpu.mem.map(0, 0x4000, 0x4000, this->ram[1], true);
        cpu.mem.map(0, 0x8000, 0x4000, this->ram[2], true);

        // lower ROM (OS), writes always go to RAM bank 0
        YAKC_ASSERT(sizeof(dump_cpc464_os) == 0x4000);
        cpu.mem.map_rw(0, 0x0000, 0x4000, dump_cpc464_os, this->ram[0]);

        // upper ROM (BASIC), writes always go to RAM bank 3
        YAKC_ASSERT(sizeof(dump_cpc464_basic) == 0x4000);
        cpu.mem.map_rw(0, 0xC000, 0x4000, dump_cpc464_basic, this->ram[3]);
    }
    else if (device::cpc6128 == this->cur_model) {
        // FIXME
    }
}

//------------------------------------------------------------------------------
void
cpc::on_context_switched() {
    // FIXME!
}

//------------------------------------------------------------------------------
void
cpc::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_cpc) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;
    this->select_pen = 0;
    this->border_color = 0;
    clear(this->pens, sizeof(this->pens));

    // map memory
    clear(this->ram, sizeof(this->ram));
    this->init_memory_map();

    // initialize clock and timers
    this->board->clck.init(4000);

    // CPU start state
    this->board->cpu.init();
    this->board->cpu.PC = 0x0000;
}

//------------------------------------------------------------------------------
void
cpc::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->cpu.mem.unmap_all();
    this->on = false;
    this->select_pen = 0;
    this->border_color = 0;
    clear(this->pens, sizeof(this->pens));
}

//------------------------------------------------------------------------------
void
cpc::reset() {
    this->board->cpu.reset();
    this->board->cpu.PC = 0x0000;
    this->init_memory_map();
}

//------------------------------------------------------------------------------
uint64_t
cpc::step(uint64_t start_tick, uint64_t end_tick) {
    // step the system for given number of cycles, return actually
    // executed number of cycles
    z80& cpu = this->board->cpu;
    z80dbg& dbg = this->board->dbg;
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        if (dbg.check_break(cpu)) {
            dbg.paused = true;
            return end_tick;
        }
        dbg.store_pc_history(cpu); // FIXME: only if debug window open?
        int ticks_step = cpu.step(this);
        // need to round up ticks to 4, this is a CPC specialty
        ticks_step += cpu.handle_irq();
        ticks_step = (ticks_step + 3) & ~3;
        this->board->clck.update(this, ticks_step);
        cur_tick += ticks_step;
    }

    // FIXME
    for (uint16_t y = 0; y < 200; y++) {
        this->decode_video_line(y);
    }
    return cur_tick;
}

//------------------------------------------------------------------------------
void
cpc::cpu_out(uword port, ubyte val) {
    YAKC_ASSERT(device::cpc464 == this->cur_model);
    if ((port & 0xFF00) == 0x7F00) {
        //
        // http://www.grimware.org/doku.php/documentations/devices/gatearray
        // http://www.cpcwiki.eu/index.php/Gate_Array
        //
        //  bits 7 and 6 (on CPC plus also 5) define the command
        //
        const ubyte reg = val & 0xC0;
        if (reg == 0x00) {
            // if bit 4 is set, this means that the border color should be set
            this->select_pen = val & 0x1F;
        }
        else if (reg == 0x40) {
            if (this->select_pen & 0x10) {
                // set border color
                this->border_color = hw_palette[val & 0x1F];
            }
            else {
                // set pen
                this->pens[this->select_pen & 0x0F] = hw_palette[val & 0x1F];
            }
        }
        else if (reg == 0x80) {
            //
            //  select screen mode, rom config and int ctrl
            //
            //  - bits 0 and 1 select the screen mode
            //      00: Mode 0 (160x200 @ 16 colors)
            //      01: Mode 1 (320x200 @ 4 colors)
            //      02: Mode 2 (640x200 @ 2 colors)
            //      11: Mode 3 (160x200 @ 2 colors, undocumented)
            //
            //  - bit 2: disable/enable lower ROM
            //  - bit 3: disable/enable upper ROM
            //
            //  - bit 4: interrupt generation control
            //
            auto& mem = this->board->cpu.mem;

            // enable/disable lower ROM
            if (val & (1<<2)) {
                mem.map_rw(0, 0x0000, 0x4000, this->ram[0], this->ram[0]);
            }
            else {
                mem.map_rw(0, 0x0000, 0x4000, dump_cpc464_os, this->ram[0]);
            }

            // enable/disable upper ROM
            if (val & (1<<3)) {
                mem.map_rw(0, 0xC000, 0x4000, this->ram[3], this->ram[3]);
            }
            else {
                mem.map_rw(0, 0xC000, 0x4000, dump_cpc464_basic, this->ram[3]);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
cpc::put_key(ubyte ascii) {
    // FIXME
}

//------------------------------------------------------------------------------
ubyte
cpc::cpu_in(uword port) {
    // FIXME
    return 0xFF;
}

//------------------------------------------------------------------------------
void
cpc::irq() {
    // forward interrupt request to CPU
    this->board->cpu.irq();
}

//------------------------------------------------------------------------------
void
cpc::timer(int timer_id) {
    // FIXME
}

//------------------------------------------------------------------------------
const char*
cpc::system_info() const {
    return "FIXME!";
}

//------------------------------------------------------------------------------
void
cpc::decode_video_line(uint16_t y) {
    //
    // mode 0: 160x200      2 pixels per byte
    // mode 1: 320x200      4 pixels per byte
    // mode 2: 640x200      8 pixels per byte
    //
    uint32_t y_offset = (y&7)*2048 + (y>>3)*80;
    uint32_t* dst = &(this->rgba8_buffer[y * 640]);
    const uint8_t* src = &(this->ram[3][y_offset]);
    uint32_t p;
    for (uint32_t x = 0; x < 80; x++) {
        const uint8_t val = src[x];

        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
        p = this->pens[((val>>2)&2)|((val>>7)&1)];
        *dst++ = p; *dst++ = p;
        p = this->pens[((val>>1)&2)|((val>>6)&1)];
        *dst++ = p; *dst++ = p;
        p = this->pens[((val>>0)&2)|((val>>5)&1)];
        *dst++ = p; *dst++ = p;
        p = this->pens[((val<<1)&2)|((val>>4)&1)];
        *dst++ = p; *dst++ = p;
    }
}

} // namespace YAKC
