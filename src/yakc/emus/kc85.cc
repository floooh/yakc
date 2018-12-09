//------------------------------------------------------------------------------
//  kc85.cc
//------------------------------------------------------------------------------
#include "kc85.h"
#include "yakc/util/filetypes.h"

YAKC::kc85_t YAKC::kc85;

namespace YAKC {

//------------------------------------------------------------------------------
bool
kc85_t::check_roms(system model, os_rom os) {
    if (system::kc85_2 == model) {
        if (os_rom::caos_hc900 == os) {
            return roms.has(rom_images::hc900);
        }
        else if (os_rom::caos_2_2 == os) {
            return roms.has(rom_images::caos22);
        }
    }
    else if (system::kc85_3 == model) {
        if (os_rom::caos_3_1 == os) {
            return roms.has(rom_images::caos31) && roms.has(rom_images::kc85_basic_rom);
        }
        else if (os_rom::caos_3_4 == os) {
            return roms.has(rom_images::caos34) && roms.has(rom_images::kc85_basic_rom);
        }
    }
    else if (system::kc85_4 == model) {
        if (os_rom::caos_4_2 == os) {
            return roms.has(rom_images::caos42c) &&
                   roms.has(rom_images::caos42e) &&
                   roms.has(rom_images::kc85_basic_rom);
        }
    }
    return false;
}

//------------------------------------------------------------------------------
void
kc85_t::poweron(system m, os_rom os) {
    YAKC_ASSERT(int(system::any_kc85) & int(m));

    this->cur_model = m;
    this->on = true;

    kc85_desc_t desc = {};
    switch (m) {
        case system::kc85_2: desc.type = KC85_TYPE_2; break;
        case system::kc85_3: desc.type = KC85_TYPE_3; break;
        default:             desc.type = KC85_TYPE_4; break;
    }
    desc.pixel_buffer = board.rgba8_buffer;
    desc.pixel_buffer_size = sizeof(board.rgba8_buffer);
    desc.audio_cb = kc85_t::audio_cb;
    desc.audio_sample_rate = board.audio_sample_rate;
    desc.patch_cb = kc85_t::patch_cb;
    switch (os) {
        case os_rom::caos_hc900:
            desc.rom_caos22 = roms.ptr(rom_images::hc900);
            desc.rom_caos22_size = roms.size(rom_images::hc900);
            break;
        case os_rom::caos_2_2:
            desc.rom_caos22 = roms.ptr(rom_images::caos22);
            desc.rom_caos22_size = roms.size(rom_images::caos22);
            break;
        case os_rom::caos_3_1:
            desc.rom_caos31 = roms.ptr(rom_images::caos31);
            desc.rom_caos31_size = roms.size(rom_images::caos31);
            desc.rom_kcbasic = roms.ptr(rom_images::kc85_basic_rom);
            desc.rom_kcbasic_size = roms.size(rom_images::kc85_basic_rom);
            break;
        case os_rom::caos_3_4:
            desc.rom_caos31 = roms.ptr(rom_images::caos34);
            desc.rom_caos31_size = roms.size(rom_images::caos34);
            desc.rom_kcbasic = roms.ptr(rom_images::kc85_basic_rom);
            desc.rom_kcbasic_size = roms.size(rom_images::kc85_basic_rom);
            break;
        default:
            desc.rom_caos42c = roms.ptr(rom_images::caos42c);
            desc.rom_caos42c_size = roms.size(rom_images::caos42c);
            desc.rom_caos42e = roms.ptr(rom_images::caos42e);
            desc.rom_caos42e_size = roms.size(rom_images::caos42e);
            desc.rom_kcbasic = roms.ptr(rom_images::kc85_basic_rom);
            desc.rom_kcbasic_size = roms.size(rom_images::kc85_basic_rom);
            break;
    }
    kc85_init(&sys, &desc);

    board.z80 = &sys.cpu;
    board.z80pio_1 = &sys.pio;
    board.z80ctc = &sys.ctc;
    board.beeper_1 = &sys.beeper_1;
    board.beeper_2 = &sys.beeper_2;
    board.kbd = &sys.kbd;
    board.mem = &sys.mem;
}

//------------------------------------------------------------------------------
void
kc85_t::poweroff() {
    YAKC_ASSERT(on);
    this->on = false;
    kc85_discard(&sys);
    board.clear();
}

//------------------------------------------------------------------------------
void
kc85_t::reset() {
    YAKC_ASSERT(on);
    kc85_reset(&sys);
}

//------------------------------------------------------------------------------
void
kc85_t::exec(uint32_t micro_seconds) {
    YAKC_ASSERT(on);
    kc85_exec(&sys, micro_seconds);
}

//------------------------------------------------------------------------------
void
kc85_t::on_ascii(uint8_t ascii) {
    YAKC_ASSERT(on);
    kc85_key_down(&sys, ascii);
    kc85_key_up(&sys, ascii);
}

//------------------------------------------------------------------------------
void
kc85_t::on_key_down(uint8_t key) {
    YAKC_ASSERT(on);
    kc85_key_down(&sys, key);
}

//------------------------------------------------------------------------------
void
kc85_t::on_key_up(uint8_t key) {
    YAKC_ASSERT(on);
    kc85_key_up(&sys, key);
}

//------------------------------------------------------------------------------
void
kc85_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
}

