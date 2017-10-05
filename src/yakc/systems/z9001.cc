//------------------------------------------------------------------------------
//  z9001.cc
//------------------------------------------------------------------------------
#include "z9001.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

static uint32_t palette[8] = {
    0xFF000000,     // black
    0xFF0000FF,     // red
    0xFF00FF00,     // green
    0xFF00FFFF,     // yellow
    0xFFFF0000,     // blue
    0xFFFF00FF,     // purple
    0xFFFFFF00,     // cyan
    0xFFFFFFFF,     // white
};

//------------------------------------------------------------------------------
static uint64_t kbd_bits(int col, int line) {
    return (uint64_t(1)<<line)<<(col*8);
}

//------------------------------------------------------------------------------
void
z9001::init(breadboard* b, rom_images* r) {
    this->board = b;
    this->roms = r;
    this->rgba8_buffer = this->board->rgba8_buffer;

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
                uint8_t c = kbd_matrix[shift*64 + line*8 + col];
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
bool
z9001::check_roms(const rom_images& roms, system model, os_rom os) {
    if ((system::z9001 == model) && (os_rom::z9001_os_1_2 == os)) {
        return roms.has(rom_images::z9001_os12_1) &&
               roms.has(rom_images::z9001_os12_2) &&
               roms.has(rom_images::z9001_basic_507_511) &&
               roms.has(rom_images::z9001_font);
    }
    else if ((system::kc87 == model) && (os_rom::kc87_os_2 == os)) {
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
z9001::init_memory_mapping() {
    z80& cpu = this->board->z80;
    cpu.mem.unmap_all();
    if (system::z9001 == this->cur_model) {
        // emulate a Z9001 with 16 KByte RAM module and BASIC module
        cpu.mem.map(0, 0x0000, 0x8000, this->board->ram[0], true);
        cpu.mem.map(1, 0xC000, 0x2800, this->roms->ptr(rom_images::z9001_basic_507_511), false);
        cpu.mem.map(1, 0xF000, 0x0800, this->roms->ptr(rom_images::z9001_os12_1), false);
        cpu.mem.map(1, 0xF800, 0x0800, this->roms->ptr(rom_images::z9001_os12_2), false);
    }
    else if (system::kc87 == this->cur_model) {
        // emulate a KC87 with 48 KByte RAM
        cpu.mem.map(0, 0x0000, 0xC000, this->board->ram[0], true);
        cpu.mem.map(1, 0xC000, 0x2000, this->roms->ptr(rom_images::z9001_basic), false);
        cpu.mem.map(1, 0xE000, 0x2000, this->roms->ptr(rom_images::kc87_os_2), false);
        cpu.mem.map(0, 0xE800, 0x0400, this->board->ram[color_ram_page], true);
    }
    cpu.mem.map(0, 0xEC00, 0x0400, this->board->ram[video_ram_page], true);
}

//------------------------------------------------------------------------------
void
z9001::on_context_switched() {
    this->init_memory_mapping();
    z80& cpu = this->board->z80;
    z80pio& pio1 = this->board->z80pio;
    z80pio& pio2 = this->board->z80pio2;
    z80ctc& ctc = this->board->z80ctc;
    cpu.connect_irq_device(&pio1.int_ctrl);
    pio1.int_ctrl.connect_irq_device(&pio2.int_ctrl);
    pio2.int_ctrl.connect_irq_device(&ctc.channels[0].int_ctrl);
    ctc.init_daisychain(nullptr);
}

//------------------------------------------------------------------------------
void
z9001::poweron(system m, os_rom os) {
    YAKC_ASSERT(this->board);
    YAKC_ASSERT(int(system::any_z9001) & int(m));
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->cur_os = os;
    this->on = true;
    this->key_mask = 0;
    this->kbd_column_mask = 0;
    this->kbd_line_mask = 0;
    this->keybuf.init(4);
    this->ctc0_mode = z80ctc::RESET;
    this->ctc0_constant = 0;

    // map memory
    for (int i = 0; i < breadboard::num_ram_banks; i++) {
        memcpy(this->board->ram[i], this->board->random, breadboard::ram_bank_size);
    }
    this->init_memory_mapping();

    // initialize the clock at 2.4576 MHz
    this->board->clck.init(2458);

    // initialize hardware components
    this->board->z80.init();
    this->board->z80pio.init(0);
    this->board->z80pio2.init(1);
    this->board->z80ctc.init(0);
    this->board->speaker.init(this->board->clck.base_freq_khz, SOUND_SAMPLE_RATE);

    // setup interrupt daisy chain, from highest to lowest priority:
    //  CPU -> PIO1 -> PIO2 -> CTC
    this->board->z80.connect_irq_device(&this->board->z80pio.int_ctrl);
    this->board->z80pio.int_ctrl.connect_irq_device(&this->board->z80pio2.int_ctrl);
    this->board->z80pio2.int_ctrl.connect_irq_device(&this->board->z80ctc.channels[0].int_ctrl);
    this->board->z80ctc.init_daisychain(nullptr);

    // configure a hardware counter to control the video blink attribute
    this->board->clck.config_timer_hz(0, 100);

    // execution on power-on starts at 0xF000
    this->board->z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
void
z9001::poweroff() {
    YAKC_ASSERT(this->on);
    this->board->speaker.stop_all();
    this->board->z80.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
void
z9001::reset() {
    this->key_mask = 0;
    this->kbd_column_mask = 0;
    this->kbd_line_mask = 0;
    this->keybuf.reset();
    this->ctc0_mode = z80ctc::RESET;
    this->ctc0_constant = 0;
    this->board->speaker.stop_all();
    this->board->z80ctc.reset();
    this->board->z80pio.reset();
    this->board->z80pio2.reset();
    this->board->z80.reset();
    this->keybuf.reset();

    // execution after reset starts at 0x0000(??? -> doesn't work)
    this->board->z80.PC = 0xF000;
}

//------------------------------------------------------------------------------
uint64_t
z9001::step(uint64_t start_tick, uint64_t end_tick) {
    auto& cpu = this->board->z80;
    auto& dbg = this->board->dbg;
    this->handle_key();
    this->cur_tick = start_tick;
    while (this->cur_tick < end_tick) {
        uint32_t ticks = cpu.step(this);
        ticks += cpu.handle_irq(this);
        this->board->clck.step(this, ticks);
        this->board->z80ctc.step(this, ticks);
        this->board->speaker.step(ticks);
        if (dbg.step(cpu.PC, ticks)) {
            return end_tick;
        }
        this->cur_tick += ticks;
    }
    this->decode_video();
    return this->cur_tick;
}

//------------------------------------------------------------------------------
uint32_t
z9001::step_debug() {
    auto& cpu = this->board->z80;
    auto& dbg = this->board->dbg;
    uint64_t all_ticks = 0;
    uint16_t old_pc;
    do {
        old_pc = cpu.PC;
        uint32_t ticks = cpu.step(this);
        ticks += cpu.handle_irq(this);
        this->board->clck.step(this, ticks);
        this->board->z80ctc.step(this, ticks);
        this->board->speaker.step(ticks);
        dbg.step(cpu.PC, ticks);
        all_ticks += ticks;
    }
    while ((old_pc == cpu.PC) && !cpu.INV);    
    this->decode_video();
    return uint32_t(all_ticks);
}

//------------------------------------------------------------------------------
void
z9001::cpu_out(uint16_t port, uint8_t val) {
    // NOTE: there are 2 port numbers each for all CTC and PIO ports!
    z80pio& pio1 = this->board->z80pio;
    z80pio& pio2 = this->board->z80pio2;
    z80ctc& ctc = this->board->z80ctc;
    switch (port & 0xFF) {
        case 0x80:
        case 0x84:
            ctc.write(this, z80ctc::CTC0, val);
            break;
        case 0x81:
        case 0x85:
            ctc.write(this, z80ctc::CTC1, val);
            break;
        case 0x82:
        case 0x86:
            ctc.write(this, z80ctc::CTC2, val);
            break;
        case 0x83:
        case 0x87:
            ctc.write(this, z80ctc::CTC3, val);
            break;
        case 0x88:
        case 0x8C:
            pio1.write_data(this, z80pio::A, val);
            break;
        case 0x89:
        case 0x8D:
            pio1.write_data(this, z80pio::B, val);
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
            pio2.write_data(this, z80pio::A, val);
            break;
        case 0x91:
        case 0x95:
            pio2.write_data(this, z80pio::B, val);
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
uint8_t
z9001::cpu_in(uint16_t port) {
    z80pio& pio1 = this->board->z80pio;
    z80pio& pio2 = this->board->z80pio2;
    z80ctc& ctc = this->board->z80ctc;
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
            return pio1.read_data(this, z80pio::A);
        case 0x89:
        case 0x8D:
            return pio1.read_data(this, z80pio::B);
        case 0x8A:
        case 0x8E:
        case 0x8B:
        case 0x8F:
            return pio1.read_control();
        case 0x90:
        case 0x94:
            return pio2.read_data(this, z80pio::A);
        case 0x91:
        case 0x95:
            return pio2.read_data(this, z80pio::B);
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
z9001::pio_out(int pio_id, int port_id, uint8_t val) {
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
uint8_t
z9001::pio_in(int pio_id, int port_id) {
    if (0 == pio_id) {
        return 0;
    }
    else {
        if (z80pio::A == port_id) {
            // return column bits for requested line bits of currently pressed key
            uint8_t column_bits = 0;
            for (int col=0; col<8; col++) {
                const uint8_t line_bits = (this->key_mask>>(col*8)) & 0xFF;
                if (line_bits & this->kbd_line_mask) {
                    column_bits |= (1<<col);
                }
            }
            return ~column_bits;
        }
        else {
            // return line bits for requested column bits of currently pressed key
            uint8_t line_bits = 0;
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
    z80ctc& ctc = this->board->z80ctc;
    if (0 == chn_id) {
        // has the CTC channel state changed since last time?
        const auto& ctc_chn = ctc.channels[0];
        if ((ctc_chn.constant != this->ctc0_constant) ||
            (ctc_chn.mode ^ this->ctc0_mode)) {

            if (!(this->ctc0_mode & z80ctc::RESET) && (ctc_chn.mode & z80ctc::RESET)) {
                // CTC channel has become inactive, call the stop-callback
                this->board->speaker.stop(0);
                this->ctc0_mode = ctc_chn.mode;
            }
            else if (!(ctc_chn.mode & z80ctc::RESET)) {
                // CTC channel has become active or constant has changed, call sound-callback
                int div = ctc_chn.constant * ((ctc_chn.mode & z80ctc::PRESCALER_256) ? 256 : 16);
                if (div > 0) {
                    int hz = int((float(2457600) / float(div)) / 2.0f);
                    this->board->speaker.start(0, hz);
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
        this->board->z80ctc.ctrg(this, z80ctc::CTC3);
    }
}

//------------------------------------------------------------------------------
void
z9001::irq(bool b) {
    // forward interrupt request to CPU
    this->board->z80.irq(b);
}

//------------------------------------------------------------------------------
void
z9001::timer(int timer_id) {
    if (0 == timer_id) {
        this->blink_flipflop = 0 != (this->blink_counter++ & 0x10);
    }
}


//------------------------------------------------------------------------------
void
z9001::put_key(uint8_t ascii) {
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
        uint8_t line_bits = 0;
        for (int col=0; col<8; col++) {
            if ((1<<col) & this->kbd_column_mask) {
                line_bits |= (this->key_mask>>(col*8)) & 0xFF;
            }
        }
        this->board->z80pio2.write(this, z80pio::B, ~line_bits);
    }
}

//------------------------------------------------------------------------------
void
z9001::border_color(float& out_red, float& out_green, float& out_blue) {
    if (system::kc87 == this->cur_model) {
        uint32_t bc = palette[this->brd_color & 7];
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
    const uint8_t* vidmem = this->board->ram[video_ram_page];
    const uint8_t* colmem = this->board->ram[color_ram_page];
    uint8_t* font;
    if (system::kc87 == this->cur_model) {
        font = this->roms->ptr(rom_images::kc87_font_2);
    }
    else {
        font = this->roms->ptr(rom_images::z9001_font);
    }
    int off = 0;
    if (system::kc87 == this->cur_model) {
        uint32_t fg, bg;
        for (int y = 0; y < 24; y++) {
            for (int py = 0; py < 8; py++) {
                for (int x = 0; x < 40; x++) {
                    uint8_t chr = vidmem[off+x];
                    uint8_t pixels = font[(chr<<3)|py];
                    uint8_t color = colmem[off+x];
                    if ((color & 0x80) && this->blink_flipflop) {
                        // blinking: swap bg and fg
                        fg = palette[color&7];
                        bg = palette[(color>>4)&7];
                    }
                    else {
                        fg = palette[(color>>4)&7];
                        bg = palette[color&7];
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
                    uint8_t chr = vidmem[off+x];
                    uint8_t pixels = font[(chr<<3)|py];
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
void
z9001::decode_audio(float* buffer, int num_samples) {
    this->board->speaker.fill_samples(buffer, num_samples);
}

//------------------------------------------------------------------------------
const void*
z9001::framebuffer(int& out_width, int& out_height) {
    out_width = display_width;
    out_height = display_height;
    return this->rgba8_buffer;
}

//------------------------------------------------------------------------------
bool
z9001::quickload(filesystem* fs, const char* name, filetype type, bool start) {

    auto fp = fs->open(name, filesystem::mode::read);
    if (!fp) {
        return false;
    }

    // same fileformats as KC85/2..4
    kctap_header hdr;
    uint16_t exec_addr = 0;
    bool has_exec_addr = false;
    bool hdr_valid = false;
    auto& mem = this->board->z80.mem;
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
                    mem.w8(addr++, block[i]);
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
                    mem.w8(addr++, buf[i]);
                }
            }
        }
    }
    fs->close(fp);
    fs->rm(name);
    if (!hdr_valid) {
        return false;
    }

    // start loaded image
    if (start && has_exec_addr) {
        auto& cpu = this->board->z80;
        cpu.A = 0x00;
        cpu.F = 0x10;
        cpu.BC = cpu.BC_ = 0x0000;
        cpu.DE = cpu.DE_ = 0x0000;
        cpu.HL = cpu.HL_ = 0x0000;
        cpu.AF_ = 0x0000;
        cpu.PC = exec_addr;
    }
    return true;
}

//------------------------------------------------------------------------------
const char*
z9001::system_info() const {
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
