//------------------------------------------------------------------------------
//  z1013.cc
//------------------------------------------------------------------------------
#include "z1013.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z1013::init(breadboard* b, rom_images* r) {
    this->board = b;
    this->roms = r;
    this->rgba8_buffer = this->board->rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
z1013::check_roms(const rom_images& roms, system model, os_rom os) {
    if (system::z1013_01 == model) {
        return roms.has(rom_images::z1013_mon202) && roms.has(rom_images::z1013_font);
    }
    else {
        return roms.has(rom_images::z1013_mon_a2) && roms.has(rom_images::z1013_font);
    }
}

//------------------------------------------------------------------------------
void
z1013::init_memory_mapping() {
    z80& cpu = this->board->z80;
    cpu.mem.unmap_all();
    if (system::z1013_64 == this->cur_model) {
        // 64 kByte RAM
        cpu.mem.map(1, 0x0000, 0x10000, this->board->ram[0], true);
    }
    else {
        // 16 kByte RAM
        cpu.mem.map(1, 0x0000, 0x4000, this->board->ram[0], true);
    }
    // 1 kByte video memory
    cpu.mem.map(0, 0xEC00, 0x0400, this->board->ram[vidmem_page], true);
    // 2 kByte system rom
    if (os_rom::z1013_mon202 == this->cur_os) {
        cpu.mem.map(0, 0xF000, this->roms->size(rom_images::z1013_mon202), this->roms->ptr(rom_images::z1013_mon202), false);
    }
    else {
        cpu.mem.map(0, 0xF000, this->roms->size(rom_images::z1013_mon_a2), this->roms->ptr(rom_images::z1013_mon_a2), false);
    }
}

//------------------------------------------------------------------------------
void
z1013::init_keymaps() {
    if (this->cur_model == system::z1013_01) {
        this->init_keymap_8x4();
    }
    else {
        this->init_keymap_8x8();
    }
}

//------------------------------------------------------------------------------
void
z1013::on_context_switched() {
    this->init_keymaps();
    this->init_memory_mapping();
}

