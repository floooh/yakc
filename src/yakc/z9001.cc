//------------------------------------------------------------------------------
//  z9001.cc
//------------------------------------------------------------------------------
#include "z9001.h"

namespace YAKC {

//------------------------------------------------------------------------------
static uint64_t kbd_bits(int col, int line) {
    return (uint64_t(1)<<line)<<(col*8);
}

//------------------------------------------------------------------------------
void
z9001::init(breadboard* b) {
    this->board = b;

    // setup color palette (FIXME: fore- and background colors are identical?)
    this->pal[0] = 0xFF000000;     // black
    this->pal[1] = 0xFF0000FF;     // red
    this->pal[2] = 0xFF00FF00;     // green
    this->pal[3] = 0xFF00FFFF;     // yellow
    this->pal[4] = 0xFFFF0000;     // blue
    this->pal[5] = 0xFFFF00FF;     // purple
    this->pal[6] = 0xFFFFFF00;     // cyan
    this->pal[7] = 0xFFFFFFFF;     // white

    // setup the key map which translates ASCII to keyboard matrix bits
    const char* kbd_matrix =
        // no shift key pressed
        "01234567"
        "89:;,=.?"
        "@ABCDEFG"
        "HIJKLMNO"
        "PQRSTUVW"
        "XYZ   ^ "
        "        "
        "        "
        // shift key pressed
        "_!\"#$%&'"
        "()*+<->/"
        " abcdefg"
        "hijklmno"
        "pqrstuvw"
        "xyz     "
        "        "
        "        ";

    for (int shift=0; shift<2; shift++) { // shift layer
        for (int line=0; line<8; line++) {
            for (int col=0; col<8; col++) {
                ubyte c = kbd_matrix[shift*64 + line*8 + col];
                if (c != 0x20) {
                    uint64_t mask = kbd_bits(col, line);
                    if (shift) {
                        mask |= kbd_bits(0, 7);
                    }
                    this->key_map[c] = mask;
                }
            }
        }
    }

    // special keys
    this->key_map[0x00] = 0;
    this->key_map[0x03] = kbd_bits(6, 6);       // stop
    this->key_map[0x08] = kbd_bits(0, 6);       // cursor left
    this->key_map[0x09] = kbd_bits(1, 6);       // cursor right
    this->key_map[0x0A] = kbd_bits(2, 6);       // cursor up
    this->key_map[0x0B] = kbd_bits(3, 6);       // cursor down
    this->key_map[0x0D] = kbd_bits(5, 6);       // enter
    this->key_map[0x13] = kbd_bits(4, 5);       // pause
    this->key_map[0x14] = kbd_bits(1, 7);       // color
    this->key_map[0x19] = kbd_bits(3, 5);       // home
    this->key_map[0x1A] = kbd_bits(5, 5);       // insert
    this->key_map[0x1B] = kbd_bits(4, 6);       // esc
    this->key_map[0x1C] = kbd_bits(4, 7);       // list
    this->key_map[0x1D] = kbd_bits(5, 7);       // run
    this->key_map[0x20] = kbd_bits(7, 6);       // space
}

//------------------------------------------------------------------------------
void
z9001::setup_sound_funcs(const sound_funcs& funcs) {
    this->sound_cb = funcs;
}

//------------------------------------------------------------------------------
void
z9001::poweron(device m, os_rom os) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(device::any_z9001) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->cur_os = os;
    this->on = true;
    this->abs_cycle_count = 0;
    this->overflow_cycles = 0;
    this->key_mask = 0;
    this->kbd_column_mask = 0;
    this->kbd_line_mask = 0;
    this->keybuf.init(4);

    // map memory
    clear(this->ram, sizeof(this->ram));
    fill_random(this->color_ram, sizeof(this->color_ram));
    fill_random(this->video_ram, sizeof(this->video_ram));
    z80& cpu = this->board->cpu;
    cpu.mem.unmap_all();
    if (device::z9001 == m) {
        // emulate a Z9001 with 16 KByte RAM module and BASIC module
        cpu.mem.map(0, 0x0000, 0x8000, this->ram, true);
        cpu.mem.map(1, 0xC000, 0x2800, dump_basic_507_511, false);
        cpu.mem.map(1, 0xF000, 0x0800, dump_z9001_os12_1, false);
        cpu.mem.map(1, 0xF800, 0x0800, dump_z9001_os12_2, false);
    }
    else if (device::kc87 == m) {
        // emulate a KC87 with 48 KByte RAM
        cpu.mem.map(0, 0x0000, 0xC000, this->ram, true);
        cpu.mem.map(1, 0xC000, 0x2000, dump_z9001_basic, false);
        cpu.mem.map(1, 0xE000, 0x2000, dump_kc87_os_2, false);
        cpu.mem.map(0, 0xE800, 0x0400, this->color_ram, true);
    }
    cpu.mem.map(0, 0xEC00, 0x0400, this->video_ram, true);

