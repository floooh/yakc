//------------------------------------------------------------------------------
//  atom.cc
//------------------------------------------------------------------------------
#include "atom.h"

// #include <stdio.h>

namespace YAKC {

atom* atom::self = nullptr;

//------------------------------------------------------------------------------
void
atom::init(breadboard* b, rom_images* r) {
    YAKC_ASSERT(b && r);
    self = this;
    board = b;
    roms = r;
    vdg = &(board->mc6847);
    init_keymap();
}

//------------------------------------------------------------------------------
void
atom::init_key_mask(uint8_t ascii, int col, int row, bool shift, bool ctrl) {
    YAKC_ASSERT((col >= 0) && (col < 10));
    YAKC_ASSERT((row >= 0) && (row < 6));
    key_map[ascii] = key_mask();
    key_map[ascii].col[col] = (1<<row);
    uint8_t mod = 0x00;
    if (ctrl) {
        // ctrl is complete row 6
        mod |= (1<<6);
    }
    if (shift) {
        // shift is complete row 7
        mod |= (1<<7);
    }
    if (mod) {
        for (int i = 0; i < 10; i++) {
            key_map[ascii].col[i] |= mod;
        }
    }
}

//------------------------------------------------------------------------------
void
atom::init_keymap() {
    clear(key_map, sizeof(key_map));

    // 10x5 keyboard matrix (10 columns, 6 rows,
    // row 6 is Ctrl, row 7 is Shift,
    // the shift key simply sets bit 7 which
    // inverts the visual character
    const char* kbd =
        // no shift
        "     ^]\\[ "   // row 0
        "3210      "    // row 1
        "-,;:987654"    // row 2
        "GFEDCBA@/."    // row 3
        "QPONMLKJIH"    // row 4
        " ZYXWVUTSR"    // row 5

        // shift
        "          "   // row 0
        "#\"!       "   // row 1
        "=<+*)('&%$"   // row 2
        "gfedcba ?>"   // row 3
        "qponmlkjih"   // row 4
        " zyxwvutsr";  // row 5
    YAKC_ASSERT(strlen(kbd) == 120);
    for (int shift = 0; shift < 2; shift++) {
        for (int col = 0; col < 10; col++) {
            for (int row = 0; row < 6; row++) {
                uint8_t ascii = kbd[shift*60 + row*10 + col];
                init_key_mask(ascii, col, row, shift != 0);
            }
        }
    }

    // special keys
    //init_key_mask(0x00, 4, 0);     // FIXME capslock
    //init_key_mask(0x00, 5, 0);     // FIXME tab
    init_key_mask(0x20, 9, 0);       // space
    init_key_mask(0x01, 4, 1);       // backspace
    init_key_mask(0x07, 0, 3, false, true);     // Ctrl+G: bleep
    init_key_mask(0x08, 3, 0, true);            // key left
    init_key_mask(0x09, 3, 0);                  // key right
    init_key_mask(0x0A, 2, 0, true);            // key down
    init_key_mask(0x0B, 2, 0);                  // key up
    init_key_mask(0x0D, 6, 1);                  // return/enter
    init_key_mask(0x0C, 5, 4, false, true);     // Ctrl+L clear screen
    init_key_mask(0x0E, 3, 4, false, true);     // Ctrl+N page mode on
    init_key_mask(0x0F, 2, 4, false, true);     // Ctrl+O page mode off
    init_key_mask(0x15, 6, 5, false, true);     // Ctrl+U end screen
    init_key_mask(0x18, 3, 5, false, true);     // Ctrl+X cancel
    init_key_mask(0x1B, 0, 5);       // escape
}

//------------------------------------------------------------------------------
bool
atom::check_roms(const rom_images& roms, system model, os_rom os) {
    if (system::acorn_atom == model) {
        return roms.has(rom_images::atom_basic) &&
               roms.has(rom_images::atom_float) &&
               roms.has(rom_images::atom_dos);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
atom::on_context_switched() {
    // FIXME
}

//------------------------------------------------------------------------------
void
atom::poweron() {
    YAKC_ASSERT(board && roms);
    YAKC_ASSERT(!on);
    
    on = true;
    scan_kbd_col = 0;
    next_key_mask = key_mask();
    cur_key_mask = key_mask();

    // map memory
    memcpy(board->ram[0], board->random, breadboard::ram_bank_size);
    memcpy(board->ram[1], board->random, breadboard::ram_bank_size);
    memcpy(board->ram[2], board->random, breadboard::ram_bank_size);
    clear(board->ram[3], sizeof(board->ram[3]));
    auto& mem = board->mos6502.mem;
    mem.unmap_all();
    mem.map(0, 0x0000, 0x0400, board->ram[0], true);
    mem.map(0, 0x2000, 0x8000, board->ram[1], true);
    mem.map_io(0, 0xB000, 0x1000, memio);
    mem.map(0, 0xC000, 0x1000, roms->ptr(rom_images::atom_basic), false);
    mem.map(0, 0xD000, 0x1000, roms->ptr(rom_images::atom_float), false);
    mem.map(0, 0xE000, 0x1000, roms->ptr(rom_images::atom_dos), false);
    mem.map(0, 0xF000, 0x1000, roms->ptr(rom_images::atom_basic) + 0x1000, false);
    vidmem_base = mem.read_ptr(0x8000);

    // 1 MHz CPU clock frequency
    const int freq_khz = 1000;
    board->clck.init(freq_khz);
    board->mos6502.init(this);
    board->mos6502.reset();
    board->i8255.init(0);
    vdg->init(read_vidmem, board->rgba8_buffer, freq_khz);
    counter_2_4khz.init(freq_khz * 1000 / 4800);
    board->beeper.init(freq_khz, SOUND_SAMPLE_RATE);
}

//------------------------------------------------------------------------------
void
atom::poweroff() {
    YAKC_ASSERT(on);
    board->mos6502.mem.unmap_all();
    on = false;
}

//------------------------------------------------------------------------------
void
atom::reset() {
    board->mos6502.reset();
    board->i8255.reset();
    board->mc6847.reset();
    board->beeper.reset();
    scan_kbd_col = 0;
    next_key_mask = key_mask();
    cur_key_mask = key_mask();
}

//------------------------------------------------------------------------------
uint64_t
atom::step(uint64_t start_tick, uint64_t end_tick) {
    auto& cpu = board->mos6502;
    auto& dbg = board->dbg;
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        if (dbg.check_break(cpu.PC)) {
            dbg.paused = true;
            return end_tick;
        }
        dbg.store_pc_history(cpu.PC); // FIXME: only if debug window open?    
        cur_tick += cpu.step();
    }
    return cur_tick;
}

//------------------------------------------------------------------------------
void
atom::put_input(uint8_t ascii) {
    next_key_mask = key_map[ascii];
}

//------------------------------------------------------------------------------
void
atom::cpu_tick() {
    vdg->step();

    // update the sound beeper
    // NOTE: don't make the cassette output audible, since it
    // seems to get stuck at a 2.4kHz sound at the end of saving BASIC programs
    board->beeper.write(out_beep);// || ((!state_2_4khz && out_cass1) && out_cass0));
    board->beeper.step(1);

    // tick the 2.4kHz counter
    if (counter_2_4khz.tick()) {
        state_2_4khz = !state_2_4khz;
    }

    // on FSYNC, feed next input key mask, this gives the OS
    // 1 full frame to scan the keyboard matrix
    if (vdg->on(mc6847::FSYNC)) {
        cur_key_mask = next_key_mask;
    }
}

//------------------------------------------------------------------------------
uint8_t
atom::memio(bool write, uint16_t addr, uint8_t inval) {
    if ((addr >= 0xB000) && (addr < 0xB400)) {
        if (write) {
            self->board->i8255.write(self, addr & 0x0003, inval);
        }
        else {
            return self->board->i8255.read(self, addr & 0x0003);
        }
    }
    else if ((addr >= 0xB800) && (addr < 0xBC00)) {
        //printf("VIA: addr=%04X %s %02X\n", addr, write ? "write":"read", inval);
        return 0x00;
    }
    else {
        //printf("UNKNOWN: addr=%04X %s %02X\n", addr, write ? "write":"read", inval);
    }
    return 0xFF;
}

//------------------------------------------------------------------------------
uint8_t
atom::read_vidmem(uint16_t addr) {
    uint8_t chr = self->vidmem_base[addr];

    // the upper 2 databus bits are directly wired to MC6847 pins:
    //  bit 7 -> INV pin (in text mode, invert pixel pattern)
    //  bit 6 -> A/S and INT/EXT pin, A/S actives semigraphics mode
    //           and INT/EXT selects the 2x3 semigraphics pattern
    //           (so 4x4 semigraphics isn't possible)
    self->vdg->inv(chr & (1<<7));
    self->vdg->as(chr & (1<<6));
    self->vdg->int_ext(chr & (1<<6));
    return chr;
}

//------------------------------------------------------------------------------
void
atom::pio_out(int pio_id, int port_id, uint8_t val) {
    /*
    FROM Atom Theory and Praxis (and MAME)

    The  8255  Programmable  Peripheral  Interface  Adapter  contains  three
    8-bit ports, and all but one of these lines is used by the ATOM.

    Port A - #B000
           Output bits:      Function:
                O -- 3     Keyboard column
                4 -- 7     Graphics mode (4: A/G, 5..7: GM0..2)

    Port B - #B001
           Input bits:       Function:
                O -- 5     Keyboard row
                  6        CTRL key (low when pressed)
                  7        SHIFT keys {low when pressed)

    Port C - #B002
           Output bits:      Function:
                O          Tape output
                1          Enable 2.4 kHz to cassette output
                2          Loudspeaker
                3          Not used (??? see below)

           Input bits:       Function:
                4          2.4 kHz input
                5          Cassette input
                6          REPT key (low when pressed)
                7          60 Hz sync signal (low during flyback)

    The port C output lines, bits O to 3, may be used for user
    applications when the cassette interface is not being used.
    */
    switch (port_id) {
        // PPI port A
        //  0..4:   keyboard matrix column
        //  5:      MC6847 A/G
        //  6:      MC6847 GM0
        //  7:      MC6847 GM1
        //  8:      MC6847 GM2
        case i8255::PORT_A:
            scan_kbd_col = val & 0x0F;
            vdg->ag(val & (1<<4));
            vdg->gm0(val & (1<<5));
            vdg->gm1(val & (1<<6));
            vdg->gm2(val & (1<<7));
            break;

        // PPI port C output:
        //  0:  output: cass 0
        //  1:  output: cass 1
        //  2:  output: speaker
        //  3:  output: MC6847 CSS
        //
        // the resulting cassette out signal is
        // created like this (see the Atom circuit diagram
        // right of the keyboard matrix:
        //
        //  (((not 2.4khz) and cass1) & and cass0)
        //
        // but the cassette out bits seem to get stuck on 0
        // after saving a BASIC program, so don't make it audible
        //
        case i8255::PORT_C:
            out_cass0 = 0 == (val & 1);
            out_cass1 = 0 == (val & 2);
            out_beep = 0 == (val & 4);
            vdg->css(val & (1<<3));
            break;
    }
}

//------------------------------------------------------------------------------
uint8_t
atom::pio_in(int pio_id, int port_id) {
    uint8_t val = 0x00;
    switch (port_id) {
        // PPI port B: keyboard row state
        case i8255::PORT_B:
            if (scan_kbd_col < 10) {
                val = ~(cur_key_mask.col[scan_kbd_col]);
                if (scan_kbd_col == 9) {
                    cur_key_mask = next_key_mask;
                }
            }
            else {
                val = 0xFF;
            }
            break;
        // PPI port C input:
        //  4:  input: 2400 Hz
        //  5:  input: cassette
        //  6:  input: keyboard repeat
        //  7:  input: MC6847 FSYNC
        case i8255::PORT_C:
            if (state_2_4khz) {
                val |= (1<<4);
            }
            // FIXME: always send REPEAT key as 'not pressed'
            val |= (1<<6);
            if (!board->mc6847.test(mc6847::FSYNC)) {
                val |= (1<<7);
            }
            break;
    }
    return val;
}

//------------------------------------------------------------------------------
const void*
atom::framebuffer(int& out_width, int& out_height) {
    out_width = mc6847::disp_width_with_border;
    out_height = mc6847::disp_height_with_border;
    return board->mc6847.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
atom::decode_audio(float* buffer, int num_samples) {
    board->beeper.fill_samples(buffer, num_samples);
}

//------------------------------------------------------------------------------
const char*
atom::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
