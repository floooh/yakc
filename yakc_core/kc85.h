#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85
    @brief wrapper class for the entire KC85/3 or KC85/4 system
*/
#include "yakc_core/clock.h"
#include "yakc_core/z80.h"
#include "yakc_core/z80pio.h"
#include "yakc_core/z80ctc.h"
#include "yakc_roms/roms.h"
#include "yakc_core/kc85_video.h"
#include "yakc_core/kc85_expansion.h"

namespace yakc {

class kc85 {
public:
    /// ram bank 0 (0x0000..0x3FFF)
    ubyte ram0[0x4000];

    /// PIO bits
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
        PIO_B_BLINK_ENABLED = (1<<7),
    };

    /// hardware components
    clock clck;
    z80 cpu;
    z80pio pio;
    z80ctc ctc;
    kc85_video video;
    kc85_expansion exp;

    /// debugging support
    bool breakpoint_enabled;
    uword breakpoint_address;
    bool paused;

    /// constructor
    kc85();

    /// power-on the device
    void switchon(kc85_model m, ubyte* caos_rom, uword caos_rom_size);
    /// power-off the device
    void switchoff();
    /// reset the device
    void reset();
    /// return true if device is switched on
    bool ison() const;
    /// get the KC model
    kc85_model model() const;
    /// get pointer to currently mapped rom
    ubyte* caos_rom() const;
    /// get size of currently mapped rom
    uword caos_rom_size() const;

    /// process one frame
    void onframe(int speed_multiplier, int micro_secs);
    /// do a debug-step (executes until PC changes)
    void debug_step();
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// handle keyboard input
    void handle_keyboard_input();

private:
    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// update module/memory mapping
    void update_bank_switching();

    kc85_model cur_model;
    bool on;
    ubyte key_code;
    ubyte* cur_caos_rom;
    uword cur_caos_rom_size;
};

//------------------------------------------------------------------------------
inline kc85::kc85():
breakpoint_enabled(false),
breakpoint_address(0x0000),
paused(false),
cur_model(kc85_model::kc85_3),
key_code(0),
cur_caos_rom(rom_caos31),
cur_caos_rom_size(sizeof(rom_caos31)) {
    // empty
}

//------------------------------------------------------------------------------
inline void
kc85::switchon(kc85_model m, ubyte* caos_rom, uword caos_rom_size) {
    YAKC_ASSERT(kc85_model::none != m);
    YAKC_ASSERT(!this->on);
    YAKC_ASSERT(0x2000 == caos_rom_size);
    YAKC_ASSERT(0x2000 == sizeof(rom_basic_c0));

    this->cur_model = m;
    this->cur_caos_rom = caos_rom;
    this->cur_caos_rom_size = caos_rom_size;
    this->on = true;
    this->key_code = 0;

    // initialize hardware components
    this->exp.init();
    this->video.init(m);
    this->pio.init();
    this->ctc.init();
    this->cpu.init(in_cb, out_cb, this);

    // setup interrupt controller daisy chain (CTC has highest priority before PIO)
    this->pio.int_ctrl.connect_cpu(z80::irq, &this->cpu);
    this->ctc.int_ctrl.connect_cpu(z80::irq, &this->cpu);
    this->cpu.connect_irq_device(&this->ctc.int_ctrl);
    this->ctc.int_ctrl.connect_irq_device(&this->pio.int_ctrl);

    if (kc85_model::kc85_3 == m) {

        // initialize clock to 1.75 MHz
        this->clck.init(1750);

        // connect CTC2 trigger to a 50Hz vertical-blank-timer,
        // this controls the foreground color blinking flag
        this->clck.config_timer(0, 50, z80ctc::ctrg2, &this->ctc);

        // connect the CTC2 ZC/TO2 output line to the video decoder blink flag
        this->ctc.connect_zcto2(kc85_video::ctc_blink_cb, &this->video);

        // fill RAM banks with noise
        fill_random(this->ram0, sizeof(this->ram0));

        // initial memory map
        this->cpu.out(0x88, 0x9f);
    }

    // execution on switch-on starts at 0xF000
    this->cpu.state.PC = 0xF000;
}

