#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z1013
    @brief wrapper class for Z1013 system
*/
#include "core/breadboard.h"
#include "roms/roms.h"
#include "z1013/z1013_video.h"
#include "z1013/z1013_roms.h"

namespace yakc {

class z1013 {
public:
    /// ram banks
    ubyte ram[4][0x4000];

    /// hardware components
    breadboard* board;
    z1013_video video;
    z1013_roms roms;

    /// constructor
    z1013();
    /// one-time setup
    void setup(breadboard* board);

    /// power-on the device
    void poweron(device m);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get the Z1013 model
    device model() const;

    /// process one frame, up to absolute number of cycles
    void onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count);

    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);

    device cur_model;
    os_rom cur_os;
    bool on;
    uint64_t abs_cycle_count;   // total CPU cycle count
    uint32_t overflow_cycles;   // cycles that have overflowed from last frame
    const ubyte* os_ptr;
    int os_size;
};

//------------------------------------------------------------------------------
inline
z1013::z1013() :
board(nullptr),
cur_model(device::z1013_01),
cur_os(os_rom::z1013_mon202),
on(false),
abs_cycle_count(0),
overflow_cycles(0),
os_ptr(nullptr),
os_size(0) {
    // empty
}

//------------------------------------------------------------------------------
inline void
z1013::setup(breadboard* b) {
    this->board = b;
}

//------------------------------------------------------------------------------
inline void
z1013::poweron(device m) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_z1013) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->cur_os = os_rom::z1013_mon202;
    this->on = true;

    // map memory
    this->board->cpu.mem.map(0, 0x0000, 0x4000, this->ram[0], true);            // RAM
    this->board->cpu.mem.map(0, 0xEC00, 0x0400, this->video.irm, true);         // video mem
    this->board->cpu.mem.map(0, 0xF000, 0x0800, (ubyte*)this->roms.ptr(z1013_roms::mon202), false);   // OS ROM

    // initialize the clock, the z1013_01 runs at 1MHz, all others at 2MHz
    this->board->clck.init((m == device::z1013_01) ? 1000 : 2000);

    // initialize hardware components
    this->board->pio.init();
    this->board->ctc.init();
    this->board->cpu.init(in_cb, out_cb, this);
    this->video.init();

    // clear RAM
    clear(this->ram, sizeof(this->ram));

    // execution on power-on starts at 0xF000
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
inline void
z1013::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
inline void
z1013::reset() {
    this->video.reset();
    this->board->ctc.reset();
    this->board->pio.reset();
    this->board->cpu.reset();
    // FIXME
    YAKC_ASSERT(false);
}

//------------------------------------------------------------------------------
inline void
z1013::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    // FIXME: the speed multiplier isn't currently working because of the min/max cycle count limiter!
    YAKC_ASSERT(speed_multiplier > 0);
    z80& cpu = this->board->cpu;
    z80dbg& dbg = this->board->dbg;
    z80ctc& ctc = this->board->ctc;
    clock& clk = this->board->clck;

    if (!dbg.paused) {
        // compute the end-cycle-count for the current frame
        const int64_t num_cycles = clk.cycles(micro_secs*speed_multiplier) - this->overflow_cycles;
        uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
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
            ctc.update_timers(cycles_step);
            this->abs_cycle_count += cycles_step;
        }
        this->overflow_cycles = uint32_t(this->abs_cycle_count - abs_end_cycles);
    }
    this->video.decode();
}

//------------------------------------------------------------------------------
inline void
z1013::out_cb(void* userdata, uword port, ubyte val) {
    // FIXME
}

//------------------------------------------------------------------------------
inline ubyte
z1013::in_cb(void* userdata, uword port) {
    // FIXME
    return 0;
}

} // namespace yakc
