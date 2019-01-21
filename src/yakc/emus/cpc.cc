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
    board.mc6845 = &sys.crtc;
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
    uint8_t m = 0;
    if (mask & joystick::up) {
        m |= CPC_JOYSTICK_UP;
    }
    if (mask & joystick::down) {
        m |= CPC_JOYSTICK_DOWN;
    }
    if (mask & joystick::left) {
        m |= CPC_JOYSTICK_LEFT;
    }
    if (mask & joystick::right) {
        m |= CPC_JOYSTICK_RIGHT;
    }
    if (mask & joystick::btn0) {
        m |= CPC_JOYSTICK_BTN0;
    }
    if (mask & joystick::btn1) {
        m |= CPC_JOYSTICK_BTN1;
    }
    cpc_joystick(&sys, m);
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
    out_width = cpc_display_width(&sys);
    out_height = cpc_display_height(&sys);
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
cpc_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    YAKC_ASSERT(on);
    bool success = false;
    int num_bytes = 0;
    const uint8_t* ptr = (const uint8_t*) fs->get(name, num_bytes);
    if (ptr && (num_bytes > 0)) {
        if (type == filetype::cpc_tap) {
            success = cpc_insert_tape(&sys, ptr, num_bytes);
        }
        else {
            success = cpc_quickload(&sys, ptr, num_bytes);
        }
    }
    fs->rm(name);
    return success;
}

//------------------------------------------------------------------------------
const char*
cpc_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
