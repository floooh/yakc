//------------------------------------------------------------------------------
//  zx.cc
//------------------------------------------------------------------------------
#include "zx.h"

#include <stdio.h>

namespace YAKC {

//------------------------------------------------------------------------------
void
zx::init(breadboard* b) {
    this->board = b;

    // color palette
    this->pal[0] = 0xFF000000;      // black
    this->pal[1] = 0xFFFF0000;      // blue
    this->pal[2] = 0xFF0000FF;      // red
    this->pal[3] = 0xFFFF00FF;      // magenta
    this->pal[4] = 0xFF00FF00;      // green
    this->pal[5] = 0xFFFFFF00;      // cyan
    this->pal[6] = 0xFF00FFFF;      // yello
    this->pal[7] = 0xFFFFFFFF;      // white
}

//------------------------------------------------------------------------------
void
zx::init_memory_mapping() {
    z80& cpu = this->board->cpu;
    cpu.mem.unmap_all();
    if (device::zxspectrum48k == this->cur_model) {
        // 48k RAM between 0x4000 and 0xFFFF
        cpu.mem.map(0, 0x4000, 0x4000, this->ram[0], true);
        cpu.mem.map(0, 0x8000, 0x4000, this->ram[1], true);
        cpu.mem.map(0, 0xC000, 0x4000, this->ram[2], true);

        // 16k ROM between 0x0000 and 0x3FFF
        YAKC_ASSERT(sizeof(dump_amstrad_zx48k) == 0x4000);
        cpu.mem.map(0, 0x0000, 0x4000, dump_amstrad_zx48k, false);
    }
    else {
        // Spectrum 128k
        cpu.mem.map(0, 0x4000, 0x4000, this->ram[5], true);
        cpu.mem.map(0, 0x8000, 0x4000, this->ram[2], true);
        cpu.mem.map(0, 0xC000, 0x4000, this->ram[0], true);
        YAKC_ASSERT(sizeof(dump_amstrad_zx128k_0) == 0x4000);
        cpu.mem.map(0, 0x0000, 0x4000, dump_amstrad_zx128k_0, false);
    }
}

//------------------------------------------------------------------------------
void
zx::on_context_switched() {
    // FIXME
}

//------------------------------------------------------------------------------
void
zx::border_color(float& out_red, float& out_green, float& out_blue) {
    out_blue  = ((this->brder_color >> 16) & 0xFF) / 255.0f;
    out_green = ((this->brder_color >> 8) & 0xFF) / 255.0f;
    out_red  =  (this->brder_color & 0xFF) / 255.0f;
}

//------------------------------------------------------------------------------
void
zx::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_zx) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->brder_color = 0;
    if (m == device::zxspectrum48k) {
        this->cur_os = os_rom::amstrad_zx48k;
        this->display_ram_bank = 0;
    }
    else {
        this->cur_os = os_rom::amstrad_zx128k;
        this->display_ram_bank = 5;
    }
    this->on = true;

    // map memory
    this->init_memory_mapping();

    // initialize the clock, the z1013_01 runs at 1MHz, all others at 2MHz
    this->board->clck.init((m == device::zxspectrum48k) ? 3500 : 3547);

    // initialize hardware components
    this->board->cpu.init();

    // execution on power-on starts at 0x0000
    this->board->cpu.PC = 0x0000;
}

//------------------------------------------------------------------------------
void
zx::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
void
zx::reset() {
    this->board->cpu.reset();
    this->board->cpu.PC = 0x0000;
}

//------------------------------------------------------------------------------
uint64_t
zx::step(uint64_t start_tick, uint64_t end_tick) {
    z80& cpu = this->board->cpu;
    z80dbg& dbg = this->board->dbg;
    clock& clk = this->board->clck;
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        if (dbg.check_break(cpu)) {
            dbg.paused = true;
            return end_tick;
        }
        dbg.store_pc_history(cpu); // FIXME: only if debug window open?
        int ticks_step = cpu.step(this);
        ticks_step += cpu.handle_irq();
        clk.update(this, ticks_step);
        cur_tick += ticks_step;
    }
    this->decode_video();
    return cur_tick;
}

