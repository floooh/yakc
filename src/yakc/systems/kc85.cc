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
    out_width = KC85_DISPLAY_WIDTH;
    out_height = KC85_DISPLAY_HEIGHT;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
kc85_t::patch_cb(const char* snapshot_name, void* user_data) {
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
    /* FIXME
    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }
    kctap_header hdr;
    uint16_t exec_addr = 0;
    bool has_exec_addr = false;
    bool hdr_valid = false;
    if (filetype::kc_tap == type) {
        if (fs->read(fp, &hdr, sizeof(hdr)) == sizeof(hdr)) {
            hdr_valid = true;
            uint16_t addr = (hdr.kcc.load_addr_h<<8 | hdr.kcc.load_addr_l) & 0xFFFF;
            uint16_t end_addr = (hdr.kcc.end_addr_h<<8 | hdr.kcc.end_addr_l) & 0xFFFF;
            exec_addr = (hdr.kcc.exec_addr_h<<8 | hdr.kcc.exec_addr_l) & 0xFFFF;
            has_exec_addr = hdr.kcc.num_addr > 2;
            while (addr < end_addr) {
                // each block is 1 lead byte + 128 bytes data
                static const int block_size = 129;
                uint8_t block[block_size];
                fs->read(fp, block, block_size);
                for (int i = 1; (i < block_size) && (addr < end_addr); i++) {
                    mem_wr(&mem, addr++, block[i]);
                }
            }
        }
    }
    else if (filetype::kcc == type) {
        if (fs->read(fp, &hdr.kcc, sizeof(hdr.kcc)) == sizeof(hdr.kcc)) {
            hdr_valid = true;
            uint16_t addr = (hdr.kcc.load_addr_h<<8 | hdr.kcc.load_addr_l) & 0xFFFF;
            uint16_t end_addr = (hdr.kcc.end_addr_h<<8 | hdr.kcc.end_addr_l) & 0xFFFF;
            exec_addr = (hdr.kcc.exec_addr_h<<8 | hdr.kcc.exec_addr_l) & 0xFFFF;
            has_exec_addr = hdr.kcc.num_addr > 2;
            while (addr < end_addr) {
                static const int buf_size = 1024;
                uint8_t buf[buf_size];
                fs->read(fp, buf, buf_size);
                for (int i = 0; (i < buf_size) && (addr < end_addr); i++) {
                    mem_wr(&mem, addr++, buf[i]);
                }
            }
        }
    }
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }

    // FIXME: patch JUNGLE until I have time to do a proper
    // 'restoration', see Alexander Lang's KC emu here:
    // http://lanale.de/kc85_emu/KC85_Emu.html
    char image_name[sizeof(hdr.kcc.name) + 1];
    memcpy(image_name, hdr.kcc.name, sizeof(hdr.kcc.name));
    image_name[sizeof(image_name)-1] = 0;
    if (strcmp(image_name, "JUNGLE     ") == 0) {
        // patch start level 1 into memory
        mem_wr(&mem, 0x36b7, 1);
        mem_wr(&mem, 0x3697, 1);
        for (int i = 0; i < 5; i++) {
            mem_wr(&mem, 0x1770 + i, mem_rd(&mem, 0x36b6 + i));
        }
    }
    // FIXME: patch Digger (see http://lanale.de/kc85_emu/KC85_Emu.html)
    if (strcmp(image_name, "DIGGER  COM\x01") == 0) {
        mem_wr16(&mem, 0x09AA, 0x0160);   // time for delay-loop 0160 instead of 0260
        mem_wr(&mem, 0x3d3a, 0xB5);       // OR L instead of OR (HL)
    }
    if (strcmp(image_name, "DIGGERJ") == 0) {
        mem_wr16(&mem, 0x09AA, 0x0260);
        mem_wr(&mem, 0x3d3a, 0xB5);       // OR L instead of OR (HL)
    }

    // start loaded image
    if (start && has_exec_addr) {
        auto* cpu = &z80;
        z80_set_a(cpu, 0x00);
        z80_set_f(cpu, 0x10);
        z80_set_bc(cpu, 0x0000); z80_set_bc_(cpu, 0x0000);
        z80_set_de(cpu, 0x0000); z80_set_de_(cpu, 0x0000);
        z80_set_hl(cpu, 0x0000); z80_set_hl_(cpu, 0x0000);
        z80_set_af_(cpu, 0x0000);
        z80_set_sp(cpu, 0x01C2);
        // delete ASCII buffer
        for (uint16_t addr = 0xb200; addr < 0xb700; addr++) {
            mem_wr(&mem, addr, 0);
        }
        mem_wr(&mem, 0xb7a0, 0);
        if (system::kc85_3 == this->cur_model) {
            cpu_tick(1, Z80_MAKE_PINS(Z80_IORQ|Z80_WR, 0x89, 0x9f), 0);
            mem_wr16(&mem, z80_sp(cpu), 0xf15c);
        }
        else if (system::kc85_4 == this->cur_model) {
            cpu_tick(1, Z80_MAKE_PINS(Z80_IORQ|Z80_WR, 0x89, 0xff), 0);
            mem_wr16(&mem, z80_sp(cpu), 0xf17e);
        }
        z80_set_pc(cpu, exec_addr);
    }
    */
    return true;
}

