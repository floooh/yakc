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
    this->selected_pen = 0;
    this->border_color = 0;
    clear(this->pens, sizeof(this->pens));

    // reset CRTC registers
    // http://www.cpcwiki.eu/index.php/CRTC
    this->crtc = crtc_t();
    this->crtc.regs[crtc_t::HORI_TOTAL] = 0x3F;
    this->crtc.regs[crtc_t::HORI_DISPLAYED] = 0x28;
    this->crtc.regs[crtc_t::HORI_SYNC_POS] = 0x2E;
    this->crtc.regs[crtc_t::SYNC_WIDTHS] = 0x8E;
    this->crtc.regs[crtc_t::VERT_TOTAL] = 0x26;
    this->crtc.regs[crtc_t::VERT_DISPLAYED] = 0x19;
    this->crtc.regs[crtc_t::VERT_SYNC_POS] = 0x1E;
    this->crtc.regs[crtc_t::MAX_RASTER_ADDR] = 0x07;
    this->crtc.regs[crtc_t::DISPLAY_START_ADDR_HI] = 0x30;

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

    this->update_crtc_values();
}

//------------------------------------------------------------------------------
void
cpc_video::update_crtc_values() {
    crtc.dirty = false;
    
    // length of a scanline in CPU cycles
    crtc.scanline_end = (crtc.regs[crtc_t::HORI_TOTAL]+1) * 4;
    // start of HSYNC signal inside scanline in CPU cycles
    crtc.hsync_start = crtc.regs[crtc_t::HORI_SYNC_POS] * 4;
    // end of HSYNC signal inside scanline in CPU cycles
    crtc.hsync_end = crtc.hsync_start + ((crtc.regs[crtc_t::SYNC_WIDTHS]&0xF) * 4);
    // height of a character row in scanlines
    crtc.row_height = crtc.regs[crtc_t::MAX_RASTER_ADDR] + 1;
    // start of VSYNC signal in scanlines
    crtc.vsync_start = crtc.regs[crtc_t::VERT_SYNC_POS] * crtc.row_height;
    // end of VSYNC signal in scanlines
    // FIXME: this is hardwired to 16, but this does not seem to be enough
    // for the HSYNC interrupt to detect the VSYNC bit, so extend the length
    // for the VSYNC flag...
    const int vblank_lines = 16;
    crtc.vsync_end = crtc.vsync_start + vblank_lines;
    crtc.vsync_irq_end = crtc.vsync_end + 16;
    // end of PAL frame
    crtc.frame_end = crtc.regs[crtc_t::VERT_TOTAL]*crtc.row_height + crtc.regs[crtc_t::VERT_TOTAL_ADJUST];
    // number of visible scanlines
    crtc.visible_scanlines = crtc.regs[crtc_t::VERT_DISPLAYED] * crtc.row_height;

    // compute left border width (in emulator pixels)
    const int total_scanline_pixels = crtc.scanline_end * 4;
    const int hsync_end_pixels = crtc.hsync_end * 4;
    crtc.left_border_width = (total_scanline_pixels - hsync_end_pixels);
    if (crtc.left_border_width < 0) {
        crtc.left_border_width = 0;
    }
    // width of visible area in emulator framebuffer pixels
    crtc.visible_width = crtc.regs[crtc_t::HORI_DISPLAYED] * 8 * 2;
    if ((crtc.left_border_width + crtc.visible_width) >= max_display_width) {
        crtc.visible_width = max_display_width - crtc.left_border_width;
    }
}

