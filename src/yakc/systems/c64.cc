//------------------------------------------------------------------------------
//  c64.cc
//------------------------------------------------------------------------------
#include "c64.h"
#include "yakc/util/tapedeck.h"

namespace YAKC {

c64_t c64;

//------------------------------------------------------------------------------
bool
c64_t::check_roms(system model, os_rom os) {
    if ((system::c64_pal == model) || (system::c64_ntsc == model)) {
        return roms.has(rom_images::c64_basic) &&
               roms.has(rom_images::c64_char) &&
               roms.has(rom_images::c64_kernalv3);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
c64_t::poweron(system m) {
    YAKC_ASSERT(!this->on);
    YAKC_ASSERT((system::c64_pal == m) || (system::c64_ntsc == m));

    this->on = true;
    this->model = m;
    this->cpu_port = 0xF7;      // for initial memory configuration
    this->cia1_joy_bits = 0;
    this->io_mapped = true;
    this->tap_header = c64tap_header();
    this->tape_valid = false;
    this->tape_tick_count = 0;
    this->tape_byte_count = 0;

    // setup the keyboard matrix
    this->init_keymap();

    // setup initial memory map
    this->init_memory_map();

    // initialize the CPU
    board.freq_hz = (m == system::c64_pal) ? 985248 : 1022727;
    m6502_desc_t cpu_desc = { };
    cpu_desc.tick_cb = cpu_tick;
    cpu_desc.in_cb = cpu_port_in;
    cpu_desc.out_cb = cpu_port_out;
    cpu_desc.m6510_io_pullup = 0x17;
    cpu_desc.m6510_io_floating = 0xC8;
    m6502_init(&board.m6502, &cpu_desc);

    // initialize the CIAs
    m6526_init(&board.m6526_1, cia1_in, cia1_out);
    m6526_init(&board.m6526_2, cia2_in, cia2_out);
    
    // initialize the VIC-II display chip
    m6567_desc_t vic_desc = { };
    vic_desc.type = (m == system::c64_pal) ? M6567_TYPE_6569 : M6567_TYPE_6567R8;
    vic_desc.fetch_cb = vic_fetch;
    vic_desc.rgba8_buffer = board.rgba8_buffer;
    vic_desc.rgba8_buffer_size = sizeof(board.rgba8_buffer);
    vic_desc.vis_x = 64;
    vic_desc.vis_y = 24;
    vic_desc.vis_w = 392;
    vic_desc.vis_h = 272;
    m6567_init(&board.m6567, &vic_desc);

    // use a beeper for audible datasette output
    beeper_init(&board.beeper_1, board.freq_hz, SOUND_SAMPLE_RATE, 0.1f);

    // put the CPU into start state
    m6502_reset(&board.m6502);
}

//------------------------------------------------------------------------------
void
c64_t::poweroff() {
    YAKC_ASSERT(this->on);
    mem_unmap_all(&board.mem);
    this->on = false;
}

//------------------------------------------------------------------------------
void
c64_t::reset() {
    this->cpu_port = 0xF7;
    this->cia1_joy_bits = 0;
    this->io_mapped = true;
    this->update_memory_map();
    m6502_reset(&board.m6502);
    m6526_reset(&board.m6526_1);
    m6526_reset(&board.m6526_2);
    m6567_reset(&board.m6567);
    this->tap_header = c64tap_header();
    this->tape_valid = false;
    this->tape_tick_count = 0;
    this->tape_byte_count = 0;
}

//------------------------------------------------------------------------------
uint64_t
c64_t::exec(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = uint32_t(end_tick - start_tick);
    uint32_t ticks_executed = m6502_exec(&board.m6502, num_ticks);
    kbd_update(&board.kbd);
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint64_t
c64_t::cpu_tick(uint64_t pins) {
    const uint16_t addr = M6502_GET_ADDR(pins);

    // tick the datasette, when the datasette output pulse
    // toggles, the FLAG input pin on CIA-1 will go active for 1 tick
    uint64_t cia1_pins = pins;
    if (c64.tape_tick()) {
        cia1_pins |= M6526_FLAG;
    }

    // tick the CIAs:
    //  - CIA-1 gets the FLAG pin from the datasette
    //  - the CIA-1 IRQ pin is connected to the CPU IRQ pin
    //  - the CIA-2 IRQ pin is connected to the CPU NMI pin
    if (m6526_tick(&board.m6526_1, cia1_pins & ~M6502_IRQ) & M6502_IRQ) {
        pins |= M6502_IRQ;
    }
    if (m6526_tick(&board.m6526_2, pins & ~M6502_IRQ) & M6502_IRQ) {
        pins |= M6502_NMI;
    }

    // tick the VIC-II display chip:
    //  - the VIC-II IRQ pin is connected to the CPU IRQ pin and goes
    //    active when the VIC-II requests a rasterline interrupt
    //  - the VIC-II BA pin is connected to the CPU RDY pin, and stops
    //    the CPU on the first CPU read access after BA goes active
    //  - the VIC-II AEC pin is connected to the CPU AEC pin, currently
    //    this goes active during a badline, but is not checked
    pins = m6567_tick(&board.m6567, pins);

    // Special handling when the VIC-II asks the CPU to stop during a
    // 'badline' via the BA=>RDY pin. If the RDY pin is active, the
    // CPU will 'loop' on the tick callback during the next READ access
    // until the RDY pin goes inactive. The tick callback must make sure
    // that only a single READ access happens during the entire RDY period.
    // Currently this happens on the very last tick when RDY goes from
    // active to inactive (I haven't found definitive documentation if
    // this is the right behaviour, but it made the Boulderdash fast loader work).
    //
    // FIXME: another option to handle the RDY pin would be to loop *inside*
    // the tick callback until the RDY pin goes inactive, and only then return
    // to the CPU instruction decoder with the number of cycles spent in the
    // RDY loop.
    if ((pins & (M6502_RDY|M6502_RW)) == (M6502_RDY|M6502_RW)) {
        return pins;
    }

    // handle IO requests
    if (M6510_CHECK_IO(pins)) {
        // ...the integrated IO port in the M6510 CPU at addresses 0 and 1
        pins = m6510_iorq(&board.m6502, pins);
    }
    else {
        // ...the memory-mapped IO area from 0xD000 to 0xDFFF
        if (c64.io_mapped && ((addr & 0xF000) == 0xD000)) {
            if (addr < 0xD400) {
                // VIC-II (D000..D3FF)
                uint64_t vic_pins = (pins & M6502_PIN_MASK)|M6567_CS;
                pins = m6567_iorq(&board.m6567, vic_pins) & M6502_PIN_MASK;
            }
            else if (addr < 0xD800) {
                // SID (D400..D7FF)
                if (pins & M6502_RW) {
                    //printf("SID READ: %04X\n", addr);
                }
                else {
                    //printf("SID WRITE: %04X=%02X\n", addr, M6502_GET_DATA(pins));
                }
            }
            else if (addr < 0xDC00) {
                // the special color Static-RAM bank (D800..DBFF)
                if (pins & M6502_RW) {
                    M6502_SET_DATA(pins, c64.color_ram[addr & 0x03FF]);
                }
                else {
                    c64.color_ram[addr & 0x03FF] = M6502_GET_DATA(pins);
                }
            }
            else if (addr < 0xDD00) {
                // CIA-1 (DC00..DCFF)
                uint64_t cia_pins = (pins & M6502_PIN_MASK)|M6526_CS;
                pins = m6526_iorq(&board.m6526_1, cia_pins) & M6502_PIN_MASK;
            }
            else if (addr < 0xDE00) {
                // CIA-2 (DD00..DDFF)
                uint64_t cia_pins = (pins & M6502_PIN_MASK)|M6526_CS;
                pins = m6526_iorq(&board.m6526_2, cia_pins) & M6502_PIN_MASK;
            }
            else {
                // expansion system (not yet implemented)
                if (pins & M6502_RW) {
                    // printf("EXP READ: %04X\n", addr);
                }
                else {
                    // printf("EXP WRITE: %04X=%02X\n", addr, M6502_GET_DATA(pins));
                }
            }
        }
        else {
            // a regular memory access
            if (pins & M6502_RW) {
                // memory read
                M6502_SET_DATA(pins, mem_rd(&board.mem, addr));
            }
            else {
                // memory write
                mem_wr(&board.mem, addr, M6502_GET_DATA(pins));
            }
        }
    }
    return pins;
}

//------------------------------------------------------------------------------
uint8_t
c64_t::cpu_port_in() {
    /*
        Input from the integrated M6510 CPU IO port

        bit 4: [in] datasette button status (1: no button pressed)
    */
    uint8_t val = 7;
    if (!tape.playing) {
        val |= (1<<4);
    }
    return val;
}

//------------------------------------------------------------------------------
void
c64_t::cpu_port_out(uint8_t data) {
    /*
        Output to the integrated M6510 CPU IO port

        bits 0..2:  [out] memory configuration

        bit 3: [out] datasette output signal level
        bit 5: [out] datasette motor control (1: motor off)
    */
    if (data & (1<<5)) {
        tape.stop_motor();
    }
    else {
        tape.start_motor();
    }
    // only update memory configuration if the relevant bits have changed
    bool need_mem_update = 0 != ((c64.cpu_port ^ data) & 7);
    c64.cpu_port = data;
    if (need_mem_update) {
        c64.update_memory_map();
    }
}

//------------------------------------------------------------------------------
void
c64_t::cia1_out(int port_id, uint8_t data) {
    /*
        Write CIA-1 ports:

        port A:
            write keyboard matrix lines
        port B:
            ---
    */
    if (port_id == M6526_PORT_A) {
        kbd_set_active_lines(&board.kbd, ~data);
    }
    else {
        //printf("CIA-1 out %s: %02X\n", port_id==0?"A":"B", data);
    }
}

//------------------------------------------------------------------------------
uint8_t
c64_t::cia1_in(int port_id) {
    /*
        Read CIA-1 ports:

        Port A:
            joystick 2 input (FIXME)
        Port B:
            combined keyboard matrix columns and joystick 1
    */
    if (port_id == M6526_PORT_A) {
        /* FIXME: currently use same input for joystick 1 and 2 */
        return ~c64.cia1_joy_bits;
    }
    else {
        /* read keyboard matrix columns */
        return ~(kbd_scan_columns(&board.kbd) | c64.cia1_joy_bits);
    }
}

//------------------------------------------------------------------------------
void
c64_t::cia2_out(int port_id, uint8_t data) {
    /*
        Write CIA-2 ports:

        Port A:
            bits 0..1: VIC-II bank select:
                00: bank 3 C000..FFFF
                01: bank 2 8000..BFFF
                10: bank 1 4000..7FFF
                11: bank 0 0000..3FFF
            bit 2: RS-232 TXD Outout (not implemented)
            bit 3..5: serial bus output (not implemented)
            bit 6..7: input (see cia2_in)
        Port B:
            RS232 / user functionality (not implemented)
    */
    if (port_id == M6526_PORT_A) {
        c64.vic_bank_select = ((~data)&3)<<14;
    }
    else {
        // FIXME
    }
}

//------------------------------------------------------------------------------
uint8_t
c64_t::cia2_in(int port_id) {
    /*
        Read CIA-2 ports:

        Port A:
            bits 0..5: output (see cia2_out)
            bits 6..7: serial bus input, not implemented
        Port B:
            RS232 / user functionality (not implemented)
    */
    return 0xFF;
}

//------------------------------------------------------------------------------
uint16_t
c64_t::vic_fetch(uint16_t addr) {
    /*
        Fetch data into the VIC-II.

        The VIC-II has a 14-bit address bus and 12-bit data bus, and
        has a different memory mapping then the CPU (that's why it
        goes through the board.mem2 pagetable):
            - a full 16-bit address is formed by taking the address bits
              14 and 15 from the value written to CIA-1 port A
            - the lower 8 bits of the VIC-II data bus are connected
              to the shared system data bus, this is used to read
              character mask and pixel data
            - the upper 4 bits of the VIC-II data bus are hardwired to the
              static color RAM
    */
    addr |= c64.vic_bank_select;
    uint16_t data = (c64.color_ram[addr & 0x03FF]<<8) | mem_rd(&board.mem2, addr);
    return data;
}

//------------------------------------------------------------------------------
void
c64_t::init_memory_map() {
    YAKC_ASSERT(roms.size(rom_images::c64_basic) == 0x2000);
    YAKC_ASSERT(roms.size(rom_images::c64_char) == 0x1000);
    YAKC_ASSERT(roms.size(rom_images::c64_kernalv3) == 0x2000);

    // the C64 has a weird RAM init pattern of 64 bytes 00 and 64 bytes FF
    // alternating, probably with some randomness sprinkled in
    // (see this thread: http://csdb.dk/forums/?roomid=11&topicid=116800&firstpost=2)
    // this is important at least for the value of the 'ghost byte' at 0x3FFF,
    // which is 0xFF
    uint8_t* ram = &(board.ram[0][0]);
    for (int i = 0; i < (1<<16); i++) {
        ram[i] = (i & (1<<6)) ? 0xFF : 0x00;
    }

    // setup the initial CPU memory map
    mem_unmap_all(&board.mem);
    // 0000..9FFF and C000.CFFF is always RAM
    mem_map_ram(&board.mem, 0, 0x0000, 0xA000, ram);
    mem_map_ram(&board.mem, 0, 0xC000, 0x1000, ram+0xC000);
    // A000..BFFF, D000..DFFF and E000..FFFF are configurable
    this->update_memory_map();

    // setup the separate VIC-II memory map (64 KByte RAM) overlayed with
    // character ROMS at 0x1000.0x1FFF and 0x9000..0x9FFF
    mem_unmap_all(&board.mem2);
    mem_map_ram(&board.mem2, 1, 0x0000, 0x10000, ram);
    mem_map_rom(&board.mem2, 0, 0x1000, 0x1000, roms.ptr(rom_images::c64_char));
    mem_map_rom(&board.mem2, 0, 0x9000, 0x1000, roms.ptr(rom_images::c64_char));
}

//------------------------------------------------------------------------------
void
c64_t::update_memory_map() {
    this->io_mapped = false;
    uint8_t* ram = &(board.ram[0][0]);
    uint8_t* read_ptr;
    const uint8_t charen = (1<<2);
    const uint8_t hiram = (1<<1);
    const uint8_t loram = (1<<0);
    // shortcut if HIRAM and LORAM is 0, everything is RAM
    if ((this->cpu_port & (hiram|loram)) == 0) {
        mem_map_ram(&board.mem, 0, 0xA000, 0x6000, ram+0xA000);
    }
    else {
        // A000..BFFF is either RAM-behind-BASIC-ROM or RAM
        if ((this->cpu_port & (hiram|loram)) == (hiram|loram)) {
            read_ptr = roms.ptr(rom_images::c64_basic);
        }
        else {
            read_ptr = ram + 0xA000;
        }
        mem_map_rw(&board.mem, 0, 0xA000, 0x2000, read_ptr, ram+0xA000);

        // E000..FFFF is either RAM-behind-KERNAL-ROM or RAM
        if (this->cpu_port & hiram) {
            read_ptr = roms.ptr(rom_images::c64_kernalv3);
        }
        else {
            read_ptr = ram + 0xE000;
        }
        mem_map_rw(&board.mem, 0, 0xE000, 0x2000, read_ptr, ram+0xE000);

        // D000..DFFF can be Char-ROM or I/O
        if  (this->cpu_port & charen) {
            this->io_mapped = true;
        }
        else {
            mem_map_rw(&board.mem, 0, 0xD000, 0x1000, roms.ptr(rom_images::c64_char), ram+0xD000);
        }
    }
}

//------------------------------------------------------------------------------
void
c64_t::init_keymap() {
    /*
        http://sta.c64.org/cbm64kbdlay.html
        http://sta.c64.org/cbm64petkey.html
    */
    kbd_init(&board.kbd, 1);
    const char* keymap =
        // no shift
        "        "
        "3WA4ZSE "
        "5RD6CFTX"
        "7YG8BHUV"
        "9IJ0MKON"
        "+PL-.:@,"
        "~*;  = /"  // ~ is actually the British Pound sign
        "1  2  Q "

        // shift
        "        "
        "#wa$zse "
        "%rd&cftx"
        "'yg(bhuv"
        ")ij0mkon"
        " pl >[ <"
        "$ ]    ?"
        "!  \"  q ";
    YAKC_ASSERT(strlen(keymap) == 128);
    // shift is column 7, line 1
    kbd_register_modifier(&board.kbd, 0, 7, 1);
    // ctrl is column 2, line 7
    kbd_register_modifier(&board.kbd, 1, 2, 7);
    for (int shift = 0; shift < 2; shift++) {
        for (int col = 0; col < 8; col++) {
            for (int line = 0; line < 8; line++) {
                int c = keymap[shift*64 + line*8 + col];
                if (c != 0x20) {
                    kbd_register_key(&board.kbd, c, col, line, shift?(1<<0):0);
                }
            }
        }
    }

    // special keys
    kbd_register_key(&board.kbd, 0x20, 4, 7, 0);    // space
    kbd_register_key(&board.kbd, 0x08, 2, 0, 1);    // cursor left
    kbd_register_key(&board.kbd, 0x09, 2, 0, 0);    // cursor right
    kbd_register_key(&board.kbd, 0x0A, 7, 0, 0);    // cursor down
    kbd_register_key(&board.kbd, 0x0B, 7, 0, 1);    // cursor up
    kbd_register_key(&board.kbd, 0x01, 0, 0, 0);    // delete
    kbd_register_key(&board.kbd, 0x0C, 3, 6, 1);    // clear
    kbd_register_key(&board.kbd, 0x0D, 1, 0, 0);    // return
    kbd_register_key(&board.kbd, 0x03, 7, 7, 0);    // stop

    // function keys
    kbd_register_key(&board.kbd, 0xF1, 4, 0, 0);
    kbd_register_key(&board.kbd, 0xF2, 4, 0, 1);
    kbd_register_key(&board.kbd, 0xF3, 5, 0, 0);
    kbd_register_key(&board.kbd, 0xF4, 5, 0, 1);
    kbd_register_key(&board.kbd, 0xF5, 6, 0, 0);
    kbd_register_key(&board.kbd, 0xF6, 6, 0, 1);
    kbd_register_key(&board.kbd, 0xF7, 3, 0, 0);
    kbd_register_key(&board.kbd, 0xF8, 3, 0, 1);

// FIXME!
//    kbd_register_key(&board.kbd, 0xF1, 5, 7, 1);    // F1 -> Shift + C= key
}

//------------------------------------------------------------------------------
void
c64_t::on_ascii(uint8_t ascii) {
    kbd_key_down(&board.kbd, ascii);
    kbd_key_up(&board.kbd, ascii);
}

//------------------------------------------------------------------------------
void
c64_t::on_key_down(uint8_t key) {
    kbd_key_down(&board.kbd, key);
}

//------------------------------------------------------------------------------
void
c64_t::on_key_up(uint8_t key) {
    kbd_key_up(&board.kbd, key);
}

//------------------------------------------------------------------------------
void
c64_t::on_joystick(uint8_t mask) {
    this->cia1_joy_bits = 0;
    if (mask & joystick::left) {
        this->cia1_joy_bits |= (1<<2);
    }
    if (mask & joystick::right) {
        this->cia1_joy_bits |= (1<<3);
    }
    if (mask & joystick::up) {
        this->cia1_joy_bits |= (1<<0);
    }
    if (mask & joystick::down) {
        this->cia1_joy_bits |= (1<<1);
    }
    if (mask & joystick::btn0) {
        this->cia1_joy_bits |= (1<<4);
    }
}

//------------------------------------------------------------------------------
const void*
c64_t::framebuffer(int& out_width, int &out_height) {
    m6567_display_size(&board.m6567, &out_width, &out_height);
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
c64_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer2.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
void
c64_t::on_tape_inserted() {
    // read the .TAP header
    tape.read(&this->tap_header, sizeof(this->tap_header));
    const char* sig = "C64-TAPE-RAW";
    if (0 != memcmp(&this->tap_header.signature, sig, 12)) {
        // FIXME: unknown file type
        return;
    }
    YAKC_ASSERT(1 == this->tap_header.version);
    // tape is valid, rewind counters
    this->tape_valid = true;
    this->tape_tick_count = 0;
    this->tape_byte_count = this->tap_header.size;
}

//------------------------------------------------------------------------------
bool
c64_t::tape_tick() {
    if (this->tape_valid && tape.motor_on && (this->tape_byte_count >= 0)) {

        /* REMOVE ME: this is special debug dumping code for the
            Boulderdash fast loader
        */
        /*
        static uint16_t old_addr = 0;
        uint16_t new_addr = mem_rd16(&board.mem, 0xC1);
        if (new_addr != old_addr) {
            const uint8_t* ram = &(board.ram[0][0]);
            printf("%04X: %02X\n", old_addr, ram[0xBD]);
            old_addr = new_addr;
        }
        */

        // toggle the datasette beeper output
        if (beeper_tick(&board.beeper_1)) {
            board.audiobuffer2.write(board.beeper_1.sample);
        }

        // handle the actual datasette pulses
        if (this->tape_tick_count == 0) {
            if (tape.eof()) {
                return false;
            }

            // toggle the datasette beeper output (not sure if frequency must be doubled)
            beeper_toggle(&board.beeper_1);

            uint8_t val = 0;
            tape.read(&val, 1);
            if (val == 0) {
                uint8_t bytes[3];
                tape.read(&bytes, sizeof(bytes));
                this->tape_tick_count = bytes[2]<<16 | bytes[1]<<8 | bytes[0];
                this->tape_byte_count -= 4;
            }
            else {
                this->tape_tick_count = val * 8;
                this->tape_byte_count--;
            }
            return true;
        }
        else {
            this->tape_tick_count--;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool
c64_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }
    uint16_t start_addr;
    // start address
    fs->read(fp, &start_addr, sizeof(start_addr));
    uint16_t end_addr = start_addr + (fs->size(fp) - 2);
    uint16_t addr = start_addr;
    while (addr < end_addr) {
        uint8_t byte;
        fs->read(fp, &byte, 1);
        mem_wr(&board.mem, addr++, byte);
    }
    fs->close(fp);
    fs->rm(name);
    return true;
}

//------------------------------------------------------------------------------
const char*
c64_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
