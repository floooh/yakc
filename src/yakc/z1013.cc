//------------------------------------------------------------------------------
//  z1013.cc
//------------------------------------------------------------------------------
#include "z1013.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z1013::init(breadboard* b) {
    this->board = b;
    this->init_key_map();
}

//------------------------------------------------------------------------------
void
z1013::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_z1013) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->cur_os = os_rom::z1013_mon202;
    this->on = true;
    this->kbd_column_nr_requested = 0;
    this->next_kbd_column_bits = 0;
    this->kbd_column_bits = 0;

    // map memory
    this->board->cpu.mem.map(0, 0x0000, 0x4000, this->ram[0], true);            // RAM
    this->board->cpu.mem.map(0, 0xEC00, 0x0400, this->video.irm, true);         // video mem
    this->board->cpu.mem.map(0, 0xF000, 0x0800, (ubyte*)this->roms.ptr(z1013_roms::mon202), false);   // OS ROM

    // initialize the clock, the z1013_01 runs at 1MHz, all others at 2MHz
    this->board->clck.init((m == device::z1013_01) ? 1000 : 2000);

    // initialize hardware components
    this->board->pio.init();
    this->board->cpu.init(in_cb, out_cb, this);
    this->video.init();

    // clear RAM
    clear(this->ram, sizeof(this->ram));

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
    this->video.reset();
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
        // compute the end-cycle-count for the current frame
        // compute the end-cycle-count for the current frame
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
    this->video.decode();
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
            break;
        case 0x02:
            // FIXME: directly lookup keyboard matrix column bits,
            // normally these would have to be read from PIO B
            return 0xF & ~self->get_kbd_column_bits(self->kbd_column_nr_requested & 7);
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
ubyte
z1013::get_kbd_column_bits(int col) const {
    return (this->kbd_column_bits >> (col*4)) & 0xF;
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
uint32_t
z1013::kbd_bit(int col, int line) {
    return (1<<line)<<(col*4);
}

//------------------------------------------------------------------------------
void
z1013::init_key(ubyte ascii, int col, int line, int shift) {
    YAKC_ASSERT((ascii < 128) && (col>=0) && (col<8) && (line>=0) && (line<4) && (shift>=0) && (shift<5));
    uint32_t mask = kbd_bit(col, line);
    if (shift != 0) {
        mask |= kbd_bit(shift-1, 3);
    }
    this->key_map[ascii] = mask;
}

//------------------------------------------------------------------------------
void
z1013::init_key_map() {
    memset(this->key_map, 0, sizeof(this->key_map));

    // keyboard layers (no shift key, plus the 4 shift keys)
    // use space as special placeholder
    const char* layers =
        "@ABCDEFG"
        "HIJKLMNO"
        "PQRSTUVW"
        "        "
        // Shift 1:
        "XYZ[\\]^-"
        "01234567"
        "89:.<=>?"
        "        "
        // Shift 2:
        "   {|}~ "
        " !\"#$%&'"
        "()*+,-,/"
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
                ubyte c = layers[shift*32 + line*8 + col];
                if (c != 0x20) {
                    this->init_key(c, col, line, shift);
                }
            }
        }
    }

    // special keys
    this->init_key(' ', 5, 3, 0);   // Space
    this->init_key(0x08, 4, 3, 0);  // Cursor Left
    this->init_key(0x09, 6, 3, 0);  // Cursor Right
    this->init_key(0x0D, 7, 3, 0);  // Enter
}

} // namespace YAKC