//------------------------------------------------------------------------------
void
kc85_t::audio_cb(const float* samples, int num_samples, void* /*user_data*/) {
    for (int i = 0; i < num_samples; i++) {
        board.audiobuffer.write(samples[i]);
    }
}

//------------------------------------------------------------------------------
const void*
kc85_t::framebuffer(int& out_width, int& out_height) {
    YAKC_ASSERT(on);
    out_width = kc85_display_width(&sys);
    out_height = kc85_display_height(&sys);
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
kc85_t::patch_cb(const char* snapshot_name, void* user_data) {
    YAKC_ASSERT(kc85.on);
    if (strcmp(snapshot_name, "JUNGLE     ") == 0) {
        /* patch start level 1 into memory */
        mem_wr(&kc85.sys.mem, 0x36b7, 1);
        mem_wr(&kc85.sys.mem, 0x3697, 1);
        for (int i = 0; i < 5; i++) {
            mem_wr(&kc85.sys.mem, 0x1770 + i, mem_rd(&kc85.sys.mem, 0x36b6 + i));
        }
    }
    else if (strcmp(snapshot_name, "DIGGER  COM\x01") == 0) {
        mem_wr16(&kc85.sys.mem, 0x09AA, 0x0160);    /* time for delay-loop 0160 instead of 0260 */
        mem_wr(&kc85.sys.mem, 0x3d3a, 0xB5);        /* OR L instead of OR (HL) */
    }
    else if (strcmp(snapshot_name, "DIGGERJ") == 0) {
        mem_wr16(&kc85.sys.mem, 0x09AA, 0x0260);
        mem_wr(&kc85.sys.mem, 0x3d3a, 0xB5);       /* OR L instead of OR (HL) */
    }
}

//------------------------------------------------------------------------------
bool
kc85_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
    YAKC_ASSERT(on);
    bool success = false;
    int num_bytes = 0;
    const uint8_t* ptr = (const uint8_t*) fs->get(name, num_bytes);
    if (ptr && (num_bytes > 0)) {
        success = kc85_quickload(&sys, ptr, num_bytes);
    }
    fs->rm(name);
    return success;
}

//------------------------------------------------------------------------------
const char*
kc85_t::module_name(kc85_module_type_t type) {
    switch (type) {
        case KC85_MODULE_NONE:              return "NONE";
        case KC85_MODULE_M006_BASIC:        return "M006 BASIC";
        case KC85_MODULE_M011_64KBYE:       return "M011 64 KBYTE RAM";
        case KC85_MODULE_M012_TEXOR:        return "M012 TEXOR";
        case KC85_MODULE_M022_16KBYTE:      return "M022 EXPANDER RAM";
        case KC85_MODULE_M026_FORTH:        return "M026 FORTH";
        case KC85_MODULE_M027_DEVELOPMENT:  return "M027 DEVELOPMENT";
        default:                            return "UNKNOWN";
    }
}

