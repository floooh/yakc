//------------------------------------------------------------------------------
//  kc85.cc
//------------------------------------------------------------------------------
#include "kc85.h"
#include "yakc/util/filetypes.h"

namespace YAKC {

kc85_t kc85;

// foreground colors
static uint32_t fg_palette[16] = {
    0xFF000000,     // black
    0xFFFF0000,     // blue
    0xFF0000FF,     // red
    0xFFFF00FF,     // magenta
    0xFF00FF00,     // green
    0xFFFFFF00,     // cyan
    0xFF00FFFF,     // yellow
    0xFFFFFFFF,     // white
    0xFF000000,     // black #2
    0xFFFF00A0,     // violet
    0xFF00A0FF,     // orange
    0xFFA000FF,     // purple
    0xFFA0FF00,     // blueish green
    0xFFFFA000,     // greenish blue
    0xFF00FFA0,     // yellow-green
    0xFFFFFFFF,     // white #2
};

// background colors
static uint32_t bg_palette[8] = {
    0xFF000000,      // black
    0xFFA00000,      // dark-blue
    0xFF0000A0,      // dark-red
    0xFFA000A0,      // dark-magenta
    0xFF00A000,      // dark-green
    0xFFA0A000,      // dark-cyan
    0xFF00A0A0,      // dark-yellow
    0xFFA0A0A0,      // gray
};

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
    this->cur_caos = os;
    this->on = true;
    this->io84 = 0;
    this->io86 = 0;
    this->pio_a = 0;
    this->pio_b = 0;

    // use keyboard helper only as input buffer to store multiple key pressed,
    // the KC85 doesn't have a typical keyboard matrix but has a serial
    // connection to the keyboard
    kbd_init(&board.kbd, 1);

    // fill RAM banks with noise (but not on KC85/4? at least the 4
    // doesn't have the random-color-pattern when switching it on)
    mem_unmap_all(&board.mem);
    if (system::kc85_4 == m) {
        clear(board.ram, sizeof(board.ram));
    }
    else {
        for (int i = 0; i < breadboard::num_ram_banks; i++) {
            memcpy(board.ram[i], board.random, breadboard::ram_bank_size);
        }
    }

    // set operating system pointers
    this->update_rom_pointers();

    // initialize hardware components
    board.freq_hz = (m == system::kc85_4) ? 1770000 : 1750000;
    z80_init(&board.z80, cpu_tick);
    z80pio_init(&board.z80pio, pio_in, pio_out);
    z80ctc_init(&board.z80ctc);
    beeper_init(&board.beeper, board.freq_hz, SOUND_SAMPLE_RATE, 0.5f);
    beeper_init(&board.beeper2, board.freq_hz, SOUND_SAMPLE_RATE, 0.5f);
    this->exp.poweron();
    this->cur_scanline = 0;

    // scanline length in clock ticks
    this->scanline_period = (m == system::kc85_4) ? 113 : 112;
    this->scanline_counter = this->scanline_period;
    this->cur_scanline = 0;

    // initial memory map
    this->pio_a = PIO_A_RAM|PIO_A_IRM|PIO_A_CAOS_ROM;
    if (system::kc85_3 == this->cur_model) {
        this->pio_a |= PIO_A_BASIC_ROM;
    }
    this->update_bank_switching();

