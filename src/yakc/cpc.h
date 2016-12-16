#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc
    @brief Amstrad CPC 464/6128 and KC Compact emulation
*/
#include "yakc/breadboard.h"
#include "roms/roms.h"
#include "yakc/z80bus.h"

namespace YAKC {

class cpc : public z80bus {
public:
    /// ram banks
    ubyte ram[8][0x4000];

    /// the main board
    breadboard* board = nullptr;

    /// one-time setup
    void init(breadboard* board);
    /// initialize the memory map
    void init_memory_map();
    /// power-on the device
    void poweron(device m);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// FIXME: decode video memory
    void decode_video_memory();

    /// the z80 out callback
    virtual void cpu_out(uword port, ubyte val) override;
    /// the z80 in callback
    virtual ubyte cpu_in(uword port) override;
    /// interrupt request callback
    virtual void irq() override;
    /// clock timer-trigger callback
    virtual void timer(int timer_id) override;

    device cur_model = device::cpc464;
    bool on = false;
    uint32_t pal[16];
    uint32_t rgba8_buffer[320*200];
};

} // namespace YAKC