//------------------------------------------------------------------------------
uint8_t
kc85_t::module_id(kc85_module_type_t type) {
    switch (type) {
        case KC85_MODULE_NONE:              return 0xFF;
        case KC85_MODULE_M006_BASIC:        return 0xFC;
        case KC85_MODULE_M011_64KBYE:       return 0xF6;
        case KC85_MODULE_M012_TEXOR:        return 0xFB;
        case KC85_MODULE_M022_16KBYTE:      return 0xF4;
        case KC85_MODULE_M026_FORTH:        return 0xFB;
        case KC85_MODULE_M027_DEVELOPMENT:  return 0xFB;
        default:                            return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
kc85_t::register_none_module(const char* name, const char* desc) {
    module& mod = this->mod_registry[KC85_MODULE_NONE];
    mod.registered = true;
    mod.type = KC85_MODULE_NONE;
    mod.name = name;
    mod.desc = desc;
}

//------------------------------------------------------------------------------
void
kc85_t::register_ram_module(kc85_module_type_t type, const char* desc) {
    YAKC_ASSERT(!this->is_module_registered(type));
    module& mod = this->mod_registry[type];
    mod.registered = true;
    mod.type       = type;
    mod.id         = module_id(type);
    mod.name       = module_name(type);
    mod.desc       = desc;
    mod.mem_ptr    = nullptr;
    mod.mem_size   = 0;
}

//------------------------------------------------------------------------------
void
kc85_t::register_rom_module(kc85_module_type_t type, const uint8_t* ptr, unsigned int size, const char* desc) {
    YAKC_ASSERT(!this->is_module_registered(type));
    YAKC_ASSERT(ptr && size > 0 && desc);
    module& mod = this->mod_registry[type];
    mod.registered = true;
    mod.type       = type;
    mod.id         = module_id(type);
    mod.name       = module_name(type);
    mod.desc       = desc;
    mod.mem_ptr    = (uint8_t*) ptr;
    mod.mem_size   = size;
}

//------------------------------------------------------------------------------
bool
kc85_t::is_module_registered(kc85_module_type_t type) const {
    YAKC_ASSERT((type >= 0) && (type < KC85_MODULE_NUM));
    return this->mod_registry[type].registered;
}

//------------------------------------------------------------------------------
const kc85_t::module&
kc85_t::module_template(kc85_module_type_t type) const {
    YAKC_ASSERT(is_module_registered(type));
    return this->mod_registry[type];
}

//------------------------------------------------------------------------------
const kc85_t::module&
kc85_t::mod_by_slot_addr(uint8_t slot_addr) const {
    kc85_module_type_t mod_type = KC85_MODULE_NONE;
    for (int i = 0; i < KC85_NUM_SLOTS; i++) {
        const auto& slot = sys.exp.slot[i];
        if (slot.addr == slot_addr) {
            mod_type = slot.mod.type;
            break;
        }
    }
    return this->mod_registry[mod_type];
}

//------------------------------------------------------------------------------
bool
kc85_t::slot_occupied(uint8_t slot_addr) {
    return kc85_slot_occupied(&sys, slot_addr);
}

//------------------------------------------------------------------------------
void
kc85_t::insert_module(uint8_t slot_addr, kc85_module_type_t type) {
    const auto& mod = mod_registry[type];
    if (mod.mem_ptr) {
        kc85_insert_rom_module(&sys, slot_addr, type, mod.mem_ptr, mod.mem_size);
    }
    else {
        kc85_insert_ram_module(&sys, slot_addr, type);
    }
}

//------------------------------------------------------------------------------
void
kc85_t::remove_module(uint8_t slot_addr) {
    kc85_remove_module(&sys, slot_addr);
}

//------------------------------------------------------------------------------
const char*
kc85_t::system_info() const {
    if (this->cur_model == system::kc85_2) {
        return
            "The KC85/2 was originally named HC900 (HC: Home Computer), "
            "but it soon became clear that the system would be too expensive "
            "and too few could be built to be sold as a 'home computer'.\n\n"
            "It was renamed to KC85/2 (KC: Kleincomputer, small computer) and "
            "made available only to schools, 'people-owned' companies and "
            "the military.\n\n"
            "On paper, the KC85 computer line had impressive color graphics "
            "capabilities (slightly better than the ZX Spectrum), but with "
            "its slow CPU speed and lack of custom chips it was hard to "
            "achieve the impressive graphics and audio effects known from "
            "8-bit Commodore or Atari machines of the time."
            "\n\n\n"
            "Manufacturer:      VEB Mikroelektronik Mühlhausen \n"
            "Release Date:      1985\n"
            "OS:                HC-CAOS 2.2 in ROM\n"
            "Chips:             U880 @ 1.75 MHz (unlicensed Z80 clone)\n"
            "                   U855 (Z80 PIO clone)\n"
            "                   U857 (Z80 CTC clone)\n"
            "Memory:            16 KB RAM\n"
            "                   16 KB video memory\n"
            "                    8 KB ROM\n"
            "Graphics:          320x256 display resolution\n"
            "                   40x64 color attribute resolution\n"
            "                   16 foreground and 8 background colors\n"
            "Audio:             CTC-controlled beeper, mono\n"
            "Special Power:     great graphics for an 8-bitter without custom chips";

    }
    else if (this->cur_model == system::kc85_3) {
        return
            "The hardware of the KC85/3 was identical to it's "
            "predecessor KC85/2, except for an additional 8 KByte "
            "of ROM for a built-in BASIC interpreter.\n\n"
            "The only other difference is the new OS "
            "version HC-CAOS 3.1 which added a few more commands."
            "\n\n\n"
            "Manufacturer:      VEB Mikroelektronik Mühlhausen \n"
            "Release Date:      1986\n"
            "OS:                HC-CAOS 3.1, HC-BASIC in ROM\n"
            "Chips:             U880 @ 1.75 MHz (unlicensed Z80 clone)\n"
            "                   U855 (Z80 PIO clone)\n"
            "                   U857 (Z80 CTC clone)\n"
            "Memory:            16 KB RAM\n"
            "                   16 KB video memory\n"
            "                   16 KB ROM (8 KB BASIC, 8 KB CAOS)\n"
            "Graphics:          320x256 display resolution\n"
            "                   40x64 color attribute resolution\n"
            "                   16 foreground and 8 background colors\n"
            "Audio:             CTC-controlled beeper, stereo\n"
            "Special Power:     built-in BASIC interpreter";
    }
    else {
        return
            "The KC85/4 was the pinnacle of East German 8-bit computer "
            "development, and a massive improvement to the KC85/3 with "
            "64 KByte RAM and a huge 64 KByte banked video memory.\n\n"
            "The big video memory allowed a greatly improved color resolution "
            "(8x1 pixels instead of 8x4), and true double buffering was "
            "possible by allowing the CPU to write to hidden video memory banks.\n\n"
            "The video memory layout was rotated by 90 degrees which "
            "simplified address computations on the CPU and allowed faster "
            "scrolling.\n\n"
            "There was also a little used 'hicolor' mode where each pixel could "
            "be assigned one out of 4 colors."
            "\n\n\n"
            "Manufacturer:      VEB Mikroelektronik Mühlhausen \n"
            "Release Date:      1989\n"
            "OS:                KC-CAOS 4.2, HC-BASIC in ROM\n"
            "Chips:             U880 @ 1.77 MHz (unlicensed Z80 clone)\n"
            "                   U855 (Z80 PIO clone)\n"
            "                   U857 (Z80 CTC clone)\n"
            "Memory:            64 KB RAM\n"
            "                   64 KB video memory\n"
            "                   20 KB ROM (8 KB BASIC, 12 KB CAOS)\n"
            "Graphics:          320x256 display resolution\n"
            "                   40x256 color attribute resolution\n"
            "                   16 foreground and 8 background colors\n"
            "                   special per-pixel color mode with 4 colors\n"
            "Audio:             CTC-controlled beeper, stereo\n"
            "Special Power:     90-degree rotated video memory layout";
    }
}

} // namespace YAKC

