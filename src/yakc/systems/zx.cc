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

YAKC::zx_t YAKC::zx;

namespace YAKC {

//------------------------------------------------------------------------------
bool
zx_t::check_roms(system model) {
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
zx_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_zx) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;

    zx_desc_t desc = {};
    desc.type = (m == system::zxspectrum48k) ? ZX_TYPE_48K : ZX_TYPE_128;
    desc.pixel_buffer = board.rgba8_buffer;
    desc.pixel_buffer_size = sizeof(board.rgba8_buffer);
    desc.audio_cb = zx_t::audio_cb;
    desc.audio_sample_rate = board.audio_sample_rate;
    if (m == system::zxspectrum48k) {
        desc.rom_zx48k = roms.ptr(rom_images::zx48k);
        desc.rom_zx48k_size = roms.size(rom_images::zx48k);
    }
    else {
        desc.rom_zx128_0 = roms.ptr(rom_images::zx128k_0);
        desc.rom_zx128_0_size = roms.size(rom_images::zx128k_0);
        desc.rom_zx128_1 = roms.ptr(rom_images::zx128k_1);
        desc.rom_zx128_1_size = roms.size(rom_images::zx128k_1);
    }
    zx_init(&sys, &desc);

    board.z80 = &sys.cpu;
    board.ay38910 = &sys.ay;
    board.beeper_1 = &sys.beeper;
    board.kbd = &sys.kbd;
    board.mem = &sys.mem;
}

//------------------------------------------------------------------------------
void
zx_t::poweroff() {
    YAKC_ASSERT(this->on);
    this->on = false;
    zx_discard(&sys);
    board.clear();
}

//------------------------------------------------------------------------------
void
zx_t::reset() {
    YAKC_ASSERT(this->on);
    zx_reset(&sys);
}

//------------------------------------------------------------------------------
void
zx_t::exec(uint32_t micro_seconds) {
    YAKC_ASSERT(this->on);
    zx_exec(&sys, micro_seconds);
}

//------------------------------------------------------------------------------
void
zx_t::on_ascii(uint8_t ascii) {
    YAKC_ASSERT(this->on);
    zx_key_down(&sys, ascii);
    zx_key_up(&sys, ascii);
}

//------------------------------------------------------------------------------
void
zx_t::on_key_down(uint8_t key) {
    YAKC_ASSERT(this->on);
    zx_key_down(&sys, key);
}

//------------------------------------------------------------------------------
void
zx_t::on_key_up(uint8_t key) {
    YAKC_ASSERT(this->on);
    zx_key_up(&sys, key);
}

//------------------------------------------------------------------------------
void
zx_t::on_joystick(uint8_t mask) {
    YAKC_ASSERT(this->on);
    zx_joystick(&sys, mask);
}

//------------------------------------------------------------------------------
void
zx_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
void
zx_t::audio_cb(const float* samples, int num_samples, void* /*user_data*/) {
    for (int i = 0; i < num_samples; i++) {
        board.audiobuffer.write(samples[i]);
    }
}

//------------------------------------------------------------------------------
const void*
zx_t::framebuffer(int& out_width, int& out_height) {
    out_width = ZX_DISPLAY_WIDTH;
    out_height = ZX_DISPLAY_HEIGHT;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
zx_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    /* FIXME
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
                                *dst_ptr++ = data;
                            }
                        }
                        else {
                            // single ED
                            *dst_ptr++ = val[0];
                            src_pos++;
                        }
                    }
                    else {
                        // any value
                        *dst_ptr++ = val[0];
                        src_pos++;
                    }
                }
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
        z80_t* cpu = &z80;
        z80_set_a(cpu, hdr.A); z80_set_f(cpu, hdr.F);
        z80_set_b(cpu, hdr.B); z80_set_c(cpu, hdr.C);
        z80_set_d(cpu, hdr.D); z80_set_e(cpu, hdr.E);
        z80_set_h(cpu, hdr.H); z80_set_l(cpu, hdr.L);
        z80_set_ix(cpu, hdr.IX_h<<8 | hdr.IX_l);
        z80_set_iy(cpu, hdr.IY_h<<8 | hdr.IY_l);
        z80_set_af_(cpu, hdr.A_<<8 | hdr.F_);
        z80_set_bc_(cpu, hdr.B_<<8 | hdr.C_);
        z80_set_de_(cpu, hdr.D_<<8 | hdr.E_);
        z80_set_hl_(cpu, hdr.H_<<8 | hdr.L_);
        z80_set_sp(cpu, hdr.SP_h<<8 | hdr.SP_l);
        z80_set_i(cpu, hdr.I);
        z80_set_r(cpu, (hdr.R & 0x7F) | ((hdr.flags0 & 1)<<7));
        z80_set_iff2(cpu, hdr.IFF2 != 0);
        z80_set_ei_pending(cpu, hdr.EI != 0);
        if (hdr.flags1 != 0xFF) {
            z80_set_im(cpu, hdr.flags1 & 3);
        }
        else {
            z80_set_im(cpu, 1);
        }
        if (ext_hdr_valid) {
            z80_set_pc(cpu, ext_hdr.PC_h<<8 | ext_hdr.PC_l);
            for (int i = 0; i < 16; i++) {
                // latch AY-3-8912 register address
                ay38910_iorq(&ay38910, AY38910_BDIR|AY38910_BC1|(i<<16));
                // write AY-3-8912 register value
                ay38910_iorq(&ay38910, AY38910_BDIR|(ext_hdr.audio[i]<<16));
            }
            // simulate an out of port 0xFFFD and 0x7FFD
            uint64_t pins = Z80_IORQ|Z80_WR;
            Z80_SET_ADDR(pins, 0xFFFD);
            Z80_SET_DATA(pins, ext_hdr.out_fffd);
            zx_t::cpu_tick(4, pins, nullptr);
            Z80_SET_ADDR(pins, 0x7FFD);
            Z80_SET_DATA(pins, ext_hdr.out_7ffd);
            zx_t::cpu_tick(4, pins, nullptr);
        }
        else {
            z80_set_pc(cpu, hdr.PC_h<<8 | hdr.PC_l);
        }
        this->border_color = zx_t::palette[(hdr.flags0>>1) & 7] & 0xFFD7D7D7;
    }
    */
    return true;
/*
error:
    fs->close(fp);
    fs->rm(name);
    return false;
*/
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
