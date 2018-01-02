//------------------------------------------------------------------------------
//  atom.cc
//
//  TODO
//      - trap OS Load function
//      - joystick input
//      - interrupt from VIA
//      - handle Shift key (some games use this as jump button)
//
//------------------------------------------------------------------------------
#include "atom.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

atom_t atom;

//------------------------------------------------------------------------------
void
atom_t::init_keymap() {
    kbd_init(&board.kbd, 1);
    /* shift key is entire line 7 */
    const int shift = (1<<0); kbd_register_modifier_line(&board.kbd, 0, 7);
    /* ctrl key is entire line 6 */
    const int ctrl = (1<<1); kbd_register_modifier_line(&board.kbd, 1, 6);

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
    for (int layer = 0; layer < 2; layer++) {
        for (int col = 0; col < 10; col++) {
            for (int line = 0; line < 6; line++) {
                uint8_t c = kbd[shift*60 + line*10 + col];
                kbd_register_key(&board.kbd, c, col, line, layer?shift:0);
            }
        }
    }

    // special keys
    //init_key_mask(0x00, 4, 0);     // FIXME capslock
    //init_key_mask(0x00, 5, 0);     // FIXME tab
    kbd_register_key(&board.kbd, 0x20, 9, 0, 0);         // space
    kbd_register_key(&board.kbd, 0x01, 4, 1, 0);         // backspace
    kbd_register_key(&board.kbd, 0x07, 0, 3, ctrl);      // Ctrl+G: bleep
    kbd_register_key(&board.kbd, 0x08, 3, 0, shift);     // key left
    kbd_register_key(&board.kbd, 0x09, 3, 0, 0);         // key right
    kbd_register_key(&board.kbd, 0x0A, 2, 0, shift);     // key down
    kbd_register_key(&board.kbd, 0x0B, 2, 0, 0);         // key up
    kbd_register_key(&board.kbd, 0x0D, 6, 1, 0);         // return/enter
    kbd_register_key(&board.kbd, 0x0C, 5, 4, ctrl);      // Ctrl+L clear screen
    kbd_register_key(&board.kbd, 0x0E, 3, 4, ctrl);      // Ctrl+N page mode on
    kbd_register_key(&board.kbd, 0x0F, 2, 4, ctrl);      // Ctrl+O page mode off
    kbd_register_key(&board.kbd, 0x15, 6, 5, ctrl);      // Ctrl+U end screen
    kbd_register_key(&board.kbd, 0x18, 3, 5, ctrl);      // Ctrl+X cancel
    kbd_register_key(&board.kbd, 0x1B, 0, 5, 0);         // escape

    // keyboard joystick (just use some unused upper ascii codes)
    kbd_register_key(&board.kbd, 0xF0, 0, 0, 0);
    kbd_register_key(&board.kbd, 0xF1, 0, 1, 0);
    kbd_register_key(&board.kbd, 0xF2, 0, 2, 0);
    kbd_register_key(&board.kbd, 0xF3, 0, 3, 0);
    kbd_register_key(&board.kbd, 0xF4, 0, 4, 0);
}

