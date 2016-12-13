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
zx::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_zx) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->border_color = 0xFF000000;
    this->pal_line_counter = 0;
    this->key_code = 0;
    if (device::zxspectrum48k == m) {
        this->cur_os = os_rom::amstrad_zx48k;
        this->display_ram_bank = 0;
    }
    else {
        this->cur_os = os_rom::amstrad_zx128k;
        this->display_ram_bank = 5;
    }
    this->on = true;

    // map memory
    clear(this->ram, sizeof(this->ram));
    this->init_memory_mapping();

    // initialize the system clock and PAL-line timer
    if (device::zxspectrum48k == m) {
        // Spectrum48K is exactly 3.5 MHz
        this->board->clck.init(3500);
        this->board->clck.config_timer_cycles(0, 224);
    }
    else {
        // 128K is slightly faster
        this->board->clck.init(3547);
        this->board->clck.config_timer_cycles(0, 228);
    }

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
    return cur_tick;
}

//------------------------------------------------------------------------------
void
zx::cpu_out(uword port, ubyte val) {
    if ((port & 0xFF) == 0xFE) {        
        this->border_color = this->pal[val & 7] & 0xFFD7D7D7;
        // FIXME:
        //      bit 3: MIC output (CAS SAVE, 0=On, 1=Off)
        //      bit 4: Beep output (ULA sound, 0=Off, 1=On)
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
//    printf("IN port: %04x\n", port);
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
zx::timer(int timer_id) {
    if (0 == timer_id) {
        this->pal_line();
    }
}

//------------------------------------------------------------------------------
void
zx::pal_line() {
    // this is called by the timer callback for every PAL line, controlling
    // the vidmem decoding and vblank interrupt
    //
    // detailed information about frame timings is here:
    //  for 48K:    http://rk.nvg.ntnu.no/sinclair/faq/tech_48.html#48K
    //  for 128K:   http://rk.nvg.ntnu.no/sinclair/faq/tech_128.html
    //
    // one PAL line takes 224 T-states on 48K, and 228 T-states on 128K
    // one PAL frame is 312 lines on 48K, and 311 lines on 128K
    //
    const uint32_t max_pal_lines = this->cur_model == device::zxspectrum128k ? 311 : 312;
    const uint32_t top_border = this->cur_model == device::zxspectrum128k ? 63 : 64;

    // decode the next videomem line into the emulator framebuffer,
    // the border area of a real Spectrum is bigger than the emulator
    // (the emu has 32 pixels border on each side, the hardware has:
    //
    //  63 or 64 lines top border
    //  56 border lines bottom border
    //  48 pixels on each side horizontal border
    //
    const uint32_t top_decode_line = top_border - 32;
    const uint32_t btm_decode_line = top_border + 192 + 32;
    if ((this->pal_line_counter >= top_decode_line) && (this->pal_line_counter < btm_decode_line)) {
        this->decode_video_line(this->pal_line_counter - top_decode_line);
    }

    this->pal_line_counter++;
    if (this->pal_line_counter > max_pal_lines) {
        // start new frame, request vblank interrupt
        this->pal_line_counter = 0;
        this->blink_counter++;
        this->irq();
    }
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
        if (this->key_code != 0) {
            mem.w8(lastk_addr, this->key_code);
            mem.w8(flags_addr, flags | (1<<5));
        }
    }
}

//------------------------------------------------------------------------------
void
zx::decode_video_line(uint16_t y) {
    YAKC_ASSERT((y >= 0) && (y < 256));

    // decode a single line from the ZX framebuffer into the
    // emulator's framebuffer
    //
    // this is called from the pal_line() callback so that video decoding
    // is synchronized with the CPU and vblank interrupt,
    // y is ranging from 0 to 256 (32 pixels vertical border on each side)

    uint32_t* dst = &(this->rgba8_buffer[y*320]);
    const uint8_t* vidmem_bank = this->ram[this->display_ram_bank];
    const bool blink = 0 != (this->blink_counter & 0x10);
    uint32_t fg, bg;
    if ((y < 32) || (y >= 224)) {
        // upper/lower border
        for (int x = 0; x < 320; x++) {
            *dst++ = this->border_color;
        }
    }
    else {
        // compute video memory Y offset (inside 256x192 area)
        uint16_t yy = y-32;
        uint16_t y_offset = ((yy & 0xC0)<<5) | ((yy & 0x07)<<8) | ((yy & 0x38)<<2);

        // left border
        for (int x = 0; x < (4*8); x++) {
            *dst++ = this->border_color;
        }

        // valid 256x192 vidmem area
        for (uint16_t x = 0; x < 32; x++) {
            // pixel offset:
            // | 0| 1| 0|Y7|Y6|Y2|Y1|Y0|Y5|Y4|Y3|X4|X3|X2|X1|X0|
            //
            // color offset is linear
            uint16_t pix_offset = y_offset | x;
            uint16_t clr_offset = 0x1800 + (((yy & ~0x7)<<2) | x);

            // pixel mask and color attribute bytes
            uint8_t pix = vidmem_bank[pix_offset];
            uint8_t clr = vidmem_bank[clr_offset];

            // foreground and background color
            if ((clr & (1<<7)) && blink) {
                fg = this->pal[(clr>>3) & 7];
                bg = this->pal[clr & 7];
            }
            else {
                fg = this->pal[clr & 7];
                bg = this->pal[(clr>>3) & 7];
            }
            if (0 == (clr & (1<<6))) {
                // standard brightness
                fg &= 0xFFD7D7D7;
                bg &= 0xFFD7D7D7;
            }
            for (int px = 7; px >=0; px--) {
                *dst++ = pix & (1<<px) ? fg : bg;
            }
        }

        // right border
        for (int x = 0; x < (4*8); x++) {
            *dst++ = this->border_color;
        }
    }
}

} // namespace YAKC
