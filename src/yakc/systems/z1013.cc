//------------------------------------------------------------------------------
//  z1013.cc
//------------------------------------------------------------------------------
#include "z1013.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

z1013* z1013::ptr = nullptr;

//------------------------------------------------------------------------------
z1013::z1013() {
    YAKC_ASSERT(!ptr);
    ptr = this;
}

//------------------------------------------------------------------------------
z1013::~z1013() {
    YAKC_ASSERT(ptr);
    ptr = nullptr;
}

//------------------------------------------------------------------------------
void
z1013::init(rom_images* r) {
    this->roms = r;
    this->rgba8_buffer = board.rgba8_buffer;
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
    memory* m = &board.mem;
    mem_unmap_all(m);
    if (system::z1013_64 == this->cur_model) {
        // 64 kByte RAM
        mem_map_ram(m, 1, 0x0000, 0x10000, board.ram[0]);
    }
    else {
        // 16 kByte RAM
        mem_map_ram(m, 1, 0x0000, 0x4000, board.ram[0]);
    }
    // 1 kByte video memory
    mem_map_ram(m, 0, 0xEC00, 0x0400, board.ram[vidmem_page]);
    // 2 kByte system rom
    if (os_rom::z1013_mon202 == this->cur_os) {
        mem_map_rom(m, 0, 0xF000, this->roms->size(rom_images::z1013_mon202), this->roms->ptr(rom_images::z1013_mon202));
    }
    else {
        mem_map_rom(m, 0, 0xF000, this->roms->size(rom_images::z1013_mon_a2), this->roms->ptr(rom_images::z1013_mon_a2));
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
    YAKC_ASSERT(int(system::any_z1013) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    if (m == system::z1013_01) {
        this->cur_os = os_rom::z1013_mon202;
    }
    else {
        this->cur_os = os_rom::z1013_mon_a2;
    }
    board.init_kbd(2);
    this->init_keymaps();
    this->on = true;
    this->kbd_request_column = 0;
    this->kbd_request_line_hilo = false;

    // map memory
    clear(board.ram, sizeof(board.ram));
    mem_init(&board.mem);
    this->init_memory_mapping();

    // initialize the clock, the z1013_01 runs at 1MHz, all others at 2MHz
    board.clck.init((m == system::z1013_01) ? 1000 : 2000);

    // initialize hardware components
    board.init_z80(cpu_tick);
    board.init_pio(0, pio_in, pio_out);

    // execution on power-on starts at 0xF000
    board.z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z1013::poweroff() {
    YAKC_ASSERT(this->on);
    mem_unmap_all(&board.mem);
    this->on = false;
}

//------------------------------------------------------------------------------
void
z1013::reset() {
    z80pio_reset(&board.z80pio);
    z80_reset(&board.z80);
    this->kbd_request_column = 0;
    // execution after reset starts at 0x0000(??? -> doesn't work)
    board.z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
uint64_t
z1013::step(uint64_t start_tick, uint64_t end_tick) {
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = z80_exec(&board.z80, num_ticks);
    kbd_update(&board.kbd);
    this->decode_video();
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint32_t
z1013::step_debug() {
return 0;
/* FIME
    auto& cpu = this->board->z80;
    auto& dbg = this->board->dbg;
    auto& clk = this->board->clck;
    uint64_t all_ticks = 0;
    uint16_t old_pc;
    do {
        old_pc = cpu.PC;
        uint32_t ticks = cpu.handle_irq(this);
        if (0 == ticks) {
            ticks = cpu.step(this);
        }
        clk.step(this, ticks);
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
z1013::cpu_tick(int num_ticks, uint64_t pins) {
    if (pins & Z80_MREQ) {
        // a memory request
        const uint16_t addr = Z80_ADDR(pins);
        if (pins & Z80_RD) {
            // a memory read
            const uint8_t data = mem_rd(&board.mem, addr);
            Z80_SET_DATA(pins, data);
        }
        else if (pins & Z80_WR) {
            // a memory write
            mem_wr(&board.mem, addr, Z80_DATA(pins));
        }
    }
    else if (pins & Z80_IORQ) {
        /*
            The PIO Chip-Enable pin (Z80PIO_CE) is connected to output pin 0 of
            a MH7442 BCD-to-Decimal decoder (looks like this is a Czech
            clone of a SN7442). The lower 3 input pins of the MH7442
            are connected to address bus pins A2..A4, and the 4th input
            pin is connected to IORQ. This means the PIO is enabled when
            the CPU IORQ pin is low (active), and address bus bits 2..4 are
            off. This puts the PIO at the lowest 4 addresses of an 32-entry
            address space (so the PIO should be visible at port number
            0..4, but also at 32..35, 64..67 and so on).

            The PIO Control/Data select pin (Z80PIO_CDSEL) is connected to
            address bus pin A0. This means even addresses select a PIO data
            operation, and odd addresses select a PIO control operation.

            The PIO port A/B select pin (Z80PIO_BASEL) is connected to address
            bus pin A1. This means the lower 2 port numbers address the PIO
            port A, and the upper 2 port numbers address the PIO port B.

            The keyboard matrix columns are connected to another MH7442
            BCD-to-Decimal converter, this converts a hardware latch at port
            address 8 which stores a keyboard matrix column number from the CPU
            to the column lines. The operating system scans the keyboard by
            writing the numbers 0..7 to this latch, which is then converted
            by the MH7442 to light up the keyboard matrix column lines
            in that order. Next the CPU reads back the keyboard matrix lines
            in 2 steps of 4 bits each from PIO port B.
        */
        if ((pins & (Z80_A4|Z80_A3|Z80_A2)) == 0) {
            /* address bits A2..A4 are zero, this activates the PIO chip-select pin */
            uint64_t pio_pins = (pins & Z80_PIN_MASK) | Z80PIO_CE;
            /* address bit 0 selects data/ctrl */
            if (pio_pins & (1<<0)) pio_pins |= Z80PIO_CDSEL;
            /* address bit 1 selects port A/B */
            if (pio_pins & (1<<1)) pio_pins |= Z80PIO_BASEL;
            pins = z80pio_iorq(&board.z80pio, pio_pins) & Z80_PIN_MASK;
        }
        else if ((pins & (Z80_A3|Z80_WR)) == (Z80_A3|Z80_WR)) {
            /* port 8 is connected to a hardware latch to store the
               requested keyboard column for the next keyboard scan
            */
            z1013::ptr->kbd_request_column = Z80_DATA(pins);
        }
    }
    /* there are no interrupts happening in a vanilla Z1013,
       so don't trigger the interrupt daisy chain
    */
    return pins;
}

//------------------------------------------------------------------------------
void
z1013::pio_out(int port_id, uint8_t val) {
    z1013* self = z1013::ptr;
    if (Z80PIO_PORT_B == port_id) {
        // for z1013a2, bit 4 is for monitor A.2 with 8x8 keyboard
        // and selects the upper/lower 4 keyboard matrix lines
        self->kbd_request_line_hilo = 0 != (val & (1<<4));
        // FIXME: bit 7 is for cassette output
    }
}

//------------------------------------------------------------------------------
uint8_t
z1013::pio_in(int port_id) {
    z1013* self = z1013::ptr;
    if (Z80PIO_PORT_A == port_id) {
        // nothing to return here, PIO-A is for user devices
        return 0xFF;
    }
    else {
        // FIXME: handle bit 7 for cassette input
        // read keyboard matrix state into lower 4 bits
        uint8_t data = 0;
        if (system::z1013_01 == self->cur_model) {
            uint16_t column_mask = (1<<(self->kbd_request_column & 7));
            uint16_t line_mask = kbd_test_lines(&board.kbd, column_mask);
            data = 0xF & ~(line_mask & 0xF);
        }
        else {
            uint16_t column_mask = (1<<self->kbd_request_column);
            uint16_t line_mask = kbd_test_lines(&board.kbd, column_mask);
            if (self->kbd_request_line_hilo) {
                line_mask >>= 4;
            }
            data = 0xF & ~(line_mask & 0xF);
        }
        return data;
    }
}

//------------------------------------------------------------------------------
void
z1013::put_key(uint8_t ascii) {
    if (ascii) {
        kbd_key_down(&board.kbd, ascii);
        kbd_key_up(&board.kbd, ascii);
    }
}

//------------------------------------------------------------------------------
void
z1013::init_keymap_8x4() {
/*
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
                uint8_t c = layers_8x4[shift*32 + line*8 + col];
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
*/
}

//------------------------------------------------------------------------------
void
z1013::init_keymap_8x8() {
/*
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
                uint8_t c = layers_8x8[shift*64 + line*8 + col];
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
*/
}

//------------------------------------------------------------------------------
void
z1013::decode_video() {
    uint32_t* dst = rgba8_buffer;
    const uint8_t* src = board.ram[vidmem_page];
    const uint8_t* font = this->roms->ptr(rom_images::z1013_font);
    for (int y = 0; y < 32; y++) {
        for (int py = 0; py < 8; py++) {
            for (int x = 0; x < 32; x++) {
                uint8_t chr = src[(y<<5) + x];
                uint8_t bits = font[(chr<<3)|py];
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
        while (addr < end_addr) {
            static const int buf_size = 1024;
            uint8_t buf[buf_size];
            fs->read(fp, buf, buf_size);
            for (int i = 0; (i < buf_size) && (addr < end_addr); i++) {
                mem_wr(&board.mem, addr++, buf[i]);
            }
        }
    }
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }

    if (start) {
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
