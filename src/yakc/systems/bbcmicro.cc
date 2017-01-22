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
bbcmicro::check_roms(const rom_images& roms, device model, os_rom os) {
    if (device::bbcmicro_b == model) {
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
uint8_t
bbcmicro::memio(bool write, uint16_t addr, uint8_t inval) {
    if (addr >= 0xFF00) {
        if (!write) {
            return self->roms->ptr(rom_images::bbcmicro_b_os)[addr - 0xC000];
        }
        return 0x00;
    }
    else {
        // FIXME: memory-mapped-io
        return 0x00;
    }
}

//------------------------------------------------------------------------------
void
bbcmicro::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_bbcmicro) & int(m));
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
}

//------------------------------------------------------------------------------
uint64_t
bbcmicro::step(uint64_t start_tick, uint64_t end_tick) {
    auto& cpu = this->board->mos6502;
    auto& dbg = this->board->dbg;
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
const void*
bbcmicro::framebuffer(int& out_width, int& out_height) {
    out_width = 320;
    out_height = 256;
    return this->rgba8_buffer;
}

//------------------------------------------------------------------------------
const char*
bbcmicro::system_info() const {
    return "FIXME!";
}

} // namespace YAKC