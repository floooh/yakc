//------------------------------------------------------------------------------
//  cpc.cc
//------------------------------------------------------------------------------
#include "cpc.h"
#include "roms/roms.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
cpc::init(breadboard* b) {
    this->board = b;

    // FIXME: init palette to some test colors
    this->pal[0] = 0xFF000000;
    this->pal[1] = 0xFFFF0000;
    this->pal[2] = 0xFF00FF00;
    this->pal[3] = 0xFF0000FF;
    this->pal[4] = 0xFFFFFF00;
    this->pal[5] = 0xFFFF00FF;
    this->pal[6] = 0xFF00FFFF;
    this->pal[7] = 0xFFFF00FF;
    this->pal[8] = 0xFFFFFFFF;
    this->pal[9] = 0xFFFF0000;
    this->pal[10] = 0xFF007F00;
    this->pal[11] = 0xFF0000FF;
    this->pal[12] = 0xFF7F7F00;
    this->pal[13] = 0xFF7F007F;
    this->pal[14] = 0xFF007F7F;
    this->pal[15] = 0xFF7F007F;
}

//------------------------------------------------------------------------------
void
cpc::init_memory_map() {
    z80& cpu = this->board->cpu;
    cpu.mem.unmap_all();

    if (device::cpc464 == this->cur_model) {
        // map 32 KByte RAM from 4000 to BFFF
        cpu.mem.map(0, 0x4000, 0x4000, this->ram[1], true);
        cpu.mem.map(0, 0x8000, 0x4000, this->ram[2], true);

        // lower ROM (OS), writes always go to RAM bank 0
        YAKC_ASSERT(sizeof(dump_cpc464_os) == 0x4000);
        cpu.mem.map_rw(0, 0x0000, 0x4000, dump_cpc464_os, this->ram[0]);

        // upper ROM (BASIC), writes always go to RAM bank 3
        YAKC_ASSERT(sizeof(dump_cpc464_basic) == 0x4000);
        cpu.mem.map_rw(0, 0xC000, 0x4000, dump_cpc464_basic, this->ram[3]);
    }
    else if (device::cpc6128 == this->cur_model) {
        // FIXME
    }
}

//------------------------------------------------------------------------------
void
cpc::on_context_switched() {
    // FIXME!
}

//------------------------------------------------------------------------------
void
cpc::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_cpc) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;

    // map memory
    clear(this->ram, sizeof(this->ram));
    this->init_memory_map();

    // initialize clock and timers
    this->board->clck.init(4000);

    // CPU start state
    this->board->cpu.init();
    this->board->cpu.PC = 0x0000;
}

//------------------------------------------------------------------------------
void
cpc::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
void
cpc::reset() {
    this->board->cpu.reset();
    this->board->cpu.PC = 0x0000;
    this->init_memory_map();
}

//------------------------------------------------------------------------------
uint64_t
cpc::step(uint64_t start_tick, uint64_t end_tick) {
    // step the system for given number of cycles, return actually
    // executed number of cycles
    z80& cpu = this->board->cpu;
    z80dbg& dbg = this->board->dbg;
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        if (dbg.check_break(cpu)) {
            dbg.paused = true;
            return end_tick;
        }
        dbg.store_pc_history(cpu); // FIXME: only if debug window open?
        int ticks_step = cpu.step(this);
        // need to round up ticks to 4, this is a CPC specialty
        ticks_step += cpu.handle_irq();
        ticks_step = (ticks_step + 3) & ~3;
        this->board->clck.update(this, ticks_step);
        cur_tick += ticks_step;
    }
    this->decode_video_memory();
    return cur_tick;
}

//------------------------------------------------------------------------------
void
cpc::cpu_out(uword port, ubyte val) {
    YAKC_ASSERT(device::cpc464 == this->cur_model);
    if ((port & 0xFF00) == 0x7F00) {
        if ((val & 0xC0) == 0x80) {
            //
            // Register 2:
            //
            // - bits 0 and 1 select the screen mode
            //      00: Mode 0 (160x200 @ 16 colors)
            //      01: Mode 1 (320x200 @ 4 colors)
            //      02: Mode 2 (640x200 @ 2 colors)
            //      11: Mode 3 (160x200 @ 2 colors, undocumented)
            //
            //  bit 2: disable/enable lower ROM
            //  bit 3: disable/enable upper ROM
            //
            //  bit 4: interrupt generation control
            //
            auto& mem = this->board->cpu.mem;

            // enable/disable lower ROM
            if (val & (1<<2)) {
                mem.map_rw(0, 0x0000, 0x4000, this->ram[0], this->ram[0]);
            }
            else {
                mem.map_rw(0, 0x0000, 0x4000, dump_cpc464_os, this->ram[0]);
            }

            // enable/disable upper ROM
            if (val & (1<<3)) {
                mem.map_rw(0, 0xC000, 0x4000, this->ram[3], this->ram[3]);
            }
            else {
                mem.map_rw(0, 0xC000, 0x4000, dump_cpc464_basic, this->ram[3]);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
cpc::put_key(ubyte ascii) {
    // FIXME
}

//------------------------------------------------------------------------------
ubyte
cpc::cpu_in(uword port) {
    // FIXME
    return 0xFF;
}

//------------------------------------------------------------------------------
void
cpc::irq() {
    // forward interrupt request to CPU
    this->board->cpu.irq();
}

//------------------------------------------------------------------------------
void
cpc::timer(int timer_id) {
    // FIXME
}

//------------------------------------------------------------------------------
const char*
cpc::system_info() const {
    return "FIXME!";
}

//------------------------------------------------------------------------------
void
cpc::decode_video_memory() {
    uint32_t* dst = this->rgba8_buffer;
    const uint8_t* src = this->ram[3];
    for (uint32_t y = 0; y < 200; y++) {
        uint32_t y_offset = (y>>3)*80 + (y&7)*2048;
        for (uint32_t x = 0; x < 80; x++) {
            uint8_t val = src[y_offset + x];
            *dst++ = this->pal[((val>>6)&2)|((val>>3)&1)];
            *dst++ = this->pal[((val>>5)&2)|((val>>2)&1)];
            *dst++ = this->pal[((val>>4)&2)|((val>>1)&1)];
            *dst++ = this->pal[((val>>3)&2)|((val>>0)&1)];
        }
    }
}

} // namespace YAKC
