#pragma once
//------------------------------------------------------------------------------
/**
    @class kc85_video
    @brief wraps the KC85 video RAM (IRM) and video decoding hardware
*/
#include "yakc/core.h"

namespace YAKC {

class kc85_video {
public:
    /// video memory banks
    ubyte irm[4][0x4000];

    /// initialize the video hardware
    void init(device m);
    /// reset the video hardware
    void reset();

    /// enable/disable blinking based on PIO B bit 7
    void pio_blink_enable(bool b);
    /// toggle blink flag, must be connected to CTC ZC/TO2 line
    void ctc_blink();
    /// PAL-line callback, scan-converts one video memory line
    void pal_line();
    /// update the KC85/4 IRM control byte (written to port 84)
    void kc85_4_irm_control(ubyte val);

    /// decode 8 pixels
    void decode8(unsigned int* ptr, ubyte pixels, ubyte colors, bool blink_off) const;
    /// decode the next line
    void decode_one_line(unsigned int* ptr, int y, bool blink_bg);

    /// decoded linear RGBA8 video buffer
    unsigned int rgba8_buffer[320*256];

    device model = device::kc85_3;
    ubyte irm_control = 0;
    bool pio_blink_flag = true;
    bool ctc_blink_flag = true;
    uword cur_pal_line = 0;
};

} // namespace YAKC
