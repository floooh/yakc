//------------------------------------------------------------------------------
//  zx.cc
//------------------------------------------------------------------------------
#include "zx.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
zx::init(breadboard* b) {
    this->board = b;

    // setup color palette
    this->pal[0] = 0xFF000000;      // black
    this->pal[1] = 0xFFFF0000;      // blue
    this->pal[2] = 0xFF0000FF;      // red
    this->pal[3] = 0xFFFF00FF;      // magenta
    this->pal[4] = 0xFF00FF00;      // green
    this->pal[5] = 0xFFFFFF00;      // cyan
    this->pal[6] = 0xFF00FFFF;      // yello
    this->pal[7] = 0xFFFFFFFF;      // white

    // setup key translation table
    this->init_keymap();
}

//------------------------------------------------------------------------------
static uint64_t
key_bit(int column, int line) {
    // returns keyboard matrix bit mask with single bit set at column and line
    YAKC_ASSERT((column >= 0) && (column < 8));
    YAKC_ASSERT((line >= 0) && (line < 5));
    return (uint64_t(1)<<line)<<(column*5);
}

//------------------------------------------------------------------------------
void
zx::init_key_mask(ubyte ascii, int column, int line, int shift) {
    // initialize an entry in the keyboard mapping table which
    // maps ASCII codes to keyboard matrix bit mask
    YAKC_ASSERT((column >= 0) && (column < 8));
    YAKC_ASSERT((line >= 0) && (line < 5));
    YAKC_ASSERT((shift >= 0) && (shift < 3));
    uint64_t mask = key_bit(column, line);
    if (1 == shift) {
        // caps-shift
        mask |= key_bit(0, 0);
    }
    if (2 == shift) {
        // sym-shift
        mask |= key_bit(7, 1);
    }
    this->key_map[ascii] = mask;
}

//------------------------------------------------------------------------------
void
zx::init_keymap() {
    // initialize the keyboard matrix lookup table, maps
    // ASCII codes to keyboard matrix state
    clear(this->key_map, sizeof(this->key_map));
    const char* kbd =
        // no shift
        " zxcv"         // A8       shift,z,x,c,v
        "asdfg"         // A9       a,s,d,f,g
        "qwert"         // A10      q,w,e,r,t
        "12345"         // A11      1,2,3,4,5
        "09876"         // A12      0,9,8,7,6
        "poiuy"         // A13      p,o,i,u,y
        " lkjh"         // A14      enter,l,k,j,h
        "  mnb"         // A15      space,symshift,m,n,b

        // shift
        " ZXCV"         // A8
        "ASDFG"         // A9
        "QWERT"         // A10
        "     "         // A11
        "     "         // A12
        "POIUY"         // A13
        " LKJH"         // A14
        "  MNB"         // A15

        // symshift
        " : ?/"         // A8
        "     "         // A9
        "   <>"         // A10
        "!@#$%"         // A11
        "_)('&"         // A12
        "\";   "        // A13
        " =+-^"         // A14
        "  .,*";        // A15

    for (int shift = 0; shift < 3; shift++) {
        for (int column = 0; column < 8; column++) {
            for (int line = 0; line < 5; line++) {
                const ubyte c = kbd[shift*40 + column*5 + line];
                if (c != 0x20) {
                    this->init_key_mask(c, column, line, shift);
                }
            }
        }
    }

    // special keys
    this->init_key_mask(' ', 7, 0, 0);  // Space
    this->init_key_mask(0x08, 3, 4, 1); // Cursor Left (Shift+5)
    this->init_key_mask(0x0A, 4, 4, 1); // Cursor Down (Shift+6)
    this->init_key_mask(0x0B, 4, 3, 1); // Cursor Up (Shift+7)
    this->init_key_mask(0x09, 4, 2, 1); // Cursor Right (Shift+8)
    this->init_key_mask(0x07, 3, 0, 1); // Edit (Shift+1)
    this->init_key_mask(0x0C, 4, 0, 1); // Delete (Shift+0)
    this->init_key_mask(0x0D, 6, 0, 0); // Enter
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
        // Spectrum 128k initial memory mapping
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
    // FIXME!
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
    this->memory_paging_disabled = false;
    this->next_kbd_mask = 0;
    this->cur_kbd_mask = 0;
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
    this->memory_paging_disabled = false;
    this->next_kbd_mask = 0;
    this->cur_kbd_mask = 0;    
    this->pal_line_counter = 0;
    this->blink_counter = 0;
    if (device::zxspectrum48k == this->cur_model) {
        this->display_ram_bank = 0;
    }
    else {
        this->display_ram_bank = 5;
    }
    this->board->cpu.reset();
    this->board->cpu.PC = 0x0000;
    this->init_memory_mapping();
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
    // handle Z80 OUT instruction
    if ((port & 1) == 0) {
        // "every even IO port addresses the ULA but to avoid
        // problems with other I/O devices, only FE should be used"
        this->border_color = this->pal[val & 7] & 0xFFD7D7D7;
        // FIXME:
        //      bit 3: MIC output (CAS SAVE, 0=On, 1=Off)
        //      bit 4: Beep output (ULA sound, 0=Off, 1=On)
        return;
    }
    if (device::zxspectrum128k == this->cur_model) {
        if (0x7FFD == port) {
            if (!this->memory_paging_disabled) {
                // store display RAM bank index
                this->display_ram_bank = (val & (1<<3)) ? 7 : 5;

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
                if (val & (1<<4)) {
                    // bit 4 set: ROM1
                    mem.map(0, 0x0000, 0x4000, dump_amstrad_zx128k_1, false);
                }
                else {
                    // bit 4 clear: ROM0
                    mem.map(0, 0x0000, 0x4000, dump_amstrad_zx128k_0, false);
                }
            }
            if (val & (1<<5)) {
                // bit 5 prevents further changes to memory pages
                // until computer is reset, this is used when switching
                // to the 48k ROM
                this->memory_paging_disabled = true;
            }
            return;
        }
    }
}

