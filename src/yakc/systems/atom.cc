//------------------------------------------------------------------------------
//  atom.cc
//
//  TODO
//      - interrupt from VIA
//      - handle Shift key (some games use this as jump button)
//
//------------------------------------------------------------------------------
#include "atom.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

atom* atom::self = nullptr;

//------------------------------------------------------------------------------
void
atom::init(breadboard* b, rom_images* r, tapedeck* t) {
    YAKC_ASSERT(b && r && t);
    self = this;
    board = b;
    roms = r;
    tape = t;
    cpu = &(board->mos6502);
    vdg = &(board->mc6847);
    ppi = &(board->i8255);
    via = &(board->mos6522);
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

    // 10x5 keyboard matrix (10 columns, 7 rows,
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

    // keyboard joystick (just use some unused upper ascii codes)
    init_key_mask(0xF0, 0, 0);
    init_key_mask(0xF1, 0, 1);
    init_key_mask(0xF2, 0, 2);
    init_key_mask(0xF3, 0, 3);
    init_key_mask(0xF4, 0, 4);
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
    mmc_joymask = 0;
    mmc_cmd = 0;
    mmc_latch = 0;

    // initialize the memory map
    // fill memory with random junk
    memcpy(board->ram[0], board->random, breadboard::ram_bank_size);
    memcpy(board->ram[1], board->random, breadboard::ram_bank_size);
    memcpy(board->ram[2], board->random, breadboard::ram_bank_size);
    clear(board->ram[3], sizeof(board->ram[3]));
    auto& mem = cpu->mem;
    mem.unmap_all();
    // give it the full 32 KByte RAM (0x0000..0x7FFF) + 8 KByte videomem (0x8000..0x9FFF)
    mem.map(0, 0x0000, 0xA000, board->ram[0], true);
    // hole in 0xA000 to 0xAFFF (for utility roms)
    // 0xB000 to 0xBFFF: I/O area
    mem.map_io(0, 0xB000, 0x1000, memio);
    // ROM area
    mem.map(0, 0xC000, 0x1000, roms->ptr(rom_images::atom_basic), false);
    mem.map(0, 0xD000, 0x1000, roms->ptr(rom_images::atom_float), false);
    mem.map(0, 0xE000, 0x1000, roms->ptr(rom_images::atom_dos), false);
    mem.map(0, 0xF000, 0x1000, roms->ptr(rom_images::atom_basic) + 0x1000, false);
    vidmem_base = mem.read_ptr(0x8000);

    // 1 MHz CPU clock frequency
    const int freq_khz = 1000;
    board->clck.init(freq_khz);
    cpu->init(this);
    cpu->reset();
    ppi->init(0);
    via->init(1);
    vdg->init(read_vidmem, board->rgba8_buffer, freq_khz);
    counter_2_4khz.init(freq_khz * 1000 / 4800);
    board->beeper.init(freq_khz, SOUND_SAMPLE_RATE);

    // trap the OSLOAD function
    // http://ladybug.xs4all.nl/arlet/fpga/6502/kernel.dis
    this->osload_trap = 0xF96E;
}

//------------------------------------------------------------------------------
void
atom::poweroff() {
    YAKC_ASSERT(on);
    cpu->mem.unmap_all();
    on = false;
}

//------------------------------------------------------------------------------
void
atom::reset() {
    cpu->reset();
    ppi->reset();
    via->reset();
    vdg->reset();
    counter_2_4khz.reset();
    state_2_4khz = false;
    board->beeper.reset();
    scan_kbd_col = 0;
    next_key_mask = key_mask();
    cur_key_mask = key_mask();
    out_beep = false;
    out_cass0 = false;
    out_cass1 = false;
}

//------------------------------------------------------------------------------
uint64_t
atom::step(uint64_t start_tick, uint64_t end_tick) {
    auto& dbg = board->dbg;
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        uint32_t ticks = cpu->step();
        if (dbg.step(cpu->PC, ticks)) {
            return end_tick;
        }
        if (cpu->PC == osload_trap) {
            // trapped OSLOAD function
            osload();
        }
        cur_tick += ticks;
    }
    return cur_tick;
}

//------------------------------------------------------------------------------
uint32_t
atom::step_debug() {
    uint32_t ticks = cpu->step();
    board->dbg.step(cpu->PC, ticks);
    return ticks;
}

//------------------------------------------------------------------------------
void
atom::put_input(uint8_t ascii, uint8_t joy0mask) {
    mmc_joymask = 0;
    if (0 == joy0mask) {
        next_key_mask = key_map[ascii];
    }
    else {
        next_key_mask = key_mask();
        if (joy0mask & joystick::left) {
            mmc_joymask |= 0x2;
            next_key_mask.combine(this->key_map[0xF1]);
        }
        if (joy0mask & joystick::right) {
            mmc_joymask |= 0x1;
            next_key_mask.combine(this->key_map[0xF3]);
        }
        if (joy0mask & joystick::up) {
            mmc_joymask |= 0x8;
            next_key_mask.combine(this->key_map[0xF4]);
        }
        if (joy0mask & joystick::down) {
            mmc_joymask |= 0x4;
            next_key_mask.combine(this->key_map[0xF2]);
        }
        if (joy0mask & joystick::btn0) {
            mmc_joymask |= 0x10;
            next_key_mask.combine(this->key_map[0xF0]);
        }
    }
}

