#pragma once
//------------------------------------------------------------------------------
/**
    @class kc85_video
    @brief wraps the KC85 video RAM (IRM) and video decoding hardware
*/
#include "yakc/common.h"

namespace yakc {

class kc85_video {
public:
    /// video memory banks
    ubyte irm[4][0x4000];

    /// initialize the video hardware
    void init(device m);
    /// reset the video hardware
    void reset();

    /// enable/disable blinkg based on PIO B bit 7
    void pio_blink_enable(bool b);
    /// toggle blink flag, must be connected to CTC ZC/TO2 line
    static void ctc_blink_cb(void* userdata);
    /// PAL-line callback, scan-converts one video memory line
    static void pal_line_cb(void* userdata);
    /// update the KC85/4 IRM control byte (written to port 84)
    void kc85_4_irm_control(ubyte val);

    /// decoded linear RGBA8 video buffer
    unsigned int LinearBuffer[320*256];

    /// decode 8 pixels
    void decode8(unsigned int* ptr, ubyte pixels, ubyte colors, bool blink_off) const;
    /// decode the next line
    void decode_one_line(unsigned int* ptr, int y, bool blink_bg);

    device model = device::kc85_3;
    ubyte irm_control = 0;
    bool pio_blink_flag = true;
    bool ctc_blink_flag = true;
    uword cur_pal_line = 0;
    unsigned int fg_pal[16];
    unsigned int bg_pal[16];
};

//------------------------------------------------------------------------------
inline void
kc85_video::init(device m) {
    this->model = m;
    this->irm_control = 0;

    if (m == device::kc85_4) {
        clear(this->irm, sizeof(this->irm));
    }
    else {
        fill_random(this->irm, sizeof(this->irm));
    }

    // setup foreground color palette
    this->fg_pal[0] = 0xFF000000;     // black
    this->fg_pal[1] = 0xFFFF0000;     // blue
    this->fg_pal[2] = 0xFF0000FF;     // red
    this->fg_pal[3] = 0xFFFF00FF;     // magenta
    this->fg_pal[4] = 0xFF00FF00;     // green
    this->fg_pal[5] = 0xFFFFFF00;     // cyan
    this->fg_pal[6] = 0xFF00FFFF;     // yellow
    this->fg_pal[7] = 0xFFFFFFFF;     // white
    this->fg_pal[8] = 0xFF000000;     // black #2
    this->fg_pal[9] = 0xFFFF00A0;     // violet
    this->fg_pal[10] = 0xFF00A0FF;     // orange
    this->fg_pal[11] = 0xFFA000FF;     // purple
    this->fg_pal[12] = 0xFFA0FF00;     // blueish green
    this->fg_pal[13] = 0xFFFFA000;     // greenish blue
    this->fg_pal[14] = 0xFF00FFA0;     // yellow-green
    this->fg_pal[15] = 0xFFFFFFFF;     // white #2

    // setup background color palette
    this->bg_pal[0] = 0xFF000000;      // black
    this->bg_pal[1] = 0xFFA00000;      // dark-blue
    this->bg_pal[2] = 0xFF0000A0;      // dark-red
    this->bg_pal[3] = 0xFFA000A0;      // dark-magenta
    this->bg_pal[4] = 0xFF00A000;      // dark-green
    this->bg_pal[5] = 0xFFA0A000;      // dark-cyan
    this->bg_pal[6] = 0xFF00A0A0;      // dark-yellow
    this->bg_pal[7] = 0xFFA0A0A0;      // gray
}

//------------------------------------------------------------------------------
inline void
kc85_video::reset() {
    this->irm_control = 0;
}

//------------------------------------------------------------------------------
inline void
kc85_video::pio_blink_enable(bool b) {
    this->pio_blink_flag = b;
}

//------------------------------------------------------------------------------
inline void
kc85_video::ctc_blink_cb(void* userdata) {
    kc85_video* self = (kc85_video*) userdata;
    self->ctc_blink_flag = !self->ctc_blink_flag;
}

//------------------------------------------------------------------------------
inline void
kc85_video::kc85_4_irm_control(ubyte val) {
    this->irm_control = val;
}

//------------------------------------------------------------------------------
inline void
kc85_video::pal_line_cb(void* userdata) {
    // this needs to be called for each PAL line (one PAL line: 64 microseconds)
    kc85_video* self = (kc85_video*) userdata;
    if (self->cur_pal_line < 256) {
        const bool blink_bg = self->ctc_blink_flag && self->pio_blink_flag;
        self->decode_one_line(self->LinearBuffer, self->cur_pal_line, blink_bg);
    }
    self->cur_pal_line++;
    // 326 wrap-around is a magic value to make the Digger ultra-fast
    // flash look somewhat right, the diamond-scrolling is very sensitive
    // to this value, and also changes depending on what instructions
    // the CPU executes, there must be some remaining precision problem
    // in the CTC counters/timers (probably some 'left-over' cycles
    // are not accounted for correctly?
    if (self->cur_pal_line >= 326) {
        self->cur_pal_line = 0;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_video::decode8(unsigned int* ptr, ubyte pixels, ubyte colors, bool blink_bg) const {
    // select foreground- and background color:
    //  bit 7: blinking
    //  bits 6..3: foreground color
    //  bits 2..0: background color
    //
    // index 0 is background color, index 1 is foreground color
    const ubyte bg_index = colors & 0x7;
    const ubyte fg_index = (colors>>3)&0xF;
    const unsigned int bg = this->bg_pal[bg_index];
    const unsigned int fg = (blink_bg && (colors & 0x80)) ? bg : this->fg_pal[fg_index];
    ptr[0] = pixels & 0x80 ? fg : bg;
    ptr[1] = pixels & 0x40 ? fg : bg;
    ptr[2] = pixels & 0x20 ? fg : bg;
    ptr[3] = pixels & 0x10 ? fg : bg;
    ptr[4] = pixels & 0x08 ? fg : bg;
    ptr[5] = pixels & 0x04 ? fg : bg;
    ptr[6] = pixels & 0x02 ? fg : bg;
    ptr[7] = pixels & 0x01 ? fg : bg;
}

//------------------------------------------------------------------------------
inline void
kc85_video::decode_one_line(unsigned int* dst_start, int y, bool blink_bg) {
    const int width = 320>>3;
    unsigned int* dst_ptr = &(dst_start[y*320]);
    if (device::kc85_4 == this->model) {
        // KC85/4
        int irm_index = (this->irm_control & 1) * 2;
        const ubyte* pixel_data = this->irm[irm_index];
        const ubyte* color_data = this->irm[irm_index+1];
        for (int x = 0; x < width; x++) {
            int offset = y | (x<<8);
            ubyte src_pixels = pixel_data[offset];
            ubyte src_colors = color_data[offset];
            this->decode8(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_bg);
        }
    }
    else {
        // KC85/3
        const ubyte* pixel_data = this->irm[0];
        const ubyte* color_data = this->irm[0] + 0x2800;
        const int left_pixel_offset  = (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>4)&0xF)<<9);
        const int left_color_offset  = (((y>>2)&0x3f)<<5);
        const int right_pixel_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>6)&0x3)<<9);
        const int right_color_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | (((y>>6)&0x3)<<7);
        int pixel_offset, color_offset;
        for (int x = 0; x < width; x++) {
            if (x < 0x20) {
                // left 256x256 quad
                pixel_offset = x | left_pixel_offset;
                color_offset = x | left_color_offset;
            }
            else {
                // right 64x256 strip
                pixel_offset = 0x2000 + ((x&0x7) | right_pixel_offset);
                color_offset = 0x0800 + ((x&0x7) | right_color_offset);
            }
            ubyte src_pixels = pixel_data[pixel_offset];
            ubyte src_colors = color_data[color_offset];
            this->decode8(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_bg);
        }

    }
}

} // namespace yakc