//------------------------------------------------------------------------------
void
zx::put_key(ubyte ascii) {
    // register a new key press with the emulator
    if (ascii) {
        this->next_kbd_mask = this->key_map[ascii];
    }
    else {
        this->next_kbd_mask = 0;
    }
}

//------------------------------------------------------------------------------
static ubyte
extract_kbd_line_bits(uint64_t mask, int column) {
    // extract keyboard matrix line bits by column
    return (ubyte) ((mask>>(column*5)) & 0x1F);
}

//------------------------------------------------------------------------------
ubyte
zx::cpu_in(uword port) {
    // handle Z80 IN instruction
    switch (port) {
        case 0xFEFE:
            // start of a new keyboard request cycle,
            // fetch next key's keymask
            this->cur_kbd_mask = this->next_kbd_mask;
            // keyboard matrix column 0: shift, z, x, c, v
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 0);
        case 0xFDFE:
            // keyboard matrix column 1: a, s, d, f, g
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 1);
        case 0xFBFE:
            // keyboard matrix column 2: q, w, e, r, t
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 2);
        case 0xF7FE:
            // keyboard matrix column: 1, 2, 3, 4, 5
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 3);
        case 0xEFFE:
            // keyboard matrix column: 0, 9, 8, 7, 6
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 4);
        case 0xDFFE:
            // keyboard matrix column: p, o, i, u, y
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 5);
        case 0xBFFE:
            // keyboard matrix column: enter, l, k, j, h
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 6);
        case 0x7FFE:
            // keyboard matrix column: space, sym shift, m, n, b
            return ~extract_kbd_line_bits(this->cur_kbd_mask, 7);
        case 0xFFFD:
            // FIXME: audio related
            return 0xFF;
        default:
            return 0xFF;
    }
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
    // clock timer callback
    if (0 == timer_id) {
        // timer 0: new PAL line
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
    // start new frame, request vblank interrupt
    if (this->pal_line_counter > max_pal_lines) {
        this->pal_line_counter = 0;
        this->blink_counter++;
        this->irq();
    }
}

//------------------------------------------------------------------------------
const char*
zx::system_info() const {
    return
        "FIXME!\n\n"
        "ZX ROM images acknowledgement:\n\n"
        "Amstrad have kindly given their permission for the redistribution of their copyrighted material but retain that copyright.\n\n"
        "See: http://www.worldofspectrum.org/permits/amstrad-roms.txt";
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
    //
    // the blink flag flips every 16 frames
    //
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
