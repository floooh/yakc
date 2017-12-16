//------------------------------------------------------------------------------
//  z1013.cc
//------------------------------------------------------------------------------
#include "z1013.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

z1013_t z1013;

//------------------------------------------------------------------------------
void
z1013_t::init() {
    // empty
}

//------------------------------------------------------------------------------
bool
z1013_t::check_roms(system model, os_rom os) {
    if (system::z1013_01 == model) {
        return roms.has(rom_images::z1013_mon202) && roms.has(rom_images::z1013_font);
    }
    else {
        return roms.has(rom_images::z1013_mon_a2) && roms.has(rom_images::z1013_font);
    }
}

//------------------------------------------------------------------------------
void
z1013_t::init_memory_mapping() {
    mem_unmap_all(&board.mem);
    if (system::z1013_64 == this->cur_model) {
        // 64 kByte RAM
        mem_map_ram(&board.mem, 1, 0x0000, 0x10000, board.ram[0]);
    }
    else {
        // 16 kByte RAM
        mem_map_ram(&board.mem, 1, 0x0000, 0x4000, board.ram[0]);
    }
    // 1 kByte video memory
    mem_map_ram(&board.mem, 0, 0xEC00, 0x0400, board.ram[vidmem_page]);
    // 2 kByte system rom
    if (os_rom::z1013_mon202 == this->cur_os) {
        mem_map_rom(&board.mem, 0, 0xF000, roms.size(rom_images::z1013_mon202), roms.ptr(rom_images::z1013_mon202));
    }
    else {
        mem_map_rom(&board.mem, 0, 0xF000, roms.size(rom_images::z1013_mon_a2), roms.ptr(rom_images::z1013_mon_a2));
    }
}

//------------------------------------------------------------------------------
void
z1013_t::init_keymaps() {
    if (this->cur_model == system::z1013_01) {
        this->init_keymap_8x4();
    }
    else {
        this->init_keymap_8x8();
    }
}

//------------------------------------------------------------------------------
void
z1013_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_z1013) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    if (m == system::z1013_01) {
        this->cur_os = os_rom::z1013_mon202;
    }
    else {
        this->cur_os = os_rom::z1013_mon_a2;
    }
    this->on = true;
    kbd_init(&board.kbd, 2);
    this->init_keymaps();
    this->kbd_request_column = 0;
    this->kbd_request_line_hilo = false;

    // map memory
    clear(board.ram, sizeof(board.ram));
    mem_init(&board.mem);
    this->init_memory_mapping();

    // initialize the clock, the z1013_01 runs at 1MHz, all others at 2MHz
    board.freq_khz = (m == system::z1013_01) ? 1000 : 2000;

    // initialize hardware components
    z80_init(&board.z80, cpu_tick);
    z80pio_init(&board.z80pio, pio_in, pio_out);

    // execution on power-on starts at 0xF000
    board.z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z1013_t::poweroff() {
    YAKC_ASSERT(this->on);
    mem_unmap_all(&board.mem);
    this->on = false;
}

