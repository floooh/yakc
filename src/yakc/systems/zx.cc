//------------------------------------------------------------------------------
//  zx.cc
//
//  TODO
//  - wait states when CPU accesses 'contended memory' and IO ports
//  - reads from port 0xFF must return 'current VRAM byte'
//  - video decoding only has scanline accuracy, not pixel accuracy
//
//  https://www.worldofspectrum.org/faq/reference/48kreference.htm
//  https://www.worldofspectrum.org/faq/reference/128kreference.htm
//  http://problemkaputt.de/zxdocs.htm
//------------------------------------------------------------------------------
#include "zx.h"
#include "yakc/util/filetypes.h"

namespace YAKC {

zx_t zx;

uint32_t zx_t::palette[8] = {
    0xFF000000,     // black
    0xFFFF0000,     // blue
    0xFF0000FF,     // red
    0xFFFF00FF,     // magenta
    0xFF00FF00,     // green
    0xFFFFFF00,     // cyan
    0xFF00FFFF,     // yellow
    0xFFFFFFFF,     // white
};

//------------------------------------------------------------------------------
bool
zx_t::check_roms(system model, os_rom os) {
    if (system::zxspectrum48k == model) {
        return roms.has(rom_images::zx48k);
    }
    else if (system::zxspectrum128k == model) {
        return roms.has(rom_images::zx128k_0) && roms.has(rom_images::zx128k_1);
    }
    return false;
}

//------------------------------------------------------------------------------
void
zx_t::init_memorymap() {
    mem_unmap_all(&board.mem);
    if (system::zxspectrum48k == this->cur_model) {
        // 48k RAM between 0x4000 and 0xFFFF
        mem_map_ram(&board.mem, 0, 0x4000, 0x4000, board.ram[0]);
        mem_map_ram(&board.mem, 0, 0x8000, 0x4000, board.ram[1]);
        mem_map_ram(&board.mem, 0, 0xC000, 0x4000, board.ram[2]);

        // 16k ROM between 0x0000 and 0x3FFF
        YAKC_ASSERT(roms.has(rom_images::zx48k) && (roms.size(rom_images::zx48k) == 0x4000));
        mem_map_rom(&board.mem, 0, 0x0000, 0x4000, roms.ptr(rom_images::zx48k));
    }
    else {
        // Spectrum 128k initial memory mapping
        mem_map_ram(&board.mem, 0, 0x4000, 0x4000, board.ram[5]);
        mem_map_ram(&board.mem, 0, 0x8000, 0x4000, board.ram[2]);
        mem_map_ram(&board.mem, 0, 0xC000, 0x4000, board.ram[0]);
        YAKC_ASSERT(roms.has(rom_images::zx128k_0) && (roms.size(rom_images::zx128k_0) == 0x4000));
        YAKC_ASSERT(roms.has(rom_images::zx128k_1) && (roms.size(rom_images::zx128k_1) == 0x4000));
        mem_map_rom(&board.mem, 0, 0x0000, 0x4000, roms.ptr(rom_images::zx128k_0));
    }
}

//------------------------------------------------------------------------------
void
zx_t::init_keymap() {
    // caps-shift is column 0, line 0
    kbd_register_modifier(&board.kbd, 0, 0, 0);
    // sym-shift is column 7, line 1
    kbd_register_modifier(&board.kbd, 1, 7, 1);
    // alpha-numeric keys
    const char* keymap =
        // no shift
        " zxcv"         // A8       shift,z,x,c,v
        "asdfg"         // A9       a,s,d,f,g
        "qwert"         // A10      q,w,e,r,t
        "12345"         // A11      1,2,3,4,5
        "09876"         // A12      0,9,8,7,6
        "poiuy"         // A13      p,o,i,u,y
        " lkjh"         // A14      enter,l,k,j,h
        "  mnb"         // A15      space,symshift,m,n,b

        // shift
        " ZXCV"         // A8
        "ASDFG"         // A9
        "QWERT"         // A10
        "     "         // A11
        "     "         // A12
        "POIUY"         // A13
        " LKJH"         // A14
        "  MNB"         // A15

        // symshift
        " : ?/"         // A8
        "     "         // A9
        "   <>"         // A10
        "!@#$%"         // A11
        "_)('&"         // A12
        "\";   "        // A13
        " =+-^"         // A14
        "  .,*";        // A15
    for (int layer = 0; layer < 3; layer++) {
        for (int col = 0; col < 8; col++) {
            for (int line = 0; line < 5; line++) {
                const uint8_t c = keymap[layer*40 + col*5 + line];
                if (c != 0x20) {
                    kbd_register_key(&board.kbd, c, col, line, (layer>0) ? (1<<(layer-1)) : 0);
                }
            }
        }
    }

    // special keys
    kbd_register_key(&board.kbd, ' ', 7, 0, 0);  // Space
    kbd_register_key(&board.kbd, 0x0F, 7, 1, 0); // SymShift
    kbd_register_key(&board.kbd, 0x08, 3, 4, 1); // Cursor Left (Shift+5)
    kbd_register_key(&board.kbd, 0x0A, 4, 4, 1); // Cursor Down (Shift+6)
    kbd_register_key(&board.kbd, 0x0B, 4, 3, 1); // Cursor Up (Shift+7)
    kbd_register_key(&board.kbd, 0x09, 4, 2, 1); // Cursor Right (Shift+8)
    kbd_register_key(&board.kbd, 0x07, 3, 0, 1); // Edit (Shift+1)
    kbd_register_key(&board.kbd, 0x0C, 4, 0, 1); // Delete (Shift+0)
    kbd_register_key(&board.kbd, 0x0D, 6, 0, 0); // Enter
}

//------------------------------------------------------------------------------
void
zx_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_zx) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;

    // keyboard init
    kbd_init(&board.kbd, 1);
    this->init_keymap();

    this->border_color = 0xFF000000;
    this->last_fe_out = 0;
    this->scanline_y = 0;
    this->blink_counter = 0;
    this->memory_paging_disabled = false;
    this->int_requested = false;
    this->joy_mask = 0;
    if (system::zxspectrum48k == this->cur_model) {
        this->display_ram_bank = 0;
    }
    else {
        this->display_ram_bank = 5;
    }

    // map memory
    clear(board.ram, sizeof(board.ram));
    this->init_memorymap();

    // initialize the system clock and PAL-line timer
    if (system::zxspectrum48k == m) {
        // Spectrum48K is exactly 3.5 MHz
        board.freq_hz = 3500000;
        this->frame_scanlines = 312;
        this->top_border_scanlines = 64;
        this->scanline_period = 224;
    }
    else {
        // Spectrum 128 CPU clock is (17.73447MHz / 5) (and video clock is (17.73447MHz / 4)
        board.freq_hz = 3546894;
        this->frame_scanlines = 311;
        this->top_border_scanlines = 63;
        this->scanline_period = 228;
    }
    this->scanline_counter = this->scanline_period;
    z80_init(&board.z80, cpu_tick);

    // initialize hardware components
    beeper_init(&board.beeper, board.freq_hz, SOUND_SAMPLE_RATE, 0.5f);
    if (system::zxspectrum128k == this->cur_model) {
        ay38912_init(&board.ay38912, board.freq_hz/2, SOUND_SAMPLE_RATE, 0.5f);
    }

    // cpu start state
    board.z80.PC = 0x0000;
}

