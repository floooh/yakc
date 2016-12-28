#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc_video
    @brief CPC video decoder (CRTC, color palette, video mode)
*/
#include "yakc/system_bus.h"
#include "yakc/breadboard.h"

namespace YAKC {

class cpc_video {
public:
    /// initialize the object
    void init(device model, breadboard* board);
    /// perform a reset
    void reset();
    /// call after each CPU instruction, calls the scanline() method
    void update(system_bus* bus, int cycles);

    /// called from CPU OUT handler to select pen for modification
    void select_pen(ubyte val);
    /// called from CPU OUT handler to assign color to selected pen
    void assign_color(ubyte val);
    /// set display mode (0..2)
    void set_video_mode(ubyte val);
    /// called when bit 4 in CPU OUT 0x7Fxx is set (resets HSYNC counter)
    void interrupt_control();
    /// called when CPU acknowledges interrupt, clears bit 5 of HSYNC counter
    void interrupt_acknowledge();

    /// select crtc register
    void select_crtc(ubyte val);
    /// write crtc register
    void write_crtc(ubyte val);
    /// read crtc register
    ubyte read_crtc() const;
    /// get current state of the vsync bit
    bool vsync_bit() const;

    /// update derived CRTC values
    void update_crtc_values();
    /// called by update at start of a scanline
    void scanline(system_bus* bus);
    /// decode border scanline at framebuffer coordinate y
    void decode_border_scanline(int dst_y);
    /// decode a scanline into rgba8_buffer
    void decode_visible_scanline(int src_y, int dst_y);

    breadboard* board = nullptr;
    device model = device::none;

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
        ubyte regs[NUM_REGS] = { };

        // current state
        int scanline_count = 0;         // current scanline counter (src y)
        int palline_count = 0;          // current position on screen (dst y)
        int scanline_cycle_count = 0;   // position in scanline in CPU cycles
        bool hsync = false;             // inside hsync_start/hsync_end
        bool vsync = false;             // inside vsync_start/vsync_end
        bool hsync_triggered = false;   // hsync processing already triggered this scanline
        bool vsync_triggered = false;   // vsync already triggered this frame
        int hsync_irq_count = 0;        // interrupt counter, incremented each scanline, reset at 52
        int hsync_after_vsync_counter = 0;  // special case hsync irq after vsync (32 instead of 52 lines)

        // computed values
        int scanline_end = 0;           // end of scanline, in CPU cycles
        int visible_scanlines = 0;      // number of visible scanlines (inside vertical border)
        int frame_end = 0;              // last scanline in frame
        int hsync_start = 0;            // start of HSYNC in scanline, in CPU cycles
        int hsync_end = 0;              // end of HSYNC in scanline, in CPU cycles
        int row_height = 0;             // height of a character row in scanlines
        int vsync_start = 0;            // start of VSYNC, in number of scanlines
        int vsync_end = 0;              // end of VSYNC in number of scanlines (official)
        int left_border_width = 0;      // width of left border in emulator framebuffer pixels
        int visible_width = 0;          // width of visible area in emulator framebuffer pixels
    };
    crtc_t crtc;

    static const int max_display_width = 768;
    static const int max_display_height = 272;

    uint32_t mode = 1;
    uint32_t selected_pen = 0;
    uint32_t border_color = 0;
    uint32_t palette[32];
    uint32_t pens[16];
    uint32_t rgba8_buffer[max_display_width * max_display_height]; // enough pixels for overscan mode
};

} // namespace YAKC