//------------------------------------------------------------------------------
void
zx::cpu_out(uword port, ubyte val) {
    if ((port & 0xFF) == 0xFE) {
        this->brder_color = this->pal[val & 7] & 0xFFD7D7D7;
        // FIXME: bit 3 activate MIC output, bit 4 activate EAR/speaker output
        return;
    }
    if (device::zxspectrum128k == this->cur_model) {
        if (0x7FFD == port) {
            // store display RAM bank index
            this->display_ram_bank = (val & 0x8) ? 7 : 5;

            // FIXME: bit 5 locks future writing to 0x7FFD until reset

            // control memory bank switching on 128K
            // http://8bit.yarek.pl/computer/zx.128/
            auto& mem = this->board->cpu.mem;
            mem.unmap_layer(0);

            // section D (0xC000 .. 0xFFFF)
            mem.map(0, 0xC000, 0x4000, this->ram[val & 0x7], true);
            // section C, always mapped to RAM2 (0x8000 .. 0xBFFF)
            mem.map(0, 0x8000, 0x4000, this->ram[2], true);
            // section B, always mapped to RAM5 (0x4000 .. 0x7FFF)
            mem.map(0, 0x4000, 0x4000, this->ram[5], true);
            // ROM bank
            if (val & 0x10) {
                // bit 4 set: ROM1
                mem.map(0, 0x0000, 0x4000, dump_amstrad_zx128k_1, false);
            }
            else {
                // bit 4 clear: ROM0
                mem.map(0, 0x0000, 0x4000, dump_amstrad_zx128k_0, false);
            }
            return;
        }
    }
}

//------------------------------------------------------------------------------
ubyte
zx::cpu_in(uword port) {
    printf("IN port: %04x\n", port);
    return 0xFF;
}

//------------------------------------------------------------------------------
void
zx::irq() {
    // forward interrupt request to CPU
    this->board->cpu.irq();
}

//------------------------------------------------------------------------------
void
zx::put_key(ubyte ascii) {
    if (ascii != this->key_code) {
        this->key_code = ascii;
        this->handle_keyboard_input();
    }
}

//------------------------------------------------------------------------------
const char*
zx::system_info() const {
    return "FIXME!";
}

//------------------------------------------------------------------------------
void
zx::handle_keyboard_input() {
    // FIXME: this is a quick'n'dirty keyboard input which
    // directly writes to system locations
    uword lastk_addr = 0x5C08;
    uword flags_addr = 0x5C3B;
    auto& mem = this->board->cpu.mem;

    // only write new keycode if previous one was handled
    ubyte flags = mem.r8(flags_addr);
    if (0 == (flags & (1<<5))) {
        if (key_code != 0) {
            mem.w8(lastk_addr, this->key_code);
            mem.w8(flags_addr, flags | (1<<5));
        }
    }
}

//------------------------------------------------------------------------------
void
zx::decode_video() {
    uint32_t* dst = rgba8_buffer;
    uint8_t* vidmem_bank = this->ram[this->display_ram_bank];
    for (uint16_t y = 0; y < 192; y++) {
        uint16_t y_offset = ((y & 0xC0)<<5) | ((y & 0x07)<<8) | ((y & 0x38)<<2);
        for (uint16_t x = 0; x < 32; x++) {
            // pixel offset:
            // | 0| 1| 0|Y7|Y6|Y2|Y1|Y0|Y5|Y4|Y3|X4|X3|X2|X1|X0|
            //
            // color offset is linear
            uint16_t pix_offset = y_offset | x;
            uint16_t clr_offset = 0x1800 + (((y & ~0x7)<<2) | x);

            // pixel mask and color attribute bytes
            uint8_t pix = vidmem_bank[pix_offset];
            uint8_t clr = vidmem_bank[clr_offset];

            // foreground and background color
            uint32_t fg = this->pal[clr & 7];
            uint32_t bg = this->pal[(clr>>3) & 7];
            if (0 == (clr & (1<<6))) {
                // standard brightness
                fg &= 0xFFD7D7D7;
                bg &= 0xFFD7D7D7;
            }
            for (int px = 7; px >=0; px--) {
                *dst++ = pix & (1<<px) ? fg : bg;
            }
        }
    }
}

} // namespace YAKC

