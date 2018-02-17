#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc_video
    @brief CPC video decoder (CRTC, color palette, video mode)
*/
#include "yakc/systems/breadboard.h"

namespace YAKC {

class cpc_video {
public:
    /// initialize the object
    void init(system model);
    /// perform a reset
    void reset();
    /// call per CPU tick
    uint64_t tick(uint64_t cpu_pins);

    /// called from CPU OUT handler to select pen for modification
    void select_pen(uint8_t val);
    /// called from CPU OUT handler to assign color to selected pen
    void assign_color(uint8_t val);
    /// set display mode (0..2)
    void set_video_mode(uint8_t val);
    /// called when bit 4 in CPU OUT 0x7Fxx is set (resets HSYNC counter)
    void interrupt_control();
    /// called when CPU acknowledges interrupt, clears bit 5 of HSYNC counter
    void interrupt_acknowledge();

    /// get current state of the vsync bit
    bool vsync_bit() const;
    /// gate array hsync/vsync stuff (irq and vblank)  
    void handle_crtc_sync(uint64_t crtc_pins);
    /// decode the next 16 pixels into the emulator framebuffer
    void decode_pixels(uint32_t* dst, uint64_t crtc_pins);

    system model = system::none;
    bool debug_video = false;

    uint64_t prev_crtc_pins = 0;
    int hsync_irq_count = 0;        // interrupt counter, incremented each scanline, reset at 52
    int hsync_after_vsync_counter = 0;  // special case hsync irq after vsync (32 instead of 52 lines)
    int hsync_start_count = 0;
    int hsync_end_count = 0;
    int int_acknowledge_counter = 0;
    bool request_interrupt = false;

    static const int max_display_width = 768;
    static const int max_display_height = 272;
    static_assert(max_display_width <= global_max_fb_width, "cpc display size");
    static_assert(max_display_height <= global_max_fb_height, "cpc display size");

    static const int dbg_max_display_width  = 1024;     // 64*16
    static const int dbg_max_display_height = 312;
    static_assert(dbg_max_display_width <= global_max_fb_width, "cpc display size");
    static_assert(dbg_max_display_height <= global_max_fb_height, "cpc display size");

    uint32_t next_video_mode = 1;
    uint32_t video_mode = 1;
    uint32_t selected_pen = 0;
    uint32_t border_color = 0;
    uint32_t palette[32];
    uint32_t pens[16];
    uint32_t* rgba8_buffer = nullptr;
};

//------------------------------------------------------------------------------
inline void
cpc_video::select_pen(uint8_t val) {
    this->selected_pen = val & 0x1F;
}

//------------------------------------------------------------------------------
inline void
cpc_video::assign_color(uint8_t val) {
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
cpc_video::set_video_mode(uint8_t val) {
    // video mode is realized at the end of HSYNC
    this->next_video_mode = val & 3;
}

//------------------------------------------------------------------------------
inline bool
cpc_video::vsync_bit() const {
    return board.mc6845.vs;
}

//------------------------------------------------------------------------------
inline void
cpc_video::interrupt_control() {
    this->hsync_irq_count = 0;
}

//------------------------------------------------------------------------------
inline void
cpc_video::interrupt_acknowledge() {
    this->int_acknowledge_counter = 2;
}

} // namespace YAKC
