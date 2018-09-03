//------------------------------------------------------------------------------
//  atom.cc
//
//  TODO
//      - minimal VIA emulation (required by some games)
//      - handle Shift key (some games use this as jump button)
//
//------------------------------------------------------------------------------
#include "atom.h"
#include "yakc/util/filetypes.h"

YAKC::atom_t YAKC::atom;

namespace YAKC {

//------------------------------------------------------------------------------
bool
atom_t::check_roms(system model) {
    if (system::acorn_atom == model) {
        return roms.has(rom_images::atom_basic) &&
               roms.has(rom_images::atom_float) &&
               roms.has(rom_images::atom_dos);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
atom_t::poweron() {
    YAKC_ASSERT(!on);
    on = true;

    atom_desc_t desc = {};
    desc.audio_cb = atom_t::audio_cb;
    desc.audio_sample_rate = board.audio_sample_rate;
    desc.pixel_buffer = board.rgba8_buffer;
    desc.pixel_buffer_size = sizeof(board.rgba8_buffer);
    desc.rom_abasic = roms.ptr(rom_images::atom_basic);
    desc.rom_abasic_size = roms.size(rom_images::atom_basic);
    desc.rom_afloat = roms.ptr(rom_images::atom_float);
    desc.rom_afloat_size = roms.size(rom_images::atom_float);
    desc.rom_dosrom = roms.ptr(rom_images::atom_dos);
    desc.rom_dosrom_size = roms.size(rom_images::atom_dos);
    atom_init(&sys, &desc);
    
    board.m6502 = &sys.cpu;
    board.i8255 = &sys.ppi;
    board.m6522 = &sys.via;
    board.mc6847 = &sys.vdg;
    board.beeper_1 = &sys.beeper;
    board.kbd = &sys.kbd;
    board.mem = &sys.mem;
}

//------------------------------------------------------------------------------
void
atom_t::poweroff() {
    YAKC_ASSERT(on);
    on = false;
    atom_discard(&sys);
    board.clear();
}

//------------------------------------------------------------------------------
void
atom_t::reset() {
    YAKC_ASSERT(on);
    atom_reset(&sys);
}

//------------------------------------------------------------------------------
void
atom_t::exec(uint32_t micro_seconds) {
    YAKC_ASSERT(on);
    atom_exec(&sys, micro_seconds);
}

//------------------------------------------------------------------------------
void
atom_t::on_ascii(uint8_t ascii) {
    YAKC_ASSERT(on);
    atom_key_down(&sys, ascii);
    atom_key_up(&sys, ascii);
}

//------------------------------------------------------------------------------
void
atom_t::on_key_down(uint8_t key) {
    atom_key_down(&sys,key);
}

//------------------------------------------------------------------------------
void
atom_t::on_key_up(uint8_t key) {
    atom_key_up(&sys, key);
}

//------------------------------------------------------------------------------
void
atom_t::on_joystick(uint8_t mask) {
    uint8_t m = 0;
    if (mask & joystick::left) {
        m |= ATOM_JOYSTICK_LEFT;
    }
    if (mask & joystick::right) {
        m |= ATOM_JOYSTICK_RIGHT;
    }
    if (mask & joystick::up) {
        m |= ATOM_JOYSTICK_UP;
    }
    if (mask & joystick::down) {
        m |= ATOM_JOYSTICK_DOWN;
    }
    if (mask & joystick::btn0) {
        m |= ATOM_JOYSTICK_BTN;
    }
    atom_joystick(&sys, m);
}

//------------------------------------------------------------------------------
const void*
atom_t::framebuffer(int& out_width, int& out_height) {
    out_width = MC6847_DISPLAY_WIDTH;
    out_height = MC6847_DISPLAY_HEIGHT;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
atom_t::audio_cb(const float* samples, int num_samples, void* /*user_data*/) {
    for (int i = 0; i < num_samples; i++) {
        board.audiobuffer.write(samples[i]);
    }
}

//------------------------------------------------------------------------------
void
atom_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
bool
atom_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    YAKC_ASSERT(on);
    bool success = false;
    int num_bytes = 0;
    const uint8_t* ptr = (const uint8_t*) fs->get(name, num_bytes);
    if (ptr && (num_bytes > 0)) {
        success = atom_insert_tape(&sys, ptr, num_bytes);
    }
    fs->rm(name);
    return success;
}

//------------------------------------------------------------------------------
const char*
atom_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
