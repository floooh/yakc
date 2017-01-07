//------------------------------------------------------------------------------
//  kc85.cc
//------------------------------------------------------------------------------
#include "kc85.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
kc85::init(breadboard* b, rom_images* r) {
    this->board = b;
    this->roms = r;
    this->exp.init();
}

//------------------------------------------------------------------------------
bool
kc85::check_roms(const rom_images& roms, device model, os_rom os) {
    if (device::kc85_2 == model) {
        if (os_rom::caos_hc900 == os) {
            return roms.has(rom_images::hc900);
        }
        else if (os_rom::caos_2_2 == os) {
            return roms.has(rom_images::caos22);
        }
    }
    else if (device::kc85_3 == model) {
        if (os_rom::caos_3_1 == os) {
            return roms.has(rom_images::caos31) && roms.has(rom_images::kc85_basic_rom);
        }
        else if (os_rom::caos_3_4 == os) {
            return roms.has(rom_images::caos34) && roms.has(rom_images::kc85_basic_rom);
        }
    }
    else if (device::kc85_4 == model) {
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
kc85::on_context_switched() {
    YAKC_ASSERT(this->board);
    this->update_rom_pointers();
    this->update_bank_switching();
    this->board->cpu.connect_irq_device(&this->board->z80ctc.channels[0].int_ctrl);
    this->board->z80ctc.init_daisychain(&this->board->z80pio.int_ctrl);
}

//------------------------------------------------------------------------------
void
kc85::decode_audio(float* buffer, int num_samples) {
    this->board->speaker.fill_samples(buffer, num_samples);
}

//------------------------------------------------------------------------------
const void*
kc85::framebuffer(int& out_width, int& out_height) {
    out_width = 320;
    out_height = 256;
    return this->video.rgba8_buffer;
}

//------------------------------------------------------------------------------
void
kc85::poweron(device m, os_rom os) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_kc85) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->cur_caos = os;
    this->on = true;
    this->key_code = 0;
    this->io84 = 0;
    this->io86 = 0;
    this->pio_a = 0;
    this->pio_b = 0;

    // fill RAM banks with noise (but not on KC85/4? at least the 4
    // doesn't have the random-color-pattern when switching it on)
    if (device::kc85_4 == m) {
        clear(this->board->ram, sizeof(this->board->ram));
    }
    else {
        fill_random(this->board->ram, sizeof(this->board->ram));
    }

    // set operating system pointers
    this->update_rom_pointers();

    // initialize the clock, the 85/4 runs at 1.77 MHz, the others at 1.75 MHz
    this->board->clck.init((m == device::kc85_4) ? 1770 : 1750);

    // initialize hardware components
    this->board->cpu.mem.unmap_all();
    this->board->z80pio.init(0);
    this->board->z80ctc.init(0);
    this->board->cpu.init();
    this->exp.init();
    this->video.init(m, this->board);
    this->audio.init(this->board);

    // setup interrupt controller daisy chain (CTC has highest priority before PIO)
    this->board->cpu.connect_irq_device(&this->board->z80ctc.channels[0].int_ctrl);
    this->board->z80ctc.init_daisychain(&this->board->z80pio.int_ctrl);

    // a 50Hz timer which trigger every vertical blank
    this->board->clck.config_timer_hz(0, 50);
    // a timer which triggers every PAL line for video memory decoding
    this->board->clck.config_timer_cycles(1, (m == device::kc85_4) ? 113 : 112);

    // initial memory map
    this->board->cpu.out(this, 0x88, 0x9f);

    // execution on power-on starts at 0xF000
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
kc85::poweroff() {
    YAKC_ASSERT(this->on);
    this->audio.reset();
    this->board->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
void
kc85::reset() {
    this->exp.reset();
    this->video.reset();
    this->audio.reset();
    this->board->z80ctc.reset();
    this->board->z80pio.reset();
    this->board->cpu.reset();
    this->io84 = 0;
    this->io86 = 0;
    // execution after reset starts at 0xE000
    this->board->cpu.PC = 0xE000;
}

//------------------------------------------------------------------------------
void
kc85::update_rom_pointers() {
    this->caos_c_ptr = nullptr;
    this->caos_c_size = 0;
    this->basic_ptr = nullptr;
    this->basic_size = 0;
    switch (this->cur_caos) {
        case os_rom::caos_hc900:
            this->caos_e_ptr  = this->roms->ptr(rom_images::hc900);
            this->caos_e_size = this->roms->size(rom_images::hc900);
            break;
        case os_rom::caos_2_2:
            this->caos_e_ptr  = this->roms->ptr(rom_images::caos22);
            this->caos_e_size = this->roms->size(rom_images::caos22);
            break;
        case os_rom::caos_3_1:
            this->caos_e_ptr  = this->roms->ptr(rom_images::caos31);
            this->caos_e_size = this->roms->size(rom_images::caos31);
            this->basic_ptr   = this->roms->ptr(rom_images::kc85_basic_rom);
            this->basic_size  = this->roms->size(rom_images::kc85_basic_rom);
            break;
        case os_rom::caos_3_4:
            this->caos_e_ptr  = this->roms->ptr(rom_images::caos34);
            this->caos_e_size = this->roms->size(rom_images::caos34);
            this->basic_ptr   = this->roms->ptr(rom_images::kc85_basic_rom);
            this->basic_size  = this->roms->size(rom_images::kc85_basic_rom);
            break;
        case os_rom::caos_4_2:
            this->caos_e_ptr  = this->roms->ptr(rom_images::caos42e);
            this->caos_e_size = this->roms->size(rom_images::caos42e);
            this->caos_c_ptr  = this->roms->ptr(rom_images::caos42c);
            this->caos_c_size = this->roms->size(rom_images::caos42c);
            this->basic_ptr   = this->roms->ptr(rom_images::kc85_basic_rom);
            this->basic_size  = this->roms->size(rom_images::kc85_basic_rom);
            break;
        default:
            YAKC_ASSERT(false);
            break;
    }
}

//------------------------------------------------------------------------------
uint64_t
kc85::step(uint64_t start_tick, uint64_t end_tick) {
    z80& cpu = this->board->cpu;
    z80ctc& ctc = this->board->z80ctc;
    clock& clk = this->board->clck;
    z80dbg& dbg = this->board->dbg;
    this->handle_keyboard_input();
    uint64_t cur_tick = start_tick;
    while (cur_tick < end_tick) {
        if (dbg.check_break(cpu)) {
            dbg.paused = true;
            return end_tick;
        }
        dbg.store_pc_history(cpu); // FIXME: only if debug window open?
        int ticks = cpu.step(this);
        ticks += cpu.handle_irq(this);
        clk.step(this, ticks);
        ctc.step(this, ticks);
        this->audio.step(ticks);
        cur_tick += ticks;
    }
    return cur_tick;
}

//------------------------------------------------------------------------------
void
kc85::put_key(ubyte ascii) {
    this->key_code = ascii;
}

//------------------------------------------------------------------------------
void
kc85::handle_keyboard_input() {
    // this is a simplified version of the PIO-B interrupt service routine
    // which is normally triggered when the serial keyboard hardware
    // sends a new pulse (for details, see
    // https://github.com/floooh/yakc/blob/master/misc/kc85_3_kbdint.md )
    //
    // we ignore the whole tricky serial decoding and patch the
    // keycode directly into the right memory locations.

    // don't do anything if interrupts disabled, IX might point
    // to the wrong base address!
    if (!this->board->cpu.IFF1) {
        return;
    }

    // status bits
    static const ubyte timeout = (1<<3);
    static const ubyte keyready = (1<<0);
    static const ubyte repeat = (1<<4);
    static const ubyte short_repeat_count = 8;
    static const ubyte long_repeat_count = 60;

    auto& mem = this->board->cpu.mem;
    const uword ix = this->board->cpu.IX;
    if (0 == this->key_code) {
        // if keycode is 0, this basically means the CTC3 timeout was hit
        mem.w8(ix+0x8, mem.r8(ix+0x8)|timeout); // set the CTC3 timeout bit
        mem.w8(ix+0xD, 0);                      // clear current keycode
    }
    else {
        // a valid keycode has been received, clear the timeout bit
        mem.w8(ix+0x8, mem.r8(ix+0x8)&~timeout);

        // check for key-repeat
        if (this->key_code != mem.r8(ix+0xD)) {
            // no key-repeat
            mem.w8(ix+0xD, this->key_code);             // write new keycode
            mem.w8(ix+0x8, mem.r8(ix+0x8)&~repeat);     // clear the first-key-repeat bit
            mem.w8(ix+0x8, mem.r8(ix+0x8)|keyready);    // set the key-ready bit
            mem.w8(ix+0xA, 0);                          // clear the key-repeat counter
        }
        else {
            // handle key-repeat
            mem.w8(ix+0xA, mem.r8(ix+0xA)+1);   // increment repeat-pause-counter
            if (mem.r8(ix+0x8) & repeat) {
                // this is a followup, short key-repeat
                if (mem.r8(ix+0xA) < short_repeat_count) {
                    // wait some more...
                    return;
                }
            }
            else {
                // this is the first, long key-repeat
                if (mem.r8(ix+0xA) < long_repeat_count) {
                    // wait some more...
                    return;
                }
                else {
                    // first key-repeat pause over, set first-key-repeat flag
                    mem.w8(ix+0x8, mem.r8(ix+0x8)|repeat);
                }
            }
            // key-repeat triggered, just set the key-ready flag and reset repeat-count
            mem.w8(ix+0x8, mem.r8(ix+0x8)|keyready);
            mem.w8(ix+0xA, 0);
        }
    }
}

//------------------------------------------------------------------------------
void
kc85::cpu_out(uword port, ubyte val) {
    switch (port & 0xFF) {
        case 0x80:
            if (this->exp.slot_exists(port>>8)) {
                this->exp.update_control_byte(port>>8, val);
                this->update_bank_switching();
            }
            break;
        case 0x84:
            if (device::kc85_4 == this->cur_model) {
                this->io84 = val;
                this->video.kc85_4_irm_control(val);
                this->update_bank_switching();
            }
            break;
        case 0x86:
            if (device::kc85_4 == this->cur_model) {
                this->io86 = val;
                this->update_bank_switching();
            }
            break;
        case 0x88:
            this->board->z80pio.write_data(this, z80pio::A, val);
            break;
        case 0x89:
            this->board->z80pio.write_data(this, z80pio::B, val);
            break;
        case 0x8A:
            this->board->z80pio.write_control(z80pio::A, val);
            break;
        case 0x8B:
            this->board->z80pio.write_control(z80pio::B, val);
            break;
        case 0x8C:
            this->board->z80ctc.write(this, z80ctc::CTC0, val);
            break;
        case 0x8D:
            this->board->z80ctc.write(this, z80ctc::CTC1, val);
            break;
        case 0x8E:
            this->board->z80ctc.write(this, z80ctc::CTC2, val);
            break;
        case 0x8F:
            this->board->z80ctc.write(this, z80ctc::CTC3, val);
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
ubyte
kc85::cpu_in(uword port) {
    // NOTE: on KC85/4, the hardware doesn't provide a way to read-back
    // the additional IO ports at 0x84 and 0x86 (see KC85/4 service manual)
    switch (port & 0xFF) {
        case 0x80:
            return this->exp.module_type_in_slot(port>>8);
        case 0x88:
            return this->board->z80pio.read_data(this, z80pio::A);
        case 0x89:
            return this->board->z80pio.read_data(this, z80pio::B);
        case 0x8A:
        case 0x8B:
            return this->board->z80pio.read_control();
        case 0x8C:
            return this->board->z80ctc.read(z80ctc::CTC0);
        case 0x8D:
            return this->board->z80ctc.read(z80ctc::CTC1);
        case 0x8E:
            return this->board->z80ctc.read(z80ctc::CTC2);
        case 0x8F:
            return this->board->z80ctc.read(z80ctc::CTC3);
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
kc85::ctc_write(int ctc_id, int chn_id) {
    if (chn_id < 2) {
        this->audio.ctc_write(chn_id);
    }
}

//------------------------------------------------------------------------------
void
kc85::ctc_zcto(int ctc_id, int chn_id) {
    if (chn_id == 2) {
        this->video.ctc_blink();
    }
}

//------------------------------------------------------------------------------
ubyte
kc85::pio_in(int pio_id, int port_id) {
    return z80pio::A == port_id ? this->pio_a : this->pio_b;
}

//------------------------------------------------------------------------------
void
kc85::pio_out(int pio_id, int port_id, ubyte val) {
    if (z80pio::A == port_id) {
        this->pio_a = val;
        this->update_bank_switching();
    }
    else {
        this->pio_b = val;
        this->video.pio_blink_enable(0 != (val & PIO_B_BLINK_ENABLED));
        // FIXME: audio volume
        //this->audio.update_volume(val & PIO_B_VOLUME_MASK);
    }
}

//------------------------------------------------------------------------------
void
kc85::irq(bool b) {
    // forward interrupt request to CPU
    this->board->cpu.irq(b);
}

//------------------------------------------------------------------------------
void
kc85::timer(int timer_id) {
    switch (timer_id) {
        case 0:
            // timer 0 is a 50Hz vertical blank timer and controls the
            // foreground color blinking
            this->board->z80ctc.ctrg(this, z80ctc::CTC2);
            break;
        case 1:
            // timer 1 triggers every PAL line (64ns) for the video scanline
            // decoder callback
            this->video.scanline();
            break;
    }
}

//------------------------------------------------------------------------------
void
kc85::update_bank_switching() {
    z80& cpu = this->board->cpu;
    cpu.mem.unmap_layer(0);

    if ((device::kc85_2 == this->cur_model) || (device::kc85_3 == this->cur_model)) {
        // ** KC85/3 or KC85/2 **

        // 16 KByte RAM at 0x0000 (write-protection not supported)
        if (pio_a & PIO_A_RAM) {
            cpu.mem.map(0, 0x0000, 0x4000, this->board->ram[0], true);
        }
        // 16 KByte video memory at 0x8000
        if (pio_a & PIO_A_IRM) {
            cpu.mem.map(0, 0x8000, 0x4000, this->board->ram[kc85_video::irm0_page], true);
        }
        // 8 KByte BASIC ROM at 0xC000 (only KC85/3)
        if (device::kc85_3 == this->cur_model) {
            if (pio_a & PIO_A_BASIC_ROM) {
                cpu.mem.map(0, 0xC000, this->basic_size, this->basic_ptr, false);
            }
        }
        // 8 KByte CAOS ROM at 0xF000
        if (pio_a & PIO_A_CAOS_ROM) {
            cpu.mem.map(0, 0xE000, this->caos_e_size, this->caos_e_ptr, false);
        }
    }
    else if (device::kc85_4 == this->cur_model) {
        // ** KC85/4 **

        // 16 KByte RAM at 0x0000 (write-protection not supported)
        if (pio_a & PIO_A_RAM) {
            cpu.mem.map(0, 0x0000, 0x4000, this->board->ram[0], true);
        }
        // 16 KByte RAM at 0x4000
        if (this->io86 & IO86_RAM4) {
            cpu.mem.map(0, 0x4000, 0x4000, this->board->ram[1], true); // this->io86 & IO86_RAM4_RO);
        }
        // 16 KByte RAM at 0x8000 (2 banks)
        if (pio_b & PIO_B_RAM8) {
            ubyte* ram8_ptr = (this->io84 & IO84_SEL_RAM8) ? this->board->ram[3] : this->board->ram[2];
            cpu.mem.map(0, 0x8000, 0x4000, ram8_ptr, true); // pio_b & PIO_B_RAM8_RO);
        }
        // IRM is 4 banks, 2 for pixels, 2 for color,
        // the area A800 to BFFF is always mapped to IRM0!
        if (pio_a & PIO_A_IRM) {
            int irm_index = (this->io84 & 6)>>1;
            ubyte* irm_ptr = this->board->ram[kc85_video::irm0_page + irm_index];
            // on the KC85, an access to IRM banks other than the
            // first is only possible for the first 10 KByte until
            // A800, memory access to the remaining 6 KBytes
            // (A800 to BFFF) is always forced to the first IRM bank
            // by the address decoder hardware (see KC85/4 service manual)
            cpu.mem.map(0, 0x8000, 0x2800, irm_ptr, true);

            // always force access to A800 and above to the first IRM bank
            cpu.mem.map(0, 0xA800, 0x1800, this->board->ram[kc85_video::irm0_page]+0x2800, true);
        }
        // 8 KByte BASIC ROM at 0xC000
        if (pio_a & PIO_A_BASIC_ROM) {
            cpu.mem.map(0, 0xC000, this->basic_size, this->basic_ptr, false);
        }
        // 4 KByte CAOS ROM-C at 0xC000
        if (this->io86 & IO86_CAOS_ROM_C) {
            cpu.mem.map(0, 0xC000, this->caos_c_size, this->caos_c_ptr, false);
        }
        // 8 KByte CAOS ROM-E at 0xE000
        if (pio_a & PIO_A_CAOS_ROM) {
            cpu.mem.map(0, 0xE000, this->caos_e_size, this->caos_e_ptr, false);
        }
    }

    // map modules in base-device expansion slots
    this->exp.update_memory_mappings(cpu.mem);
}

//------------------------------------------------------------------------------
const char*
kc85::system_info() const {
    if (this->cur_model == device::kc85_2) {
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
    else if (this->cur_model == device::kc85_3) {
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

