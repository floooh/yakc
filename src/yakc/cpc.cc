//------------------------------------------------------------------------------
//  cpc.cc
//------------------------------------------------------------------------------
#include "cpc.h"

#include <stdio.h>

namespace YAKC {

//------------------------------------------------------------------------------
void
cpc::init_key_mask(ubyte ascii, int col, int bit, int shift) {
    YAKC_ASSERT((col >= 0) && (col < 10));
    YAKC_ASSERT((bit >= 0) && (bit < 8));
    YAKC_ASSERT((shift >= 0) && (shift < 2));
    this->key_map[ascii] = key_mask();
    this->key_map[ascii].col[col] = (1<<bit);
    if (shift != 0) {
        this->key_map[ascii].col[2] = (1<<5);
    }
}

//------------------------------------------------------------------------------
void
cpc::init_keymap() {
    // http://cpctech.cpc-live.com/docs/keyboard.html
    clear(this->key_map, sizeof(this->key_map));
    const char* kbd =
        // no shift
        "   ^08641 "
        "  [-97532 "
        "   @oure  "
        "  ]piytwq "
        "   ;lhgs  "
        "   :kjfda "
        "  \\/mnbc  "
        "   ., vxz "

        // shift
        "    _(&$! "
        "  {=)'%#\" "
        "   |OURE  "
        "  }PIYTWQ "
        "   +LHGS  "
        "   *KJFDA "
        "  `?MNBC  "
        "   >< VXZ ";
    int l = strlen(kbd);
    YAKC_ASSERT(l == 160);
    for (int shift = 0; shift < 2; shift++) {
        for (int col = 0; col < 10; col++) {
            for (int bit = 0; bit < 8; bit++) {
                ubyte ascii = kbd[shift*80 + bit*10 + col];
                this->init_key_mask(ascii, col, bit, shift);
            }
        }
    }

    // special keys
    this->init_key_mask(' ',  5, 7, 0);     // Space
    this->init_key_mask(0x08, 1, 0, 0);     // Cursor Left
    this->init_key_mask(0x09, 0, 1, 0);     // Cursor Right
    this->init_key_mask(0x0A, 0, 2, 0);     // Cursor Down
    this->init_key_mask(0x0B, 0, 0, 0);     // Cursor Up
    this->init_key_mask(0x01, 9, 7, 0);     // Delete
    this->init_key_mask(0x0C, 2, 0, 0);     // Clr
    this->init_key_mask(0x0D, 2, 2, 0);     // Return
    this->init_key_mask(0x03, 8, 2, 0);     // Escape
}

//------------------------------------------------------------------------------
void
cpc::init(breadboard* b, rom_images* r) {
    YAKC_ASSERT(b && r);
    this->board = b;
    this->roms = r;
    this->init_keymap();
}

//------------------------------------------------------------------------------
bool
cpc::check_roms(const rom_images& roms, device model, os_rom os) {
    if (device::cpc464 == model) {
        return roms.has(rom_images::cpc464_os) && roms.has(rom_images::cpc464_basic);
    }
    else if (device::cpc6128 == model) {
        return roms.has(rom_images::cpc6128_os) && roms.has(rom_images::cpc6128_basic);
    }
    return false;
}

//------------------------------------------------------------------------------
void
cpc::init_memory_map() {
    z80& cpu = this->board->cpu;
    cpu.mem.unmap_all();
    YAKC_ASSERT(check_roms(*this->roms, this->cur_model, os_rom::none));

    if (device::cpc464 == this->cur_model) {
        // map 32 KByte RAM from 4000 to BFFF
        cpu.mem.map(0, 0x4000, 0x4000, this->board->ram[1], true);
        cpu.mem.map(0, 0x8000, 0x4000, this->board->ram[2], true);

        // lower ROM (OS), writes always go to RAM bank 0
        cpu.mem.map_rw(0, 0x0000, 0x4000, this->roms->ptr(rom_images::cpc464_os), this->board->ram[0]);

        // upper ROM (BASIC), writes always go to RAM bank 3
        cpu.mem.map_rw(0, 0xC000, 0x4000, this->roms->ptr(rom_images::cpc464_basic), this->board->ram[3]);
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
    this->video.init(this->board);
    this->next_key_mask = key_mask();
    this->cur_key_mask = key_mask();

    // map memory
    clear(this->board->ram, sizeof(this->board->ram));
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
    this->video.reset();
    this->next_key_mask = key_mask();
    this->cur_key_mask = key_mask();
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
        this->video.update(this, ticks_step);
        cur_tick += ticks_step;
    }
    return cur_tick;
}

//------------------------------------------------------------------------------
void
cpc::cpu_out(uword port, ubyte val) {
    YAKC_ASSERT(device::cpc464 == this->cur_model);
    if ((port & 0xFF00) == 0x7F00) {
        //
        // http://www.grimware.org/doku.php/documentations/devices/gatearray
        // http://www.cpcwiki.eu/index.php/Gate_Array
        //
        //  bits 7 and 6 (on CPC plus also 5) define the command
        //
        const ubyte reg = val & 0xC0;
        if (reg == 0x00) {
            // if bit 4 is set, this means that the border color should be set
            this->video.select_pen(val);
            return;
        }
        if (reg == 0x40) {
            this->video.assign_color(val);
            return;
        }
        if (reg == 0x80) {
            //
            //  select screen mode, rom config and int ctrl
            //
            //  - bits 0 and 1 select the screen mode
            //      00: Mode 0 (160x200 @ 16 colors)
            //      01: Mode 1 (320x200 @ 4 colors)
            //      02: Mode 2 (640x200 @ 2 colors)
            //      11: Mode 3 (160x200 @ 2 colors, undocumented)
            //
            //  - bit 2: disable/enable lower ROM
            //  - bit 3: disable/enable upper ROM
            //
            //  - bit 4: interrupt generation control
            //
            auto& mem = this->board->cpu.mem;

            // enable/disable lower ROM
            if (val & (1<<2)) {
                mem.map_rw(0, 0x0000, 0x4000, this->board->ram[0], this->board->ram[0]);
            }
            else {
                mem.map_rw(0, 0x0000, 0x4000, this->roms->ptr(rom_images::cpc464_os), this->board->ram[0]);
            }

            // enable/disable upper ROM
            if (val & (1<<3)) {
                mem.map_rw(0, 0xC000, 0x4000, this->board->ram[3], this->board->ram[3]);
            }
            else {
                mem.map_rw(0, 0xC000, 0x4000, this->roms->ptr(rom_images::cpc464_basic), this->board->ram[3]);
            }
            return;
        }
    }
//    printf("OUT %04x %02x\n", port, val);
}

//------------------------------------------------------------------------------
void
cpc::put_key(ubyte ascii) {
    if (ascii) {
        this->next_key_mask = this->key_map[ascii];
    }
    else {
        this->next_key_mask = key_mask();
    }
}

//------------------------------------------------------------------------------
ubyte
cpc::cpu_in(uword port) {

    // NOTE: this is a quick-n-dirty hack to get keyboard input working!
    switch (port) {
        case 0xF470:
            return ~(this->cur_key_mask.col[0]);
        case 0xF471:
            return ~(this->cur_key_mask.col[1]);
        case 0xF472:
            return ~(this->cur_key_mask.col[2]);
        case 0xF473:
            return ~(this->cur_key_mask.col[3]);
        case 0xF474:
            return ~(this->cur_key_mask.col[4]);
        case 0xF475:
            return ~(this->cur_key_mask.col[5]);
        case 0xF476:
            return ~(this->cur_key_mask.col[6]);
        case 0xF477:
            return ~(this->cur_key_mask.col[7]);
        case 0xF478:
            return ~(this->cur_key_mask.col[8]);
        case 0xF479:
            return ~(this->cur_key_mask.col[9]);
    }

    // FIXME
//    printf("IN %04x\n", port);
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
cpc::vblank() {
    // fetch next key mask
    this->cur_key_mask = this->next_key_mask;
}

//------------------------------------------------------------------------------
const char*
cpc::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