    // execution on power-on starts at 0xF000
    board.z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
kc85_t::poweroff() {
    YAKC_ASSERT(this->on);
    mem_unmap_all(&board.mem);
    this->on = false;
}

//------------------------------------------------------------------------------
void
kc85_t::reset() {
    this->exp.reset();
    beeper_reset(&board.beeper);
    beeper_reset(&board.beeper2);
    z80_reset(&board.z80);
    z80ctc_reset(&board.z80ctc);
    z80pio_reset(&board.z80pio);
    this->pio_a = 0;
    this->pio_b = 0;
    this->io84 = 0;
    this->io86 = 0;
    this->cur_scanline = 0;
    this->scanline_counter = this->scanline_period;

    // execution after reset starts at 0xE000
    board.z80.PC = 0xE000;
}

//------------------------------------------------------------------------------
uint64_t
kc85_t::cpu_tick(int num_ticks, uint64_t pins) {

    // video decoding
    kc85.scanline_counter -= num_ticks;
    if (kc85.scanline_counter <= 0) {
        kc85.scanline_counter += kc85.scanline_period;
        if (kc85.cur_scanline < display_height) {
            kc85.decode_scanline();
        }
        kc85.cur_scanline++;
        // vertical blank signal? this triggers CTC2 for the video blinking effect
        if (kc85.cur_scanline >= 312) {
            kc85.cur_scanline = 0;
            pins |= Z80CTC_CLKTRG2;
        }
    }

    // tick the CTC
    for (int i = 0; i < num_ticks; i++) {
        pins = z80ctc_tick(&board.z80ctc, pins);
        // CTC channels 0 and 1 triggers control audio frequencies
        if (pins & Z80CTC_ZCTO0) {
            beeper_toggle(&board.beeper);
        }
        if (pins & Z80CTC_ZCTO1) {
            beeper_toggle(&board.beeper2);
        }
        // CTC channel 2 trigger controls video blink frequency
        if (pins & Z80CTC_ZCTO2) {
            kc85.ctc_blink_flag = !kc85.ctc_blink_flag;
        }
        pins &= Z80_PIN_MASK;
        if (beeper_tick(&board.beeper)) {
            board.audiobuffer.write(board.beeper.sample);
        }
        if (beeper_tick(&board.beeper2)) {
            board.audiobuffer2.write(board.beeper2.sample);
        }
    }

    // memory and IO requests
    if (pins & Z80_MREQ) {
        // memory request machine cycle
        const uint16_t addr = Z80_GET_ADDR(pins);
        if (pins & Z80_RD) {
            Z80_SET_DATA(pins, mem_rd(&board.mem, addr));
        }
        else if (pins & Z80_WR) {
            mem_wr(&board.mem, addr, Z80_GET_DATA(pins));
        }
    }
    else if (pins & Z80_IORQ) {
        // IO request machine cycle

        // on the KC85/3, the chips-select signals for the CTC and PIO
        // are generated through logic gates, on KC85/4 this is implemented
        // with a PROM chip (details are in the KC85/3 and KC85/4 service manuals)
        //
        // the I/O addresses are as follows:
        //
        //      0x88:   PIO Port A, data
        //      0x89:   PIO Port B, data
        //      0x8A:   PIO Port A, control
        //      0x8B:   PIO Port B, control
        //      0x8C:   CTC Channel 0
        //      0x8D:   CTC Channel 0
        //      0x8E:   CTC Channel 0
        //      0x8F:   CTC Channel 0
        //
        //      0x80:   controls the expansion module system, the upper
        //              8-bits of the port number address the module slot
        //      0x84:   (KC85/4 only) control the vide memory bank switching
        //      0x86:   (KC85/4 only) control RAM block at 0x4000 and ROM switching

        // check if any of the valid port numbers is addressed (0x80..0x8F)
        if ((pins & (Z80_A7|Z80_A6|Z80_A5|Z80_A4)) == Z80_A7) {
            // check if the PIO or CTC is addressed (0x88 to 0x8F)
            if (pins & Z80_A3) {
                pins &= Z80_PIN_MASK;
                // bit A2 selects the PIO or CTC
                if (pins & Z80_A2) {
                    // a CTC IO request
                    pins |= Z80CTC_CE;
                    if (pins & Z80_A0) { pins |= Z80CTC_CS0; }
                    if (pins & Z80_A1) { pins |= Z80CTC_CS1; }
                    pins = z80ctc_iorq(&board.z80ctc, pins) & Z80_PIN_MASK;
                }
                else {
                    // a PIO IO request
                    pins |= Z80PIO_CE;
                    if (pins & Z80_A0) { pins |= Z80PIO_BASEL; }
                    if (pins & Z80_A1) { pins |= Z80PIO_CDSEL; }
                    pins = z80pio_iorq(&board.z80pio, pins) & Z80_PIN_MASK;
                }
            }
            else {
                // we're in range 0x80..0x87
                const uint8_t data = Z80_GET_DATA(pins);
                switch (pins & (Z80_A2|Z80_A1|Z80_A0)) {
                    case 0x00:
                        // port 0x80: expansion module control, high byte
                        // of port address contains module slot address
                        {
                            const uint8_t slot_addr = Z80_GET_ADDR(pins)>>8;
                            if (pins & Z80_WR) {
                                // write expansion slot control byte
                                if (kc85.exp.slot_exists(slot_addr)) {
                                    kc85.exp.update_control_byte(slot_addr, data);
                                    kc85.update_bank_switching();
                                }
                            }
                            else {
                                // read expansion slot module type
                                Z80_SET_DATA(pins, kc85.exp.module_type_in_slot(slot_addr));
                            }
                        }
                        break;

                    case 0x04:
                        // port 0x84, KC85/4 only, this is a write-only 8-bit latch
                        if ((system::kc85_4 == kc85.cur_model) && (pins & Z80_WR)) {
                            kc85.io84 = data;
                            kc85.update_bank_switching();
                        }
                        break;

                    case 0x06:
                        // port 0x86, KC85/4 only, this is a write-only 8-bit latch
                        if ((system::kc85_4 == kc85.cur_model) && (pins & Z80_WR)) {
                            kc85.io86 = data;
                            kc85.update_bank_switching();
                        }
                        break;
                }
            }
        }
    }

    // interrupt daisy chain, CTC is higher priority then PIO
    Z80_DAISYCHAIN_BEGIN(pins)
    {
        pins = z80ctc_int(&board.z80ctc, pins);
        pins = z80pio_int(&board.z80pio, pins);
    }
    Z80_DAISYCHAIN_END(pins);
    
    return (pins & Z80_PIN_MASK);
}

//------------------------------------------------------------------------------
void
kc85_t::update_rom_pointers() {
    this->caos_c_ptr = nullptr;
    this->caos_c_size = 0;
    this->basic_ptr = nullptr;
    this->basic_size = 0;
    switch (this->cur_caos) {
        case os_rom::caos_hc900:
            this->caos_e_ptr  = roms.ptr(rom_images::hc900);
            this->caos_e_size = roms.size(rom_images::hc900);
            break;
        case os_rom::caos_2_2:
            this->caos_e_ptr  = roms.ptr(rom_images::caos22);
            this->caos_e_size = roms.size(rom_images::caos22);
            break;
        case os_rom::caos_3_1:
            this->caos_e_ptr  = roms.ptr(rom_images::caos31);
            this->caos_e_size = roms.size(rom_images::caos31);
            this->basic_ptr   = roms.ptr(rom_images::kc85_basic_rom);
            this->basic_size  = roms.size(rom_images::kc85_basic_rom);
            break;
        case os_rom::caos_3_4:
            this->caos_e_ptr  = roms.ptr(rom_images::caos34);
            this->caos_e_size = roms.size(rom_images::caos34);
            this->basic_ptr   = roms.ptr(rom_images::kc85_basic_rom);
            this->basic_size  = roms.size(rom_images::kc85_basic_rom);
            break;
        case os_rom::caos_4_2:
            this->caos_e_ptr  = roms.ptr(rom_images::caos42e);
            this->caos_e_size = roms.size(rom_images::caos42e);
            this->caos_c_ptr  = roms.ptr(rom_images::caos42c);
            this->caos_c_size = roms.size(rom_images::caos42c);
            this->basic_ptr   = roms.ptr(rom_images::kc85_basic_rom);
            this->basic_size  = roms.size(rom_images::kc85_basic_rom);
            break;
        default:
            YAKC_ASSERT(false);
            break;
    }
}

//------------------------------------------------------------------------------
uint8_t
kc85_t::pio_in(int port_id) {
    return 0xFF;
}

//------------------------------------------------------------------------------
void
kc85_t::pio_out(int port_id, uint8_t data) {
    if (Z80PIO_PORT_A == port_id) {
        kc85.pio_a = data;
    }
    else {
        kc85.pio_b = data;
        // FIXME: audio volume
    }
    kc85.update_bank_switching();
}

//------------------------------------------------------------------------------
uint64_t
kc85_t::exec(uint64_t start_tick, uint64_t end_tick) {
    YAKC_ASSERT(start_tick <= end_tick);
    uint32_t num_ticks = end_tick - start_tick;
    uint32_t ticks_executed = z80_exec(&board.z80, num_ticks);
    this->handle_keyboard_input();
    return start_tick + ticks_executed;
}

//------------------------------------------------------------------------------
void
kc85_t::on_ascii(uint8_t ascii) {
    kbd_key_down(&board.kbd, ascii);
    kbd_key_up(&board.kbd, ascii);
}

//------------------------------------------------------------------------------
void
kc85_t::on_key_down(uint8_t key) {
    kbd_key_down(&board.kbd, key);
}

//------------------------------------------------------------------------------
void
kc85_t::on_key_up(uint8_t key) {
    kbd_key_up(&board.kbd, key);
}

//------------------------------------------------------------------------------
void
kc85_t::handle_keyboard_input() {
    // this is a simplified version of the PIO-B interrupt service routine
    // which is normally triggered when the serial keyboard hardware
    // sends a new pulse (for details, see
    // https://github.com/floooh/yakc/blob/master/misc/kc85_3_kbdint.md )
    //
    // we ignore the whole tricky serial decoding and patch the
    // keycode directly into the right memory locations.

    // don't do anything if interrupts disabled, IX might point
    // to the wrong base address!
    if (!board.z80.IFF1) {
        return;
    }

    // get the first valid key code from the key buffer
    kbd_update(&board.kbd);
    uint8_t key_code = 0;
    for (const auto& item : board.kbd.key_buffer) {
        if (item.key != 0) {
            key_code = item.key;
            break;
        }
    }

    // status bits
    static const uint8_t timeout = (1<<3);
    static const uint8_t keyready = (1<<0);
    static const uint8_t repeat = (1<<4);
    static const uint8_t short_repeat_count = 8;
    static const uint8_t long_repeat_count = 60;

    const uint16_t ix = board.z80.IX;
    if (0 == key_code) {
        // if keycode is 0, this basically means the CTC3 timeout was hit
        mem_wr(&board.mem, ix+0x8, mem_rd(&board.mem, ix+0x8) | timeout); // set the CTC3 timeout bit
        mem_wr(&board.mem, ix+0xD, 0); // clear current keycode
    }
    else {
        // a valid keycode has been received, clear the timeout bit
        mem_wr(&board.mem, ix+0x8, mem_rd(&board.mem, ix+0x8) & ~timeout);

        // check for key-repeat
        if (key_code != mem_rd(&board.mem, ix+0xD)) {
            // no key-repeat
            mem_wr(&board.mem, ix+0xD, key_code);                               // write new keycode
            mem_wr(&board.mem, ix+0x8, mem_rd(&board.mem, ix+0x8)&~repeat);     // clear the first-key-repeat bit
            mem_wr(&board.mem, ix+0x8, mem_rd(&board.mem, ix+0x8)|keyready);    // set the key-ready bit
            mem_wr(&board.mem, ix+0xA, 0);                                      // clear the key-repeat counter
        }
        else {
            // handle key-repeat
            mem_wr(&board.mem, ix+0xA, mem_rd(&board.mem, ix+0xA)+1);   // increment repeat-pause-counter
            if (mem_rd(&board.mem, ix+0x8) & repeat) {
                // this is a followup, short key-repeat
                if (mem_rd(&board.mem, ix+0xA) < short_repeat_count) {
                    // wait some more...
                    return;
                }
            }
            else {
                // this is the first, long key-repeat
                if (mem_rd(&board.mem, ix+0xA) < long_repeat_count) {
                    // wait some more...
                    return;
                }
                else {
                    // first key-repeat pause over, set first-key-repeat flag
                    mem_wr(&board.mem, ix+0x8, mem_rd(&board.mem, ix+0x8)|repeat);
                }
            }
            // key-repeat triggered, just set the key-ready flag and reset repeat-count
            mem_wr(&board.mem, ix+0x8, mem_rd(&board.mem, ix+0x8)|keyready);
            mem_wr(&board.mem, ix+0xA, 0);
        }
    }
}

//------------------------------------------------------------------------------
void
kc85_t::update_bank_switching() {
    mem_unmap_layer(&board.mem, 0);
    if (system::kc85_4 != this->cur_model) {
        // ** KC85/3 or KC85/2 **

        // 16 KByte RAM at 0x0000 (write-protection not supported)
        if (pio_a & PIO_A_RAM) {
            mem_map_ram(&board.mem, 0, 0x0000, 0x4000, board.ram[0]);
        }
        // 16 KByte video memory at 0x8000
        if (pio_a & PIO_A_IRM) {
            mem_map_ram(&board.mem, 0, 0x8000, 0x4000, board.ram[irm0_page]);
        }
        // 8 KByte BASIC ROM at 0xC000 (only KC85/3)
        if (system::kc85_3 == this->cur_model) {
            if (pio_a & PIO_A_BASIC_ROM) {
                mem_map_rom(&board.mem, 0, 0xC000, this->basic_size, this->basic_ptr);
            }
        }
        // 8 KByte CAOS ROM at 0xF000
        if (pio_a & PIO_A_CAOS_ROM) {
            mem_map_rom(&board.mem, 0, 0xE000, this->caos_e_size, this->caos_e_ptr);
        }
    }
    else {
        // ** KC85/4 **

        // 16 KByte RAM at 0x0000 (write-protection not supported)
        if (pio_a & PIO_A_RAM) {
            mem_map_ram(&board.mem, 0, 0x0000, 0x4000, board.ram[0]);
        }
        // 16 KByte RAM at 0x4000
        if (this->io86 & IO86_RAM4) {
            mem_map_ram(&board.mem, 0, 0x4000, 0x4000, board.ram[1]); // this->io86 & IO86_RAM4_RO);
        }
        // 16 KByte RAM at 0x8000 (2 banks)
        if (pio_b & PIO_B_RAM8) {
            uint8_t* ram8_ptr = (this->io84 & IO84_SEL_RAM8) ? board.ram[3] : board.ram[2];
            mem_map_ram(&board.mem, 0, 0x8000, 0x4000, ram8_ptr); // pio_b & PIO_B_RAM8_RO);
        }
        // IRM is 4 banks, 2 for pixels, 2 for color,
        // the area A800 to BFFF is always mapped to IRM0!
        if (pio_a & PIO_A_IRM) {
            int irm_index = (this->io84 & 6)>>1;
            uint8_t* irm_ptr = board.ram[irm0_page + irm_index];
            // on the KC85, an access to IRM banks other than the
            // first is only possible for the first 10 KByte until
            // A800, memory access to the remaining 6 KBytes
            // (A800 to BFFF) is always forced to the first IRM bank
            // by the address decoder hardware (see KC85/4 service manual)
            mem_map_ram(&board.mem, 0, 0x8000, 0x2800, irm_ptr);

            // always force access to A800 and above to the first IRM bank
            mem_map_ram(&board.mem, 0, 0xA800, 0x1800, board.ram[irm0_page]+0x2800);
        }
        // 8 KByte BASIC ROM at 0xC000
        if (pio_a & PIO_A_BASIC_ROM) {
            mem_map_rom(&board.mem, 0, 0xC000, this->basic_size, this->basic_ptr);
        }
        // 4 KByte CAOS ROM-C at 0xC000
        if (this->io86 & IO86_CAOS_ROM_C) {
            mem_map_rom(&board.mem, 0, 0xC000, this->caos_c_size, this->caos_c_ptr);
        }
        // 8 KByte CAOS ROM-E at 0xE000
        if (pio_a & PIO_A_CAOS_ROM) {
            mem_map_rom(&board.mem, 0, 0xE000, this->caos_e_size, this->caos_e_ptr);
        }
    }

    // map modules in base-device expansion slots
    this->exp.update_memory_mappings();
}

//------------------------------------------------------------------------------
void
kc85_t::decode_audio(float* buffer, int num_samples) {
    board.audiobuffer.read(buffer, num_samples);
    board.audiobuffer2.read(buffer, num_samples, true);
}

//------------------------------------------------------------------------------
void
kc85_t::decode_8pixels(uint32_t* ptr, uint8_t pixels, uint8_t colors, bool blink_bg) const {
    // select foreground- and background color:
    //  bit 7: blinking
    //  bits 6..3: foreground color
    //  bits 2..0: background color
    //
    // index 0 is background color, index 1 is foreground color
    const uint8_t bg_index = colors & 0x7;
    const uint8_t fg_index = (colors>>3)&0xF;
    const unsigned int bg = bg_palette[bg_index];
    const unsigned int fg = (blink_bg && (colors & 0x80)) ? bg : fg_palette[fg_index];
    ptr[0] = pixels & 0x80 ? fg : bg;
    ptr[1] = pixels & 0x40 ? fg : bg;
    ptr[2] = pixels & 0x20 ? fg : bg;
    ptr[3] = pixels & 0x10 ? fg : bg;
    ptr[4] = pixels & 0x08 ? fg : bg;
    ptr[5] = pixels & 0x04 ? fg : bg;
    ptr[6] = pixels & 0x02 ? fg : bg;
    ptr[7] = pixels & 0x01 ? fg : bg;
}

//------------------------------------------------------------------------------
void
kc85_t::decode_scanline() {
    const int y = this->cur_scanline;
    const bool blink_bg = this->ctc_blink_flag && (this->pio_b & PIO_B_BLINK_ENABLED);
    const int width = display_width>>3;
    unsigned int* dst_ptr = &(board.rgba8_buffer[y*display_width]);
    if (system::kc85_4 == this->cur_model) {
        // KC85/4
        int irm_index = (this->io84 & 1) * 2;
        const uint8_t* pixel_data = board.ram[irm0_page + irm_index];
        const uint8_t* color_data = board.ram[irm0_page + irm_index + 1];
        for (int x = 0; x < width; x++) {
            int offset = y | (x<<8);
            uint8_t src_pixels = pixel_data[offset];
            uint8_t src_colors = color_data[offset];
            this->decode_8pixels(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_bg);
        }
    }
    else {
        // KC85/3
        const uint8_t* pixel_data = board.ram[irm0_page];
        const uint8_t* color_data = board.ram[irm0_page] + 0x2800;
        const int left_pixel_offset  = (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>4)&0xF)<<9);
        const int left_color_offset  = (((y>>2)&0x3f)<<5);
        const int right_pixel_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>6)&0x3)<<9);
        const int right_color_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | (((y>>6)&0x3)<<7);
        int pixel_offset, color_offset;
        for (int x = 0; x < width; x++) {
            if (x < 0x20) {
                // left 256x256 quad
                pixel_offset = x | left_pixel_offset;
                color_offset = x | left_color_offset;
            }
            else {
                // right 64x256 strip
                pixel_offset = 0x2000 + ((x&0x7) | right_pixel_offset);
                color_offset = 0x0800 + ((x&0x7) | right_color_offset);
            }
            uint8_t src_pixels = pixel_data[pixel_offset];
            uint8_t src_colors = color_data[color_offset];
            this->decode_8pixels(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_bg);
        }
    }
}

