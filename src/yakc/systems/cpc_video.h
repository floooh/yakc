#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc_video
    @brief CPC video decoder (CRTC, color palette, video mode)
*/
#include "yakc/core/system_bus.h"
#include "yakc/systems/breadboard.h"

namespace YAKC {

class cpc_video {
public:
    /// initialize the object
    void init(device model, breadboard* board);
    /// perform a reset
    void reset();
    /// call after each CPU instruction, calls the scanline() method
    void step(system_bus* bus, int cycles);

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

    /// get current state of the vsync bit
    bool vsync_bit() const;
    /// gate array hsync/vsync stuff (irq and vblank)  
    void handle_crtc_sync(system_bus* bus);
    /// decode the next 16 pixels into the emulator framebuffer
    void decode_pixels(uint32_t* dst);

    breadboard* board = nullptr;
    device model = device::none;
    bool debug_video = false;

    counter cycle_counter;
    int hsync_irq_count = 0;        // interrupt counter, incremented each scanline, reset at 52
    int hsync_after_vsync_counter = 0;  // special case hsync irq after vsync (32 instead of 52 lines)
    int hsync_start_count = 0;
    int hsync_end_count = 0;
    bool request_interrupt = false;

    static const int max_display_width = 768;
    static const int max_display_height = 272;

    static const int dbg_max_display_width  = 1024;     // 64*16
    static const int dbg_max_display_height = 312;

    uint32_t next_video_mode = 1;
    uint32_t video_mode = 1;
    uint32_t selected_pen = 0;
    uint32_t border_color = 0;
    uint32_t palette[32];
    uint32_t pens[16];
    uint32_t rgba8_buffer[max_display_width * max_display_height]; // enough pixels for overscan mode
    uint32_t dbg_rgba8_buffer[dbg_max_display_width * dbg_max_display_height];
};

//------------------------------------------------------------------------------
inline void
cpc_video::select_pen(ubyte val) {
    this->selected_pen = val & 0x1F;
}

//------------------------------------------------------------------------------
inline void
cpc_video::assign_color(ubyte val) {
    if (this->selected_pen & 0x10) {
        // set border color
        this->border_color = this->palette[val & 0x1F];
    }
    else {
        // set pen
        this->pens[this->selected_pen & 0x0F] = this->palette[val & 0x1F];
    }
}

//------------------------------------------------------------------------------
inline void
cpc_video::set_video_mode(ubyte val) {
    // video mode is realized at the end of HSYNC
    this->next_video_mode = val & 3;
}

//------------------------------------------------------------------------------
inline bool
cpc_video::vsync_bit() const {
    return this->board->mc6845.test(mc6845::VSYNC);
}

//------------------------------------------------------------------------------
inline void
cpc_video::interrupt_control() {
    this->hsync_irq_count = 0;
}

//------------------------------------------------------------------------------
inline void
cpc_video::interrupt_acknowledge() {
    // clear top bit of hsync counter, makes sure the next hsync irq
    // won't happen until at least 32 lines later
    this->hsync_irq_count &= ~(1<<5);
}

} // namespace YAKC
