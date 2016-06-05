//------------------------------------------------------------------------------
//  z9001.cc
//------------------------------------------------------------------------------
#include "z9001.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
z9001::init(breadboard* b) {
    this->board = b;

    // setup color palette
    this->fg_pal[0] = 0xFF000000;     // black
    this->fg_pal[1] = 0xFF0000FF;     // red
    this->fg_pal[2] = 0xFF00FF00;     // green
    this->fg_pal[3] = 0xFF00FFFF;     // yellow
    this->fg_pal[4] = 0xFFFF0000;     // blue
    this->fg_pal[5] = 0xFFFF00FF;     // purple
    this->fg_pal[6] = 0xFFFFFF00;     // cyan
    this->fg_pal[7] = 0xFFFFFFFF;     // white

    // setup background color palette
    // FIXME: are background colors exactly identical to foreground?
    this->fg_pal[0] = 0xFF000000;     // black
    this->fg_pal[1] = 0xFF0000FF;     // red
    this->fg_pal[2] = 0xFF00FF00;     // green
    this->fg_pal[3] = 0xFF00FFFF;     // yellow
    this->fg_pal[4] = 0xFFFF0000;     // blue
    this->fg_pal[5] = 0xFFFF00FF;     // purple
    this->fg_pal[6] = 0xFFFFFF00;     // cyan
    this->fg_pal[7] = 0xFFFFFFFF;     // white
}

//------------------------------------------------------------------------------
void
z9001::poweron(device m, os_rom os) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_z9001) & int(m));
    YAKC_ASSERT(!this->on);

    z80& cpu = this->board->cpu;
    this->cur_model = m;
    this->cur_os = os;
    this->on = true;

    // map memory
    clear(this->ram, sizeof(this->ram));
    clear(this->color_ram, sizeof(this->color_ram));
    clear(this->video_ram, sizeof(this->video_ram));
    cpu.mem.unmap_all();
    if (device::kc87 == m) {
        cpu.mem.map(0, 0x0000, 0x4000, this->ram, true);
        cpu.mem.map(1, 0xC000, 0x2000, dump_z9001_basic, false);
        cpu.mem.map(1, 0xE000, 0x2000, dump_kc87_os_2, false);
    }
    cpu.mem.map(0, 0xE800, 0x0400, this->color_ram, true);
    cpu.mem.map(0, 0xEC00, 0x0400, this->video_ram, true);

    // initialize the clock at ca 2.45 MHz
    this->board->clck.init(2450);

    // initialize hardware components
    cpu.init(in_cb, out_cb, this);

    // execution on power-on starts at 0xF000
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z9001::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->cpu.mem.unmap_all();
    this->on = false;    
}

//------------------------------------------------------------------------------
void
z9001::reset() {
    this->board->ctc.reset();
    this->board->pio.reset();
    this->board->cpu.reset();
    this->overflow_cycles = 0;

    // execution after reset starts at 0x0000(??? -> doesn't work)
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z9001::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    // FIXME: the speed multiplier isn't currently working because of the min/max cycle count limiter!
    YAKC_ASSERT(speed_multiplier > 0);
    this->cpu_ahead = false;
    this->cpu_behind = false;    
    z80& cpu = this->board->cpu;
    z80dbg& dbg = this->board->dbg;
    clock& clk = this->board->clck;

    if (!dbg.paused) {
        if (this->abs_cycle_count == 0) {
            this->abs_cycle_count = min_cycle_count;
        }
        const int64_t num_cycles = clk.cycles(micro_secs*speed_multiplier) - this->overflow_cycles;
        uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
        if (abs_end_cycles > max_cycle_count) {
            abs_end_cycles = max_cycle_count;
            this->cpu_ahead = true;
        }
        else if (abs_end_cycles < min_cycle_count) {
            abs_end_cycles = min_cycle_count;
            this->cpu_behind = true;
        }
        while (this->abs_cycle_count < abs_end_cycles) {
            if (dbg.check_break(cpu)) {
                dbg.paused = true;
                this->overflow_cycles = 0;
                break;
            }
            dbg.store_pc_history(cpu); // FIXME: only if debug window open?
            int cycles_step = cpu.step();
            cycles_step += cpu.handle_irq();
            clk.update(cycles_step);
            this->abs_cycle_count += cycles_step;
        }
        this->overflow_cycles = uint32_t(this->abs_cycle_count - abs_end_cycles);
    }
    this->decode_video();
}

//------------------------------------------------------------------------------
void
z9001::out_cb(void* userdata, uword port, ubyte val) {
    z9001* self = (z9001*)userdata;
    switch (port & 0xFF) {
        // FIXME
        default:
            break;
    }
}

//------------------------------------------------------------------------------
ubyte
z9001::in_cb(void* userdata, uword port) {
    z9001* self = (z9001*)userdata;
    switch (port & 0xFF) {
        // FIXME
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
z9001::put_key(ubyte ascii) {
    // FIXME: HACK!
    static ubyte chr = 0;
    if (ascii != chr) {
        chr = ascii;
        this->board->cpu.mem.w8(0x0025, ascii);
    }
    this->board->cpu.mem.w8(0x006A, 0);
}

//------------------------------------------------------------------------------
void
z9001::decode_video() {

    // FIXME: there's also a 40x20 display mode
    uint32_t* dst = RGBA8Buffer;
    ubyte* font;
    if (device::kc87 == this->cur_model) {
        font = dump_kc87_font_2;
    }
    else {
        font = dump_z9001_font;
    }
    int off = 0;
    for (int y = 0; y < 24; y++) {
        for (int py = 0; py < 8; py++) {
            for (int x = 0; x < 40; x++) {
                ubyte chr = this->video_ram[off+x];
                ubyte pixels = dump_kc87_font_2[(chr<<3)|py];
                ubyte color = this->color_ram[off];
                uint32_t fg = this->fg_pal[(color>>4)&7];
                uint32_t bg = this->bg_pal[color&7];

                // FIXME flashing
                for (int px = 7; px >=0; px--) {
                    *dst++ = pixels & (1<<px) ? fg:bg;
                }
            }
        }
        off += 40;
    }
}

//------------------------------------------------------------------------------
const char*
z9001::system_info() const {
    return "Z9001 FIXME";
}

} // namespace YAKC