//------------------------------------------------------------------------------
bool
atom_t::check_roms(system model, os_rom os) {
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
atom_t::poweron() {
    YAKC_ASSERT(!on);
    
    on = true;
    mmc_joymask = 0;
    mmc_cmd = 0;
    mmc_latch = 0;
    init_keymap();

    // initialize the memory map
    // fill memory with random junk
    memcpy(board.ram[0], board.random, breadboard::ram_bank_size);
    memcpy(board.ram[1], board.random, breadboard::ram_bank_size);
    memcpy(board.ram[2], board.random, breadboard::ram_bank_size);
    clear(board.ram[3], sizeof(board.ram[3]));
    mem_unmap_all(&board.mem);
    // give it the full 32 KByte RAM (0x0000..0x7FFF) + 8 KByte videomem (0x8000..0x9FFF)
    mem_map_ram(&board.mem, 0, 0x0000, 0xA000, board.ram[0]);
    // hole in 0xA000 to 0xAFFF (for utility roms)
    // 0xB000 to 0xBFFF: I/O area, not mapped to host memory
    // ROM area
    mem_map_rom(&board.mem, 0, 0xC000, 0x1000, roms.ptr(rom_images::atom_basic));
    mem_map_rom(&board.mem, 0, 0xD000, 0x1000, roms.ptr(rom_images::atom_float));
    mem_map_rom(&board.mem, 0, 0xE000, 0x1000, roms.ptr(rom_images::atom_dos));
    mem_map_rom(&board.mem, 0, 0xF000, 0x1000, roms.ptr(rom_images::atom_basic) + 0x1000);
    vidmem_base = mem_readptr(&board.mem, 0x8000);

    // 1 MHz CPU clock frequency
    board.freq_hz = 1000000;
    m6502_init(&board.m6502, cpu_tick);
    m6502_reset(&board.m6502);
    i8255_init(&board.i8255, ppi_in, ppi_out);
    mc6847_desc_t vdg_desc;
    vdg_desc.tick_hz = board.freq_hz;
    vdg_desc.rgba8_buffer = board.rgba8_buffer;
    vdg_desc.rgba8_buffer_size = sizeof(board.rgba8_buffer);
    vdg_desc.fetch_cb = vdg_fetch;
    mc6847_init(&board.mc6847, &vdg_desc);
    beeper_init(&board.beeper, board.freq_hz, SOUND_SAMPLE_RATE, 0.5f);
    period_2_4khz = board.freq_hz / 4800;
    count_2_4khz = 0;
    state_2_4khz = false;

    // trap the OSLOAD function
    // http://ladybug.xs4all.nl/arlet/fpga/6502/kernel.dis
    this->osload_trap = 0xF96E;
}

//------------------------------------------------------------------------------
void
atom_t::poweroff() {
    YAKC_ASSERT(on);
    mem_unmap_all(&board.mem);
    on = false;
}

//------------------------------------------------------------------------------
void
atom_t::reset() {
    m6502_reset(&board.m6502);
    i8255_reset(&board.i8255);
    mc6847_reset(&board.mc6847);
    beeper_reset(&board.beeper);
    state_2_4khz = false;
    out_cass0 = false;
    out_cass1 = false;
}

//------------------------------------------------------------------------------
uint64_t
atom_t::step(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = m6502_exec(&board.m6502, num_ticks);
    kbd_update(&board.kbd);
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint32_t
atom_t::step_debug() {
return 0;
    /* FIXME
    uint32_t ticks = cpu->step();
    board->dbg.step(cpu->PC, ticks);
    return ticks;
    */
}

//------------------------------------------------------------------------------
void
atom_t::put_input(uint8_t ascii, uint8_t joy0mask) {
    mmc_joymask = 0;
    if (0 == joy0mask) {
        if (ascii) {
            kbd_key_down(&board.kbd, ascii);
            kbd_key_up(&board.kbd, ascii);
        }
    }
    else {
        /* FIXME
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
        */
    }
}

//------------------------------------------------------------------------------
uint64_t
atom_t::cpu_tick(uint64_t pins) {

    /* tick the video chip */
    mc6847_tick(&board.mc6847);

    /* tick the 2.4khz counter */
    atom.count_2_4khz++;
    if (atom.count_2_4khz >= atom.period_2_4khz) {
        atom.state_2_4khz = !atom.state_2_4khz;
        atom.count_2_4khz -= atom.period_2_4khz;
    }

    // update the sound beeper
    // NOTE: don't make the cassette output audible, since it
    // seems to get stuck at a 2.4kHz sound at the end of saving BASIC programs
    if (beeper_tick(&board.beeper)) {
        board.audiobuffer.write(board.beeper.sample);
    }

    // decode address for memory-mapped-io or regular memory access
    const uint16_t addr = M6502_GET_ADDR(pins);
    if ((addr >= 0xB000) && (addr < 0xC000)) {
        // memory-mapped-io area
        if ((addr >= 0xB000) && (addr < 0xB400)) {
            // i8255: http://www.acornatom.nl/sites/fpga/www.howell1964.freeserve.co.uk/acorn/atom/amb/amb_8255.htm
            uint64_t ppi_pins = (pins & M6502_PIN_MASK) | I8255_CS;
            if (pins & M6502_RW) { ppi_pins |= I8255_RD; }  /* PPI read access */
            else { ppi_pins |= I8255_WR; }                  /* PPI write access */
            if (pins & M6502_A0) { ppi_pins |= I8255_A0; }  /* PPI has 4 addresses (port A,B,C or control word */
            if (pins & M6502_A1) { ppi_pins |= I8255_A1; }
            pins = i8255_iorq(&board.i8255, ppi_pins) & M6502_PIN_MASK;
        }
        else if ((addr >= 0xB400) && (addr < 0xB800)) {
            // extensions (only rudimentary)
            // FIXME: implement a proper AtoMMC emulation, for now just
            // a quick'n'dirty hack for joystick input
            if (pins & M6502_RW) {
                // read from MMC extension
                if (addr == 0xB400) {
                    // reading from 0xB400 returns a status/error code, the important
                    // ones are STATUS_OK=0x3F, and STATUS_BUSY=0x80, STATUS_COMPLETE
                    // together with an error code is used to communicate errors
                    M6502_SET_DATA(pins, 0x3F);
                }
                else if ((addr == 0xB401) && (atom.mmc_cmd == 0xA2)) {
                    // read MMC joystick
                    M6502_SET_DATA(pins, ~atom.mmc_joymask);
                }
            }
            else {
                // write to MMC extension
                if (addr == 0xB400) {
                    atom.mmc_cmd = M6502_GET_DATA(pins);
                }
            }
        }
        else if ((addr >= 0xB800) && (addr < 0xBC00)) {
            // 6522 VIA: http://www.acornatom.nl/sites/fpga/www.howell1964.freeserve.co.uk/acorn/atom/amb/amb_6522.htm
            // FIXME
            M6502_SET_DATA(pins, 0);
        }
        else {
            //printf("UNKNOWN: addr=%04X %s %02X\n", addr, write ? "write":"read", inval);
            M6502_SET_DATA(pins, 0);
        }
    }
    else {
        // regular memory access
        if (pins & M6502_RW) {
            // memory read
            M6502_SET_DATA(pins, mem_rd(&board.mem, addr));
        }
        else {
            // memory write
            mem_wr(&board.mem, addr, M6502_GET_DATA(pins));
        }
    }
    return pins;
}

//------------------------------------------------------------------------------
uint64_t
atom_t::vdg_fetch(uint64_t pins) {
    const uint16_t addr = MC6847_GET_ADDR(pins);
    uint8_t data = atom.vidmem_base[addr];
    MC6847_SET_DATA(pins, data);

    /*  the upper 2 databus bits are directly wired to MC6847 pins:
        bit 7 -> INV pin (in text mode, invert pixel pattern)
        bit 6 -> A/S and INT/EXT pin, A/S actives semigraphics mode
                 and INT/EXT selects the 2x3 semigraphics pattern
                 (so 4x4 semigraphics isn't possible)
    */
    if (data & (1<<7)) { pins |= MC6847_INV; }
    else               { pins &= ~MC6847_INV; }
    if (data & (1<<6)) { pins |= (MC6847_AS|MC6847_INTEXT); }
    else               { pins &= ~(MC6847_AS|MC6847_INTEXT); }
    return pins;
}

//------------------------------------------------------------------------------
uint64_t
atom_t::ppi_out(int port_id, uint64_t pins, uint8_t data) {
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
        uint64_t vdg_pins, vdg_mask;
        // PPI port A
        //  0..4:   keyboard matrix column
        //  5:      MC6847 A/G
        //  6:      MC6847 GM0
        //  7:      MC6847 GM1
        //  8:      MC6847 GM2
        case I8255_PORT_A:
            kbd_set_active_columns(&board.kbd, 1<<(data & 0x0F));
            vdg_pins = 0;
            vdg_mask = MC6847_AG|MC6847_GM0|MC6847_GM1|MC6847_GM2;
            if (data & (1<<4)) { vdg_pins |= MC6847_AG; }
            if (data & (1<<5)) { vdg_pins |= MC6847_GM0; }
            if (data & (1<<6)) { vdg_pins |= MC6847_GM1; }
            if (data & (1<<7)) { vdg_pins |= MC6847_GM2; }
            mc6847_ctrl(&board.mc6847, vdg_pins, vdg_mask);
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
        case I8255_PORT_C:
            atom.out_cass0 = 0 == (data & (1<<0));
            atom.out_cass1 = 0 == (data & (1<<1));
            beeper_write(&board.beeper, 0 == (data & (1<<2)));
            vdg_pins = 0;
            vdg_mask = MC6847_CSS;
            if (data & (1<<3)) {
                vdg_pins |= MC6847_CSS;
            }
            mc6847_ctrl(&board.mc6847, vdg_pins, vdg_mask);
            break;
    }
    return pins;
}

//------------------------------------------------------------------------------
uint8_t
atom_t::ppi_in(int port_id) {
    uint8_t data = 0x00;
    switch (port_id) {
        // PPI port B: keyboard row state
        case I8255_PORT_B:
            data = ~kbd_scan_lines(&board.kbd);
            break;
        // PPI port C input:
        //  4:  input: 2400 Hz
        //  5:  input: cassette
        //  6:  input: keyboard repeat
        //  7:  input: MC6847 FSYNC
        case I8255_PORT_C:
            if (atom.state_2_4khz) {
                data |= (1<<4);
            }
            // FIXME: always send REPEAT key as 'not pressed'
            data |= (1<<6);
            if (0 == (board.mc6847.pins & MC6847_FS)) {
                data |= (1<<7);
            }
            break;
    }
    return data;
}

//------------------------------------------------------------------------------
const void*
atom_t::framebuffer(int& out_width, int& out_height) {
    out_width = MC6847_DISPLAY_WIDTH;
    out_height = MC6847_DISPLAY_HEIGHT;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
atom_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
bool
atom_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    // FIXME
    fs->rm(name);
    return false;
}

//------------------------------------------------------------------------------
void
atom_t::osload() {
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
    if (tape.read(&hdr, sizeof(hdr)) == sizeof(hdr)) {
        uint16_t addr = hdr.load_addr;
        // use file load address?
        if (mem_rd(&board.mem, 0xCD) & 0x80) {
            addr = mem_rd16(&board.mem, 0xCB);
        }
        for (int i = 0; i < hdr.length; i++) {
            uint8_t val;
            tape.read(&val, sizeof(val));
            mem_wr(&board.mem, addr++, val);
        }
        success = true;
    }
    // set or clear bit 6 and clear bit 7 of 0xDD
    uint8_t dd = mem_rd(&board.mem, 0xDD);
    if (success) {
        dd |= (1<<6);
    }
    else {
        dd &= ~(1<<6);
    }
    dd &= ~(1<<7);
    mem_wr(&board.mem, 0xDD, dd);

    // execute RTS
    auto& cpu = board.m6502;
    cpu.S++;
    uint8_t l = mem_rd(&board.mem, 0x0100|cpu.S++);
    uint8_t h = mem_rd(&board.mem, 0x0100|cpu.S);
    cpu.PC = (h<<8)|l;
    cpu.PC++;

    // FIXME: patch PC????
    if (success) {
        board.m6502.PC = hdr.exec_addr;
    }
}

//------------------------------------------------------------------------------
const char*
atom_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