    // initialize the clock at 2.4576 MHz
    this->board->clck.init(2458);

    // initialize hardware components
    z80pio& pio1 = this->board->pio;
    z80pio& pio2 = this->board->pio2;
    z80ctc& ctc = this->board->ctc;
    cpu.init(this);
    pio1.init(0, this);
    pio2.init(1, this);
    ctc.init(0, this);

    // setup interrupt daisy chain, from highest to lowest priority:
    //  CPU -> PIO1 -> PIO2 -> CTC
    cpu.connect_irq_device(&pio1.int_ctrl);
    pio1.int_ctrl.connect_irq_device(&pio2.int_ctrl);
    pio2.int_ctrl.connect_irq_device(&ctc.channels[0].int_ctrl);
    ctc.init_daisychain(nullptr);

    // configure a hardware counter to control the video blink attribute
    this->board->clck.config_timer(0, 100, blink_cb, this);

    // execution on power-on starts at 0xF000
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z9001::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->cpu.mem.unmap_all();
    this->on = false;    
}

//------------------------------------------------------------------------------
void
z9001::reset() {
    this->board->ctc.reset();
    this->board->pio.reset();
    this->board->pio2.reset();
    this->board->cpu.reset();
    this->overflow_cycles = 0;
    this->keybuf.reset();

    // execution after reset starts at 0x0000(??? -> doesn't work)
    this->board->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z9001::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    // FIXME: the speed multiplier isn't currently working because of the min/max cycle count limiter!
    YAKC_ASSERT(speed_multiplier > 0);
    this->cpu_ahead = false;
    this->cpu_behind = false;    
    z80& cpu = this->board->cpu;
    z80dbg& dbg = this->board->dbg;
    z80ctc& ctc = this->board->ctc;
    clock& clk = this->board->clck;

    this->handle_key();
    if (!dbg.paused) {
        if (this->abs_cycle_count == 0) {
            this->abs_cycle_count = min_cycle_count;
        }
        const int64_t num_cycles = clk.cycles(micro_secs*speed_multiplier) - this->overflow_cycles;
        uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
        if ((max_cycle_count != 0) && (abs_end_cycles > max_cycle_count)) {
            abs_end_cycles = max_cycle_count;
            this->cpu_ahead = true;
        }
        else if ((min_cycle_count != 0) && (abs_end_cycles < min_cycle_count)) {
            abs_end_cycles = min_cycle_count;
            this->cpu_behind = true;
        }
        while (this->abs_cycle_count < abs_end_cycles) {
            if (dbg.check_break(cpu)) {
                dbg.paused = true;
                this->overflow_cycles = 0;
                break;
            }
            dbg.store_pc_history(cpu); // FIXME: only if debug window open?
            int cycles_step = cpu.step();
            cycles_step += cpu.handle_irq();
            clk.update(cycles_step);
            ctc.update_timers(cycles_step);
            this->abs_cycle_count += cycles_step;
        }
        this->overflow_cycles = uint32_t(this->abs_cycle_count - abs_end_cycles);
    }
    this->decode_video();
}