//------------------------------------------------------------------------------
void
z1013_t::reset() {
    z80pio_reset(&board.z80pio);
    z80_reset(&board.z80);
    z80pio_reset(&board.z80pio);
    this->kbd_request_column = 0;
    // execution after reset starts at 0x0000(??? -> doesn't work)
    board.z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
uint64_t
z1013_t::step(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = z80_exec(&board.z80, num_ticks);
    kbd_update(&board.kbd);
    this->decode_video();
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint32_t
z1013_t::step_debug() {
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
z1013_t::cpu_tick(int num_ticks, uint64_t pins) {
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
            z1013.kbd_request_column = Z80_DATA(pins);
        }
    }
    /* there are no interrupts happening in a vanilla Z1013,
       so don't trigger the interrupt daisy chain
    */
    return pins;
}

//------------------------------------------------------------------------------
void
z1013_t::pio_out(int port_id, uint8_t val) {
    if (Z80PIO_PORT_B == port_id) {
        // for z1013a2, bit 4 is for monitor A.2 with 8x8 keyboard
        // and selects the upper/lower 4 keyboard matrix lines
        z1013.kbd_request_line_hilo = 0 != (val & (1<<4));
        // FIXME: bit 7 is for cassette output
    }
}

//------------------------------------------------------------------------------
uint8_t
z1013_t::pio_in(int port_id) {
    if (Z80PIO_PORT_A == port_id) {
        // nothing to return here, PIO-A is for user devices
        return 0xFF;
    }
    else {
        // FIXME: handle bit 7 for cassette input
        // read keyboard matrix state into lower 4 bits
        uint8_t data = 0;
        if (system::z1013_01 == z1013.cur_model) {
            uint16_t column_mask = (1<<(z1013.kbd_request_column & 7));
            uint16_t line_mask = kbd_test_lines(&board.kbd, column_mask);
            data = 0xF & ~(line_mask & 0xF);
        }
        else {
            uint16_t column_mask = (1<<z1013.kbd_request_column);
            uint16_t line_mask = kbd_test_lines(&board.kbd, column_mask);
            if (z1013.kbd_request_line_hilo) {
                line_mask >>= 4;
            }
            data = 0xF & ~(line_mask & 0xF);
        }
        return data;
    }
}

//------------------------------------------------------------------------------
void
z1013_t::put_key(uint8_t ascii) {
    if (ascii) {
        kbd_key_down(&board.kbd, ascii);
        kbd_key_up(&board.kbd, ascii);
    }
}

//------------------------------------------------------------------------------
void
z1013_t::init_keymap_8x4() {
    // keyboard layers for the 4x4 keyboard (no shift key, plus the 4 shift keys)
    // use space as special placeholder
    const char* layers_8x4 =
        "@ABCDEFG"  "HIJKLMNO"  "PQRSTUVW"  "        "
        // Shift 1:
        "XYZ[\\]^-" "01234567"  "89:;<=>?"  "        "
        // Shift 2:
        "   {|}~ "  " !\"#$%&'" "()*+,-./"  "        "
        // Shift 3:
        " abcdefg"  "hijklmno"  "pqrstuvw"  "        "
        // Shift 4:
        "xyz     "  "        "  "        "  "        ";

    // 4 shift keys are on column 0/1/2/3, line 3
    kbd_register_modifier(&board.kbd, 0, 0, 3);
    kbd_register_modifier(&board.kbd, 1, 0, 3);
    kbd_register_modifier(&board.kbd, 2, 0, 3);
    kbd_register_modifier(&board.kbd, 3, 0, 3);
    for (int layer = 0; layer < 5; layer++) {
        for (int line = 0; line < 4; line++) {
            for (int col = 0; col < 8; col++) {
                uint8_t c = layers_8x4[layer*32 + line*8 + col];
                if (c != 0x20) {
                    kbd_register_key(&board.kbd, c, col, line, (layer>0) ? (1<<(layer-1)) : 0);
                }
            }
        }
    }

    // special keys
    kbd_register_key(&board.kbd, ' ', 5, 3, 0);   // Space
    kbd_register_key(&board.kbd, 0x08, 4, 3, 0);  // Cursor Left
    kbd_register_key(&board.kbd, 0x09, 6, 3, 0);  // Cursor Right
    kbd_register_key(&board.kbd, 0x0D, 7, 3, 0);  // Enter
    kbd_register_key(&board.kbd, 0x03, 3, 1, 4);  // Break/Escape
}

//------------------------------------------------------------------------------
void
z1013_t::init_keymap_8x8() {
    // see: http://www.z1013.de/images/21.gif
    /* shift key is column 7, line 6 */
    const int shift = 0, shift_mask = (1<<shift);
    kbd_register_modifier(&board.kbd, shift, 7, 6);
    /* ctrl key is column 6, line 5 */
    const int ctrl = 1, ctrl_mask = (1<<ctrl);
    kbd_register_modifier(&board.kbd, ctrl, 6, 5);
    /* alpha-numeric keys */
    const char* keymap =
        /* unshifted keys */
        "13579-  QETUO@  ADGJL*  YCBM.^  24680[  WRZIP]  SFHK+\\  XVN,/_  "
        /* shift layer */
        "!#%')=  qetuo`  adgjl:  ycbm>~  \"$&( {  wrzip}  sfhk;|  xvn<?   ";
    for (int layer = 0; layer < 2; layer++) {
        for (int line = 0; line < 8; line++) {
            for (int col = 0; col < 8; col++) {
                int c = keymap[layer*64 + line*8 + col];
                if (c != 0x20) {
                    kbd_register_key(&board.kbd, c, col, line, layer?shift_mask:0);
                }
            }
        }
    }
    /* special keys */
    kbd_register_key(&board.kbd, ' ',  6, 4, 0);  /* space */
    kbd_register_key(&board.kbd, 0x08, 6, 2, 0);  /* cursor left */
    kbd_register_key(&board.kbd, 0x09, 6, 3, 0);  /* cursor right */
    kbd_register_key(&board.kbd, 0x0A, 6, 7, 0);  /* cursor down */
    kbd_register_key(&board.kbd, 0x0B, 6, 6, 0);  /* cursor up */
    kbd_register_key(&board.kbd, 0x0D, 6, 1, 0);  /* enter */
    kbd_register_key(&board.kbd, 0x03, 1, 3, ctrl_mask); /* map Esc to Ctrl+C (STOP/BREAK) */
}

//------------------------------------------------------------------------------
void
z1013_t::decode_video() {
    uint32_t* dst = board.rgba8_buffer;
    const uint8_t* src = board.ram[vidmem_page];
    const uint8_t* font = roms.ptr(rom_images::z1013_font);
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
z1013_t::framebuffer(int& out_width, int& out_height) {
    out_width = display_width;
    out_height = display_height;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
z1013_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
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
z1013_t::system_info() const {
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
