#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc_video
    @brief CPC PAL and CTRC emulation
*/
#include "yakc/z80bus.h"
#include "yakc/breadboard.h"

namespace YAKC {

class cpc_video {
public:
    /// initialize the object
    void init(breadboard* board);
    /// perform a reset
    void reset();
    /// call after each CPU instruction, calls the scanline() method
    void update(z80bus* bus, int cycles);

    /// called from CPU OUT handler to select pen for modification
    void select_pen(uint32_t val);
    /// called from CPU OUT handler to assign color to selected pen
    void assign_color(uint32_t val);

    /// called by update at start of a scanline
    void scanline(z80bus* bus);
    /// decode a scanline into rgba8_buffer
    void decode_scanline(uint16_t y);

    static const int scanline_cycles = 256;     // 64ns * 4 cycles per ns
    static const int frame_scanlines = 312;     // number of scanline in each PAL frame
    static const int irq_scanlines = 52;        // interrupt request every 52 scanlines

    breadboard* board = nullptr;

    int cycle_count = 0;        // scanline cycle counter
    int scanline_count = 0;     // current scanline
    int hsync_count = 0;        // count scanline until

    uint32_t selected_pen = 0;
    uint32_t border_color = 0;
    uint32_t pens[16];
    uint32_t rgba8_buffer[640*200];
};

} // namespace YAKC