//------------------------------------------------------------------------------
const char*
kc85_t::module_name(module_type type) {
    switch (type) {
        case none_module:       return "NONE";
        case m006_basic:        return "M006 BASIC";
        case m011_64kbyte:      return "M011 64 KBYTE RAM";
        case m012_texor:        return "M012 TEXOR";
        case m022_16kbyte:      return "M022 EXPANDER RAM";
        case m026_forth:        return "M026 FORTH";
        case m027_development:  return "M027 DEVELOPMENT";
        default:                return "UNKNOWN";
    }
}

//------------------------------------------------------------------------------
uint8_t
kc85_t::module_id(module_type type) {
    switch (type) {
        case none_module:       return 0xFF;
        case m006_basic:        return 0xFC;
        case m011_64kbyte:      return 0xF6;
        case m012_texor:        return 0xFB;
        case m022_16kbyte:      return 0xF4;
        case m026_forth:        return 0xFB;
        case m027_development:  return 0xFB;
        default:                return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
kc85_t::register_none_module(const char* name, const char* desc) {
    module& mod = this->mod_registry[none_module];
    mod.registered = true;
    mod.type = none_module;
    mod.name = name;
    mod.desc = desc;
}

//------------------------------------------------------------------------------
void
kc85_t::register_ram_module(module_type type, const char* desc) {
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
kc85_t::register_rom_module(module_type type, const uint8_t* ptr, unsigned int size, const char* desc) {
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
kc85_t::is_module_registered(module_type type) const {
    YAKC_ASSERT((type >= 0) && (type < num_module_types));
    return this->mod_registry[type].registered;
}

//------------------------------------------------------------------------------
const kc85_t::module&
kc85_t::module_template(module_type type) const {
    YAKC_ASSERT(is_module_registered(type));
    return this->mod_registry[type];
}

//------------------------------------------------------------------------------
const kc85_t::module&
kc85_t::mod_by_slot_addr(uint8_t slot_addr) const {
    module_type mod_type = none_module;
    for (int i = 0; i < KC85_NUM_SLOTS; i++) {
        const auto& slot = sys.exp.slot[i];
        if (slot.addr == slot_addr) {
            mod_type = (module_type) slot.mod.type;
            break;
        }
    }
    return this->mod_registry[mod_type];
}

//------------------------------------------------------------------------------
bool
kc85_t::slot_occupied(uint8_t slot_addr) const {
    for (int i = 0; i < KC85_NUM_SLOTS; i++) {
        if (sys.exp.slot[i].mod.type != KC85_MODULE_NONE) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
void
kc85_t::insert_module(uint8_t slot_addr, module_type type) {
    const auto& mod = mod_registry[type];
    if (mod.mem_ptr) {
        kc85_insert_rom_module(&sys, slot_addr, (kc85_module_type_t) type, mod.mem_ptr, mod.mem_size);
    }
    else {
        kc85_insert_ram_module(&sys, slot_addr, (kc85_module_type_t) type);
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