//------------------------------------------------------------------------------
inline void
kc85::switchoff() {
    YAKC_ASSERT(this->on);
    this->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
inline void
kc85::reset() {
    this->exp.reset();
    this->video.reset();
    this->ctc.reset();
    this->pio.reset();
    this->cpu.reset();
    // execution after reset starts at 0xE000
    this->cpu.state.PC = 0xE000;
}

//------------------------------------------------------------------------------
inline ubyte*
kc85::caos_rom() const {
    return this->cur_caos_rom;
}

//------------------------------------------------------------------------------
inline uword
kc85::caos_rom_size() const {
    return this->cur_caos_rom_size;
}

//------------------------------------------------------------------------------
inline bool
kc85::ison() const {
    return this->on;
}

//------------------------------------------------------------------------------
inline kc85_model
kc85::model() const {
    return this->cur_model;
}

//------------------------------------------------------------------------------
inline void
kc85::onframe(int speed_multiplier, int micro_secs) {
    YAKC_ASSERT(speed_multiplier > 0);
    this->handle_keyboard_input();
    if (!this->paused) {
        const unsigned int num_cycles = this->clck.cycles(micro_secs*speed_multiplier);
        unsigned int cycles_executed = 0;
        while (cycles_executed < num_cycles) {
            if (this->breakpoint_enabled) {
                if (this->cpu.state.PC == this->breakpoint_address) {
                    this->paused = true;
                    break;
                }
            }
            unsigned int cycles_opcode = this->cpu.step();
            this->clck.update(cycles_opcode);
            this->ctc.update_timers(cycles_opcode);
            cycles_opcode += this->cpu.handle_irq();
            cycles_executed += cycles_opcode;
        }
    }
}

//------------------------------------------------------------------------------
inline void
kc85::debug_step() {
    uword pc;
    do {
        pc = this->cpu.state.PC;
        this->cpu.step();
    }
    while ((pc == this->cpu.state.PC) && !this->cpu.state.INV);
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
    // https://github.com/floooh/yakc/blob/master/scribble/kc85_3_kbdint.md )
    //
    // we ignore the whole tricky serial decoding and patch the
    // keycode directly into the right memory locations.

    // don't do anything if interrupts disabled, IX might point
    // to the wrong base address!
    if (!this->cpu.state.IFF1) {
        return;
    }

    // status bits
    static const ubyte timeout = (1<<3);
    static const ubyte keyready = (1<<0);
    static const ubyte repeat = (1<<4);
    static const ubyte short_repeat_count = 8;
    static const ubyte long_repeat_count = 60;

    auto& mem = this->cpu.mem;
    const uword ix = this->cpu.state.IX;
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
        case 0x88:
            self->pio.write(z80pio::A, val);
            self->update_bank_switching();
            break;
        case 0x89:
            self->pio.write(z80pio::B, val);
            self->video.pio_blink_enable(val & PIO_B_BLINK_ENABLED);
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
            // unknown
            //YAKC_ASSERT(false);
            break;
    }
}

//------------------------------------------------------------------------------
inline ubyte
kc85::in_cb(void* userdata, uword port) {
    kc85* self = (kc85*)userdata;
    switch (port & 0xFF) {
        case 0x80:
            return self->exp.module_type(port>>8);
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
            // unknown
            YAKC_ASSERT(false);
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::update_bank_switching() {
    const ubyte pio_a = this->pio.read(z80pio::A);

    // first map the base-device memory banks
    this->cpu.mem.unmap_layer(0);
    // 16 KByte RAM
    if (pio_a & PIO_A_RAM) {
        this->cpu.mem.map(0, 0x0000, 0x4000, ram0, true);
    }
    // 16 KByte video memory
    if (pio_a & PIO_A_IRM) {
        this->cpu.mem.map(0, 0x8000, 0x4000, this->video.irm0, true);
    }
    // 8 KByte BASIC ROM
    if (pio_a & PIO_A_BASIC_ROM) {
        this->cpu.mem.map(0, 0xC000, 0x2000, rom_basic_c0, false);
    }
    // 8 KByte CAOS ROM
    if (pio_a & PIO_A_CAOS_ROM) {
        this->cpu.mem.map(0, 0xE000, 0x2000, caos_rom(), false);
    }

    // map modules in base-device expansion slots
    this->exp.update_memory_mappings(this->cpu.mem);
}

} // namespace yakc

