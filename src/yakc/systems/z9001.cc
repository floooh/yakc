//------------------------------------------------------------------------------
//  z9001.cc
//------------------------------------------------------------------------------
#include "z9001.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

z9001_t z9001;

static uint32_t palette[8] = {
    0xFF000000,     // black
    0xFF0000FF,     // red
    0xFF00FF00,     // green
    0xFF00FFFF,     // yellow
    0xFFFF0000,     // blue
    0xFFFF00FF,     // purple
    0xFFFFFF00,     // cyan
    0xFFFFFFFF,     // white
};

//------------------------------------------------------------------------------
bool
z9001_t::check_roms(system model, os_rom os) {
    if ((system::z9001 == model) && (os_rom::z9001_os_1_2 == os)) {
        return roms.has(rom_images::z9001_os12_1) &&
               roms.has(rom_images::z9001_os12_2) &&
               roms.has(rom_images::z9001_basic_507_511) &&
               roms.has(rom_images::z9001_font);
    }
    else if ((system::kc87 == model) && (os_rom::kc87_os_2 == os)) {
        return roms.has(rom_images::kc87_os_2) &&
               roms.has(rom_images::z9001_basic) &&
               roms.has(rom_images::kc87_font_2);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
z9001_t::init_memorymap() {
    mem_unmap_all(&board.mem);
    if (system::z9001 == this->cur_model) {
        // emulate a Z9001 with 16 KByte RAM module and BASIC module
        mem_map_ram(&board.mem, 0, 0x0000, 0x8000, board.ram[0]);
        mem_map_rom(&board.mem, 1, 0xC000, 0x2800, roms.ptr(rom_images::z9001_basic_507_511));
        mem_map_rom(&board.mem, 1, 0xF000, 0x0800, roms.ptr(rom_images::z9001_os12_1));
        mem_map_rom(&board.mem, 1, 0xF800, 0x0800, roms.ptr(rom_images::z9001_os12_2));
    }
    else if (system::kc87 == this->cur_model) {
        // emulate a KC87 with 48 KByte RAM
        mem_map_ram(&board.mem, 0, 0x0000, 0xC000, board.ram[0]);
        mem_map_ram(&board.mem, 0, 0xE800, 0x0400, board.ram[color_ram_page]);
        mem_map_rom(&board.mem, 1, 0xC000, 0x2000, roms.ptr(rom_images::z9001_basic));
        mem_map_rom(&board.mem, 1, 0xE000, 0x2000, roms.ptr(rom_images::kc87_os_2));
    }
    mem_map_ram(&board.mem, 0, 0xEC00, 0x0400, board.ram[video_ram_page]);
}

//------------------------------------------------------------------------------
void
z9001_t::init_keymap() {
    /* shift key is column 0, line 7 */
    kbd_register_modifier(&board.kbd, 0, 0, 7);
    /* register alpha-numeric keys */
    const char* keymap =
        /* unshifted keys */
        "01234567"
        "89:;,=.?"
        "@ABCDEFG"
        "HIJKLMNO"
        "PQRSTUVW"
        "XYZ   ^ "
        "        "
        "        "
        /* shifted keys */
        "_!\"#$%&'"
        "()*+<->/"
        " abcdefg"
        "hijklmno"
        "pqrstuvw"
        "xyz     "
        "        "
        "        ";
    for (int shift = 0; shift < 2; shift++) {
        for (int line = 0; line < 8; line++) {
            for (int col = 0; col < 8; col++) {
                int c = keymap[shift*64 + line*8 + col];
                if (c != 0x20) {
                    kbd_register_key(&board.kbd, c, col, line, shift?(1<<0):0);
                }
            }
        }
    }
    /* special keys */
    kbd_register_key(&board.kbd, 0x03, 6, 6, 0);      /* stop (Esc) */
    kbd_register_key(&board.kbd, 0x08, 0, 6, 0);      /* cursor left */
    kbd_register_key(&board.kbd, 0x09, 1, 6, 0);      /* cursor right */
    kbd_register_key(&board.kbd, 0x0A, 2, 6, 0);      /* cursor up */
    kbd_register_key(&board.kbd, 0x0B, 3, 6, 0);      /* cursor down */
    kbd_register_key(&board.kbd, 0x0D, 5, 6, 0);      /* enter */
    kbd_register_key(&board.kbd, 0x13, 4, 5, 0);      /* pause */
    kbd_register_key(&board.kbd, 0x14, 1, 7, 0);      /* color */
    kbd_register_key(&board.kbd, 0x19, 3, 5, 0);      /* home */
    kbd_register_key(&board.kbd, 0x1A, 5, 5, 0);      /* insert */
    kbd_register_key(&board.kbd, 0x1B, 4, 6, 0);      /* esc (Shift+Esc) */
    kbd_register_key(&board.kbd, 0x1C, 4, 7, 0);      /* list */
    kbd_register_key(&board.kbd, 0x1D, 5, 7, 0);      /* run */
    kbd_register_key(&board.kbd, 0x20, 7, 6, 0);      /* space */
}

//------------------------------------------------------------------------------
void
z9001_t::poweron(system m, os_rom os) {
    YAKC_ASSERT(int(system::any_z9001) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->cur_os = os;
    this->on = true;

    // keyboard initialization
    kbd_init(&board.kbd, 5);
    this->init_keymap();

    // map memory
    for (int i = 0; i < breadboard::num_ram_banks; i++) {
        memcpy(board.ram[i], board.random, breadboard::ram_bank_size);
    }
    this->init_memorymap();

    // initialize hardware components, main clock frequency is 2.4576
    board.freq_hz = 2457600;
    z80_init(&board.z80, cpu_tick);
    z80pio_init(&board.z80pio, pio1_in, pio1_out);
    z80pio_init(&board.z80pio2, pio2_in, pio2_out);
    z80ctc_init(&board.z80ctc);
    beeper_init(&board.beeper, board.freq_hz, SOUND_SAMPLE_RATE, 0.5f);
    this->ctc_zcto2 = 0;
    
    // execution on power-on starts at 0xF000
    board.z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z9001_t::poweroff() {
    YAKC_ASSERT(this->on);
    mem_unmap_all(&board.mem);
    this->on = false;
}

//------------------------------------------------------------------------------
void
z9001_t::reset() {
    z80_reset(&board.z80);
    z80pio_reset(&board.z80pio);
    z80pio_reset(&board.z80pio2);
    z80ctc_reset(&board.z80ctc);
    beeper_reset(&board.beeper);
    this->init_memorymap();

    // execution after reset starts at 0x0000(??? -> doesn't work)
    board.z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
uint64_t
z9001_t::step(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = z80_exec(&board.z80, num_ticks);
    kbd_update(&board.kbd);
    this->decode_video();
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint32_t
z9001_t::step_debug() {
return 0;
/*
    auto& cpu = this->board->z80;
    auto& dbg = this->board->dbg;
    uint64_t all_ticks = 0;
    uint16_t old_pc;
    do {
        old_pc = cpu.PC;
        uint32_t ticks = cpu.handle_irq(this);
        if (0 == ticks) {
            ticks = cpu.step(this);
        }
        this->board->clck.step(this, ticks);
        this->board->z80ctc.step(this, ticks);
        this->board->speaker.step(ticks);
        dbg.step(cpu.PC, ticks);
        all_ticks += ticks;
    }
    while ((old_pc == cpu.PC) && !cpu.INV);    
    this->decode_video();
    return uint32_t(all_ticks);
*/
}

//------------------------------------------------------------------------------
uint64_t
z9001_t::cpu_tick(int num_ticks, uint64_t pins) {
    // FIXME: video memory access wait state!

    /* tick the CTC channels, the CTC channel 2 output signal ZCTO2 is connected
       to CTC channel 3 input signal CLKTRG3 to form a timer cascade
       which drives the system clock, store the state of ZCTO2 for the
       next tick
    */
    pins |= z9001.ctc_zcto2;
    for (int i = 0; i < num_ticks; i++) {
        if (pins & Z80CTC_ZCTO2) { pins |= Z80CTC_CLKTRG3; }
        else                     { pins &= ~Z80CTC_CLKTRG3; }
        pins = z80ctc_tick(&board.z80ctc, pins);
        if (pins & Z80CTC_ZCTO0) {
            // CTC channel 0 controls the beeper frequency
            beeper_toggle(&board.beeper);
        }
        /* the blink flip flop is controlled by a 'bisync' video signal
           (I guess that means it triggers at half PAL frequency: 25Hz),
           going into a binary counter, bit 4 of the counter is connected
           to the blink flip flop.
        */
        if (0 >= z9001.blink_counter--) {
            z9001.blink_counter = (board.freq_hz * 8) / 25;
            z9001.blink_flip_flop = !z9001.blink_flip_flop;
        }
    }
    z9001.ctc_zcto2 = (pins & Z80CTC_ZCTO2);
    if (beeper_tick(&board.beeper, num_ticks)) {
        // new audio sample is ready
        board.audiobuffer.write(board.beeper.sample);
    }

    /* memory and IO requests */
    if (pins & Z80_MREQ) {
        /* a memory request machine cycle */
        const uint16_t addr = Z80_ADDR(pins);
        if (pins & Z80_RD) {
            Z80_SET_DATA(pins, mem_rd(&board.mem, addr));
        }
        else if (pins & Z80_WR) {
            mem_wr(&board.mem, addr, Z80_DATA(pins));
        }
    }
    else if (pins & Z80_IORQ) {
        /* an IO request machine cycle */

        /* check if any of the PIO/CTC chips is enabled */
        /* address line 7 must be on, 6 must be off, IORQ on, M1 off */
        const bool chip_enable = (pins & (Z80_IORQ|Z80_M1|Z80_A7|Z80_A6)) == (Z80_IORQ|Z80_A7);
        const int chip_select = (pins & (Z80_A5|Z80_A4|Z80_A3))>>3;

        pins = pins & Z80_PIN_MASK;
        if (chip_enable) {
            switch (chip_select) {
                /* IO request on CTC? */
                case 0:
                    /* CTC is mapped to ports 0x80 to 0x87 (each port is mapped twice) */
                    pins |= Z80CTC_CE;
                    if (pins & Z80_A0) { pins |= Z80CTC_CS0; };
                    if (pins & Z80_A1) { pins |= Z80CTC_CS1; };
                    pins = z80ctc_iorq(&board.z80ctc, pins) & Z80_PIN_MASK;
                /* IO request on PIO1? */
                case 1:
                    /* PIO1 is mapped to ports 0x88 to 0x8F (each port is mapped twice) */
                    pins |= Z80PIO_CE;
                    if (pins & Z80_A0) { pins |= Z80PIO_BASEL; }
                    if (pins & Z80_A1) { pins |= Z80PIO_CDSEL; }
                    pins = z80pio_iorq(&board.z80pio, pins) & Z80_PIN_MASK;
                    break;
                /* IO request on PIO2? */
                case 2:
                    /* PIO2 is mapped to ports 0x90 to 0x97 (each port is mapped twice) */
                    pins |= Z80PIO_CE;
                    if (pins & Z80_A0) { pins |= Z80PIO_BASEL; }
                    if (pins & Z80_A1) { pins |= Z80PIO_CDSEL; }
                    pins = z80pio_iorq(&board.z80pio2, pins) & Z80_PIN_MASK;
            }
        }
    }

    /* handle interrupt requests by PIOs and CTCs, the interrupt priority
       is PIO1>PIO2>CTC, the interrupt handling functions must be called
       in this order
    */
    Z80_DAISYCHAIN_BEGIN(pins)
    {
        pins = z80pio_int(&board.z80pio, pins);
        pins = z80pio_int(&board.z80pio2, pins);
        pins = z80ctc_int(&board.z80ctc, pins);
    }
    Z80_DAISYCHAIN_END(pins);
    return (pins & Z80_PIN_MASK);
}

//------------------------------------------------------------------------------
uint8_t z9001_t::pio1_in(int port_id) {
    return 0xFF;
}

//------------------------------------------------------------------------------
void z9001_t::pio1_out(int port_id, uint8_t data) {
    if (Z80PIO_PORT_A == port_id) {
        /*
            PIO1-A bits:
            0..1:    unused
            2:       display mode (0: 24 lines, 1: 20 lines)
            3..5:    border color
            6:       graphics LED on keyboard (0: off)
            7:       enable audio output (1: enabled)
        */
        z9001.brd_color = (data>>3) & 7;
    }
    else {
        // PIO1-B is reserved for external user devices
    }
}

//------------------------------------------------------------------------------
uint8_t z9001_t::pio2_in(int port_id) {
    if (Z80PIO_PORT_A == port_id) {
        /* return keyboard matrix column bits for requested line bits */
        uint8_t columns = (uint8_t) kbd_scan_columns(&board.kbd);
        return ~columns;
    }
    else {
        /* return keyboard matrix line bits for requested column bits */
        uint8_t lines = (uint8_t) kbd_scan_lines(&board.kbd);
        return ~lines;
    }
}

//------------------------------------------------------------------------------
void z9001_t::pio2_out(int port_id, uint8_t data) {
    if (Z80PIO_PORT_A == port_id) {
        kbd_set_active_columns(&board.kbd, ~data);
    }
    else {
        kbd_set_active_lines(&board.kbd, ~data);
    }
}

//------------------------------------------------------------------------------
void
z9001_t::put_key(uint8_t ascii) {
    if (ascii) {
        // replace BREAK with STOP
        if (ascii == 0x13) {
            ascii = 0x3;
        }
        kbd_key_down(&board.kbd, ascii);
        kbd_key_up(&board.kbd, ascii);
        /* keyboard matrix lines are directly connected to the PIO2's Port B */
        z80pio_write_port(&board.z80pio2, Z80PIO_PORT_B, ~kbd_scan_lines(&board.kbd));
    }
}

//------------------------------------------------------------------------------
void
z9001_t::decode_video() {

    // FIXME: there's also a 40x20 display mode
    uint32_t* dst = board.rgba8_buffer;
    const uint8_t* vidmem = board.ram[video_ram_page];
    const uint8_t* colmem = board.ram[color_ram_page];
    uint8_t* font;
    if (system::kc87 == this->cur_model) {
        font = roms.ptr(rom_images::kc87_font_2);
    }
    else {
        font = roms.ptr(rom_images::z9001_font);
    }
    int off = 0;
    if (system::kc87 == this->cur_model) {
        uint32_t fg, bg;
        for (int y = 0; y < 24; y++) {
            for (int py = 0; py < 8; py++) {
                for (int x = 0; x < 40; x++) {
                    uint8_t chr = vidmem[off+x];
                    uint8_t pixels = font[(chr<<3)|py];
                    uint8_t color = colmem[off+x];
                    if ((color & 0x80) && this->blink_flip_flop) {
                        // blinking: swap bg and fg
                        fg = palette[color&7];
                        bg = palette[(color>>4)&7];
                    }
                    else {
                        fg = palette[(color>>4)&7];
                        bg = palette[color&7];
                    }
                    for (int px = 7; px >=0; px--) {
                        *dst++ = pixels & (1<<px) ? fg:bg;
                    }
                }
            }
            off += 40;
        }
    }
    else {
        for (int y = 0; y < 24; y++) {
            for (int py = 0; py < 8; py++) {
                for (int x = 0; x < 40; x++) {
                    uint8_t chr = vidmem[off+x];
                    uint8_t pixels = font[(chr<<3)|py];
                    for (int px = 7; px >=0; px--) {
                        *dst++ = pixels & (1<<px) ? 0xFFFFFFFF : 0xFF000000;
                    }
                }
            }
            off += 40;
        }
    }
}

//------------------------------------------------------------------------------
void
z9001_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
const void*
z9001_t::framebuffer(int& out_width, int& out_height) {
    out_width = display_width;
    out_height = display_height;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
z9001_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {

    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }

    // same fileformats as KC85/2..4
    kctap_header hdr;
    uint16_t exec_addr = 0;
    bool has_exec_addr = false;
    bool hdr_valid = false;
    if (filetype::kc_tap == type) {
        if (fs->read(fp, &hdr, sizeof(hdr)) == sizeof(hdr)) {
            hdr_valid = true;
            uint16_t addr = (hdr.kcc.load_addr_h<<8 | hdr.kcc.load_addr_l) & 0xFFFF;
            uint16_t end_addr = (hdr.kcc.end_addr_h<<8 | hdr.kcc.end_addr_l) & 0xFFFF;
            exec_addr = (hdr.kcc.exec_addr_h<<8 | hdr.kcc.exec_addr_l) & 0xFFFF;
            has_exec_addr = hdr.kcc.num_addr > 2;
            while (addr < end_addr) {
                // each block is 1 lead byte + 128 bytes data
                static const int block_size = 129;
                uint8_t block[block_size];
                fs->read(fp, block, block_size);
                for (int i = 1; (i < block_size) && (addr < end_addr); i++) {
                    mem_wr(&board.mem, addr++, block[i]);
                }
            }
        }
    }
    else if (filetype::kcc == type) {
        if (fs->read(fp, &hdr.kcc, sizeof(hdr.kcc)) == sizeof(hdr.kcc)) {
            hdr_valid = true;
            uint16_t addr = (hdr.kcc.load_addr_h<<8 | hdr.kcc.load_addr_l) & 0xFFFF;
            uint16_t end_addr = (hdr.kcc.end_addr_h<<8 | hdr.kcc.end_addr_l) & 0xFFFF;
            exec_addr = (hdr.kcc.exec_addr_h<<8 | hdr.kcc.exec_addr_l) & 0xFFFF;
            has_exec_addr = hdr.kcc.num_addr > 2;
            while (addr < end_addr) {
                static const int buf_size = 1024;
                uint8_t buf[buf_size];
                fs->read(fp, buf, buf_size);
                for (int i = 0; (i < buf_size) && (addr < end_addr); i++) {
                    mem_wr(&board.mem, addr++, buf[i]);
                }
            }
        }
    }
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }

    // start loaded image
    if (start && has_exec_addr) {
        auto& cpu = board.z80;
        cpu.A = 0x00;
        cpu.F = 0x10;
        cpu.BC = cpu.BC_ = 0x0000;
        cpu.DE = cpu.DE_ = 0x0000;
        cpu.HL = cpu.HL_ = 0x0000;
        cpu.AF_ = 0x0000;
        cpu.PC = exec_addr;
    }
    return true;
}

//------------------------------------------------------------------------------
const char*
z9001_t::system_info() const {
    if (system::z9001 == this->cur_model) {
        return
            "The Z9001 (later retconned to KC85/1) was independently developed "
            "to the HC900 (aka KC85/2) by Robotron Dresden. It had a pretty "
            "slick design with an integrated keyboard which was legendary for "
            "how hard it was to type on.\n\nThe standard model had 16 KByte RAM, "
            "a monochrome 40x24 character display, and a 2.5 MHz U880 CPU "
            "(making it the fastest East German 8-bitter). The Z9001 could "
            "be extended by up to 4 expansion modules, usually one or two "
            "16 KByte RAM modules, and a 10 KByte ROM BASIC module (the "
            "version emulated here comes with 32 KByte RAM and a BASIC module) "
            "\n\n\n"
            "Manufacturer:      VEB Robotron-Messelektronik \"Otto Schön\" Dresden\n"
            "Release Date:      1984\n"
            "OS:                OS 1.1 (Z9001), OS 1.2 (KC85/1)\n"
            "Chips:             U880 @ 2.5 MHz (unlicensed Z80 clone)\n"
            "                   2x U855 (Z80 PIO clone)\n"
            "                   U857 (Z80 CTC clone)\n"
            "Memory:            16 KB RAM\n"
            "                    1 KB video memory\n"
            "                    4 KB ROM\n"
            "Graphics:          40x24 or 40x20 chars, monochrome\n"
            "Audio:             CTC-controlled beeper, mono\n"
            "Special Power:     2 PIO chips (one exclusively for keyboard input)";

    }
    else {
        return
            "The KC87 was the successor to the KC85/1. The only real difference "
            "was the built-in BASIC interpreter in ROM. The KC87 emulated here "
            "has 48 KByte RAM and the video color extension which could "
            "assign 8 foreground and 8 (identical) background colors per character, "
            "plus a blinking flag. This video extension was already available on the "
            "Z9001 though."
            "\n\n\n"
            "Manufacturer:      VEB Robotron-Messelektronik \"Otto Schön\" Dresden\n"
            "Release Date:      1987\n"
            "OS:                OS 1.3\n"
            "Chips:             U880 @ 2.5 MHz (unlicensed Z80 clone)\n"
            "                   2x U855 (Z80 PIO clone)\n"
            "                   U857 (Z80 CTC clone)\n"
            "Memory:            16 KB RAM\n"
            "                    2 KB video memory (1+1 KB for chars+color)\n"
            "                   10 KB BASIC ROM\n"
            "                    4 KB OS ROM\n"
            "Graphics:          40x24 or 40x20 chars\n"
            "                   1-of-8 foreground and background colors\n"
            "Audio:             CTC-controlled beeper, mono\n"
            "Special Power:     fastest 8-bitter this side of the Iron Curtain";
    }
}

} // namespace YAKC
