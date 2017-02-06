//------------------------------------------------------------------------------
//  bbcmicro.cc
//------------------------------------------------------------------------------
#include "bbcmicro.h"

namespace YAKC {

bbcmicro* bbcmicro::self = nullptr;

//------------------------------------------------------------------------------
void
bbcmicro::init(breadboard* b, rom_images* r) {
    YAKC_ASSERT(b && r);
    self = this;
    this->board = b;
    this->roms = r;
}

//------------------------------------------------------------------------------
bool
bbcmicro::check_roms(const rom_images& roms, system model, os_rom os) {
    if (system::bbcmicro_b == model) {
        return roms.has(rom_images::bbcmicro_b_os) && roms.has(rom_images::bbcmicro_b_basic);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
bbcmicro::on_context_switched() {
    // FIXME
}

//------------------------------------------------------------------------------
void
bbcmicro::init_memory_map() {
    auto& cpu = this->board->mos6502;
    cpu.mem.unmap_all();
    YAKC_ASSERT(check_roms(*this->roms, this->cur_model, os_rom::none));
    cpu.mem.map(0, 0x0000, 0x8000, this->board->ram[0], true);
    cpu.mem.map(1, 0x8000, 0x4000, this->roms->ptr(rom_images::bbcmicro_b_basic), false);
    cpu.mem.map(1, 0xC000, 0x4000, this->roms->ptr(rom_images::bbcmicro_b_os), false);
    cpu.mem.map_io(0, 0xFC00, 0x0400, memio);

}

//------------------------------------------------------------------------------
void
bbcmicro::poweron(system m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;
    
    // map memory
    clear(this->board->ram, sizeof(this->board->ram));
    this->init_memory_map();

    // initialize clock to 2 MHz
    this->board->clck.init(2000);

    // CPU start state
    this->board->mos6502.init(this);
    this->board->mos6502.reset();

    // hardware subsystems
    this->video.init(this->board);
}

//------------------------------------------------------------------------------
void
bbcmicro::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->mos6502.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
void
bbcmicro::reset() {
    this->board->mos6502.reset();
    this->video.reset();
}

//------------------------------------------------------------------------------
uint64_t
bbcmicro::step(uint64_t start_tick, uint64_t end_tick) {
    auto& cpu = this->board->mos6502;
    auto& dbg = this->board->dbg;
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        uint32_t ticks = cpu.step();
        if (dbg.step(cpu.PC, ticks)) {
            return end_tick;
        }
        cur_tick += ticks;
    }
    return cur_tick;
}

//------------------------------------------------------------------------------
uint32_t
bbcmicro::step_debug() {
    auto& cpu = board->mos6502;
    uint32_t ticks = cpu.step();
    board->dbg.step(cpu.PC, ticks);
    return ticks;
}

//------------------------------------------------------------------------------
void
bbcmicro::cpu_tick() {
    this->video.step();
}

//------------------------------------------------------------------------------
uint8_t
bbcmicro::memio(bool write, uint16_t addr, uint8_t inval) {
    if (write) {
        if (addr >= 0xFF00) {
            // the last 256 bytes of ROM
            return 0x00;
        }
        else if (addr >= 0xFE00) {
            // SHEILA write
            if (addr < 0xFE08) {
                // CRTC write
                if ((addr & 0x0001) == 0) {
                    self->board->mc6845.select(inval);
                }
                else {
                    self->board->mc6845.write(inval);
                }
            }
            else if ((addr >= 0xFE20) && (addr < 0xFE30)) {
                // ULA
                if ((addr & 0x0001) == 0) {
                    self->video.write_video_control(inval);
                }
                else {
                    self->video.write_palette(inval);
                }
            }
        }
        else if (addr >= 0xFD00) {

        }
        else if (addr >= 0xFC00) {

        }
    }
    else {
        if (addr >= 0xFF00) {
            // the last 256 bytes of ROM
            return self->roms->ptr(rom_images::bbcmicro_b_os)[addr - 0xC000];
        }
        else if (addr >= 0xFE00) {
            // SHEILA read
            if (addr < 0xFE08) {
                // CRTC read
                if ((addr & 0x0001) == 1) {
                    return self->board->mc6845.read();
                }
            }
            else if ((addr >= 0xFE40) && (addr < 0xFE60)) {
                // VIA
                return 0x00;
            }
            else if (addr >= 0xFEE0) {
                // Tube ULA
                return 0x00;
            }

            //printf("read: %04X\n", addr);
            return 0xFF;
        }
        else if (addr >= 0xFD00) {
            // JIM (1 MHz) read
            return 0xFF;
        }
        else if (addr >= 0xFC00) {
            // FRED (1 MHz) read
            return 0xFF;
        }
    }
    // fallthrough
    return 0x00;
}

//------------------------------------------------------------------------------
const void*
bbcmicro::framebuffer(int& out_width, int& out_height) {
    out_width = bbcmicro_video::display_width;
    out_height = bbcmicro_video::display_height;
    return this->video.rgba8_buffer;
}

//------------------------------------------------------------------------------
const char*
bbcmicro::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
