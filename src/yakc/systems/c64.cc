//------------------------------------------------------------------------------
//  c64.cc
//------------------------------------------------------------------------------
#include "c64.h"

#include <stdio.h>

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
    this->io_mapped = true;

    // setup initial memory map
    this->init_memory_map();

    board.freq_hz = (m == system::c64_pal) ? 985248 : 1022727;
    m6502_desc_t cpu_desc = { };
    cpu_desc.tick_cb = cpu_tick;
    cpu_desc.in_cb = cpu_port_in;
    cpu_desc.out_cb = cpu_port_out;
    m6502_init(&board.m6502, &cpu_desc);
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
    this->io_mapped = true;
    this->update_memory_map();
    m6502_reset(&board.m6502);
}

//------------------------------------------------------------------------------
uint64_t
c64_t::exec(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = m6502_exec(&board.m6502, num_ticks);
    kbd_update(&board.kbd);
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint64_t
c64_t::cpu_tick(uint64_t pins) {
    const uint16_t addr = M6502_GET_ADDR(pins);

    // CPU port I/O?
    if (M6510_CHECK_IO(pins)) {
        pins = m6510_iorq(&board.m6502, pins);
    }
    else {
        // check for I/O range at 0xD000..0xDFFF
        if (c64.io_mapped && ((addr & 0xF000) == 0xD000)) {
            // FIXME: perform memory-mapped IO
            if (pins & M6502_RW) {
                printf("IO READ: %04X\n", addr);
            }
            else {
                printf("IO WRITE: %04X=%02X\n", addr, M6502_GET_DATA(pins));
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
    }
    return pins;
}

//------------------------------------------------------------------------------
uint8_t
c64_t::cpu_port_in() {
    // FIXME
    return 0xFF;
}

//------------------------------------------------------------------------------
void
c64_t::cpu_port_out(uint8_t data) {
    /*
        Output to CPU port:

        bits 0..2:  memory configuration

        bit 3: datasette output signal level
        bit 4: datasette button status (1: no button pressed)
        bit 5: datasette motor control (1: motor off)
    */
    c64.cpu_port = data;
    c64.update_memory_map();
}

//------------------------------------------------------------------------------
void
c64_t::init_memory_map() {
    YAKC_ASSERT(roms.size(rom_images::c64_basic) == 0x2000);
    YAKC_ASSERT(roms.size(rom_images::c64_char) == 0x1000);
    YAKC_ASSERT(roms.size(rom_images::c64_kernalv3) == 0x2000);
    clear(board.ram, sizeof(board.ram));
    mem_unmap_all(&board.mem);
    uint8_t* ram = &(board.ram[0][0]);
    // 0000..9FFF and C000.CFFF is always RAM
    mem_map_ram(&board.mem, 0, 0x0000, 0xA000, ram);
    mem_map_ram(&board.mem, 0, 0xC000, 0x1000, ram+0xC000);
    // A000..BFFF, D000..DFFF and E000..FFFF are configurable
    this->update_memory_map();
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
    // FIXME
}

//------------------------------------------------------------------------------
const void*
c64_t::framebuffer(int& out_width, int &out_height) {
    // FIXME
    out_width = 320;
    out_height = 200;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
c64_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
const char*
c64_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
