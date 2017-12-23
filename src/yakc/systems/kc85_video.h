#pragma once
//------------------------------------------------------------------------------
/**
    @class kc85_video
    @brief wraps the KC85 video RAM (IRM) and video decoding hardware
*/
#include "yakc/systems/breadboard.h"

namespace YAKC {

class kc85_video {
public:
    /// initialize the video hardware
    void poweron(system m);
    /// reset the video hardware
    void reset();

    /// enable/disable blinking based on PIO B bit 7
    void pio_blink_enable(bool b);
    /// toggle blink flag, must be connected to CTC ZC/TO2 line
    void ctc_blink();
    /// PAL-scanline callback, scan-converts one video memory line
    void scanline();
    /// update the KC85/4 IRM control byte (written to port 84)
    void kc85_4_irm_control(uint8_t val);

    /// decode 8 pixels
    void decode8(unsigned int* ptr, uint8_t pixels, uint8_t colors, bool blink_off) const;
    /// decode the next line
    void decode_one_line(unsigned int* ptr, int y, bool blink_bg);

    /// decoded linear RGBA8 video buffer
    static const int display_width = 320;
    static const int display_height = 256;
    static_assert(display_width <= global_max_fb_width, "kc85 fb size");
    static_assert(display_height <= global_max_fb_height, "kc85 fb size");

    static const int irm0_page = 4;
    system model = system::kc85_3;
    uint8_t irm_control = 0;
    bool pio_blink_flag = true;
    bool ctc_blink_flag = true;
    uint16_t cur_scanline = 0;
};

} // namespace YAKC
