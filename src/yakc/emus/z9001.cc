//------------------------------------------------------------------------------
//  z9001.cc
//------------------------------------------------------------------------------
#include "z9001.h"
#include "yakc/util/filetypes.h"

YAKC::z9001_t YAKC::z9001;

namespace YAKC {

//------------------------------------------------------------------------------
bool
z9001_t::check_roms(system model) {
    if (system::z9001 == model) {
        return roms.has(rom_images::z9001_os12_1) &&
               roms.has(rom_images::z9001_os12_2) &&
               roms.has(rom_images::z9001_basic_507_511) &&
               roms.has(rom_images::z9001_font);
    }
    else if (system::kc87 == model) {
        return roms.has(rom_images::kc87_os_2) &&
               roms.has(rom_images::z9001_basic) &&
               roms.has(rom_images::kc87_font_2);
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
z9001_t::poweron(system m) {
    YAKC_ASSERT(int(system::any_z9001) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->on = true;

    z9001_desc_t desc = {};
    desc.type = (m == system::z9001) ? Z9001_TYPE_Z9001 : Z9001_TYPE_KC87;
    desc.pixel_buffer = board.rgba8_buffer;
    desc.pixel_buffer_size = sizeof(board.rgba8_buffer);
    desc.audio_cb = z9001_t::audio_cb;
    desc.audio_sample_rate = board.audio_sample_rate;
    if (m == system::z9001) {
        desc.rom_z9001_os_1 = roms.ptr(rom_images::z9001_os12_1);
        desc.rom_z9001_os_1_size = roms.size(rom_images::z9001_os12_1);
        desc.rom_z9001_os_2 = roms.ptr(rom_images::z9001_os12_2);
        desc.rom_z9001_os_2_size = roms.size(rom_images::z9001_os12_2);
        desc.rom_z9001_font = roms.ptr(rom_images::z9001_font);
        desc.rom_z9001_font_size = roms.size(rom_images::z9001_font);
        desc.rom_z9001_basic = roms.ptr(rom_images::z9001_basic_507_511);
        desc.rom_z9001_basic_size = roms.size(rom_images::z9001_basic_507_511);
    }
    else {
        desc.rom_kc87_os = roms.ptr(rom_images::kc87_os_2);
        desc.rom_kc87_os_size = roms.size(rom_images::kc87_os_2);
        desc.rom_kc87_font = roms.ptr(rom_images::kc87_font_2);
        desc.rom_kc87_font_size = roms.size(rom_images::kc87_font_2);
        desc.rom_kc87_basic = roms.ptr(rom_images::z9001_basic);
        desc.rom_kc87_basic_size = roms.size(rom_images::z9001_basic);
    }
    z9001_init(&sys, &desc);

    board.z80 = &sys.cpu;
    board.z80pio_1 = &sys.pio1;
    board.z80pio_2 = &sys.pio2;
    board.z80ctc = &sys.ctc;
    board.beeper_1 = &sys.beeper;
    board.kbd = &sys.kbd;
    board.mem = &sys.mem;
}

//------------------------------------------------------------------------------
void
z9001_t::poweroff() {
    YAKC_ASSERT(this->on);
    this->on = false;
    z9001_discard(&sys);
    board.clear();
}

//------------------------------------------------------------------------------
void
z9001_t::reset() {
    YAKC_ASSERT(this->on);
    z9001_reset(&sys);
}

//------------------------------------------------------------------------------
void
z9001_t::exec(uint32_t micro_seconds) {
    YAKC_ASSERT(this->on);
    z9001_exec(&sys, micro_seconds);
}

//------------------------------------------------------------------------------
void
z9001_t::on_ascii(uint8_t ascii) {
    YAKC_ASSERT(this->on);
    z9001_key_down(&sys, ascii);
    z9001_key_up(&sys, ascii);
}

//------------------------------------------------------------------------------
void
z9001_t::on_key_down(uint8_t key) {
    YAKC_ASSERT(this->on);
    z9001_key_down(&sys, key);
}

//------------------------------------------------------------------------------
void
z9001_t::on_key_up(uint8_t key) {
    YAKC_ASSERT(this->on);
    z9001_key_up(&sys, key);
}

//------------------------------------------------------------------------------
void
z9001_t::audio_cb(const float* samples, int num_samples, void* /*user_data*/) {
    for (int i = 0; i < num_samples; i++) {
        board.audiobuffer.write(samples[i]);
    }
}

//------------------------------------------------------------------------------
void
z9001_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
const void*
z9001_t::framebuffer(int& out_width, int& out_height) {
    out_width = z9001_display_width(&sys);
    out_height = z9001_display_height(&sys);
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
z9001_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    YAKC_ASSERT(on);
    bool success = false;
    int num_bytes = 0;
    const uint8_t* ptr = (const uint8_t*) fs->get(name, num_bytes);
    if (ptr && (num_bytes > 0)) {
        success = z9001_quickload(&sys, ptr, num_bytes);
    }
    fs->rm(name);
    return success;
}

//------------------------------------------------------------------------------
const char*
z9001_t::system_info() const {
    if (system::z9001 == this->cur_model) {
        return
            "The Z9001 (later retconned to KC85/1) was independently developed "
            "to the HC900 (aka KC85/2) by Robotron Dresden. It had a pretty "
            "slick design with an integrated keyboard which was legendary for "
            "how hard it was to type on.\n\nThe standard model had 16 KByte RAM, "
            "a monochrome 40x24 character display, and a 2.5 MHz U880 CPU "
            "(making it the fastest East German 8-bitter). The Z9001 could "
            "be extended by up to 4 expansion modules, usually one or two "
            "16 KByte RAM modules, and a 10 KByte ROM BASIC module (the "
            "version emulated here comes with 32 KByte RAM and a BASIC module) "
            "\n\n\n"
            "Manufacturer:      VEB Robotron-Messelektronik \"Otto Schön\" Dresden\n"
            "Release Date:      1984\n"
            "OS:                OS 1.1 (Z9001), OS 1.2 (KC85/1)\n"
            "Chips:             U880 @ 2.5 MHz (unlicensed Z80 clone)\n"
            "                   2x U855 (Z80 PIO clone)\n"
            "                   U857 (Z80 CTC clone)\n"
            "Memory:            16 KB RAM\n"
            "                    1 KB video memory\n"
            "                    4 KB ROM\n"
            "Graphics:          40x24 or 40x20 chars, monochrome\n"
            "Audio:             CTC-controlled beeper, mono\n"
            "Special Power:     2 PIO chips (one exclusively for keyboard input)";

    }
    else {
        return
            "The KC87 was the successor to the KC85/1. The only real difference "
            "was the built-in BASIC interpreter in ROM. The KC87 emulated here "
            "has 48 KByte RAM and the video color extension which could "
            "assign 8 foreground and 8 (identical) background colors per character, "
            "plus a blinking flag. This video extension was already available on the "
            "Z9001 though."
            "\n\n\n"
            "Manufacturer:      VEB Robotron-Messelektronik \"Otto Schön\" Dresden\n"
            "Release Date:      1987\n"
            "OS:                OS 1.3\n"
            "Chips:             U880 @ 2.5 MHz (unlicensed Z80 clone)\n"
            "                   2x U855 (Z80 PIO clone)\n"
            "                   U857 (Z80 CTC clone)\n"
            "Memory:            16 KB RAM\n"
            "                    2 KB video memory (1+1 KB for chars+color)\n"
            "                   10 KB BASIC ROM\n"
            "                    4 KB OS ROM\n"
            "Graphics:          40x24 or 40x20 chars\n"
            "                   1-of-8 foreground and background colors\n"
            "Audio:             CTC-controlled beeper, mono\n"
            "Special Power:     fastest 8-bitter this side of the Iron Curtain";
    }
}

} // namespace YAKC
