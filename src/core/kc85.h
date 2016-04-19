#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85
    @brief wrapper class for the entire KC85/3 or KC85/4 system
*/
#include "core/clock.h"
#include "core/z80.h"
#include "core/z80dbg.h"
#include "core/z80pio.h"
#include "core/z80ctc.h"
#include "roms/roms.h"
#include "core/kc85_video.h"
#include "core/kc85_audio.h"
#include "core/kc85_exp.h"
#include "core/kc85_roms.h"

namespace yakc {

class kc85 {
public:
    /// ram banks
    ubyte ram[4][0x4000];

    /// IO bits
    enum {
        PIO_A_CAOS_ROM    = (1<<0),
        PIO_A_RAM         = (1<<1),
        PIO_A_IRM         = (1<<2),
        PIO_A_RAM_RO      = (1<<3),
        PIO_A_UNUSED      = (1<<4),
        PIO_A_TAPE_LED    = (1<<5),
        PIO_A_TAPE_MOTOR  = (1<<6),
        PIO_A_BASIC_ROM   = (1<<7),

        PIO_B_VOLUME_MASK = (1<<5)-1,
        PIO_B_RAM8 = (1<<5),            // KC85/4 only
        PIO_B_RAM8_RO = (1<<6),         // KC85/4 only
        PIO_B_BLINK_ENABLED = (1<<7),

        // KC85/4 only
        IO84_SEL_VIEW_IMG   = (1<<0),  // 0: display img0, 1: display img1
        IO84_SEL_CPU_COLOR  = (1<<1),  // 0: access pixels, 1: access colors
        IO84_SEL_CPU_IMG    = (1<<2),  // 0: access img0, 1: access img1
        IO84_HICOLOR        = (1<<3),  // 0: hicolor mode off, 1: hicolor mode on
        IO84_SEL_RAM8       = (1<<4),  // select RAM8 block 0 or 1
        IO84_BLOCKSEL_RAM8  = (1<<5),  // no idea what that does...?

        IO86_RAM4   = (1<<0),
        IO86_RAM4_RO = (1<<1),
        IO86_CAOS_ROM_C = (1<<7)
    };

    /// hardware components
    clock clck;
    z80 cpu;
    z80pio pio;
    z80ctc ctc;
    kc85_video video;
    kc85_audio audio;
    kc85_exp exp;
    kc85_roms roms;
    ubyte io84;             // special KC85/4 io register
    ubyte io86;             // special KC85/4 io register

    /// debugging support
    z80dbg dbg;
    bool paused;

    /// constructor
    kc85();

    /// power-on the device
    void poweron(kc85_model m, kc85_caos os);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get the KC model
    kc85_model model() const;
    /// get the CAOS version
    kc85_caos caos() const;

    /// process one frame, up to absolute number of cycles
    void onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count);
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// handle keyboard input
    void handle_keyboard_input();

    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// update module/memory mapping
    void update_bank_switching();
    /// update the rom pointers
    void update_rom_pointers();

    kc85_model cur_model;
    kc85_caos cur_caos;
    bool on;
    bool cpu_ahead;             // cpu would have been ahead of max_cycle_count
    bool cpu_behind;            // cpu would have been behind of min_cycle_count
    uint64_t abs_cycle_count;   // total cycle count, reset at reboot
    uint32_t leftover_cycles;   // left-over cycles from last frame
    ubyte key_code;
    const ubyte* caos_c_ptr;
    int caos_c_size;
    const ubyte* caos_e_ptr;
    int caos_e_size;
};

//------------------------------------------------------------------------------
inline kc85::kc85():
io84(0),
io86(0),
paused(false),
cur_model(kc85_model::kc85_3),
cur_caos(kc85_caos::caos_3_1),
on(false),
cpu_ahead(false),
cpu_behind(false),
abs_cycle_count(0),
leftover_cycles(0),
key_code(0),
caos_c_ptr(nullptr),
caos_c_size(0),
caos_e_ptr(nullptr),
caos_e_size(0) {
    // empty
}

