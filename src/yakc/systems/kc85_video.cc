//------------------------------------------------------------------------------
//  kc85_video.cc
//------------------------------------------------------------------------------
#include "kc85_video.h"

namespace YAKC {

static uint32_t fg_palette[16] = {
    0xFF000000,     // black
    0xFFFF0000,     // blue
    0xFF0000FF,     // red
    0xFFFF00FF,     // magenta
    0xFF00FF00,     // green
    0xFFFFFF00,     // cyan
    0xFF00FFFF,     // yellow
    0xFFFFFFFF,     // white
    0xFF000000,     // black #2
    0xFFFF00A0,     // violet
    0xFF00A0FF,     // orange
    0xFFA000FF,     // purple
    0xFFA0FF00,     // blueish green
    0xFFFFA000,     // greenish blue
    0xFF00FFA0,     // yellow-green
    0xFFFFFFFF,     // white #2
};

static uint32_t bg_palette[8] = {
    0xFF000000,      // black
    0xFFA00000,      // dark-blue
    0xFF0000A0,      // dark-red
    0xFFA000A0,      // dark-magenta
    0xFF00A000,      // dark-green
    0xFFA0A000,      // dark-cyan
    0xFF00A0A0,      // dark-yellow
    0xFFA0A0A0,      // gray
};

//------------------------------------------------------------------------------
void
kc85_video::init(system m, breadboard* b) {
    this->model = m;
    this->board = b;
    this->rgba8_buffer = this->board->rgba8_buffer;
    this->irm_control = 0;
}

//------------------------------------------------------------------------------
void
kc85_video::reset() {
    this->irm_control = 0;
}

//------------------------------------------------------------------------------
void
kc85_video::pio_blink_enable(bool b) {
    this->pio_blink_flag = b;
}

//------------------------------------------------------------------------------
void
kc85_video::ctc_blink() {
    this->ctc_blink_flag = !this->ctc_blink_flag;
}

//------------------------------------------------------------------------------
void
kc85_video::kc85_4_irm_control(uint8_t val) {
    this->irm_control = val;
}

//------------------------------------------------------------------------------
void
kc85_video::scanline() {
    // this needs to be called for each PAL line (one PAL line: 64 microseconds)
    if (this->cur_scanline < display_height) {
        const bool blink_bg = this->ctc_blink_flag && this->pio_blink_flag;
        this->decode_one_line(this->rgba8_buffer, this->cur_scanline, blink_bg);
    }
    this->cur_scanline++;
    // wraparound pal line counter at 312 lines (see KC85/3 service manual),
    if (this->cur_scanline >= 312) {
        this->cur_scanline = 0;
    }
}

//------------------------------------------------------------------------------
void
kc85_video::decode8(unsigned int* ptr, uint8_t pixels, uint8_t colors, bool blink_bg) const {
    // select foreground- and background color:
    //  bit 7: blinking
    //  bits 6..3: foreground color
    //  bits 2..0: background color
    //
    // index 0 is background color, index 1 is foreground color
    const uint8_t bg_index = colors & 0x7;
    const uint8_t fg_index = (colors>>3)&0xF;
    const unsigned int bg = bg_palette[bg_index];
    const unsigned int fg = (blink_bg && (colors & 0x80)) ? bg : fg_palette[fg_index];
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
void
kc85_video::decode_one_line(unsigned int* dst_start, int y, bool blink_bg) {
    const int width = display_width>>3;
    unsigned int* dst_ptr = &(dst_start[y*display_width]);
    if (system::kc85_4 == this->model) {
        // KC85/4
        int irm_index = (this->irm_control & 1) * 2;
        const uint8_t* pixel_data = this->board->ram[irm0_page + irm_index];
        const uint8_t* color_data = this->board->ram[irm0_page + irm_index + 1];
        for (int x = 0; x < width; x++) {
            int offset = y | (x<<8);
            uint8_t src_pixels = pixel_data[offset];
            uint8_t src_colors = color_data[offset];
            this->decode8(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_bg);
        }
    }
    else {
        // KC85/3
        const uint8_t* pixel_data = this->board->ram[irm0_page];
        const uint8_t* color_data = this->board->ram[irm0_page] + 0x2800;
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
            uint8_t src_pixels = pixel_data[pixel_offset];
            uint8_t src_colors = color_data[color_offset];
            this->decode8(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_bg);
        }
    }
}

} // namespace YAKC