//------------------------------------------------------------------------------
void
atom::cpu_tick() {
    vdg->step();
    via->step(this);

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
        // i8255: http://www.acornatom.nl/sites/fpga/www.howell1964.freeserve.co.uk/acorn/atom/amb/amb_8255.htm
        if (write) {
            self->ppi->write(self, addr & 0x0003, inval);
        }
        else {
            return self->ppi->read(self, addr & 0x0003);
        }
    }
    else if ((addr >= 0xB400) && (addr < 0xB800)) {
        // extensions (only rudimentary)
        // FIXME: implement a proper AtoMMC emulation, for now just
        // a quick'n'dirty hack for joystick input
        if (write) {
            if (addr == 0xB400) {
                self->mmc_cmd = inval;
            }
        }
        else {
            if (addr == 0xB400) {
                // reading from 0xB400 returns a status/error code, the important
                // ones are STATUS_OK=0x3F, and STATUS_BUSY=0x80, STATUS_COMPLETE
                // together with an error code is used to communicate errors
                return 0x3F;
            }
            else if ((addr == 0xB401) && (self->mmc_cmd == 0xA2)) {
                // read MMC joystick
                return ~self->mmc_joymask;
            }
        }
    }
    else if ((addr >= 0xB800) && (addr < 0xBC00)) {
        // 6522 VIA: http://www.acornatom.nl/sites/fpga/www.howell1964.freeserve.co.uk/acorn/atom/amb/amb_6522.htm
        if (write) {
            self->via->write(self, addr & 0x000F, inval);
        }
        else {
            return self->via->read(self, addr & 0x000F);
        }
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
    self->vdg->inv(0 != (chr & (1<<7)));
    self->vdg->as(0 != (chr & (1<<6)));
    self->vdg->int_ext(0 != (chr & (1<<6)));
    return chr;
}

//------------------------------------------------------------------------------
void
atom::pio_out(int pio_id, int port_id, uint8_t val) {
    if (0 == pio_id) {
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
                vdg->ag(0 != (val & (1<<4)));
                vdg->gm0(0 != (val & (1<<5)));
                vdg->gm1(0 != (val & (1<<6)));
                vdg->gm2(0 != (val & (1<<7)));
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
                vdg->css(0 != (val & (1<<3)));
                break;
        }
    }
    else if (1 == pio_id) {
        // the 6522 VIA
        //printf("write to VIA Port %s: %d\n", port_id==0?"A":"B", val);
    }
}

//------------------------------------------------------------------------------
uint8_t
atom::pio_in(int pio_id, int port_id) {
    uint8_t val = 0x00;
    if (0 == pio_id) {
        // the i8255 PIO
        switch (port_id) {
            // PPI port B: keyboard row state
            case i8255::PORT_B:
                if (scan_kbd_col < key_mask::num_cols) {
                    val = ~(cur_key_mask.col[scan_kbd_col]);
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
    }
    else if (1 == pio_id) {
        // the 6522 VIA
        //printf("read from VIA Port %s\n", port_id==0?"A":"B");
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
bool
atom::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    // FIXME
    fs->rm(name);
    return false;
}

//------------------------------------------------------------------------------
void
atom::osload() {
    //
    // trapped OSLOAD function, load ATM block in a TAP file:
    //   https://github.com/hoglet67/Atomulator/blob/master/docs/atommmc2.txt
    //
    //
    // from: http://ladybug.xs4all.nl/arlet/fpga/6502/kernel.dis
    //
    //   OSLOAD Load File subroutine
    //    ---------------------------
    //  
    //  - Entry: 0,X = LSB File name string address
    //           1,X = MSB File name string address
    //           2,X = LSB Data dump start address
    //           3,X = MSB Data dump start address
    //           4,X : If bit 7 is clear, then the file's own start address is
    //                 to be used
    //           #DD = FLOAD flag - bit 7 is set if in FLOAD mode
    //  
    //  - Uses:  #C9 = LSB File name string address
    //           #CA = MSB File name string address
    //           #CB = LSB Data dump start address
    //           #CC = MSB Data dump start address
    //           #CD = load flag - if bit 7 is set, then the load address at
    //                 (#CB) is to be used instead of the file's load address
    //           #D0 = MSB Current block number
    //           #D1 = LSB Current block number
    //  
    //  - Header format: <*>                      )
    //                   <*>                      )
    //                   <*>                      )
    //                   <*>                      ) Header preamble
    //                   <Filename>               ) Name is 1 to 13 bytes long
    //                   <Status Flag>            ) Bit 7 clear if last block
    //                                            ) Bit 6 clear to skip block
    //                                            ) Bit 5 clear if first block
    //                   <LSB block number>
    //                   <MSB block number>       ) Always zero
    //                   <Bytes in block>
    //                   <MSB run address>
    //                   <LSB run address>
    //                   <MSB block load address>
    //                   <LSB block load address>
    //  
    //  - Data format:   <....data....>           ) 1 to #FF bytes
    //                   <Checksum>               ) LSB sum of all data bytes
    //
    bool success = false;

    // load the ATM header
    atomtap_header hdr;
    if (tape->read(&hdr, sizeof(hdr)) == sizeof(hdr)) {
        uint16_t addr = hdr.load_addr;
        // use file load address?
        if (cpu->mem.r8io(0xCD) & 0x80) {
            addr = cpu->mem.r16io(0xCB);
        }
        for (int i = 0; i < hdr.length; i++) {
            uint8_t val;
            tape->read(&val, sizeof(val));
            cpu->mem.w8io(addr++, val);
        }
        success = true;
    }
    // set or clear bit 6 and clear bit 7 of 0xDD
    uint8_t dd = cpu->mem.r8io(0xDD);
    if (success) {
        dd |= (1<<6);
    }
    else {
        dd &= ~(1<<6);
    }
    dd &= ~(1<<7);
    cpu->mem.w8io(0xDD, dd);

    // execute RTS
    cpu->rts();

    // FIXME: patch PC????
    if (success) {
        cpu->PC = hdr.exec_addr;
    }
}

//------------------------------------------------------------------------------
const char*
atom::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