//------------------------------------------------------------------------------
void
z9001::cpu_out(uword port, ubyte val) {
    // NOTE: there are 2 port numbers each for all CTC and PIO ports!
    z80pio& pio1 = this->board->pio;
    z80pio& pio2 = this->board->pio2;
    z80ctc& ctc = this->board->ctc;
    switch (port & 0xFF) {
        case 0x80:
        case 0x84:
            ctc.write(z80ctc::CTC0, val);
            break;
        case 0x81:
        case 0x85:
            ctc.write(z80ctc::CTC1, val);
            break;
        case 0x82:
        case 0x86:
            ctc.write(z80ctc::CTC2, val);
            break;
        case 0x83:
        case 0x87:
            ctc.write(z80ctc::CTC3, val);
            break;
        case 0x88:
        case 0x8C:
            pio1.write_data(z80pio::A, val);
            break;
        case 0x89:
        case 0x8D:
            pio1.write_data(z80pio::B, val);
            break;
        case 0x8A:
        case 0x8E:
            pio1.write_control(z80pio::A, val);
            break;
        case 0x8B:
        case 0x8F:
            pio1.write_control(z80pio::B, val);
            break;
        case 0x90:
        case 0x94:
            pio2.write_data(z80pio::A, val);
            break;
        case 0x91:
        case 0x95:
            pio2.write_data(z80pio::B, val);
            break;
        case 0x92:
        case 0x96:
            pio2.write_control(z80pio::A, val);
            break;
        case 0x93:
        case 0x97:
            pio2.write_control(z80pio::B, val);
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
ubyte
z9001::cpu_in(uword port) {
    z80pio& pio1 = this->board->pio;
    z80pio& pio2 = this->board->pio2;
    z80ctc& ctc = this->board->ctc;
    switch (port & 0xFF) {
        case 0x80:
        case 0x84:
            return ctc.read(z80ctc::CTC0);
        case 0x81:
        case 0x85:
            return ctc.read(z80ctc::CTC1);
        case 0x82:
        case 0x86:
            return ctc.read(z80ctc::CTC2);
        case 0x83:
        case 0x87:
            return ctc.read(z80ctc::CTC3);
        case 0x88:
        case 0x8C:
            return pio1.read_data(z80pio::A);
        case 0x89:
        case 0x8D:
            return pio1.read_data(z80pio::B);
        case 0x8A:
        case 0x8E:
        case 0x8B:
        case 0x8F:
            return pio1.read_control();
        case 0x90:
        case 0x94:
            return pio2.read_data(z80pio::A);
        case 0x91:
        case 0x95:
            return pio2.read_data(z80pio::B);
        case 0x92:
        case 0x96:
        case 0x93:
        case 0x97:
            return pio2.read_control();
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
z9001::pio_out(int pio_id, int port_id, ubyte val) {
    if (0 == pio_id) {
        if (z80pio::A == port_id) {
            // PIO1-A bits:
            // 0..1:    unused
            // 2:       display mode (0: 24 lines, 1: 20 lines)
            // 3..5:    border color
            // 6:       graphics LED on keyboard (0: off)
            // 7:       enable audio output (1: enabled)
            this->brd_color = (val>>3) & 7;
        }
        else {

        }
    }
    else {
        if (z80pio::A == port_id) {
            this->kbd_column_mask = ~val;
        }
        else {
            this->kbd_line_mask = ~val;
        }
    }
}

//------------------------------------------------------------------------------
ubyte
z9001::pio_in(int pio_id, int port_id) {
    if (0 == pio_id) {
        return 0;
    }
    else {
        if (z80pio::A == port_id) {
            // return column bits for requested line bits of currently pressed key
            ubyte column_bits = 0;
            for (int col=0; col<8; col++) {
                const ubyte line_bits = (this->key_mask>>(col*8)) & 0xFF;
                if (line_bits & this->kbd_line_mask) {
                    column_bits |= (1<<col);
                }
            }
            return ~column_bits;
        }
        else {
            // return line bits for requested column bits of currently pressed key
            ubyte line_bits = 0;
            for (int col=0; col<8; col++) {
                if ((1<<col) & this->kbd_column_mask) {
                    line_bits |= (this->key_mask>>(col*8)) & 0xFF;
                }
            }
            return ~line_bits;
        }
    }
}

//------------------------------------------------------------------------------
void
z9001::ctc_write(int ctc_id, int chn_id) {
    // this is the same as in the KC85/3 emu
    z80ctc& ctc = this->board->ctc;
    if (0 == chn_id) {
        // has the CTC channel state changed since last time?
        const auto& ctc_chn = ctc.channels[0];
        if ((ctc_chn.constant != this->ctc0_constant) ||
            (ctc_chn.mode ^ this->ctc0_mode)) {

            if (!(this->ctc0_mode & z80ctc::RESET) && (ctc_chn.mode & z80ctc::RESET)) {
                // CTC channel has become inactive, call the stop-callback
                if (this->sound_cb.stop) {
                    this->sound_cb.stop(this->sound_cb.userdata, this->abs_cycle_count, 0);
                }
                this->ctc0_mode = ctc_chn.mode;
            }
            else if (!(ctc_chn.mode & z80ctc::RESET)) {
                // CTC channel has become active or constant has changed, call sound-callback
                int div = ctc_chn.constant * ((ctc_chn.mode & z80ctc::PRESCALER_256) ? 256 : 16);
                if (div > 0) {
                    int hz = int((float(2457600) / float(div)) / 2.0f);
                    if (this->sound_cb.sound) {
                        this->sound_cb.sound(this->sound_cb.userdata, this->abs_cycle_count, 0, hz);
                    }
                }
                this->ctc0_constant = ctc_chn.constant;
                this->ctc0_mode = ctc_chn.mode;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
z9001::ctc_zcto(int ctc_id, int chn_id) {
    // CTC2 is configured as timer and triggers CTC3, which is configured
    // as counter, CTC3 triggers an interrupt which drives the system clock
    if (2 == chn_id) {
        this->board->ctc.ctrg(z80ctc::CTC3);
    }
}

//------------------------------------------------------------------------------
void
z9001::irq() {
    // forward interrupt request to CPU
    this->board->cpu.irq();
}

//------------------------------------------------------------------------------
void
z9001::put_key(ubyte ascii) {
    // replace BREAK with STOP
    if (ascii == 0x13) {
        ascii = 0x3;
    }
    this->keybuf.write(ascii);
}

//------------------------------------------------------------------------------
void
z9001::handle_key() {
    this->keybuf.advance();
    uint64_t new_key_mask = this->key_map[this->keybuf.read() & (max_num_keys-1)];
    if (new_key_mask != this->key_mask) {
        this->key_mask = new_key_mask;

        // PIO2-A is connected to keyboard matrix columns, PIO2-B to lines
        // send the line bits (active-low) to PIOB, this will trigger
        // an interrupt which initiates the keyboard input procedure
        ubyte line_bits = 0;
        for (int col=0; col<8; col++) {
            if ((1<<col) & this->kbd_column_mask) {
                line_bits |= (this->key_mask>>(col*8)) & 0xFF;
            }
        }
        this->board->pio2.write(z80pio::B, ~line_bits);
    }
}

//------------------------------------------------------------------------------
void
z9001::blink_cb(void* userdata) {
    z9001* self = (z9001*)userdata;
    self->blink_flipflop = 0 != (self->blink_counter++ & 0x10);
}

//------------------------------------------------------------------------------
void
z9001::border_color(float& out_red, float& out_green, float& out_blue) {
    if (device::kc87 == this->cur_model) {
        uint32_t bc = this->pal[this->brd_color & 7];
        out_red   = float(bc & 0xFF) / 255.0f;
        out_green = float((bc>>8)&0xFF) / 255.0f;
        out_blue  = float((bc>>16)&0xFF) / 255.0f;
    }
    else {
        out_red = out_blue = out_green = 0.0f;
    }
}

//------------------------------------------------------------------------------
void
z9001::decode_video() {

    // FIXME: there's also a 40x20 display mode
    uint32_t* dst = this->rgba8_buffer;
    ubyte* font;
    if (device::kc87 == this->cur_model) {
        font = dump_kc87_font_2;
    }
    else {
        font = dump_z9001_font;
    }
    int off = 0;
    if (device::kc87 == this->cur_model) {
        uint32_t fg, bg;
        for (int y = 0; y < 24; y++) {
            for (int py = 0; py < 8; py++) {
                for (int x = 0; x < 40; x++) {
                    ubyte chr = this->video_ram[off+x];
                    ubyte pixels = font[(chr<<3)|py];
                    ubyte color = this->color_ram[off+x];
                    if ((color & 0x80) && this->blink_flipflop) {
                        // blinking: swap bg and fg
                        fg = this->pal[color&7];
                        bg = this->pal[(color>>4)&7];
                    }
                    else {
                        fg = this->pal[(color>>4)&7];
                        bg = this->pal[color&7];
                    }
                    for (int px = 7; px >=0; px--) {
                        *dst++ = pixels & (1<<px) ? fg:bg;
                    }
                }
            }
            off += 40;
        }
    }
    else {
        for (int y = 0; y < 24; y++) {
            for (int py = 0; py < 8; py++) {
                for (int x = 0; x < 40; x++) {
                    ubyte chr = this->video_ram[off+x];
                    ubyte pixels = font[(chr<<3)|py];
                    for (int px = 7; px >=0; px--) {
                        *dst++ = pixels & (1<<px) ? 0xFFFFFFFF : 0xFF000000;
                    }
                }
            }
            off += 40;
        }
    }
}

//------------------------------------------------------------------------------
const char*
z9001::system_info() const {
    if (device::z9001 == this->cur_model) {
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
