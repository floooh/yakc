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
    0xff6BF300,         // #42 sea grean
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
cpc_video::init(device model_, breadboard* board_) {
    this->model = model_;
    this->board = board_;
    this->crtc.init(mc6845::type::MC6845);

    // initialize the color palette (CPC and KC Compact have slightly different colors)
    if (device::kccompact != this->model) {
        // CPC models
        for (int i = 0; i < 32; i++) {
            this->palette[i] = cpc_palette[i];
        }
    }
    else {
        // KC Compact
        for (int i = 0; i < 32; i++) {
            uint32_t rgba8 = 0xFF000000;
            const ubyte val = kcc_color_rom[i];
            // color bits:
            //  xx|gg|rr|bb
            //
            const ubyte b = val & 0x03;
            const ubyte r = (val>>2) & 0x03;
            const ubyte g = (val>>4) & 0x03;
            if (b == 0x03)      rgba8 |= 0x00FF0000;    // full blue
            else if (b != 0)    rgba8 |= 0x007F0000;    // half blue
            if (g == 0x03)      rgba8 |= 0x0000FF00;    // full green
            else if (g != 0)    rgba8 |= 0x00007F00;    // half green
            if (r == 0x03)      rgba8 |= 0x000000FF;    // full red
            else if (r != 0)    rgba8 |= 0x0000007F;    // half red
            this->palette[i] = rgba8;
        }
    }
    this->reset();
}

//------------------------------------------------------------------------------
void
cpc_video::reset() {
    this->selected_pen = 0;
    this->border_color = 0;
    clear(this->pens, sizeof(this->pens));
    this->crtc.reset();
}

//------------------------------------------------------------------------------
void
cpc_video::step(system_bus* bus, int cycles) {

    // http://cpctech.cpc-live.com/docs/ints.html
    // http://www.cpcwiki.eu/forum/programming/frame-flyback-and-interrupts/msg25106/#msg25106
    // http://www.grimware.org/doku.php/documentations/devices/gatearray#interrupt.generator

    this->crtc_cycle_count -= cycles;
    while (this->crtc_cycle_count <= 0) {
        this->crtc_cycle_count += 4;
        this->crtc.step();

        // interrupt is generated at HSYNC's falling edge
        if (this->crtc.off(mc6845::HSYNC)) {
            this->hsync_irq_count = (this->hsync_irq_count + 1) & 0x3F;

            // special handling 2 HSYNCs after VSYNC
            if (this->hsync_after_vsync_counter != 0) {
                this->hsync_after_vsync_counter--;
                if (this->hsync_after_vsync_counter == 0) {
                    if (this->hsync_irq_count & (1<<5)) {
                        // ...if counter is >=32 (bit5=1), no interrupt request is issued,
                        // and the counter is reset to 0
                        this->hsync_irq_count = 0;
                    }
                    else {
                        // ...if counter is <32 (bit5=0), generate an interrupt request,
                        // and the counter is reset to 0
                        bus->irq();
                        this->hsync_irq_count = 0;
                    }
                }
            }
            if (this->hsync_irq_count == 52) {
                this->hsync_irq_count = 0;
                bus->irq();
            }
        }

        if (this->crtc.on(mc6845::VSYNC)) {
            this->hsync_after_vsync_counter = 3;
            bus->vblank();
        }

        if (this->crtc.on(mc6845::DISPEN_H)) {
            this->dst_x = 0;
            this->dst_y++;
        }
        if (this->crtc.on(mc6845::DISPEN_V)) {
            this->dst_y = 0;
        }
        if ((this->dst_x < max_display_width) && (this->dst_y < max_display_height)) {
            // decode the next 2 pixels
            uint32_t* dst = &(this->rgba8_buffer[this->dst_x + this->dst_y * max_display_width]);
            this->dst_x += 16;

            uint32_t ma2 = this->crtc.ma * 2;
            const uint32_t ra = this->crtc.ra;
            const uint32_t ra_offset = ((ra & 7)<<11); // memory address offset contributed by RA
            uint32_t p;

            if (!this->crtc.test(mc6845::DISPEN)) {
                p = 0xFF2F2F2F;
                if (this->crtc.test(mc6845::HSYNC)) {
                    p |= 0xFF00007F;
                }
                if (this->crtc.test(mc6845::VSYNC)) {
                    p |= 0xFF007F00;
                }
                for (int i = 0; i < 16; i++) {
                    *dst++ = p;
                }
            }
            else {
                if (0 == this->mode) {
                    // 160x200 @ 16 colors
                    // pixel    bit mask
                    // 0:       |3|7|
                    // 1:       |2|6|
                    // 2:       |1|5|
                    // 3:       |0|4|
                    for (int i = 0; i < 2; i++, ma2++) {
                        const uint32_t page_index = (ma2>>13) & 3;
                        const uint32_t page_offset = ra_offset | (ma2 & 0x7FF);
                        const uint8_t val = this->board->ram[page_index][page_offset];
                        p = this->pens[((val>>7)&0x1)|((val>>2)&0x2)|((val>>3)&0x4)|((val<<2)&0x8)];
                        *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
                        p = this->pens[((val>>6)&0x1)|((val>>1)&0x2)|((val>>2)&0x4)|((val<<3)&0x8)];
                        *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
                    }
                }
                else if (1 == this->mode) {
                    // 320x200 @ 4 colors
                    // pixel    bit mask
                    // 0:       |3|7|
                    // 1:       |2|6|
                    // 2:       |1|5|
                    // 3:       |0|4|
                    for (int i = 0; i < 2; i++, ma2++) {
                        const uint32_t page_index = (ma2>>13) & 3;
                        const uint32_t page_offset = ra_offset | (ma2 & 0x7FF);
                        const uint8_t val = this->board->ram[page_index][page_offset];
                        p = this->pens[((val>>2)&2)|((val>>7)&1)];
                        *dst++ = p; *dst++ = p;
                        p = this->pens[((val>>1)&2)|((val>>6)&1)];
                        *dst++ = p; *dst++ = p;
                        p = this->pens[((val>>0)&2)|((val>>5)&1)];
                        *dst++ = p; *dst++ = p;
                        p = this->pens[((val<<1)&2)|((val>>4)&1)];
                        *dst++ = p; *dst++ = p;
                    }
                }
                else if (2 == this->mode) {
                    // 640x200 @ 2 colors
                    for (int i = 0; i < 2; i++, ma2++) {
                        const uint32_t page_index = (ma2>>13) & 3;
                        const uint32_t page_offset = ra_offset | (ma2 & 0x7FF);
                        const uint8_t val = this->board->ram[page_index][page_offset];
                        *dst++ = this->pens[(val>>7)&1];
                        *dst++ = this->pens[(val>>6)&1];
                        *dst++ = this->pens[(val>>5)&1];
                        *dst++ = this->pens[(val>>4)&1];
                        *dst++ = this->pens[(val>>3)&1];
                        *dst++ = this->pens[(val>>2)&1];
                        *dst++ = this->pens[(val>>1)&1];
                        *dst++ = this->pens[(val>>0)&1];
                    }
                }
            }
        }
    }
}

} // namespace YAKC