//------------------------------------------------------------------------------
const void*
kc85_t::framebuffer(int& out_width, int& out_height) {
    out_width = display_width;
    out_height = display_height;
    return board.rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
kc85_t::quickload(filesystem* fs, const char* name, filetype type, bool start) {
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
                    mem_wr(&board.mem, addr++, block[i]);
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
                    mem_wr(&board.mem, addr++, buf[i]);
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
        mem_wr(&board.mem, 0x36b7, 1);
        mem_wr(&board.mem, 0x3697, 1);
        for (int i = 0; i < 5; i++) {
            mem_wr(&board.mem, 0x1770 + i, mem_rd(&board.mem, 0x36b6 + i));
        }
    }
    // FIXME: patch Digger (see http://lanale.de/kc85_emu/KC85_Emu.html)
    if (strcmp(image_name, "DIGGER  COM\x01") == 0) {
        mem_wr16(&board.mem, 0x09AA, 0x0160);   // time for delay-loop 0160 instead of 0260
        mem_wr(&board.mem, 0x3d3a, 0xB5);       // OR L instead of OR (HL)
    }
    if (strcmp(image_name, "DIGGERJ") == 0) {
        mem_wr16(&board.mem, 0x09AA, 0x0260);
        mem_wr(&board.mem, 0x3d3a, 0xB5);       // OR L instead of OR (HL)
    }

    // start loaded image
    if (start && has_exec_addr) {
        auto& cpu = board.z80;
        cpu.A = 0x00;
        cpu.F = 0x10;
        cpu.BC = cpu.BC_ = 0x0000;
        cpu.DE = cpu.DE_ = 0x0000;
        cpu.HL = cpu.HL_ = 0x0000;
        cpu.AF_ = 0x0000;
        cpu.SP = 0x01C2;
        // delete ASCII buffer
        for (uint16_t addr = 0xb200; addr < 0xb700; addr++) {
            mem_wr(&board.mem, addr, 0);
        }
        mem_wr(&board.mem, 0xb7a0, 0);
        if (system::kc85_3 == this->cur_model) {
            cpu_tick(1, Z80_MAKE_PINS(Z80_IORQ|Z80_WR, 0x89, 0x9f));
            mem_wr16(&board.mem, cpu.SP, 0xf15c);
        }
        else if (system::kc85_4 == this->cur_model) {
            cpu_tick(1, Z80_MAKE_PINS(Z80_IORQ|Z80_WR, 0x89, 0xff));
            mem_wr16(&board.mem, cpu.SP, 0xf17e);
        }
        cpu.PC = exec_addr;
    }
    return true;
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

