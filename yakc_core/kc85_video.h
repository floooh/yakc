#pragma once
//------------------------------------------------------------------------------
/**
    @class kc85_video
    @brief wraps the KC85 video RAM (IRM) and video decoding hardware
*/
#include "yakc_core/common.h"

namespace yakc {

class kc85_video {
public:
    /// video memory banks
    ubyte irm[4][0x4000];

    /// initialize the video hardware
    void init(kc85_model m);
    /// reset the video hardware
    void reset();
    /// decode KC85 video memory into linear RGBA8 buffer
    void decode(ubyte* rgba8_buf, int buf_size) const;

    /// enable/disable blinkg based on PIO B bit 7
    void pio_blink_enable(bool b);
    /// toggle blink flag, must be connected to CTC ZC/TO2 line
    static void ctc_blink_cb(void* userdata);
    /// update the KC85/4 IRM control byte (written to port 84)
    void kc85_4_irm_control(ubyte val);

private:
    /// decode 8 pixels
    void decode8(unsigned int* ptr, ubyte pixels, ubyte colors, bool blink_off) const;

    kc85_model model = kc85_model::kc85_3;
    ubyte irm_control = 0;
    bool pio_blink_flag = true;
    bool ctc_blink_flag = true;
    unsigned int fgPal[16];
    unsigned int bgPal[16];
};

//------------------------------------------------------------------------------
inline void
kc85_video::init(kc85_model m) {
    this->model = m;
    this->irm_control = 0;

    if (m == kc85_model::kc85_4) {
        clear(this->irm, sizeof(this->irm));
    }
    else {
        fill_random(this->irm, sizeof(this->irm));
    }

    // setup foreground color palette
    this->fgPal[0] = 0xFF000000;     // black
    this->fgPal[1] = 0xFFFF0000;     // blue
    this->fgPal[2] = 0xFF0000FF;     // red
    this->fgPal[3] = 0xFFFF00FF;     // magenta
    this->fgPal[4] = 0xFF00FF00;     // green
    this->fgPal[5] = 0xFFFFFF00;     // cyan
    this->fgPal[6] = 0xFF00FFFF;     // yellow
    this->fgPal[7] = 0xFFFFFFFF;     // white
    this->fgPal[8] = 0xFF000000;     // black #2
    this->fgPal[9] = 0xFFFF00A0;     // violet
    this->fgPal[10] = 0xFF00A0FF;     // orange
    this->fgPal[11] = 0xFFA000FF;     // purple
    this->fgPal[12] = 0xFFA0FF00;     // blueish green
    this->fgPal[13] = 0xFFFFA000;     // greenish blue
    this->fgPal[14] = 0xFF00FFA0;     // yellow-green
    this->fgPal[15] = 0xFFFFFFFF;     // white #2

    // setup background color palette
    this->bgPal[0] = 0xFF000000;      // black
    this->bgPal[1] = 0xFFA00000;      // dark-blue
    this->bgPal[2] = 0xFF0000A0;      // dark-red
    this->bgPal[3] = 0xFFA000A0;      // dark-magenta
    this->bgPal[4] = 0xFF00A000;      // dark-green
    this->bgPal[5] = 0xFFA0A000;      // dark-cyan
    this->bgPal[6] = 0xFF00A0A0;      // dark-yellow
    this->bgPal[7] = 0xFFA0A0A0;      // gray
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
kc85_video::decode8(unsigned int* ptr, ubyte pixels, ubyte colors, bool blink_bg) const {
    // select foreground- and background color:
    //  bit 7: blinking
    //  bits 6..3: foreground color
    //  bits 2..0: background color
    //
    // index 0 is background color, index 1 is foreground color
    const ubyte bg_index = colors & 0x7;
    const ubyte fg_index = (colors>>3)&0xF;
    const unsigned int bg = this->bgPal[bg_index];
    const unsigned int fg = (blink_bg && (colors & 0x80)) ? bg : this->fgPal[fg_index];
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
kc85_video::decode(ubyte* rgba8_buf, int buf_size) const {
    YAKC_ASSERT(buf_size == 320*256*4);

    unsigned int* dst_start = (unsigned int*) rgba8_buf;
    const bool blink_bg = this->ctc_blink_flag && this->pio_blink_flag;

    if (kc85_model::kc85_4 == this->model) {
        int irm_index = (this->irm_control & 1) * 2;
        const ubyte* pixel_data = this->irm[irm_index];
        const ubyte* color_data = this->irm[irm_index+1];
        int offset;
        for (int y = 0; y < 256; y++) {
            unsigned int* dst_ptr = &(dst_start[y*320]);
            const int width = 320>>3;
            for (int x = 0; x < width; x++) {
                offset = y | (x<<8);
                ubyte src_pixels = pixel_data[offset];
                ubyte src_colors = color_data[offset];
                this->decode8(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_bg);
            }
        }
    }
    else {
        const ubyte* pixel_data = this->irm[0];
        const ubyte* color_data = this->irm[0] + 0x2800;

        int pixel_offset;
        int color_offset;
        for (int y = 0; y < 256; y++) {

            const int left_pixel_offset  = (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>4)&0xF)<<9);
            const int left_color_offset  = (((y>>2)&0x3f)<<5);
            const int right_pixel_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>6)&0x3)<<9);
            const int right_color_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | (((y>>6)&0x3)<<7);

            unsigned int* dst_ptr = &(dst_start[y*320]);

            // x is not per-pixel, but per byte
            const int width = 320>>3;
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
}

} // namespace yakc
