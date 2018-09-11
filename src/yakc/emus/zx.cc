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
    uint8_t m = 0;
    if (mask & joystick::left) {
        m |= ZX_JOYSTICK_LEFT;
    }
    if (mask & joystick::right) {
        m |= ZX_JOYSTICK_RIGHT;
    }
    if (mask & joystick::up) {
        m |= ZX_JOYSTICK_UP;
    }
    if (mask & joystick::down) {
        m |= ZX_JOYSTICK_DOWN;
    }
    if (mask & joystick::btn0) {
        m |= ZX_JOYSTICK_BTN;
    }
    zx_joystick(&sys, m);
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
    YAKC_ASSERT(on);
    bool success = false;
    int num_bytes = 0;
    const uint8_t* ptr = (const uint8_t*) fs->get(name, num_bytes);
    if (ptr && (num_bytes > 0)) {
        success = zx_quickload(&sys, ptr, num_bytes);
    }
    fs->rm(name);
    return success;
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