//------------------------------------------------------------------------------
void
z1013::poweron(system m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(system::any_z1013) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    if (m == system::z1013_01) {
        this->cur_os = os_rom::z1013_mon202;
    }
    else {
        this->cur_os = os_rom::z1013_mon_a2;
    }
    this->init_keymaps();
    this->on = true;
    this->kbd_column_nr_requested = 0;
    this->kbd_8x8_requested = false;
    this->next_kbd_column_bits = 0;
    this->kbd_column_bits = 0;

    // map memory
    clear(this->board->ram, sizeof(this->board->ram));
    this->init_memory_mapping();

    // initialize the clock, the z1013_01 runs at 1MHz, all others at 2MHz
    this->board->clck.init((m == system::z1013_01) ? 1000 : 2000);

    // initialize hardware components
    this->board->z80.init();
    this->board->z80pio.init(0);

    // execution on power-on starts at 0xF000
    this->board->z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z1013::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->z80.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
void
z1013::reset() {
    this->board->z80pio.reset();
    this->board->z80.reset();
    this->kbd_column_nr_requested = 0;
    this->next_kbd_column_bits = 0;
    this->kbd_column_bits = 0;

    // execution after reset starts at 0x0000(??? -> doesn't work)
    this->board->z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
uint64_t
z1013::step(uint64_t start_tick, uint64_t end_tick) {
    auto& cpu = this->board->z80;
    auto& dbg = this->board->dbg;
    auto& clk = this->board->clck;
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        uint32_t ticks = cpu.step(this);
        ticks += cpu.handle_irq(this);
        clk.step(this, ticks);
        if (dbg.step(cpu.PC, ticks)) {
            return end_tick;
        }
        cur_tick += ticks;
    }
    this->decode_video();
    return cur_tick;
}

//------------------------------------------------------------------------------
uint32_t
z1013::step_debug() {
    auto& cpu = this->board->z80;
    auto& dbg = this->board->dbg;
    auto& clk = this->board->clck;
    uint64_t all_ticks = 0;
    uint16_t old_pc;
    do {
        old_pc = cpu.PC;
        uint32_t ticks = cpu.step(this);
        ticks += cpu.handle_irq(this);
        clk.step(this, ticks);
        dbg.step(cpu.PC, ticks);
        all_ticks += ticks;
    }
    while ((old_pc == cpu.PC) && !cpu.INV);    
    this->decode_video();
    return all_ticks;
}

//------------------------------------------------------------------------------
void
z1013::cpu_out(uword port, ubyte val) {
    switch (port & 0xFF) {
        case 0x00:
            // PIO A, data
            this->board->z80pio.write_data(this, z80pio::A, val);
            break;
        case 0x01:
            // PIO A, control
            this->board->z80pio.write_control(z80pio::A, val);
            break;
        case 0x02:
            // PIO B, data
            this->board->z80pio.write_data(this, z80pio::B, val);
            break;
        case 0x03:
            // PIO B, control
            this->board->z80pio.write_control(z80pio::B, val);
            break;
        case 0x08:
            // keyboard column
            if (val == 0) {
                this->kbd_column_bits = this->next_kbd_column_bits;
            }
            this->kbd_column_nr_requested = val;
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
ubyte
z1013::cpu_in(uword port) {
    switch (port & 0xFF) {
        case 0x00:
            return this->board->z80pio.read_data(this, z80pio::A);
        case 0x01:
            return this->board->z80pio.read_control();
        case 0x02:
            return this->board->z80pio.read_data(this, z80pio::B);
        case 0x03:
            return this->board->z80pio.read_control();
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
z1013::pio_out(int pio_id, int port_id, ubyte val) {
    if (z80pio::B == port_id) {
        // for z1013a2, bit 4 is for monitor A.2 with 8x8 keyboard
        this->kbd_8x8_requested = 0 != (val & (1<<4));
        // FIXME: bit 7 is for cassette output
    }
}

//------------------------------------------------------------------------------
ubyte
z1013::pio_in(int pio_id, int port_id) {
    if (z80pio::A == port_id) {
        // nothing to return here, PIO-A is for user devices
        return 0xFF;
    }
    else {
        // FIXME: handle bit 7 for cassette input
        // read keyboard matrix state into lower 4 bits
        ubyte val = 0;
        if (system::z1013_01 == this->cur_model) {
            ubyte col = this->kbd_column_nr_requested & 7;
            val = 0xF & ~((this->kbd_column_bits >> (col*4)) & 0xF);
        }
        else {
            ubyte col = (this->kbd_column_nr_requested & 7);
            val = ubyte(this->kbd_column_bits >> (col*8));
            if (this->kbd_8x8_requested) {
                val >>= 4;
            }
            val = 0xF & ~(val & 0xF);
        }
        return val;
    }
}

//------------------------------------------------------------------------------
void
z1013::irq(bool b) {
    // forward interrupt request to CPU
    this->board->z80.irq(b);
}

//------------------------------------------------------------------------------
void
z1013::put_key(ubyte ascii) {
    if (ascii) {
        this->next_kbd_column_bits = this->key_map[ascii & (max_num_keys-1)];
    }
    else {
        this->next_kbd_column_bits = 0;
    }
}

//------------------------------------------------------------------------------
uint64_t
z1013::kbd_bit(int col, int line, int num_lines) {
    return (uint64_t(1)<<line)<<(col*num_lines);
}

//------------------------------------------------------------------------------
void
z1013::init_key(ubyte ascii, int col, int line, int shift, int num_lines) {
    YAKC_ASSERT((ascii < 128) && (col>=0) && (col<8) && (line>=0) && (line<num_lines) && (shift>=0) && (shift<5));
    uint64_t mask = kbd_bit(col, line, num_lines);
    if (shift != 0) {
        if (system::z1013_01 == this->cur_model) {
            mask |= kbd_bit(shift-1, 3, num_lines);
        }
        else {
            mask |= kbd_bit(7, 6, num_lines);
        }
    }
    this->key_map[ascii] = mask;
}

//------------------------------------------------------------------------------
void
z1013::init_keymap_8x4() {
    memset(this->key_map, 0, sizeof(this->key_map));

    // keyboard layers for the 4x4 keyboard (no shift key, plus the 4 shift keys)
    // use space as special placeholder
    const char* layers_8x4 =
        "@ABCDEFG"
        "HIJKLMNO"
        "PQRSTUVW"
        "        "
        // Shift 1:
        "XYZ[\\]^-"
        "01234567"
        "89:;<=>?"
        "        "
        // Shift 2:
        "   {|}~ "
        " !\"#$%&'"
        "()*+,-./"
        "        "
        // Shift 3:
        " abcdefg"
        "hijklmno"
        "pqrstuvw"
        "        "
        // Shift 4:
        "xyz     "
        "        "
        "        "
        "        ";

    for (int shift = 0; shift < 5; shift++) {
        for (int line = 0; line < 4; line++) {
            for (int col = 0; col < 8; col++) {
                ubyte c = layers_8x4[shift*32 + line*8 + col];
                if (c != 0x20) {
                    this->init_key(c, col, line, shift, 4);
                }
            }
        }
    }

    // special keys
    this->init_key(' ', 5, 3, 0, 4);   // Space
    this->init_key(0x08, 4, 3, 0, 4);  // Cursor Left
    this->init_key(0x09, 6, 3, 0, 4);  // Cursor Right
    this->init_key(0x0D, 7, 3, 0, 4);  // Enter
    this->init_key(0x03, 3, 1, 4, 4);  // Break/Escape
}

//------------------------------------------------------------------------------
void
z1013::init_keymap_8x8() {
    // see: http://www.z1013.de/images/21.gif
    memset(this->key_map, 0, sizeof(this->key_map));

    // keyboard layers for the 8x8 keyboard
    const char* layers_8x8 =
        "13579-  "
        "QETUO@  "
        "ADGJL*  "
        "YCBM.^  "
        "24680[  "
        "WRZIP]  "
        "SFHK+\\  "
        "XVN,/_  "

        // shift layer
        "!#%')=  "
        "qetuo`  "
        "adgjl:  "
        "ycbm>~  "
        "\"$&( {  "
        "wrzip}  "
        "sfhk;|  "
        "xvn<?   ";

    for (int shift = 0; shift < 2; shift++) {
        for (int line = 0; line < 8; line++) {
            for (int col = 0; col < 8; col++) {
                ubyte c = layers_8x8[shift*64 + line*8 + col];
                if (c != 0x20) {
                    this->init_key(c, col, line, shift, 8);
                }
            }
        }
    }

    // special keys
    this->init_key(' ', 6, 4, 0, 8);    // Space
    this->init_key(0x08, 6, 2, 0, 8);   // Cursor Left
    this->init_key(0x09, 6, 3, 0, 8);   // Cursor Right
    this->init_key(0x0A, 6, 7, 0, 8);   // Cursor Down
    this->init_key(0x0B, 6, 6, 0, 8);   // Cursor Up
    this->init_key(0x0D, 6, 1, 0, 8);   // Enter
    this->key_map[0x03] = kbd_bit(6, 5, 8) | kbd_bit(1, 3, 8); // Ctrl+C (== STOP/BREAK)
}

//------------------------------------------------------------------------------
void
z1013::decode_video() {
    uint32_t* dst = rgba8_buffer;
    const ubyte* src = this->board->ram[vidmem_page];
    const ubyte* font = this->roms->ptr(rom_images::z1013_font);
    for (int y = 0; y < 32; y++) {
        for (int py = 0; py < 8; py++) {
            for (int x = 0; x < 32; x++) {
                ubyte chr = src[(y<<5) + x];
                ubyte bits = font[(chr<<3)|py];
                for (int px = 7; px >=0; px--) {
                    *dst++ = bits & (1<<px) ? 0xFFFFFFFF : 0xFF000000;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
const void*
z1013::framebuffer(int& out_width, int& out_height) {
    out_width = display_width;
    out_height = display_height;
    return this->rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
z1013::quickload(filesystem* fs, const char* name, filetype type, bool start) {

    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }

    kcz80_header hdr;
    uint16_t exec_addr = 0;
    bool hdr_valid = false;
    if (fs->read(fp, &hdr, sizeof(hdr)) == sizeof(hdr)) {
        hdr_valid = true;
        uint16_t addr = (hdr.load_addr_h<<8 | hdr.load_addr_l) & 0xFFFF;
        uint16_t end_addr = (hdr.end_addr_h<<8 | hdr.end_addr_l) & 0xFFFF;
        exec_addr = (hdr.exec_addr_h<<8 | hdr.exec_addr_l) & 0xFFFF;
        auto& mem = this->board->z80.mem;
        while (addr < end_addr) {
            static const int buf_size = 1024;
            uint8_t buf[buf_size];
            fs->read(fp, buf, buf_size);
            for (int i = 0; (i < buf_size) && (addr < end_addr); i++) {
                mem.w8(addr++, buf[i]);
            }
        }
    }
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }

    if (start) {
        auto& cpu = this->board->z80;
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
z1013::system_info() const {
    if (this->cur_model == system::z1013_01) {
        return
            "The Z1013 was the only East German home computer that was "
            "available to the general public and cheap enough to be "
            "affordable by hobbyists.\n\n"
            "It was delivered as kit (just the main board and separate membrane keyboard) "
            "and had to be assembled at home, with some soldering required to attach the "
            "keyboard."
            "\n\n\n"
            "Manufacturer:      VEB Robotron-Elektronik Riesa\n"
            "Release Date:      1985\n"
            "OS:                Monitor 2.02 in ROM\n"
            "Chips:             U880 CPU @ 1 MHz (unlicensed Z80 clone)\n"
            "                   U855 PIO (Z80 PIO clone)\n"
            "Memory:            16 KByte RAM\n"
            "                    1 KByte video memory\n"
            "                    2 KByte ROM\n"
            "Display:           32x32 ASCII, monochrome\n"
            "Special Power:     the keyboard has 4 shift keys";
    }
    else if (this->cur_model == system::z1013_16) {
        return
            "The Z1013.16 was an improved version of the original Z1013 "
            "hobbyist kit. It had a CPU with higher quality-rating allowing "
            "it to run at twice the speed of the original.\n\n"
            "While the Z1013.16 was still delivered with the same terrible "
            "8x4 membrane keyboard, the system was prepared for adding a "
            "proper mechanical keyboard with an 8x8 key matrix."
            "\n\n\n"
            "Manufacturer:      VEB Robotron-Elektronik Riesa\n"
            "Release Date:      1987\n"
            "OS:                Monitor A.2 in ROM\n"
            "Chips:             U880 @ 2 MHz (unlicensed Z80 clone)\n"
            "                   U855 (Z80 PIO clone)\n"
            "Memory:            16 KB RAM\n"
            "                    1 KB video memory\n"
            "                    2 KB ROM\n"
            "Display:           32x32 ASCII, monochrome\n"
            "Special Power:     support for proper keyboards";
    }
    else {
        return
            "The Z1013.64 is identical to the improved Z1013.16, but "
            "came equipped with 64 KByte RAM. Not all of this was usable "
            "though because it was overlapped by the 1KB video memory and 2KB ROM."
            "\n\n\n"
            "Manufacturer:      VEB Robotron-Elektronik Riesa\n"
            "Release Date:      1987\n"
            "OS:                Monitor A.2 in ROM\n"
            "Chips:             U880 @ 2 MHz (unlicensed Z80 clone)\n"
            "                   U855 (Z80 PIO clone)\n"
            "Memory:            64 KB RAM\n"
            "                    1 KB video memory\n"
            "                    2 KB ROM\n"
            "Display:           32x32 ASCII, monochrome\n"
            "Special Power:     61 KB usable RAM without bank switching";
    }
}

} // namespace YAKC
