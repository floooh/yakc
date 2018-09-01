//------------------------------------------------------------------------------
//  z1013.cc
//
//  FIXME: 8x4 keyboard input currently broken!
//------------------------------------------------------------------------------
#include "z1013.h"

YAKC::z1013_t YAKC::z1013;

namespace YAKC {

//------------------------------------------------------------------------------
bool
z1013_t::check_roms(system model) {
    if (system::z1013_01 == model) {
        return roms.has(rom_images::z1013_mon202) && roms.has(rom_images::z1013_font);
    }
    else {
        return roms.has(rom_images::z1013_mon_a2) && roms.has(rom_images::z1013_font);
    }
}

//------------------------------------------------------------------------------
void
z1013_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_z1013) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;

    z1013_desc_t desc = {};
    switch (m) {
        case system::z1013_01:
            desc.type = Z1013_TYPE_01;
            break;
        case system::z1013_16:
            desc.type = Z1013_TYPE_16;
            break;
        default:
            desc.type = Z1013_TYPE_64;
            break;
    }
    desc.pixel_buffer = board.rgba8_buffer;
    desc.pixel_buffer_size = sizeof(board.rgba8_buffer);
    if (system::z1013_01 == m) {
        desc.rom_mon202 = roms.ptr(rom_images::z1013_mon202);
        desc.rom_mon202_size = roms.size(rom_images::z1013_mon202);
    }
    else {
        desc.rom_mon_a2 = roms.ptr(rom_images::z1013_mon_a2);
        desc.rom_mon_a2_size = roms.size(rom_images::z1013_mon_a2);
    }
    desc.rom_font = roms.ptr(rom_images::z1013_font);
    desc.rom_font_size = roms.size(rom_images::z1013_font);
    z1013_init(&sys, &desc);

    board.z80 = &sys.cpu;
    board.z80pio_1 = &sys.pio;
    board.kbd = &sys.kbd;
    board.mem = &sys.mem;
}

//------------------------------------------------------------------------------
void
z1013_t::poweroff() {
    YAKC_ASSERT(this->on);
    this->on = false;
    z1013_discard(&sys);
    board.clear();
}

//------------------------------------------------------------------------------
void
z1013_t::reset() {
    YAKC_ASSERT(this->on);
    z1013_reset(&sys);
}

//------------------------------------------------------------------------------
void
z1013_t::exec(uint32_t micro_seconds) {
    YAKC_ASSERT(this->on);
    z1013_exec(&sys, micro_seconds);
}

//------------------------------------------------------------------------------
void
z1013_t::on_ascii(uint8_t ascii) {
    z1013_key_down(&sys, ascii);
    z1013_key_up(&sys, ascii);
}

//------------------------------------------------------------------------------
void
z1013_t::on_key_down(uint8_t key) {
    z1013_key_down(&sys, key);
}

//------------------------------------------------------------------------------
void
z1013_t::on_key_up(uint8_t key) {
    z1013_key_up(&sys, key);
}

//------------------------------------------------------------------------------
const void*
z1013_t::framebuffer(int& out_width, int& out_height) {
    out_width = Z1013_DISPLAY_WIDTH;
    out_height = Z1013_DISPLAY_HEIGHT;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
z1013_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    /* FIXME
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
                mem_wr(&mem, addr++, buf[i]);
            }
        }
    }
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }

    if (start) {
        auto* cpu = &z80;
        z80_set_a(cpu, 0x00);
        z80_set_f(cpu, 0x10);
        z80_set_bc(cpu, 0x0000); z80_set_bc_(cpu, 0x0000);
        z80_set_de(cpu, 0x0000); z80_set_de_(cpu, 0x0000);
        z80_set_hl(cpu, 0x0000); z80_set_hl_(cpu, 0x0000);
        z80_set_af_(cpu, 0x0000);
        z80_set_pc(cpu, exec_addr);
    }
    */
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
