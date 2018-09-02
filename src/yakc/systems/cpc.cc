//------------------------------------------------------------------------------
//  cpc.cc
//
//  Schematics:
//  - 646: http://www.cpcwiki.eu/imgs/6/6d/Schaltplan_cpc_464.jpg
//  - 664: http://www.cpcwiki.eu/index.php/File:CPC664_Schematic.png
//  - 6128: http://www.cpcwiki.eu/index.php/File:CPC6128_Schematic.png
//
//  TODO:
//      - graphics in some demos still broken
//      - floppy disk controller emulation and DSK file loading
//------------------------------------------------------------------------------
#include "cpc.h"
#include "yakc/util/filetypes.h"

YAKC::cpc_t YAKC::cpc;

namespace YAKC {

//------------------------------------------------------------------------------
bool
cpc_t::check_roms(system model) {
    if (system::cpc464 == model) {
        return roms.has(rom_images::cpc464_os) && roms.has(rom_images::cpc464_basic);
    }
    else if (system::cpc6128 == model) {
        return roms.has(rom_images::cpc6128_os) &&
               roms.has(rom_images::cpc6128_basic) &&
               roms.has(rom_images::cpc6128_amsdos);

    }
    else if (system::kccompact == model) {
        return roms.has(rom_images::kcc_os) && roms.has(rom_images::kcc_basic);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
cpc_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_cpc) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;

    cpc_desc_t desc = {};
    switch (m) {
        case system::cpc464:    desc.type = CPC_TYPE_464; break;
        case system::kccompact: desc.type = CPC_TYPE_KCCOMPACT; break;
        default:                desc.type = CPC_TYPE_6128; break;
    }
    desc.pixel_buffer = board.rgba8_buffer;
    desc.pixel_buffer_size = sizeof(board.rgba8_buffer);
    desc.audio_cb = cpc_t::audio_cb;
    desc.audio_sample_rate = board.audio_sample_rate;
    desc.video_debug_cb = cpc_t::video_debug_cb;
    if (m == system::cpc464) {
        desc.rom_464_os = roms.ptr(rom_images::cpc464_os);
        desc.rom_464_os_size = roms.size(rom_images::cpc464_os);
        desc.rom_464_basic = roms.ptr(rom_images::cpc464_basic);
        desc.rom_464_basic_size = roms.size(rom_images::cpc464_basic);
    }
    else if (m == system::kccompact) {
        desc.rom_kcc_os = roms.ptr(rom_images::kcc_os);
        desc.rom_kcc_os_size = roms.size(rom_images::kcc_os);
        desc.rom_kcc_basic = roms.ptr(rom_images::kcc_basic);
        desc.rom_kcc_basic_size = roms.size(rom_images::kcc_basic);
    }
    else {
        desc.rom_6128_os = roms.ptr(rom_images::cpc6128_os);
        desc.rom_6128_os_size = roms.size(rom_images::cpc6128_os);
        desc.rom_6128_basic = roms.ptr(rom_images::cpc6128_basic);
        desc.rom_6128_basic_size = roms.size(rom_images::cpc464_basic);
        desc.rom_6128_amsdos = roms.ptr(rom_images::cpc6128_amsdos);
        desc.rom_6128_amsdos_size = roms.size(rom_images::cpc6128_amsdos);
    }
    cpc_init(&sys, &desc);

    board.z80 = &sys.cpu;
    board.ay38910 = &sys.psg;
    board.i8255 = &sys.ppi;
    board.mc6845 = &sys.vdg;
    board.crt = &sys.crt;
    board.kbd = &sys.kbd;
    board.mem = &sys.mem;
}

//------------------------------------------------------------------------------
void
cpc_t::poweroff() {
    YAKC_ASSERT(this->on);
    this->on = false;
    cpc_discard(&sys);
    board.clear();
}

//------------------------------------------------------------------------------
void
cpc_t::reset() {
    YAKC_ASSERT(this->on);
    cpc_reset(&sys);
}

//------------------------------------------------------------------------------
void
cpc_t::exec(uint32_t micro_seconds) {
    YAKC_ASSERT(this->on);
    cpc_exec(&sys, micro_seconds);
}

//------------------------------------------------------------------------------
void
cpc_t::video_debug_cb(uint64_t crtc_pins, void* user_data) {
    int dst_x = cpc.sys.crt.h_pos * 16;
    int dst_y = cpc.sys.crt.v_pos;
    if ((dst_x <= (dbg_width-16)) && (dst_y < dbg_height)) {
        uint32_t* dst = &(board.rgba8_buffer[dst_x + dst_y * dbg_width]);
        if (!(crtc_pins & MC6845_DE)) {
            uint8_t r = 0x3F;
            uint8_t g = 0x3F;
            uint8_t b = 0x3F;
            if (crtc_pins & MC6845_HS) {
                r = 0x7F; g = 0; b = 0;
            }
            if (cpc.sys.ga.sync) {
                r = 0xFF; g = 0; b = 0;
            }
            if (crtc_pins & MC6845_VS) {
                g = 0x7F;
            }
            if (cpc.sys.ga.intr) {
                b = 0xFF;
            }
            else if (0 == cpc.sys.vdg.scanline_ctr) {
                r = g = b = 0x00;
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
            cpc_ga_decode_pixels(&cpc.sys, dst, crtc_pins);
        }
    }
}

//------------------------------------------------------------------------------
void
cpc_t::on_ascii(uint8_t ascii) {
    YAKC_ASSERT(this->on);
    cpc_key_down(&sys, ascii);
    cpc_key_up(&sys, ascii);
}

//------------------------------------------------------------------------------
void
cpc_t::on_key_down(uint8_t key) {
    YAKC_ASSERT(this->on);
    cpc_key_down(&sys, key);
}

//------------------------------------------------------------------------------
void
cpc_t::on_key_up(uint8_t key) {
    YAKC_ASSERT(this->on);
    cpc_key_up(&sys, key);
}

//------------------------------------------------------------------------------
void
cpc_t::on_joystick(uint8_t mask) {
    /* FIXME
    this->joymask = 0;
    if (mask & joystick::up) {
        this->joymask |= (1<<0);
    }
    if (mask & joystick::down) {
        this->joymask |= (1<<1);
    }
    if (mask & joystick::left) {
        this->joymask |= (1<<2);
    }
    if (mask & joystick::right) {
        this->joymask |= (1<<3);
    }
    if (mask & joystick::btn0) {
        this->joymask |= (1<<4);
    }
    if (mask & joystick::btn1) {
        this->joymask |= (1<<5);
    }
    */
}

//------------------------------------------------------------------------------
void
cpc_t::audio_cb(const float* samples, int num_samples, void* /*user_data*/) {
    for (int i = 0; i < num_samples; i++) {
        board.audiobuffer.write(samples[i]);
    }
}

//------------------------------------------------------------------------------
void
cpc_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
const void*
cpc_t::framebuffer(int& out_width, int& out_height) {
    if (cpc.sys.video_debug_enabled) {
        out_width = dbg_width;
        out_height = dbg_height;
    }
    else {
        out_width = CPC_DISPLAY_WIDTH;
        out_height = CPC_DISPLAY_HEIGHT;
    }
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
cpc_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    /* FIXME
    if (filetype::cpc_sna == type) {
        return this->load_sna(fs, name, type, start);
    }
    else if (filetype::cpc_bin == type) {
        return this->load_bin(fs, name, type, start);
    }
    */
    return false;
}

//------------------------------------------------------------------------------
const char*
cpc_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
