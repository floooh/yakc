//------------------------------------------------------------------------------
//  z9001.cc
//------------------------------------------------------------------------------
#include "z9001.h"

namespace YAKC {

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

    // map memory
    clear(this->ram, sizeof(this->ram));
    clear(this->color_ram, sizeof(this->color_ram));
    clear(this->video_ram, sizeof(this->video_ram));
    z80& cpu = this->board->cpu;
    cpu.mem.unmap_all();
    if (device::kc87 == m) {
        cpu.mem.map(0, 0x0000, 0x4000, this->ram, true);
        cpu.mem.map(1, 0xC000, 0x2000, dump_z9001_basic, false);
        cpu.mem.map(1, 0xE000, 0x2000, dump_kc87_os_2, false);
    }
    cpu.mem.map(0, 0xE800, 0x0400, this->color_ram, true);
    cpu.mem.map(0, 0xEC00, 0x0400, this->video_ram, true);

    // initialize the clock at 2.4576 MHz
    this->board->clck.init(2458);

    // initialize hardware components
    z80pio& pio1 = this->board->pio;
    z80pio& pio2 = this->board->pio2;
    z80ctc& ctc = this->board->ctc;
    cpu.init(in_cb, out_cb, this);
    pio1.init();
    pio2.init();
    ctc.init();

    // setup interrupt daisy chain, from highest to lowest priority:
    //  CPU -> PIO1 -> PIO2 -> CTC
    pio1.int_ctrl.connect_cpu(z80::irq, &cpu);
    pio2.int_ctrl.connect_cpu(z80::irq, &cpu);
    for (int i = 0; i < z80ctc::num_channels; i++) {
        ctc.channels[i].int_ctrl.connect_cpu(z80::irq, &cpu);
    }
    cpu.connect_irq_device(&pio1.int_ctrl);
    pio1.int_ctrl.connect_irq_device(&pio2.int_ctrl);
    pio2.int_ctrl.connect_irq_device(&ctc.channels[0].int_ctrl);
    ctc.init_daisychain(nullptr);

    // CTC2 is configured as timer and triggers CTC3, which is configured
    // as counter, CTC3 triggers an interrupt which drives the system clock
    ctc.connect_zcto2(z80ctc::ctrg3, &ctc);

    // configure a hardware counter to control the video blink attribute
    this->board->clck.config_timer(0, 50, blink_cb, this);

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

    if (!dbg.paused) {
        if (this->abs_cycle_count == 0) {
            this->abs_cycle_count = min_cycle_count;
        }
        const int64_t num_cycles = clk.cycles(micro_secs*speed_multiplier) - this->overflow_cycles;
        uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
        if (abs_end_cycles > max_cycle_count) {
            abs_end_cycles = max_cycle_count;
            this->cpu_ahead = true;
        }
        else if (abs_end_cycles < min_cycle_count) {
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
z9001::out_cb(void* userdata, uword port, ubyte val) {
    // NOTE: there are 2 port numbers each for all CTC and PIO ports!
    z9001* self = (z9001*)userdata;
    z80pio& pio1 = self->board->pio;
    z80pio& pio2 = self->board->pio2;
    z80ctc& ctc = self->board->ctc;
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
            pio1.control(z80pio::A, val);
            break;
        case 0x8B:
        case 0x8F:
            pio1.control(z80pio::B, val);
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
            pio2.control(z80pio::A, val);
            break;
        case 0x93:
        case 0x97:
            pio2.control(z80pio::B, val);
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
ubyte
z9001::in_cb(void* userdata, uword port) {
    z9001* self = (z9001*)userdata;
    z80pio& pio1 = self->board->pio;
    z80pio& pio2 = self->board->pio2;
    z80ctc& ctc = self->board->ctc;
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
        case 0x90:
        case 0x94:
            return pio2.read_data(z80pio::A);
            break;
        case 0x91:
        case 0x95:
            return pio2.read_data(z80pio::B);
            break;
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
z9001::put_key(ubyte ascii) {
    // FIXME: HACK!
    static ubyte chr = 0;
    if (ascii != chr) {
        chr = ascii;
        this->board->cpu.mem.w8(0x0025, ascii);
    }
    this->board->cpu.mem.w8(0x006A, 0);
}

//------------------------------------------------------------------------------
void
z9001::blink_cb(void* userdata) {
    z9001* self = (z9001*)userdata;
    // FIXME: what is the exact blink frequency?
    self->blink_flipflop = self->blink_counter++ & 0x10;
}

//------------------------------------------------------------------------------
void
z9001::decode_video() {

    // FIXME: there's also a 40x20 display mode
    uint32_t* dst = RGBA8Buffer;
    ubyte* font;
    if (device::kc87 == this->cur_model) {
        font = dump_kc87_font_2;
    }
    else {
        font = dump_z9001_font;
    }
    int off = 0;
    uint32_t fg, bg;
    for (int y = 0; y < 24; y++) {
        for (int py = 0; py < 8; py++) {
            for (int x = 0; x < 40; x++) {
                ubyte chr = this->video_ram[off+x];
                ubyte pixels = dump_kc87_font_2[(chr<<3)|py];
                ubyte color = this->color_ram[off+x];
                if ((color & 0x80) && this->blink_flipflop) {
                    // implement blinking, swap bg and fg
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

//------------------------------------------------------------------------------
const char*
z9001::system_info() const {
    return "Z9001 FIXME";
}

} // namespace YAKC
