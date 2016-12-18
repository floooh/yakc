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
    this->vsync_flag = false;
    clear(this->pens, sizeof(this->pens));

    // reset CRTC registers
    // http://www.cpcwiki.eu/index.php/CRTC
    this->crtc.selected = 0;

    clear(this->crtc.regs, sizeof(this->crtc.regs));
    this->crtc.regs[crtc_t::HORI_TOTAL] = 63;
    this->crtc.regs[crtc_t::HORI_DISPLAYED] = 40;
    this->crtc.regs[crtc_t::HORI_SYNC_POS] = 46;
    this->crtc.regs[crtc_t::SYNC_WIDTHS] = 128 + 14;
    this->crtc.regs[crtc_t::VERT_TOTAL] = 38;
    this->crtc.regs[crtc_t::VERT_DISPLAYED] = 25;
    this->crtc.regs[crtc_t::VERT_SYNC_POS] = 30;
    this->crtc.regs[crtc_t::MAX_RASTER_ADDR] = 7;
    this->crtc.regs[crtc_t::DISPLAY_START_ADDR_HI] = 32;

    this->crtc.mask[crtc_t::HORI_TOTAL] = 0xFF;
    this->crtc.mask[crtc_t::HORI_DISPLAYED] = 0xFF;
    this->crtc.mask[crtc_t::HORI_SYNC_POS] = 0xFF;
    this->crtc.mask[crtc_t::SYNC_WIDTHS] = 0xFF;
    this->crtc.mask[crtc_t::VERT_TOTAL] = 0x7F;
    this->crtc.mask[crtc_t::VERT_TOTAL_ADJUST] = 0x1F;
    this->crtc.mask[crtc_t::VERT_DISPLAYED] = 0x7F;
    this->crtc.mask[crtc_t::VERT_SYNC_POS] = 0x7F;
    this->crtc.mask[crtc_t::INTERLACE_AND_SKEW] = 0x03;
    this->crtc.mask[crtc_t::MAX_RASTER_ADDR] = 0x1F;
    this->crtc.mask[crtc_t::CURSOR_START_RASTER] = 0x7F;
    this->crtc.mask[crtc_t::CURSOR_END_RASTER] = 0x1F;
    this->crtc.mask[crtc_t::DISPLAY_START_ADDR_HI] = 0x3F;
    this->crtc.mask[crtc_t::DISPLAY_START_ADDR_LO] = 0xFF;
    this->crtc.mask[crtc_t::CURSOR_ADDR_HI] = 0x3F;
    this->crtc.mask[crtc_t::CURSOR_ADDR_LO] = 0xFF;
    this->crtc.mask[crtc_t::LIGHTPEN_ADDR_HI] = 0x3F;
    this->crtc.mask[crtc_t::LIGHTPEN_ADDR_LO] = 0xFF;
}