//------------------------------------------------------------------------------
void
zx_t::poweroff() {
    YAKC_ASSERT(this->on);
    mem_unmap_all(&board.mem);
    this->on = false;
}

//------------------------------------------------------------------------------
void
zx_t::reset() {
    z80_reset(&board.z80);
    beeper_reset(&board.beeper);
    if (system::zxspectrum128k == this->cur_model) {
        ay38912_reset(&board.ay38912);
    }
    this->memory_paging_disabled = false;
    this->int_requested = false;
    this->joy_mask = 0;
    this->last_fe_out = 0;
    this->scanline_counter = this->scanline_period;
    this->scanline_y = 0;
    this->blink_counter = 0;
    if (system::zxspectrum48k == this->cur_model) {
        this->display_ram_bank = 0;
    }
    else {
        this->display_ram_bank = 5;
    }
    this->init_memorymap();
    board.z80.PC = 0x0000;
}

//------------------------------------------------------------------------------
uint64_t
zx_t::step(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = z80_exec(&board.z80, num_ticks);
    kbd_update(&board.kbd);
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
uint32_t
zx_t::step_debug() {
return 0;
/*
    auto& cpu = this->board->z80;
    auto& dbg = this->board->dbg;
    uint64_t all_ticks = 0;
    uint16_t old_pc;
    do {
        old_pc = cpu.PC;
        uint32_t ticks = cpu.handle_irq(this);
        if (0 == ticks) {
            ticks = cpu.step(this);
        }
        this->board->clck.step(this, ticks);
        this->board->beeper.step(ticks);
        if (system::zxspectrum128k == this->cur_model) {
            this->board->ay8910.step(ticks);
        }
        dbg.step(cpu.PC, ticks);
        all_ticks += ticks;
    }
    while ((old_pc == cpu.PC) && !cpu.INV);    
    return uint32_t(all_ticks);
*/
}

//------------------------------------------------------------------------------
uint64_t
zx_t::cpu_tick(int num_ticks, uint64_t pins) {
    // video decoding and vblank interrupt
    zx.scanline_counter -= num_ticks;
    if (zx.scanline_counter <= 0) {
        zx.scanline_counter += zx.scanline_period;
        // decode next video scanline
        if (zx.scanline()) {
            // request vblank interrupt
            pins |= Z80_INT;
        }
    }

    // tick audio systems
    for (int i = 0; i < num_ticks; i++) {
        zx.tick_count++;
        if (beeper_tick(&board.beeper)) {
            board.audiobuffer.write(board.beeper.sample);
        }
        // the AY-3-8912 chip runs at half CPU frequency
        if (system::zxspectrum128k == zx.cur_model) {
            if (zx.tick_count & 1) {
                if (ay38912_tick(&board.ay38912)) {
                    board.audiobuffer2.write(board.ay38912.sample);
                }
            }
        }
    }

    // memory and IO requests
    if (pins & Z80_MREQ) {
        // a memory request machine cycle
        // FIXME: 'contended memory' accesses should inject wait states
        const uint16_t addr = Z80_GET_ADDR(pins);
        if (pins & Z80_RD) {
            Z80_SET_DATA(pins, mem_rd(&board.mem, addr));
        }
        else if (pins & Z80_WR) {
            mem_wr(&board.mem, addr, Z80_GET_DATA(pins));
        }
    }
    else if (pins & Z80_IORQ) {
        // an IO request machine cycle
        // see http://problemkaputt.de/zxdocs.htm#zxspectrum for address decoding
        if (pins & Z80_RD) {
            // an IO read
            // FIXME: reading from port xxFF should return 'current VRAM data'
            if ((pins & Z80_A0) == 0) {
                // Spectrum ULA (...............0)
                // Bits 5 and 7 as read by INning from Port 0xfe are always one
                uint8_t data = (1<<7)|(1<<5);
                // MIC/EAR flags -> bit 6
                if (zx.last_fe_out & (1<<3|1<<4)) {
                    data |= (1<<6);
                }
                // keyboard matrix bits are encoded in the upper 8 bit of the port address
                uint16_t column_mask = (~(Z80_GET_ADDR(pins)>>8)) & 0x00FF;
                const uint8_t kbd_lines = kbd_test_lines(&board.kbd, column_mask);
                data |= (~kbd_lines) & 0x1F;
                Z80_SET_DATA(pins, data);
            }
            else if ((pins & (Z80_A7|Z80_A6|Z80_A5)) == 0) {
                // Kempston Joystick (........000.....)
                uint8_t data = 0;
                if (zx.joy_mask & joystick::left) {
                    data |= 1<<1;
                }
                if (zx.joy_mask & joystick::right) {
                    data |= 1<<0;
                }
                if (zx.joy_mask & joystick::up) {
                    data |= 1<<3;
                }
                if (zx.joy_mask & joystick::down) {
                    data |= 1<<2;
                }
                if (zx.joy_mask & joystick::btn0) {
                    data |= 1<<4;
                }
                Z80_SET_DATA(pins, data);
            }
            else if (system::zxspectrum128k == zx.cur_model) {
                // read from AY-3-8912 (11............0.)
                if ((pins & (Z80_A15|Z80_A14|Z80_A1)) == (Z80_A15|Z80_A14)) {
                    pins = ay38912_iorq(&board.ay38912, AY38912_BC1|pins) & Z80_PIN_MASK;
                }
            }
        }
        else if (pins & Z80_WR) {
            // an IO write
            const uint8_t data = Z80_GET_DATA(pins);
            if ((pins & Z80_A0) == 0) {
                // Spectrum ULA (...............0)
                // "every even IO port addresses the ULA but to avoid
                // problems with other I/O devices, only FE should be used"
                zx.border_color = palette[data & 7] & 0xFFD7D7D7;
                // FIXME:
                //      bit 3: MIC output (CAS SAVE, 0=On, 1=Off)
                //      bit 4: Beep output (ULA sound, 0=Off, 1=On)
                zx.last_fe_out = data;
                beeper_write(&board.beeper, 0 != (data & (1<<4)));
            }
            else if (system::zxspectrum128k == zx.cur_model) {
                // Spectrum 128 memory control (0.............0.)
                // http://8bit.yarek.pl/computer/zx.128/
                if ((pins & (Z80_A15|Z80_A1)) == 0) {
                    if (!zx.memory_paging_disabled) {
                        // bit 3 defines the video scanout memory bank (5 or 7)
                        zx.display_ram_bank = (data & (1<<3)) ? 7 : 5;
                        // only last memory bank is mappable
                        mem_map_ram(&board.mem, 0, 0xC000, 0x4000, board.ram[data & 0x7]);

                        // ROM0 or ROM1
                        if (data & (1<<4)) {
                            // bit 4 set: ROM1
                            mem_map_rom(&board.mem, 0, 0x0000, 0x4000, roms.ptr(rom_images::zx128k_1));
                        }
                        else {
                            // bit 4 clear: ROM0
                            mem_map_rom(&board.mem, 0, 0x0000, 0x4000, roms.ptr(rom_images::zx128k_0));
                        }
                    }
                    if (data & (1<<5)) {
                        // bit 5 prevents further changes to memory pages
                        // until computer is reset, this is used when switching
                        // to the 48k ROM
                        zx.memory_paging_disabled = true;
                    }
                }
                else if ((pins & (Z80_A15|Z80_A14|Z80_A1)) == (Z80_A15|Z80_A14)) {
                    // select AY-3-8912 register (11............0.)
                    ay38912_iorq(&board.ay38912, AY38912_BDIR|AY38912_BC1|pins);
                }
                else if ((pins & (Z80_A15|Z80_A14|Z80_A1)) == Z80_A15) {
                    // write to AY-3-8912 (10............0.)
                    ay38912_iorq(&board.ay38912, AY38912_BDIR|pins);
                }
            }
        }
    }
    return pins;
}

//------------------------------------------------------------------------------
void
zx_t::put_input(uint8_t ascii, uint8_t joy_mask) {
    // register a new key press with the emulator,
    // ascii=0 means no key pressed
    if (ascii) {
        kbd_key_down(&board.kbd, ascii);
        kbd_key_up(&board.kbd, ascii);
    }
    this->joy_mask = joy_mask;
}

//------------------------------------------------------------------------------
bool
zx_t::scanline() {
    // this is called by the timer callback for every PAL line, controlling
    // the vidmem decoding and vblank interrupt
    //
    // detailed information about frame timings is here:
    //  for 48K:    http://rk.nvg.ntnu.no/sinclair/faq/tech_48.html#48K
    //  for 128K:   http://rk.nvg.ntnu.no/sinclair/faq/tech_128.html
    //
    // one PAL line takes 224 T-states on 48K, and 228 T-states on 128K
    // one PAL frame is 312 lines on 48K, and 311 lines on 128K
    //

    // decode the next videomem line into the emulator framebuffer,
    // the border area of a real Spectrum is bigger than the emulator
    // (the emu has 32 pixels border on each side, the hardware has:
    //
    //  63 or 64 lines top border
    //  56 border lines bottom border
    //  48 pixels on each side horizontal border
    //
    const int top_decode_line = this->top_border_scanlines - 32;
    const int btm_decode_line = this->top_border_scanlines + 192 + 32;
    if ((this->scanline_y >= top_decode_line) && (this->scanline_y < btm_decode_line)) {
        this->decode_video_line(this->scanline_y - top_decode_line);
    }

    if (this->scanline_y++ >= this->frame_scanlines) {
        // start new frame, fetch next key, request vblank interrupt
        this->scanline_y = 0;
        this->blink_counter++;
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
zx_t::decode_video_line(uint16_t y) {
    YAKC_ASSERT(y < display_height);

    // decode a single line from the ZX framebuffer into the
    // emulator's framebuffer
    //
    // this is called from the scanline() callback so that video decoding
    // is synchronized with the CPU and vblank interrupt,
    // y is ranging from 0 to 256 (32 pixels vertical border on each side)
    //
    // the blink flag flips every 16 frames
    //
    uint32_t* dst = &(board.rgba8_buffer[y*display_width]);
    const uint8_t* vidmem_bank = board.ram[this->display_ram_bank];
    const bool blink = 0 != (this->blink_counter & 0x10);
    uint32_t fg, bg;
    if ((y < 32) || (y >= 224)) {
        // upper/lower border
        for (int x = 0; x < display_width; x++) {
            *dst++ = this->border_color;
        }
    }
    else {
        // compute video memory Y offset (inside 256x192 area)
        // this is how the 16-bit video memory address is computed
        // from X and Y coordinates:
        //
        // | 0| 1| 0|Y7|Y6|Y2|Y1|Y0|Y5|Y4|Y3|X4|X3|X2|X1|X0|
        uint16_t yy = y-32;
        uint16_t y_offset = ((yy & 0xC0)<<5) | ((yy & 0x07)<<8) | ((yy & 0x38)<<2);

        // left border
        for (int x = 0; x < (4*8); x++) {
            *dst++ = this->border_color;
        }

        // valid 256x192 vidmem area
        for (uint16_t x = 0; x < 32; x++) {
            uint16_t pix_offset = y_offset | x;
            uint16_t clr_offset = 0x1800 + (((yy & ~0x7)<<2) | x);

            // pixel mask and color attribute bytes
            uint8_t pix = vidmem_bank[pix_offset];
            uint8_t clr = vidmem_bank[clr_offset];

            // foreground and background color
            if ((clr & (1<<7)) && blink) {
                fg = palette[(clr>>3) & 7];
                bg = palette[clr & 7];
            }
            else {
                fg = palette[clr & 7];
                bg = palette[(clr>>3) & 7];
            }
            if (0 == (clr & (1<<6))) {
                // standard brightness
                fg &= 0xFFD7D7D7;
                bg &= 0xFFD7D7D7;
            }
            for (int px = 7; px >=0; px--) {
                *dst++ = pix & (1<<px) ? fg : bg;
            }
        }

        // right border
        for (int x = 0; x < (4*8); x++) {
            *dst++ = this->border_color;
        }
    }
}

//------------------------------------------------------------------------------
void
zx_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
    // if 128k, mix-in AY-8910 data
    if (system::zxspectrum128k == this->cur_model) {
        board.audiobuffer2.read(buffer, num_samples, true);
    }
}

//------------------------------------------------------------------------------
const void*
zx_t::framebuffer(int& out_width, int& out_height) {
    out_width = display_width;
    out_height = display_height;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
zx_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }
    zxz80_header hdr = { };
    zxz80ext_header ext_hdr = { };
    bool hdr_valid = false;
    bool ext_hdr_valid = false;
    if (filetype::zx_z80 == type) {
        if (fs->read(fp, &hdr, sizeof(hdr)) != sizeof(hdr)) {
            goto error;
        }
        hdr_valid = true;
        uint16_t pc = (hdr.PC_h<<8 | hdr.PC_l) & 0xFFFF;
        const bool is_version1 = 0 != pc;
        if (!is_version1) {
            // read length of ext_hdr
            if (fs->read(fp, &ext_hdr, 2) != 2) {
                goto error;
            }
            // read remaining ext-hdr bytes
            int ext_hdr_len = (ext_hdr.len_h<<8)|ext_hdr.len_l;
            YAKC_ASSERT(ext_hdr_len <= int(sizeof(ext_hdr)));
            if (fs->read(fp, &(ext_hdr.PC_l), ext_hdr_len) != ext_hdr_len) {
                goto error;
            }
            ext_hdr_valid = true;
            if (ext_hdr.hw_mode < 3) {
                if (this->cur_model != system::zxspectrum48k) {
                    goto error;
                }
            }
            else {
                if (this->cur_model != system::zxspectrum128k) {
                    goto error;
                }
            }
        }
        else {
            if (this->cur_model != system::zxspectrum48k) {
                goto error;
            }
        }
        const bool v1_compr = 0 != (hdr.flags0 & (1<<5));

        while (!fs->eof(fp)) {
            int page_index = 0;
            int src_len = 0, dst_len = 0;
            if (is_version1) {
                src_len = fs->size(fp) - sizeof(zxz80_header);
                dst_len = 48 * 1024;
            }
            else {
                zxz80page_header phdr;
                if (fs->read(fp, &phdr, sizeof(phdr)) == sizeof(phdr)) {
                    src_len = (phdr.len_h<<8 | phdr.len_l) & 0xFFFF;
                    dst_len = 0x4000;
                    page_index = phdr.page_nr - 3;
                    if ((system::zxspectrum48k == this->cur_model) && (page_index == 5)) {
                        page_index = 0;
                    }
                    if ((page_index < 0) || (page_index > 7)) {
                        page_index = -1;
                    }
                }
                else {
                    goto error;
                }
            }
            uint8_t* dst_ptr;
            if (-1 == page_index) {
                dst_ptr = board.junk;
            }
            else {
                dst_ptr = board.ram[page_index];
            }
            const uint8_t* dst_end_ptr = dst_ptr + dst_len;
            if (0xFFFF == src_len) {
                // FIXME: uncompressed not supported yet
                goto error;
            }
            else {
                // compressed
                int src_pos = 0;
                bool v1_done = false;
                uint8_t val[4];
                while ((src_pos < src_len) && !v1_done) {
                    val[0] = fs->peek_u8(fp, src_pos);
                    val[1] = fs->peek_u8(fp, src_pos+1);
                    val[2] = fs->peek_u8(fp, src_pos+2);
                    val[3] = fs->peek_u8(fp, src_pos+3);
                    // check for version 1 end marker
                    if (v1_compr && (0==val[0]) && (0xED==val[1]) && (0xED==val[2]) && (0==val[3])) {
                        v1_done = true;
                        src_pos += 4;
                    }
                    else if (0xED == val[0]) {
                        if (0xED == val[1]) {
                            uint8_t count = val[2];
                            YAKC_ASSERT(0 != count);
                            uint8_t data = val[3];
                            src_pos += 4;
                            for (int i = 0; i < count; i++) {
                                YAKC_ASSERT(dst_ptr < dst_end_ptr);
                                *dst_ptr++ = data;
                            }
                        }
                        else {
                            // single ED
                            YAKC_ASSERT(dst_ptr < dst_end_ptr);
                            *dst_ptr++ = val[0];
                            src_pos++;
                        }
                    }
                    else {
                        // any value
                        YAKC_ASSERT(dst_ptr < dst_end_ptr);
                        *dst_ptr++ = val[0];
                        src_pos++;
                    }
                }
                YAKC_ASSERT(dst_ptr == dst_end_ptr);
                YAKC_ASSERT(src_pos == src_len);
            }
            if (0xFFFF == src_len) {
                fs->skip(fp, 0x4000);
            }
            else {
                fs->skip(fp, src_len);
            }
        }
    }
    fs->close(fp);
    fs->rm(name);

    // start loaded image
    if (start && hdr_valid) {
        z80_t& cpu = board.z80;
        cpu.A = hdr.A; cpu.F = hdr.F;
        cpu.B = hdr.B; cpu.C = hdr.C;
        cpu.D = hdr.D; cpu.E = hdr.E;
        cpu.H = hdr.H; cpu.L = hdr.L;
        cpu.IXH = hdr.IX_h; cpu.IXL = hdr.IX_l;
        cpu.IYH = hdr.IY_h; cpu.IYL = hdr.IY_l;
        cpu.AF_ = (hdr.A_<<8 | hdr.F_) & 0xFFFF;
        cpu.BC_ = (hdr.B_<<8 | hdr.C_) & 0xFFFF;
        cpu.DE_ = (hdr.D_<<8 | hdr.E_) & 0xFFFF;
        cpu.HL_ = (hdr.H_<<8 | hdr.L_) & 0xFFFF;
        cpu.SP = (hdr.SP_h<<8 | hdr.SP_l) & 0xFFFF;
        cpu.I = hdr.I;
        cpu.R = (hdr.R & 0x7F) | ((hdr.flags0 & 1)<<7);
        cpu.IFF2 = hdr.IFF2 != 0;
        cpu.ei_pending = hdr.EI != 0;
        if (hdr.flags1 != 0xFF) {
            cpu.IM = (hdr.flags1 & 3);
        }
        else {
            cpu.IM = 1;
        }
        if (ext_hdr_valid) {
            cpu.PC = (ext_hdr.PC_h<<8 | ext_hdr.PC_l) & 0xFFFF;
            for (int i = 0; i < 16; i++) {
                // latch AY-3-8912 register address
                ay38912_iorq(&board.ay38912, AY38912_BDIR|AY38912_BC1|(i<<16));
                // write AY-3-8912 register value
                ay38912_iorq(&board.ay38912, AY38912_BDIR|(ext_hdr.audio[i]<<16));
            }
            // simulate an out of port 0xFFFD and 0x7FFD
            uint64_t pins = Z80_IORQ|Z80_WR;
            Z80_SET_ADDR(pins, 0xFFFD);
            Z80_SET_DATA(pins, ext_hdr.out_fffd);
            zx_t::cpu_tick(4, pins);
            Z80_SET_ADDR(pins, 0x7FFD);
            Z80_SET_DATA(pins, ext_hdr.out_7ffd);
            zx_t::cpu_tick(4, pins);
        }
        else {
            cpu.PC = (hdr.PC_h<<8 | hdr.PC_l) & 0xFFFF;
        }
        this->border_color = zx_t::palette[(hdr.flags0>>1) & 7] & 0xFFD7D7D7;
    }
    return true;

error:
    fs->close(fp);
    fs->rm(name);
    return false;
}

//------------------------------------------------------------------------------
const char*
zx_t::system_info() const {
    return
        "FIXME!\n\n"
        "ZX ROM images acknowledgement:\n\n"
        "Amstrad have kindly given their permission for the redistribution of their copyrighted material but retain that copyright.\n\n"
        "See: http://www.worldofspectrum.org/permits/amstrad-roms.txt";
}

} // namespace YAKC