//------------------------------------------------------------------------------
inline void
kc85::poweron(kc85_model m, kc85_caos os) {
    YAKC_ASSERT(kc85_model::none != m);
    YAKC_ASSERT(!this->on);

    this->cur_model = m;
    this->cur_caos = os;
    this->on = true;
    this->leftover_cycles = 0;
    this->key_code = 0;
    this->io84 = 0;
    this->io86 = 0;

    // set operating system pointers
    this->update_rom_pointers();

    // initialize the clock, the 85/4 runs at 1.77 MHz, the others at 1.75 MHz
    this->clck.init((m == kc85_model::kc85_4) ? 1770 : 1750);

    // initialize hardware components
    this->pio.init();
    this->ctc.init();
    this->cpu.init(in_cb, out_cb, this);
    this->exp.init();
    this->video.init(m);
    this->audio.init(&this->ctc);

    // setup interrupt controller daisy chain (CTC has highest priority before PIO)
    this->pio.int_ctrl.connect_cpu(z80::irq, &this->cpu);
    for (int i = 0; i < z80ctc::num_channels; i++) {
        this->ctc.channels[i].int_ctrl.connect_cpu(z80::irq, &this->cpu);
    }
    this->cpu.connect_irq_device(&this->ctc.channels[0].int_ctrl);
    this->ctc.init_daisychain(&this->pio.int_ctrl);

    // fill RAM banks with noise (but not on KC85/4? at least the 4
    // doesn't have the random-color-pattern when switching it on)
    if (kc85_model::kc85_4 == m) {
        clear(this->ram, sizeof(this->ram));
    }
    else {
        fill_random(this->ram, sizeof(this->ram));
    }

    // connect CTC2 trigger to a 50Hz vertical-blank-timer,
    // this controls the foreground color blinking flag
    this->clck.config_timer(0, 50, z80ctc::ctrg2, &this->ctc);

    // connect a timer with the duration of one PAL line
    // (~64ns) to the video scanline decoder callback
    this->clck.config_timer(1, (uint32_t)(50.136*312), kc85_video::pal_line_cb, &this->video);

    // connect the CTC2 ZC/TO2 output line to the video decoder blink flag
    this->ctc.connect_zcto2(kc85_video::ctc_blink_cb, &this->video);

    // initial memory map
    this->cpu.out(0x88, 0x9f);

    // execution on power-on starts at 0xF000
    this->cpu.PC = 0xF000;
}

//------------------------------------------------------------------------------
inline void
kc85::poweroff() {
    YAKC_ASSERT(this->on);
    this->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
inline void
kc85::reset() {
    this->exp.reset();
    this->video.reset();
    this->audio.reset();
    this->ctc.reset();
    this->pio.reset();
    this->cpu.reset();
    this->io84 = 0;
    this->io86 = 0;
    this->leftover_cycles = 0;
    // execution after reset starts at 0xE000
    this->cpu.PC = 0xE000;
}

//------------------------------------------------------------------------------
inline kc85_model
kc85::model() const {
    return this->cur_model;
}

//------------------------------------------------------------------------------
inline kc85_caos
kc85::caos() const {
    return this->cur_caos;
}

//------------------------------------------------------------------------------
inline void
kc85::update_rom_pointers() {
    this->caos_c_ptr = nullptr;
    this->caos_c_size = 0;
    switch (this->cur_caos) {
        case kc85_caos::caos_hc900:
            this->caos_e_ptr  = this->roms.ptr(kc85_roms::hc900);
            this->caos_e_size = this->roms.size(kc85_roms::hc900);
            break;
        case kc85_caos::caos_2_2:
            this->caos_e_ptr  = this->roms.ptr(kc85_roms::caos22);
            this->caos_e_size = this->roms.size(kc85_roms::caos22);
            break;
        case kc85_caos::caos_3_1:
            this->caos_e_ptr  = this->roms.ptr(kc85_roms::caos31);
            this->caos_e_size = this->roms.size(kc85_roms::caos31);
            break;
        case kc85_caos::caos_3_4:
            this->caos_e_ptr  = this->roms.ptr(kc85_roms::caos34);
            this->caos_e_size = this->roms.size(kc85_roms::caos34);
            break;
        case kc85_caos::caos_4_1:
            this->caos_e_ptr  = this->roms.ptr(kc85_roms::caos41e);
            this->caos_e_size = this->roms.size(kc85_roms::caos41e);
            this->caos_c_ptr  = this->roms.ptr(kc85_roms::caos41c);
            this->caos_c_size = this->roms.size(kc85_roms::caos41c);
            break;
        case kc85_caos::caos_4_2:
            this->caos_e_ptr  = this->roms.ptr(kc85_roms::caos42e);
            this->caos_e_size = this->roms.size(kc85_roms::caos42e);
            this->caos_c_ptr  = this->roms.ptr(kc85_roms::caos42c);
            this->caos_c_size = this->roms.size(kc85_roms::caos42c);
            break;
        default:
            YAKC_ASSERT(false);
            break;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    YAKC_ASSERT(speed_multiplier > 0);
    this->cpu_ahead = false;
    this->cpu_behind = false;
    this->handle_keyboard_input();
    if (!this->paused) {
        // compute the end-cycle-count for the current frame
        const int64_t num_cycles = this->clck.cycles(micro_secs*speed_multiplier);
        uint64_t abs_end_cycles = this->abs_cycle_count + num_cycles;
        if (abs_end_cycles > max_cycle_count) {
            abs_end_cycles = max_cycle_count;
            this->cpu_ahead = true;
        }
        else if (abs_end_cycles < min_cycle_count) {
            abs_end_cycles = min_cycle_count;
            this->cpu_behind = true;
        }

//        int cycles_frame = this->leftover_cycles;
        while (this->abs_cycle_count < abs_end_cycles) {
            if (this->dbg.check_break(this->cpu)) {
                this->paused = true;
                break;
            }
            this->dbg.store_pc_history(this->cpu); // FIXME: only if debug window open?
            int cycles_step = this->cpu.step();
            cycles_step += this->cpu.handle_irq();
            this->clck.update(cycles_step);
            this->ctc.update_timers(cycles_step);
            this->audio.update_cycles(this->abs_cycle_count);
            this->abs_cycle_count += cycles_step;
        }
        // this->leftover_cycles = cycles_frame - num_cycles;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::put_key(ubyte ascii) {
    this->key_code = ascii;
}

//------------------------------------------------------------------------------
inline void
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
    if (!this->cpu.IFF1) {
        return;
    }

    // status bits
    static const ubyte timeout = (1<<3);
    static const ubyte keyready = (1<<0);
    static const ubyte repeat = (1<<4);
    static const ubyte short_repeat_count = 8;
    static const ubyte long_repeat_count = 60;

    auto& mem = this->cpu.mem;
    const uword ix = this->cpu.IX;
    if (0 == this->key_code) {
        // if keycode is 0, this basically means the CTC3 timeout was hit
        mem.a8(ix+0x8) |= timeout;      // set the CTC3 timeout bit
        mem.w8(ix+0xD, 0);              // clear current keycode
    }
    else {
        // a valid keycode has been received, clear the timeout bit
        mem.a8(ix+0x8) &= ~timeout;

        // check for key-repeat
        if (this->key_code != mem.r8(ix+0xD)) {
            // no key-repeat
            mem.w8(ix+0xD, this->key_code);     // write new keycode
            mem.a8(ix+0x8) &= ~repeat;          // clear the first-key-repeat bit
            mem.a8(ix+0x8) |= keyready;         // set the key-ready bit
            mem.w8(ix+0xA, 0);                  // clear the key-repeat counter
        }
        else {
            // handle key-repeat
            mem.a8(ix+0xA)++;                   // increment repeat-pause-counter
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
                    mem.a8(ix+0x8) |= repeat;
                }
            }
            // key-repeat triggered, just set the key-ready flag and reset repeat-count
            mem.a8(ix+0x8) |= keyready;
            mem.w8(ix+0xA, 0);
        }
    }
}

