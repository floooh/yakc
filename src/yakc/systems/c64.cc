//------------------------------------------------------------------------------
//  c64.cc
//
//  FIXME:
//  - need a way to route joystick input either to joystick 1 or 2, 
//    in Zaxxon the jet color flashes on input from joystick 1 (probably
//    to tell the player that joystick 2 controls the jet)
//------------------------------------------------------------------------------
#include "c64.h"
#include "yakc/util/tapedeck.h"

YAKC::c64_t YAKC::c64;

namespace YAKC {

//------------------------------------------------------------------------------
bool
c64_t::check_roms(system model) {
    if (system::c64_pal == model) {
        return roms.has(rom_images::c64_basic) &&
               roms.has(rom_images::c64_char) &&
               roms.has(rom_images::c64_kernalv3);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
c64_t::poweron(system m) {
    YAKC_ASSERT(!this->on);
    YAKC_ASSERT(system::c64_pal == m);

    this->on = true;

    c64_desc_t desc = {};
    desc.pixel_buffer = board.rgba8_buffer;
    desc.pixel_buffer_size = sizeof(board.rgba8_buffer);
    desc.audio_cb = c64_t::audio_cb;
    desc.audio_sample_rate = board.audio_sample_rate;
    desc.audio_tape_sound = true;
    desc.rom_char = roms.ptr(rom_images::c64_char);
    desc.rom_char_size = roms.size(rom_images::c64_char);
    desc.rom_basic = roms.ptr(rom_images::c64_basic);
    desc.rom_basic_size = roms.size(rom_images::c64_basic);
    desc.rom_kernal = roms.ptr(rom_images::c64_kernalv3);
    desc.rom_kernal_size = roms.size(rom_images::c64_kernalv3);
    c64_init(&sys, &desc);

    board.m6502 = &sys.cpu;
    board.m6526_1 = &sys.cia_1;
    board.m6526_2 = &sys.cia_2;
    board.m6569 = &sys.vic;
    board.m6581 = &sys.sid;
    board.kbd = &sys.kbd;
    board.mem = &sys.mem_cpu;
}

//------------------------------------------------------------------------------
void
c64_t::poweroff() {
    YAKC_ASSERT(on);
    on = false;
    c64_discard(&sys);
    board.clear();
}

//------------------------------------------------------------------------------
void
c64_t::reset() {
    YAKC_ASSERT(on);
    c64_reset(&sys);
}

//------------------------------------------------------------------------------
void
c64_t::exec(uint32_t micro_seconds) {
    YAKC_ASSERT(on);
    c64_exec(&sys, micro_seconds);
}

//------------------------------------------------------------------------------
void
c64_t::on_ascii(uint8_t ascii) {
    YAKC_ASSERT(on);
    c64_key_down(&sys, ascii);
    c64_key_up(&sys, ascii);
}

//------------------------------------------------------------------------------
void
c64_t::on_key_down(uint8_t key) {
    YAKC_ASSERT(on);
    c64_key_down(&sys, key);
}

//------------------------------------------------------------------------------
void
c64_t::on_key_up(uint8_t key) {
    YAKC_ASSERT(on);
    c64_key_up(&sys, key);
}

//------------------------------------------------------------------------------
void
c64_t::on_joystick(uint8_t mask) {
    uint8_t m = 0;
    if (mask & joystick::left) {
        m |= C64_JOYSTICK_LEFT;
    }
    if (mask & joystick::right) {
        m |= C64_JOYSTICK_RIGHT;
    }
    if (mask & joystick::up) {
        m |= C64_JOYSTICK_UP;
    }
    if (mask & joystick::down) {
        m |= C64_JOYSTICK_DOWN;
    }
    if (mask & joystick::btn0) {
        m |= C64_JOYSTICK_BTN;
    }
    c64_joystick(&sys, m);
}

//------------------------------------------------------------------------------
const void*
c64_t::framebuffer(int& out_width, int &out_height) {
    m6569_display_size(&sys.vic, &out_width, &out_height);
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
c64_t::audio_cb(const float* samples, int num_samples, void* /*user_data*/) {
    for (int i = 0; i < num_samples; i++) {
        board.audiobuffer.write(samples[i]);
    }
}

//------------------------------------------------------------------------------
void
c64_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
void
c64_t::on_tape_inserted() {
    /* FIXME
    // read the .TAP header
    tape.read(&this->tap_header, sizeof(this->tap_header));
    const char* sig = "C64-TAPE-RAW";
    if (0 != memcmp(&this->tap_header.signature, sig, 12)) {
        // FIXME: unknown file type
        return;
    }
    YAKC_ASSERT(1 == this->tap_header.version);
    // tape is valid, rewind counters
    this->tape_valid = true;
    this->tape_tick_count = 0;
    this->tape_byte_count = this->tap_header.size;
    */
}

//------------------------------------------------------------------------------
bool
c64_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    /* FIXME
    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }
    uint16_t start_addr;
    // start address
    fs->read(fp, &start_addr, sizeof(start_addr));
    uint16_t end_addr = start_addr + (fs->size(fp) - 2);
    uint16_t addr = start_addr;
    while (addr < end_addr) {
        uint8_t byte;
        fs->read(fp, &byte, 1);
        mem_wr(&mem, addr++, byte);
    }
    fs->close(fp);
    fs->rm(name);
    */
    return true;
}

//------------------------------------------------------------------------------
const char*
c64_t::system_info() const {
    return "FIXME!";
}

} // namespace YAKC
