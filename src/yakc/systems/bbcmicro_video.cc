//------------------------------------------------------------------------------
//  bbcmicro_video.cc
//------------------------------------------------------------------------------
#include "bbcmicro_video.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
bbcmicro_video::init(breadboard* board_) {
    this->board = board_;
    this->board->mc6845.init(mc6845::TYPE_UM6845R);
    this->board->crt.init(crt::PAL, 32/16, 32, max_display_width/16, max_display_height);
    this->reset();
}

//------------------------------------------------------------------------------
void
bbcmicro_video::reset() {
    this->board->mc6845.reset();
    this->board->crt.reset();
    this->tick_period = 2;
    this->tick_count = 2;
    this->video_control = 0;
    this->palette = 0;
}

//------------------------------------------------------------------------------
void
bbcmicro_video::write_video_control(uint8_t v) {
    this->video_control = v;
    if (v & (1<<4)) {
        // drive CRTC with 1 MHz
        this->tick_period = 2;
    }
    else {
        // drive CRTC with 2 MHz
        this->tick_period = 1;
    }
}

//------------------------------------------------------------------------------
void
bbcmicro_video::write_palette(uint8_t v) {
    this->palette = v;
}

//------------------------------------------------------------------------------
void
bbcmicro_video::step() {
    // http://beebwiki.mdfs.net/Video_ULA
    // the CRTC can be driven at 1 or 2 MHz
    if (--this->tick_count == 0) {

        this->tick_count = this->tick_period;

        auto& crtc = this->board->mc6845;
        auto& crt = this->board->crt;
        crtc.step();
        if (crtc.on(mc6845::HSYNC)) {
            crt.trigger_hsync();
        }
        if (crtc.on(mc6845::VSYNC)) {
            crt.trigger_vsync();
        }
        crt.step();
        if (crt.visible) {
            int dst_x = crt.x * 16;
            int dst_y = crt.y;
            YAKC_ASSERT((dst_x <= (max_display_width-16)) && (dst_y < max_display_height));
            uint32_t* dst = &(this->rgba8_buffer[dst_x + dst_y * max_display_width]);


            if (crtc.test(mc6845::DISPEN)) {
                // decode visible pixels
                this->decode_pixels(dst);
            }
            else if (crtc.test(mc6845::HSYNC|mc6845::VSYNC)) {
                // blacker than black
                for (int i = 0; i < 16; i++) {
                    dst[i] = 0xFF000000;
                }
            }
            else {
                // border color
                for (int i = 0; i < 16; i++) {
                    dst[i] = 0xFFFFFF00;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
bbcmicro_video::decode_pixels(uint32_t* dst) {
    for (int i = 0; i < 16; i++) {
        *dst++ = 0xFFFF00FF;
    }
}

} // namespace YAKC
