//------------------------------------------------------------------------------
//  z1013.cc
//------------------------------------------------------------------------------
#include "z1013.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z1013::init(breadboard* b) {
    this->board = b;
}

//------------------------------------------------------------------------------
void
z1013::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_z1013) & int(m));
    YAKC_ASSERT(!this->on);

    z80& cpu = this->board->cpu;
    z80pio& pio = this->board->pio;

    this->cur_model = m;
    if (m == device::z1013_01) {
        this->cur_os = os_rom::z1013_mon202;
        this->init_keymap_8x4();
    }
    else {
        this->cur_os = os_rom::z1013_mon_a2;
        this->init_keymap_8x8();
    }
    this->on = true;
    this->kbd_column_nr_requested = 0;
    this->kbd_8x8_requested = false;
    this->next_kbd_column_bits = 0;
    this->kbd_column_bits = 0;

    // map memory
    clear(this->ram, sizeof(this->ram));
    clear(this->irm, sizeof(this->irm));    
    cpu.mem.unmap_all();
    if (m == device::z1013_64) {
        // 64 kByte RAM
        cpu.mem.map(1, 0x0000, 0x10000, this->ram, true);
    }
    else {
        // 16 kByte RAM
        cpu.mem.map(1, 0x0000, 0x4000, this->ram, true);
    }
    // 1 kByte video memory
    cpu.mem.map(0, 0xEC00, 0x0400, this->irm, true);
    // 2 kByte system rom
    if (os_rom::z1013_mon202 == this->cur_os) {
        cpu.mem.map(0, 0xF000, sizeof(dump_z1013_mon202), dump_z1013_mon202, false);
    }
    else {
        cpu.mem.map(0, 0xF000, sizeof(dump_z1013_mon_a2), dump_z1013_mon_a2, false);
    }
    
    // initialize the clock, the z1013_01 runs at 1MHz, all others at 2MHz
    this->board->clck.init((m == device::z1013_01) ? 1000 : 2000);

    // initialize hardware components
    cpu.init(in_cb, out_cb, this);
    pio.init();
    pio.connect_out_cb(z80pio::A, this, pio_a_out_cb);
    pio.connect_out_cb(z80pio::B, this, pio_b_out_cb);
    pio.connect_in_cb(z80pio::A, this, pio_a_in_cb);
    pio.connect_in_cb(z80pio::B, this, pio_b_in_cb);

    // execution on power-on starts at 0xF000
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z1013::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
void
z1013::reset() {
    this->board->pio.reset();
    this->board->cpu.reset();
    this->kbd_column_nr_requested = 0;
    this->next_kbd_column_bits = 0;
    this->kbd_column_bits = 0;
    this->overflow_cycles = 0;

    // execution after reset starts at 0x0000(??? -> doesn't work)
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z1013::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    // FIXME: the speed multiplier isn't currently working because of the min/max cycle count limiter!
    YAKC_ASSERT(speed_multiplier > 0);
    this->cpu_ahead = false;
    this->cpu_behind = false;    
    z80& cpu = this->board->cpu;
    z80dbg& dbg = this->board->dbg;
    clock& clk = this->board->clck;

    if (!dbg.paused) {
        if (this->abs_cycle_count == 0) {
            this->abs_cycle_count = min_cycle_count;
        }
        const int64_t num_cycles = clk.cycles(micro_secs*speed_multiplier) - this->overflow_cycles;
        uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
        if (abs_end_cycles > max_cycle_count) {
            abs_end_cycles = max_cycle_count;
            this->cpu_ahead = true;
        }
        else if (abs_end_cycles < min_cycle_count) {
            abs_end_cycles = min_cycle_count;
            this->cpu_behind = true;
        }
        while (this->abs_cycle_count < abs_end_cycles) {
            if (dbg.check_break(cpu)) {
                dbg.paused = true;
                this->overflow_cycles = 0;
                break;
            }
            dbg.store_pc_history(cpu); // FIXME: only if debug window open?
            int cycles_step = cpu.step();
            cycles_step += cpu.handle_irq();
            clk.update(cycles_step);
            this->abs_cycle_count += cycles_step;
        }
        this->overflow_cycles = uint32_t(this->abs_cycle_count - abs_end_cycles);
    }
    this->decode_video();
}

