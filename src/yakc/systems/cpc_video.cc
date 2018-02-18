//------------------------------------------------------------------------------
//  cpc_video.cc
//------------------------------------------------------------------------------
#include "cpc_video.h"

namespace YAKC {

//------------------------------------------------------------------------------
//
//  the fixed hardware color palette
//
//  http://www.cpcwiki.eu/index.php/CPC_Palette
//  http://www.grimware.org/doku.php/documentations/devices/gatearray
//
//  index into this palette is the 'hardware color number' & 0x1F
//  order is ABGR
//
static uint32_t cpc_palette[32] = {
    0xff6B7D6E,         // #40 white
    0xff6D7D6E,         // #41 white
    0xff6BF300,         // #42 sea green
    0xff6DF3F3,         // #43 pastel yellow
    0xff6B0200,         // #44 blue
    0xff6802F0,         // #45 purple
    0xff687800,         // #46 cyan
    0xff6B7DF3,         // #47 pink
    0xff6802F3,         // #48 purple
    0xff6BF3F3,         // #49 pastel yellow
    0xff0DF3F3,         // #4A bright yellow
    0xffF9F3FF,         // #4B bright white
    0xff0605F3,         // #4C bright red
    0xffF402F3,         // #4D bright magenta
    0xff0D7DF3,         // #4E orange
    0xffF980FA,         // #4F pastel magenta
    0xff680200,         // #50 blue
    0xff6BF302,         // #51 sea green
    0xff01F002,         // #52 bright green
    0xffF2F30F,         // #53 bright cyan
    0xff010200,         // #54 black
    0xffF4020C,         // #55 bright blue
    0xff017802,         // #56 green
    0xffF47B0C,         // #57 sky blue
    0xff680269,         // #58 magenta
    0xff6BF371,         // #59 pastel green
    0xff04F571,         // #5A lime
    0xffF4F371,         // #5B pastel cyan
    0xff01026C,         // #5C red
    0xffF2026C,         // #5D mauve
    0xff017B6E,         // #5E yellow
    0xffF67B6E,         // #5F pastel blue
};

// first 32 bytes of the KC Compact color ROM
static uint32_t kcc_color_rom[32] = {
    0x15, 0x15, 0x31, 0x3d, 0x01, 0x0d, 0x11, 0x1d,
    0x0d, 0x3d, 0x3c, 0x3f, 0x0c, 0x0f, 0x1c, 0x1f,
    0x01, 0x31, 0x30, 0x33, 0x00, 0x03, 0x10, 0x13,
    0x05, 0x35, 0x34, 0x37, 0x04, 0x07, 0x14, 0x17
};

//------------------------------------------------------------------------------
void
cpc_video::init(system model_) {
    this->model = model_;
    this->rgba8_buffer = board.rgba8_buffer;

    // initialize the color palette (CPC and KC Compact have slightly different colors)
    if (system::kccompact != this->model) {
        // CPC models
        for (int i = 0; i < 32; i++) {
            this->palette[i] = cpc_palette[i];
        }
    }
    else {
        // KC Compact
        for (int i = 0; i < 32; i++) {
            uint32_t rgba8 = 0xFF000000;
            const uint8_t val = kcc_color_rom[i];
            // color bits:
            //  xx|gg|rr|bb
            //
            const uint8_t b = val & 0x03;
            const uint8_t r = (val>>2) & 0x03;
            const uint8_t g = (val>>4) & 0x03;
            if (b == 0x03)     rgba8 |= 0x00FF0000;    // full blue
            else if (b != 0)   rgba8 |= 0x007F0000;    // half blue
            if (g == 0x03)     rgba8 |= 0x0000FF00;    // full green
            else if (g != 0)   rgba8 |= 0x00007F00;    // half green
            if (r == 0x03)     rgba8 |= 0x000000FF;    // full red
            else if (r != 0)   rgba8 |= 0x0000007F;    // half red
            this->palette[i] = rgba8;
        }
    }
    this->reset();
}

//------------------------------------------------------------------------------
void
cpc_video::reset() {
    clear(this->pens, sizeof(this->pens));
    this->hsync_irq_count = 0;
    this->hsync_after_vsync_counter = 0;
    this->hsync_start_count = 0;
    this->hsync_end_count = 0;
    this->int_acknowledge_counter = 0;
    this->request_interrupt = false;
    this->next_video_mode = 1;
    this->video_mode = 1;
    this->selected_pen = 0;
    this->border_color = 0;
}

//------------------------------------------------------------------------------
static bool edge_raise(uint64_t prev_pins, uint64_t new_pins, uint64_t mask) {
    return (new_pins & mask) && !(prev_pins & mask);
}

static bool edge_fall(uint64_t prev_pins, uint64_t new_pins, uint64_t mask) {
    return !(new_pins & mask) && (prev_pins & mask);
}

void
cpc_video::handle_crtc_sync(uint64_t crtc_pins) {
    // checks the HSYNC and VSYN CRTC flags and issues CPU interrupt
    // request and system_bus vblank callback if needed
    //
    // if an interrupt was requested the request_interrupt member will
    // be true after the method returns (used for debug visualizations)

    // feed state of hsync and vsync signals into CRT, and step the CRT
    // NOTE: HSYNC from the CRTC is delayed by 2us and will last for
    // at most 4us unless CRTC HSYNC is triggered off earlier
    if (edge_raise(this->prev_crtc_pins, crtc_pins, MC6845_HS)) {
        this->hsync_start_count = 2;
    }
    if (edge_fall(this->prev_crtc_pins, crtc_pins, MC6845_HS)) {
        if (this->hsync_end_count > 2) {
            this->hsync_end_count = 2;
        }
    }
    if (edge_raise(this->prev_crtc_pins, crtc_pins, MC6845_VS)) {
        //crt.trigger_vsync();
        this->hsync_after_vsync_counter = 2;
    }
    if (this->int_acknowledge_counter > 0) {
        this->int_acknowledge_counter--;
        if (this->int_acknowledge_counter == 0) {
            // clear top bit of hsync counter, makes sure the next hsync irq
            // won't happen until at least 32 lines later
            this->hsync_irq_count &= 0x1F;
        }
    }    
    if (hsync_start_count > 0) {
        this->hsync_start_count--;
        if (this->hsync_start_count == 0) {
            //crt.trigger_hsync();
            this->hsync_end_count = 4;
        }
    }
    if (this->hsync_end_count > 0) {
        this->hsync_end_count--;
        if (0 == this->hsync_end_count) {
            // fetch next video mode
            this->video_mode = this->next_video_mode;

            this->request_interrupt = false;
            this->hsync_irq_count = (this->hsync_irq_count + 1) & 0x3F;

            // special case handling to sync interrupt requests
            // with the VSYNC (2 HSYNCs after VSYNC)
            // http://cpctech.cpc-live.com/docs/gaint.html
            if (this->hsync_after_vsync_counter != 0) {
                this->hsync_after_vsync_counter--;
                if (this->hsync_after_vsync_counter == 0) {
                    if (this->hsync_irq_count >= 32) {
                        this->request_interrupt = true;
                    }
                    this->hsync_irq_count = 0;
                }
            }
            if (this->hsync_irq_count == 52) {
                this->hsync_irq_count = 0;
                this->request_interrupt = true;
            }
        }
    }
    this->prev_crtc_pins = crtc_pins;
}

//------------------------------------------------------------------------------
void
cpc_video::decode_pixels(uint32_t* dst, uint64_t crtc_pins) {

    // compute the source address from current CRTC ma (memory address)
    // and ra (raster address) like this:
    //
    // |ma12|ma11|ra2|ra1|ra0|ma9|ma8|...|ma2|ma1|ma0|0|
    //
    // Bits ma12 and m11 point to the 16 KByte page, and all
    // other bits are the index into that page.
    //
    const uint16_t ma = MC6845_GET_ADDR(crtc_pins);
    const uint8_t ra = MC6845_GET_RA(crtc_pins);
    const uint32_t page_index  = (ma>>12) & 3;
    const uint32_t page_offset = ((ma & 0x03FF)<<1) | ((ra & 7)<<11);
    const uint8_t* src = &(board.ram[page_index][page_offset]);
    uint8_t c;
    uint32_t p;
    if (0 == this->video_mode) {
        // 160x200 @ 16 colors
        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
        for (int i = 0; i < 2; i++) {
            c = *src++;
            p = this->pens[((c>>7)&0x1)|((c>>2)&0x2)|((c>>3)&0x4)|((c<<2)&0x8)];
            *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
            p = this->pens[((c>>6)&0x1)|((c>>1)&0x2)|((c>>2)&0x4)|((c<<3)&0x8)];
            *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
        }
    }
    else if (1 == this->video_mode) {
        // 320x200 @ 4 colors
        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
        for (int i = 0; i < 2; i++) {
            c = *src++;
            p = this->pens[((c>>2)&2)|((c>>7)&1)];
            *dst++ = p; *dst++ = p;
            p = this->pens[((c>>1)&2)|((c>>6)&1)];
            *dst++ = p; *dst++ = p;
            p = this->pens[((c>>0)&2)|((c>>5)&1)];
            *dst++ = p; *dst++ = p;
            p = this->pens[((c<<1)&2)|((c>>4)&1)];
            *dst++ = p; *dst++ = p;
        }
    }
    else if (2 == this->video_mode) {
        // 640x200 @ 2 colors
        for (int i = 0; i < 2; i++) {
            c = *src++;
            for (int j = 7; j >= 0; j--) {
                *dst++ = this->pens[(c>>j)&1];
            }
        }
    }
}

//------------------------------------------------------------------------------
uint64_t
cpc_video::tick(uint64_t cpu_pins) {

    // http://cpctech.cpc-live.com/docs/ints.html
    // http://www.cpcwiki.eu/forum/programming/frame-flyback-and-interrupts/msg25106/#msg25106
    // http://www.grimware.org/doku.php/documentations/devices/gatearray#interrupt.generator
    auto& crtc = board.mc6845;
    auto& crt = board.crt;

    uint64_t crtc_pins = mc6845_tick(&crtc);
    this->handle_crtc_sync(crtc_pins);
    crt_tick(&crt, crtc_pins & MC6845_HS, crtc_pins & MC6845_VS);
    if (this->request_interrupt) {
        cpu_pins |= Z80_INT;
    }

    if (!this->debug_video) {
        if (crt.visible) {
            int dst_x = crt.pos_x * 16;
            int dst_y = crt.pos_y;
            YAKC_ASSERT((dst_x <= (max_display_width-16)) && (dst_y < max_display_height));
            uint32_t* dst = &(this->rgba8_buffer[dst_x + dst_y * max_display_width]);
            if (crtc_pins & MC6845_DE) {
                // decode visible pixels
                this->decode_pixels(dst, crtc_pins);
            }
            else if (crtc_pins & (MC6845_HS|MC6845_VS)) {
                // blacker than black
                for (int i = 0; i < 16; i++) {
                    dst[i] = 0xFF000000;
                }
            }
            else {
                // border color
                for (int i = 0; i < 16; i++) {
                    dst[i] = this->border_color;
                }
            }
        }
    }
    else {
        // debug mode
        int dst_x = crt.h_pos * 16;
        int dst_y = crt.v_pos;
        if ((dst_x < (dbg_max_display_width-16)) && (dst_y < dbg_max_display_height)) {
            uint32_t* dst = &(this->rgba8_buffer[dst_x + dst_y * dbg_max_display_width]);
            if (!(crtc_pins & MC6845_DE)) {
                uint8_t r = 0x3F;
                uint8_t g = 0x3F;
                uint8_t b = 0x3F;
                if (crtc_pins & MC6845_HS) {
                    r = 0x7F;
                }
                if (crtc_pins & MC6845_VS) {
                    g = 0x7F;
                }
                if (this->request_interrupt) {
                    r = g = b = 0xFF;
                }
                else if (0 == crtc.scanline_ctr) {
                    r = g = b = 0x00;
                }
                if (crt.h_blank || crt.v_blank) {
                    r >>= 1;
                    g >>= 1;
                    b >>= 1;
                }
                for (int i = 0; i < 16; i++) {
                    if (i == 0) {
                        *dst++ = 0xFF000000;
                    }
                    else {
                        *dst++ = 0xFF<<24 | b<<16 | g<<8 | r;
                    }
                }
            }
            else {
                this->decode_pixels(dst, crtc_pins);
            }
        }
    }
    return cpu_pins;
}

} // namespace YAKC
