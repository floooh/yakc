//------------------------------------------------------------------------------
//  mc6845.cc
//------------------------------------------------------------------------------
#include "mc6845.h"

namespace YAKC {

static ubyte reg_bits[mc6845::NUM_REGS] = {
    0xFF,   // H_TOTAL
    0xFF,   // H_DISPLAYED
    0xFF,   // H_SYNC_POS
    0x0F,   // H_SYNC_WIDTH
    0x7F,   // V_TOTAL
    0x1F,   // V_TOTAL_ADJUST
    0x7F,   // V_DISPLAYED
    0x7F,   // V_SYNC_POS
    0x03,   // INTERLACE_MODE
    0x1F,   // MAX_SCANLINE_ADDR
    0x7F,   // CURSOR_START
    0x1F,   // CURSOR_END
    0x3F,   // START_ADDR_HI
    0xFF,   // START_ADDR_LO
    0x3F,   // CURSOR_HI
    0xFF,   // CURSOR_LO
    0x3F,   // LIGHTPEN_HI
    0xFF,   // LIGHTPEN_LO
};

// 1: writable, 2: readable, 3: read/write
static ubyte reg_access[int(mc6845::type::NUM)][mc6845::NUM_REGS] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 2, 2 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 2, 2 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 2, 2 }
};

//------------------------------------------------------------------------------
void
mc6845::init(enum type t) {
    YAKC_ASSERT(t < type::NUM);
    this->type = int(t);
    this->reset();
    for (int i = 0; i < NUM_REGS; i++) {
        this->regs[i] = 0;
    }
}

//------------------------------------------------------------------------------
void
mc6845::reset() {
    // NOTE: control registers are not cleared on reset 
    this->bits = 0;
    this->prev_bits = 0;
    this->ma = 0;
    this->ra = 0;
    this->h_count = 0;
    this->hsync_count = 0;
    this->row_count = 0;
    this->vsync_count = 0;
    this->scanline_count = 0;
    this->adjust_scanline_count = 0;
    this->set(DISPEN_H|DISPEN_V);
}

//------------------------------------------------------------------------------
void
mc6845::select(int r) {
    this->reg_sel = r & 0x1F;
}

//------------------------------------------------------------------------------
void
mc6845::write(ubyte val) {
    if (this->reg_sel < NUM_REGS) {
        // writable register?
        if (reg_access[this->type][this->reg_sel] & 1) {
            this->regs[this->reg_sel] = val & reg_bits[this->reg_sel];
        }
    }
}

//------------------------------------------------------------------------------
ubyte
mc6845::read() const {
    if (this->reg_sel < NUM_REGS) {
        // readable register?
        if (reg_access[this->type][this->reg_sel] & 2) {
            return this->regs[this->reg_sel];
        }
    }
    // special case r31 on type 1
    if ((1 == this->type) && (this->reg_sel == 31)) {
        return 0xFF;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
void
mc6845::step() {
    this->prev_bits = this->bits;

    // pre-update checks
    if (this->row_count == this->regs[V_SYNC_POS]) {
        this->set(VSYNC);
    }

    // update counters
    this->h_count = (this->h_count + 1) & 0xFF;
    this->ma = (this->ma + 1) & 0x3FFF;
    if (this->test(HSYNC)) {
        this->hsync_count++;
    }
    if (this->h_count >= (1 + this->regs[H_TOTAL])) {
        // new scanline
        this->h_count = 0;
        if (this->test(VSYNC)) {
            this->vsync_count++;
        }
        this->scanline_count++;
        if (this->scanline_count >= (1 + this->regs[MAX_SCANLINE_ADDR])) {
            // new character row
            this->scanline_count = 0;
            this->row_count = (this->row_count + 1) & 0xFF;
            this->ma_row_start += this->regs[H_DISPLAYED];
        }
        if (this->row_count >= (1 + this->regs[V_TOTAL])) {
            if (this->adjust_scanline_count >= this->regs[V_TOTAL_ADJUST]) {
                // new frame
                this->row_count = 0;
                this->scanline_count = 0;
                this->adjust_scanline_count = 0;
                this->ma_row_start = (this->regs[START_ADDR_HI]<<8) | this->regs[START_ADDR_LO];
            }
            else {
                this->adjust_scanline_count++;
            }
        }
        this->ma = this->ma_row_start;
        this->ra = this->scanline_count;
    }

    // horizontal status bits
    if (this->h_count == this->regs[H_SYNC_POS]) {
        this->set(HSYNC);
        this->hsync_count = 0;
    }    
    if (this->h_count == 0) {
        this->clear(HSYNC);
        this->set(DISPEN_H);
    }
    if (this->h_count == this->regs[H_DISPLAYED]) {
        this->clear(DISPEN_H);
    }
    if (this->hsync_count == this->regs[H_SYNC_WIDTH]) {
        this->clear(HSYNC);
        this->hsync_count = 0;
    }

    // vertical status bits
    if (this->row_count == 0) {
        this->set(DISPEN_V);
    }
    if (this->row_count == this->regs[V_DISPLAYED]) {
        this->clear(DISPEN_V);
    }
    if (this->vsync_count == 16) {
        this->clear(VSYNC);
        this->vsync_count = 0;
    }

    // set DISPEN status
    if ((this->bits & (DISPEN_V|DISPEN_H)) == (DISPEN_V|DISPEN_H)) {
        this->set(DISPEN);
    }
    else {
        this->clear(DISPEN);
    }
}

} // namespace YAKC