//------------------------------------------------------------------------------
void
z1013::out_cb(void* userdata, uword port, ubyte val) {
    z1013* self = (z1013*)userdata;
    switch (port & 0xFF) {
        case 0x00:
            // PIO A, data
            self->board->pio.write_data(z80pio::A, val);
            break;
        case 0x01:
            // PIO A, control
            self->board->pio.control(z80pio::A, val);
            break;
        case 0x02:
            // PIO B, data
            self->board->pio.write_data(z80pio::B, val);
            break;
        case 0x03:
            // PIO B, control
            self->board->pio.control(z80pio::B, val);
            break;
        case 0x08:
            // keyboard column
            if (val == 0) {
                self->kbd_column_bits = self->next_kbd_column_bits;
            }
            self->kbd_column_nr_requested = val;
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
ubyte
z1013::in_cb(void* userdata, uword port) {
    z1013* self = (z1013*)userdata;
    switch (port & 0xFF) {
        case 0x00:
            return self->board->pio.read_data(z80pio::A);
        case 0x02:
            return self->board->pio.read_data(z80pio::B);
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
z1013::pio_a_out_cb(void* userdata, ubyte val) {
    // nothing happening here, PIO-A is for user devices
}

//------------------------------------------------------------------------------
ubyte
z1013::pio_a_in_cb(void* userdata) {
    // nothing to return here, PIO-A is for user devices
    return 0xFF;
}

//------------------------------------------------------------------------------
void
z1013::pio_b_out_cb(void* userdata, ubyte val) {

    // for z1013a2, bit 4 is for monitor A.2 with 8x8 keyboard
    z1013* self = (z1013*)userdata;
    self->kbd_8x8_requested = val & (1<<4);

    // FIXME: bit 7 is for cassette output
}

//------------------------------------------------------------------------------
ubyte
z1013::pio_b_in_cb(void* userdata) {
    z1013* self = (z1013*) userdata;

    // FIXME: handle bit 7 for cassette input
    // read keyboard matrix state into lower 4 bits
    ubyte val = 0;
    if (device::z1013_01 == self->cur_model) {
        ubyte col = self->kbd_column_nr_requested & 7;
        val = 0xF & ~((self->kbd_column_bits >> (col*4)) & 0xF);
    }
    else {
        ubyte col = (self->kbd_column_nr_requested & 7);
        val = self->kbd_column_bits >> (col*8);
        if (self->kbd_8x8_requested) {
            val >>= 4;
        }
        val = 0xF & ~(val & 0xF);
    }
    return val;
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
    return uint64_t(1<<line)<<(col*num_lines);
}

//------------------------------------------------------------------------------
void
z1013::init_key(ubyte ascii, int col, int line, int shift, int num_lines) {
    YAKC_ASSERT((ascii < 128) && (col>=0) && (col<8) && (line>=0) && (line<num_lines) && (shift>=0) && (shift<5));
    uint64_t mask = kbd_bit(col, line, num_lines);
    if (shift != 0) {
        if (device::z1013_01 == this->cur_model) {
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
    uint32_t* dst = RGBA8Buffer;
    for (int y = 0; y < 32; y++) {
        for (int py = 0; py < 8; py++) {
            for (int x = 0; x < 32; x++) {
                ubyte chr = this->irm[(y<<5) + x];
                ubyte bits = dump_z1013_font[(chr<<3)|py];
                for (int px = 7; px >=0; px--) {
                    *dst++ = bits & (1<<px) ? 0xFFFFFFFF : 0xFF000000;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
const char*
z1013::system_info() const {
    if (this->cur_model == device::z1013_01) {
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
    else if (this->cur_model == device::z1013_16) {
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
            "Chipd:             U880 @ 2 MHz (unlicensed Z80 clone)\n"
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
            "CPU:               U880 @ 2 MHz (unlicensed Z80 clone)\n"
            "                   U855 (Z80 PIO clone)\n"
            "Memory:            64 KB RAM\n"
            "                    1 KB video memory\n"
            "                    2 KB ROM\n"
            "Display:           32x32 ASCII, monochrome\n"
            "Special Power:     61 KB usable RAM without bank switching";
    }
}

} // namespace YAKC
