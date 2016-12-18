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
uint32_t hw_palette[32] = {
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

//------------------------------------------------------------------------------
void
cpc_video::init(breadboard* board_) {
    this->board = board_;
    this->reset();
}

//------------------------------------------------------------------------------
void
cpc_video::reset() {
    this->cycle_count = scanline_cycles;
    this->scanline_count = 0;
    this->hsync_count = 0;
    this->selected_pen = 0;
    this->border_color = 0;
    clear(this->pens, sizeof(this->pens));
}

//------------------------------------------------------------------------------
void
cpc_video::update(z80bus* bus, int cycles) {

    // update current scanline-cycle-counter, and on wrap-around
    // call the scanline function
    this->cycle_count -= cycles;
    while (this->cycle_count <= 0) {
        this->cycle_count += scanline_cycles;
        this->scanline(bus);
    }
}

//------------------------------------------------------------------------------
void
cpc_video::scanline(z80bus* bus) {
    // http://cpctech.cpc-live.com/docs/ints.html

    // FIXME! take border into account
    if (this->scanline_count < 200) {
        this->decode_scanline(this->scanline_count);
    }

    // issue an interrupt request every 52 scan lines
    if (++this->hsync_count == 52) {
        this->hsync_count = 0;
        bus->irq();
    }

    // start new frame?
    if (++this->scanline_count == 312) {
        // start new frame, fetch next key mask
        bus->vblank();
        this->scanline_count = 0;
    }
}

//------------------------------------------------------------------------------
void
cpc_video::decode_scanline(uint16_t y) {
    //
    // mode 0: 160x200      2 pixels per byte
    // mode 1: 320x200      4 pixels per byte
    // mode 2: 640x200      8 pixels per byte
    //
    uint32_t y_offset = (y&7)*2048 + (y>>3)*80;
    uint32_t* dst = &(this->rgba8_buffer[y * 640]);
    const uint8_t* src = &(this->board->ram[3][y_offset]);
    uint32_t p;
    for (uint32_t x = 0; x < 80; x++) {
        const uint8_t val = src[x];

        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
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

//------------------------------------------------------------------------------
void
cpc_video::select_pen(uint32_t val) {
    this->selected_pen = val & 0x1F;
}

//------------------------------------------------------------------------------
void
cpc_video::assign_color(uint32_t val) {
    if (this->selected_pen & 0x10) {
        // set border color
        this->border_color = hw_palette[val & 0x1F];
    }
    else {
        // set pen
        this->pens[this->selected_pen & 0x0F] = hw_palette[val & 0x1F];
    }
}

} // namespace YAKC