//------------------------------------------------------------------------------
void
cpc_video::update(z80bus* bus, int cycles) {

    // http://cpctech.cpc-live.com/docs/ints.html
    // http://www.cpcwiki.eu/forum/programming/frame-flyback-and-interrupts/msg25106/#msg25106

    // update the scanline-cycle-counter
    crtc.scanline_cycle_count += cycles;

    // update HSYNC flag, and check if interrupt must be requested
    crtc.hsync = (crtc.scanline_cycle_count >= crtc.hsync_start) &&
                 (crtc.scanline_cycle_count < crtc.hsync_end);
    if (crtc.hsync && !crtc.hsync_triggered) {
        crtc.hsync_triggered = true;
        crtc.hsync_irq_count++;

        // request interrupt every 52 scanlines
        //
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
        if (crtc.hsync_irq_count == 52) {
            crtc.hsync_irq_count = 0;
            bus->irq();
        }
    }

    // end of scanline reached? then wraparound and start new scanline
    if (crtc.scanline_cycle_count >= crtc.scanline_end) {
        crtc.hsync_triggered = false;
        crtc.scanline_cycle_count %= crtc.scanline_end;

        if (crtc.palline_count < max_display_height) {
            if (crtc.scanline_count < crtc.visible_scanlines) {
                // decode visible scanline
                this->decode_visible_scanline(crtc.scanline_count, crtc.palline_count);
            }
            else {
                // render border
                this->decode_border_scanline(crtc.palline_count);
            }
        }

        // bump (src-)scanline and (dst-)pal-line
        crtc.scanline_count++;

        // scanline inside VSYNC area?
        crtc.vsync = (crtc.scanline_count >= crtc.vsync_start) &&
                     (crtc.scanline_count < crtc.vsync_irq_end);
        if (crtc.vsync && !crtc.vsync_triggered) {
            crtc.vsync_triggered = true;
            bus->vblank();
        }

        // wrap around pal-line
        crtc.palline_count++;
        const int skip_vert = (crtc.frame_end - max_display_height) / 2;
        if (crtc.scanline_count == (crtc.vsync_end + skip_vert)) {
            // rewind PAL line counter to top of screen
            crtc.palline_count = 0;
        }

        // wrap around scanline
        if (crtc.scanline_count == crtc.frame_end) {
            crtc.scanline_count = 0;
            crtc.vsync_triggered = false;
        }

        // update CRTC values?
        if (crtc.dirty) {
            this->update_crtc_values();
        }
    }
}

//------------------------------------------------------------------------------
void
cpc_video::decode_border_scanline(int dst_y) {
    YAKC_ASSERT((dst_y >= 0) && (dst_y < max_display_height));
    uint32_t* dst = &(this->rgba8_buffer[dst_y * max_display_width]);
    for (int i = 0; i < max_display_width; i++) {
        *dst++ = this->border_color;
    }
}

//------------------------------------------------------------------------------
void
cpc_video::decode_visible_scanline(int src_y, int dst_y) {
    YAKC_ASSERT(dst_y < max_display_height);
    //
    // NOTE:
    //
    // mode 0: 160x200      2 pixels per byte
    // mode 1: 320x200      4 pixels per byte
    // mode 2: 640x200      8 pixels per byte
    //
    //  http://www.cpcwiki.eu/index.php/CRTC
    //  http://cpctech.cpc-live.com/docs/screen.html
    //  http://www.grimware.org/doku.php/documentations/devices/crtc
    //
    // FIXME: 32 KByte overscan buffers
    //
    uint32_t* dst = &(this->rgba8_buffer[dst_y * max_display_width]);

    // compute the current CRTC RA register
    // (contributes bits 11,12,13 of the video memory address)
    const uint32_t ra = src_y % crtc.row_height;
    const uint32_t ra_offset = ((ra & 7)<<11); // memory address offset contributed by RA

    // compute the current CRTC MA register, and multiply by 2 (because
    // 2 bytes per CRTC cycle are read), MA and RA are used to compute
    // the current video memory address
    uint32_t ma2 = crtc.regs[crtc_t::DISPLAY_START_ADDR_HI]<<8;
    ma2 += crtc.regs[crtc_t::DISPLAY_START_ADDR_LO];
    ma2 += (src_y / crtc.row_height) * crtc.regs[crtc_t::HORI_DISPLAYED];
    ma2 *= 2;

    // fill left border
    for (int i = 0; i < crtc.left_border_width; i++) {
        *dst++ = this->border_color;
    }

    // http://cpctech.cpc-live.com/docs/graphics.html
    uint32_t p;
    const uint32_t line_num_bytes = crtc.visible_width / 8;
    if (0 == mode) {
        // 160x200 @ 16 colors
        //
        // pixel    bit mask
        // 0:       |3|7|
        // 1:       |2|6|
        // 2:       |1|5|
        // 3:       |0|4|
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

    // fill right border
    for (int i = (crtc.left_border_width + crtc.visible_width); i < max_display_width; i++) {
        *dst++ = this->border_color;
    }
    YAKC_ASSERT(dst == &(this->rgba8_buffer[dst_y * max_display_width]) + max_display_width);
    YAKC_ASSERT(dst <= &this->rgba8_buffer[max_display_width * max_display_height]);
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
cpc_video::set_video_mode(ubyte val) {
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
        this->crtc.dirty = true;
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
    return this->crtc.vsync;
}

} // namespace YAKC