//------------------------------------------------------------------------------
void
cpc_video::update(z80bus* bus, int cycles) {

    // update current scanline-cycle-counter, and on wrap-around
    // call the scanline function
    // FIXME: the length of a scanline is actually determined by CRTC register settings!
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
    // http://www.cpcwiki.eu/forum/programming/frame-flyback-and-interrupts/msg25106/#msg25106

    // FIXME! take border into account
    const int visible_scan_lines = (this->crtc.regs[crtc_t::MAX_RASTER_ADDR]+1) * this->crtc.regs[crtc_t::VERT_DISPLAYED];
    if (this->scanline_count < visible_scan_lines) {
        this->decode_scanline(this->scanline_count);
    }

    // issue an interrupt request every 52 scan lines
    // FIXME: when the CPU acknowledges the interrupt, bit 5 of the
    // hsync_counter must be cleared
    // FIXME #2: gate array interrupt control flagcalc
    //
    // see: http://www.retroisle.com/amstrad/cpc/Technical/hardware_Interrupts.php
    //      http://cpctech.cpc-live.com/docs/ints.html
    //
    // NOTE: for color flashing to work, an interrupt must happen while
    // the interrupt is processed by the CPU, with standard CRTC register
    // values, this would happen at scanline 260 (the 5th HSYNC of the frame)
    if (++this->hsync_count == 52) {
        this->hsync_count = 0;
        bus->irq();
    }

    // set the vsync bit
    // http://www.cpcwiki.eu/index.php/CRTC
    const uint32_t char_height = this->crtc.regs[crtc_t::MAX_RASTER_ADDR] + 1;
    const int min_vsync_line = this->crtc.regs[crtc_t::VERT_SYNC_POS] * char_height;

    // NOTE: the vsync 'duration' is fixed on 'old' CPCs, the docs say 16 lines, but
    // with this it will not be active during when a HSYNC interrupt is triggered
    // (on scanline 260), thus we double the number of lines, so that the
    // 5th IRQ on the frame sees an active vsync_flag
    const int max_vsync_line = min_vsync_line + 32;
    if ((this->scanline_count >= min_vsync_line) && (this->scanline_count < max_vsync_line)) {
        this->vsync_flag = true;
    }
    else {
        this->vsync_flag = false;
    }

    // start new frame?
    const int max_scanlines = 312;
    if (++this->scanline_count == max_scanlines) {
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
    //  http://www.cpcwiki.eu/index.php/CRTC
    //  http://cpctech.cpc-live.com/docs/screen.html
    //  http://www.grimware.org/doku.php/documentations/devices/crtc
    //
    // 32 KByte overscan buffers
    //

    // compute the current CRTC RA register
    const uint32_t char_height = this->crtc.regs[crtc_t::MAX_RASTER_ADDR] + 1;
    const uint32_t ra = y % char_height;
    const uint32_t ra_offset = ((ra & 7)<<11); // memory address offset contributed by RA

    // compute the current CRTC MA register, and multiply by 2 (because
    // 2 bytes per CRTC cycle are read), MA and RA are used to compute
    // the current video memory address
    uint32_t ma2 = this->crtc.regs[crtc_t::DISPLAY_START_ADDR_HI]<<8;
    ma2 += this->crtc.regs[crtc_t::DISPLAY_START_ADDR_LO];
    ma2 += (y / char_height) * this->crtc.regs[crtc_t::HORI_DISPLAYED];
    ma2 *= 2;
    const uint32_t line_num_bytes = this->crtc.regs[crtc_t::HORI_DISPLAYED] * 2;
    uint32_t p;
    uint32_t* dst = &(this->rgba8_buffer[y * 640]);

    // http://cpctech.cpc-live.com/docs/graphics.html
    if (0 == mode) {
        // 160x200 @ 16 colors
        //
        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
        //
        // FIXME: dst could overflow here!!!
        for (uint32_t x = 0; x < line_num_bytes; x++, ma2++) {
            const uint32_t page_index = (ma2>>13) & 3;
            const uint32_t page_offset = ra_offset | (ma2 & 0x7FF);

            const uint8_t val = this->board->ram[page_index][page_offset];
            p = this->pens[((val>>7)&0x1)|((val>>2)&0x2)|((val>>3)&0x4)|((val<<2)&0x8)];
            *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
            p = this->pens[((val>>6)&0x1)|((val>>1)&0x2)|((val>>2)&0x4)|((val<<3)&0x8)];
            *dst++ = p; *dst++ = p; *dst++ = p; *dst++ = p;
        }
    }
    else if (1 == mode) {
        // 320x200 @ 4 colors
        //
        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
        //
        // FIXME: dst could overflow here!!!
        for (uint32_t x = 0; x < line_num_bytes; x++, ma2++) {
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
    else if (2 == mode) {
        // 640x200 @ 2 colors
        //
        // FIXME: dst could overflow here!!!
        for (uint32_t x = 0; x < line_num_bytes; x++, ma2++) {
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
    else if (3 == mode) {
        // FIXME: undocumented 160x200 @ 4 colors (not on KC compact)
    }
}

//------------------------------------------------------------------------------
void
cpc_video::select_pen(ubyte val) {
    this->selected_pen = val & 0x1F;
}

//------------------------------------------------------------------------------
void
cpc_video::assign_color(ubyte val) {
    if (this->selected_pen & 0x10) {
        // set border color
        this->border_color = hw_palette[val & 0x1F];
    }
    else {
        // set pen
        this->pens[this->selected_pen & 0x0F] = hw_palette[val & 0x1F];
    }
}

//------------------------------------------------------------------------------
void
cpc_video::set_mode(ubyte val) {
    this->mode = val & 3;
}

//------------------------------------------------------------------------------
void
cpc_video::select_crtc(ubyte val) {
    this->crtc.selected = val;
}

//------------------------------------------------------------------------------
void
cpc_video::write_crtc(ubyte val) {
    if (this->crtc.selected < crtc_t::NUM_REGS) {
        this->crtc.regs[this->crtc.selected] = val & this->crtc.mask[this->crtc.selected];
    }
}

//------------------------------------------------------------------------------
ubyte
cpc_video::read_crtc() const {
    if ((this->crtc.selected >= 12) && (this->crtc.selected < 18)) {
        return this->crtc.regs[this->crtc.selected];
    }
    else {
        // write only register, or outside register range
        return 0;
    }
}

//------------------------------------------------------------------------------
bool
cpc_video::vsync_bit() const {
    return this->vsync_flag;
}

} // namespace YAKC
