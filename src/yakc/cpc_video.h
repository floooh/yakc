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
    void select_pen(ubyte val);
    /// called from CPU OUT handler to assign color to selected pen
    void assign_color(ubyte val);
    /// set display mode (0..2)
    void set_video_mode(ubyte val);

    /// select crtc register
    void select_crtc(ubyte val);
    /// write crtc register
    void write_crtc(ubyte val);
    /// read crtc register
    ubyte read_crtc() const;
    /// get current state of the vsync bit
    bool vsync_bit() const;

    /// called by update at start of a scanline
    void scanline(z80bus* bus);
    /// decode a scanline into rgba8_buffer
    void decode_scanline(uint16_t y);

    static const int scanline_cycles = 256;     // 64ns * 4 cycles per ns
    static const int irq_scanlines = 52;        // interrupt request every 52 scanlines

    breadboard* board = nullptr;

    int cycle_count = 0;        // scanline cycle counter
    int scanline_count = 0;     // current scanline
    int hsync_count = 0;        // count scanline until

    // CRTC registers
    // http://www.cpcwiki.eu/index.php/CRTC
    struct crtc_t {
        enum reg {
            HORI_TOTAL = 0,
            HORI_DISPLAYED,
            HORI_SYNC_POS,
            SYNC_WIDTHS,
            VERT_TOTAL,
            VERT_TOTAL_ADJUST,
            VERT_DISPLAYED,
            VERT_SYNC_POS,
            INTERLACE_AND_SKEW,
            MAX_RASTER_ADDR,
            CURSOR_START_RASTER,
            CURSOR_END_RASTER,
            DISPLAY_START_ADDR_HI,
            DISPLAY_START_ADDR_LO,
            CURSOR_ADDR_HI,
            CURSOR_ADDR_LO,
            LIGHTPEN_ADDR_HI,
            LIGHTPEN_ADDR_LO,

            NUM_REGS,
        };
        ubyte selected = 0;         // currently selected register for write/read
        ubyte regs[NUM_REGS];
        ubyte mask[NUM_REGS];       // used to mask reg value to valid range
    };
    crtc_t crtc;

    static const int max_display_width = 768;
    static const int max_display_height = 272;

    int top_border_start    = (max_display_height - 200) / 2;
    int bottom_border_start = ((max_display_height - 200) / 2) + 200;
    int left_border_width   = (max_display_width - 640) / 2;
    int right_border_width  = (max_display_width - 640) / 2;

    bool vsync_flag = false;
    uint32_t mode = 1;
    uint32_t selected_pen = 0;
    uint32_t border_color = 0;
    uint32_t pens[16];
    uint32_t rgba8_buffer[max_display_width * max_display_height]; // enough pixels for overscan mode
};

} // namespace YAKC