//------------------------------------------------------------------------------
inline void
kc85::out_cb(void* userdata, uword port, ubyte val) {
    kc85* self = (kc85*)userdata;
    switch (port & 0xFF) {
        case 0x80:
            if (self->exp.slot_exists(port>>8)) {
                self->exp.update_control_byte(port>>8, val);
                self->update_bank_switching();
            }
            break;
        case 0x84:
            if (kc85_model::kc85_4 == self->cur_model) {
                self->io84 = val;
                self->video.kc85_4_irm_control(val);
                self->update_bank_switching();
            }
            break;
        case 0x86:
            if (kc85_model::kc85_4 == self->cur_model) {
                self->io86 = val;
                self->update_bank_switching();
            }
            break;
        case 0x88:
            self->pio.write(z80pio::A, val);
            self->update_bank_switching();
            break;
        case 0x89:
            self->pio.write(z80pio::B, val);
            self->video.pio_blink_enable(0 != (val & PIO_B_BLINK_ENABLED));
            self->audio.update_volume(val & PIO_B_VOLUME_MASK);
            break;
        case 0x8A:
            self->pio.control(z80pio::A, val);
            break;
        case 0x8B:
            self->pio.control(z80pio::B, val);
            break;
        case 0x8C:
            self->ctc.write(z80ctc::CTC0, val);
            break;
        case 0x8D:
            self->ctc.write(z80ctc::CTC1, val);
            break;
        case 0x8E:
            self->ctc.write(z80ctc::CTC2, val);
            break;
        case 0x8F:
            self->ctc.write(z80ctc::CTC3, val);
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
inline ubyte
kc85::in_cb(void* userdata, uword port) {
    // NOTE: on KC85/4, the hardware doesn't provide a way to read-back
    // the additional IO ports at 0x84 and 0x86 (see KC85/4 service manual)
    kc85* self = (kc85*)userdata;
    switch (port & 0xFF) {
        case 0x80:
            return self->exp.module_type_in_slot(port>>8);
        case 0x88:
            return self->pio.read(z80pio::A);
        case 0x89:
            return self->pio.read(z80pio::B);
        case 0x8C:
            return self->ctc.read(z80ctc::CTC0);
        case 0x8D:
            return self->ctc.read(z80ctc::CTC1);
        case 0x8E:
            return self->ctc.read(z80ctc::CTC2);
        case 0x8F:
            return self->ctc.read(z80ctc::CTC3);
        default:
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::update_bank_switching() {
    this->cpu.mem.unmap_layer(0);
    const ubyte pio_a = this->pio.read(z80pio::A);
    const ubyte pio_b = this->pio.read(z80pio::B);

    if ((kc85_model::kc85_2 == this->cur_model) || (kc85_model::kc85_3 == this->cur_model)) {
        // ** KC85/3 or KC85/2 **

        // 16 KByte RAM at 0x0000 (write-protection not supported)
        if (pio_a & PIO_A_RAM) {
            this->cpu.mem.map(0, 0x0000, 0x4000, ram[0], true);
        }
        // 16 KByte video memory at 0x8000
        if (pio_a & PIO_A_IRM) {
            this->cpu.mem.map(0, 0x8000, 0x4000, this->video.irm[0], true);
        }
        // 8 KByte BASIC ROM at 0xC000 (only KC85/3)
        if (kc85_model::kc85_3 == this->cur_model) {
            if (pio_a & PIO_A_BASIC_ROM) {
                this->cpu.mem.map(0, 0xC000, 0x2000, dump_basic_c0, false);
            }
        }
        // 8 KByte CAOS ROM at 0xF000
        if (pio_a & PIO_A_CAOS_ROM) {
            this->cpu.mem.map(0, 0xE000, this->caos_e_size, (ubyte*)this->caos_e_ptr, false);
        }
    }
    else if (kc85_model::kc85_4 == this->cur_model) {
        // ** KC85/4 **

        // 16 KByte RAM at 0x0000 (write-protection not supported)
        if (pio_a & PIO_A_RAM) {
            this->cpu.mem.map(0, 0x0000, 0x4000, ram[0], true);
        }
        // 16 KByte RAM at 0x4000
        if (this->io86 & IO86_RAM4) {
            this->cpu.mem.map(0, 0x4000, 0x4000, ram[1], true); // this->io86 & IO86_RAM4_RO);
        }
        // 16 KByte RAM at 0x8000 (2 banks)
        if (pio_b & PIO_B_RAM8) {
            ubyte* ram8_ptr = (this->io84 & IO84_SEL_RAM8) ? ram[3] : ram[2];
            this->cpu.mem.map(0, 0x8000, 0x4000, ram8_ptr, true); // pio_b & PIO_B_RAM8_RO);
        }
        // IRM is 4 banks, 2 for pixels, 2 for color,
        // the area A800 to BFFF is always mapped to IRM0!
        if (pio_a & PIO_A_IRM) {
            int irm_index = (this->io84 & 6)>>1;
            ubyte* irm_ptr = this->video.irm[irm_index];
            // on the KC85, an access to IRM banks other than the
            // first is only possible for the first 10 KByte until
            // A800, memory access to the remaining 6 KBytes
            // (A800 to BFFF) is always forced to the first IRM bank
            // by the address decoder hardware (see KC85/4 service manual)
            this->cpu.mem.map(0, 0x8000, 0x2800, irm_ptr, true);

            // always force access to A800 and above to the first IRM bank
            this->cpu.mem.map(0, 0xA800, 0x1800, this->video.irm[0]+0x2800, true);
        }
        // 8 KByte BASIC ROM at 0xC000
        if (pio_a & PIO_A_BASIC_ROM) {
            this->cpu.mem.map(0, 0xC000, 0x2000, dump_basic_c0, false);
        }
        // 4 KByte CAOS ROM-C at 0xC000
        if (this->io86 & IO86_CAOS_ROM_C) {
            this->cpu.mem.map(0, 0xC000, this->caos_c_size, (ubyte*)this->caos_c_ptr, false);
        }
        // 8 KByte CAOS ROM-E at 0xE000
        if (pio_a & PIO_A_CAOS_ROM) {
            this->cpu.mem.map(0, 0xE000, this->caos_e_size, (ubyte*)this->caos_e_ptr, false);
        }
    }

    // map modules in base-device expansion slots
    this->exp.update_memory_mappings(this->cpu.mem);
}

} // namespace yakc

