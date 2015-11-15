#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85
    @brief wrapper class for the entire KC85/3 or KC85/4 system
*/
#include "yakc_core/z80.h"
#include "yakc_core/z80pio.h"
#include "yakc_core/z80ctc.h"
#include "yakc_roms/roms.h"

namespace yakc {

class kc85 {
public:
    /// ram bank 0 (0x0000..0x3FFF)
    ubyte ram0[0x4000];
    /// ram bank 1 (0x4000..0x7FFF), optional in KC85/3
    ubyte ram1[0x4000];
    /// vidmem bank 0 (0x8000..0xBFFF)
    ubyte irm0[0x4000];

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

    /// supported KC types
    enum class kc_model {
        kc85_3,
        kc85_4,
        none
    };

    /// the Z80 CPU
    z80 cpu;
    /// the Z80 PIO
    z80pio pio;
    /// the Z80 CTC
    z80ctc ctc;
    /// breakpoint is enabled?
    bool breakpoint_enabled;
    /// breakpoint address
    uword breakpoint_address;
    /// currently paused?
    bool paused;

    /// expansion modules
    static const int max_num_module_slots = 6;
    struct module_desc {
        const char* name = "NO MODULE"; // string is not owned
        const char* help = "no help";   // string is not owned
        ubyte* ptr = nullptr;           // if this is 0, and size > 0, backing memory will be allocated
        ubyte type = 0xFF;              // module type (http://www.mpm-kc85.de/html/ModulListe.htm)
        bool writable = false;          // keep this false for ROM modules
        uword size = 0x0000;            // size of mapped memory
    };
    struct module_slot {
        module_desc desc;
        ubyte slot_addr = 0x00;
        bool allocated = false;
        uword addr = 0x0000;
        ubyte control_byte = 0x00;
    } modules[max_num_module_slots];

    /// constructor
    kc85();

    /// power-on the device
    void switchon(kc_model m, ubyte* caos_rom, uword caos_rom_size);
    /// power-off the device
    void switchoff();
    /// reset the device
    void reset();
    /// return true if device is switched on
    bool ison() const;
    /// get the KC model
    kc_model model() const;
    /// get pointer to currently mapped rom
    ubyte* caos_rom() const;
    /// get size of currently mapped rom
    uword caos_rom_size() const;

    /// convert a slot address to a memory layer
    static int slot_to_memory_layer(ubyte slot_addr);
    /// insert module into slot, replaces previous
    void insert_module(ubyte slot_addr, const module_desc& mod);
    /// test if a module is attached to slot
    bool module_attached(ubyte slot_addr) const;
    /// get module slot state
    const module_slot& get_module_slot(ubyte slot_addr) const;

    /// process one frame
    void onframe(int micro_secs);
    /// do one step
    void step();
    /// do a debug-step (executes until PC changes)
    void debug_step();
    /// get current video blink state
    bool blink_state() const;
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// handle keyboard input
    void handle_keyboard_input();

private:
    /// attach an expansion module
    void attach_module(ubyte slot_addr, const module_desc& mod);
    /// remove an expansion module
    void remove_module(ubyte slot_addr);
    /// map or unmap a module based on control-byte
    void update_module(ubyte slot_addr);
    /// find a module slot index, -1 if not found
    int find_module_by_slot(ubyte slot_addr) const;
    /// find highest module slot with active module mapped to address
    int find_active_module_by_addr(uword addr) const;
    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// fill memory region with noise
    static void fill_noise(void* ptr, int num_bytes);
    /// update module/memory mapping
    void update_bank_switching();
    /// CTC pulse callback
    static void ctc_pulse_callback(void* userdata, z80ctc::channel c);

    kc_model cur_model;
    bool on;
    ubyte key_code;
    ubyte* cur_caos_rom;
    uword cur_caos_rom_size;
    int vsync_counter;      // 50Hz frame reset counter
    bool video_blink_flag;  // current blinking flag triggered by CTC channel 2
};

//------------------------------------------------------------------------------
inline kc85::kc85():
breakpoint_enabled(false),
breakpoint_address(0x0000),
paused(false),
cur_model(kc_model::kc85_3),
key_code(0),
cur_caos_rom(rom_caos31),
cur_caos_rom_size(sizeof(rom_caos31)),
vsync_counter(0),
video_blink_flag(false) {
    // configure module slots
    this->modules[0].slot_addr = 0x08;  // base device right module slot
    this->modules[1].slot_addr = 0x0C;  // base device left module slot
}

//------------------------------------------------------------------------------
inline void
kc85::fill_noise(void* ptr, int num_bytes) {
    YAKC_ASSERT((num_bytes & 0x03) == 0);
    unsigned int* uptr = (unsigned int*)ptr;
    int num_uints = num_bytes>>2;
    for (int i = 0; i < num_uints; i++) {
        *uptr++ = YAKC_RAND();
    }
}

//------------------------------------------------------------------------------
inline void
kc85::switchon(kc_model m, ubyte* caos_rom, uword caos_rom_size) {
    YAKC_ASSERT(kc_model::none != m);
    YAKC_ASSERT(!this->on);
    YAKC_ASSERT(0x2000 == caos_rom_size);
    YAKC_ASSERT(0x2000 == sizeof(rom_basic_c0));

    this->cur_model = m;
    this->cur_caos_rom = caos_rom;
    this->cur_caos_rom_size = caos_rom_size;
    this->on = true;
    this->key_code = 0;
    this->vsync_counter = 0;

    this->pio.init();
    this->ctc.init(ctc_pulse_callback, this);
    this->cpu.reset();
    if (kc_model::kc85_3 == m) {
        // fill RAM banks with noise
        fill_noise(this->ram0, sizeof(this->ram0));
        fill_noise(this->irm0, sizeof(this->irm0));

        // initial memory map
        this->cpu.set_inout_handlers(in_cb, out_cb, this);
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
    this->ctc.reset();
    this->pio.reset();
    this->cpu.reset();
    // execution after reset starts at 0xE000
    this->cpu.state.PC = 0xE000;
}

//------------------------------------------------------------------------------
inline int
kc85::find_module_by_slot(ubyte slot_addr) const {
    for (int i = 0; i < max_num_module_slots; i++) {
        if (this->modules[i].slot_addr == slot_addr) {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
inline int
kc85::find_active_module_by_addr(uword addr) const {
    for (int i = 0; i < max_num_module_slots; i++) {
        const auto& mod = this->modules[i];
        if ((mod.slot_addr != 0x00) && (mod.desc.type != 0xFF) && ((mod.control_byte & 0xF0) == (addr>>8))) {
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
inline int
kc85::slot_to_memory_layer(ubyte slot_addr) {
    switch (slot_addr) {
        case 0x08:  return 1;
        case 0x0C:  return 2;
        // FIXME: add more expansion slots
        default:
            // invalid slot address
            YAKC_ASSERT(false);
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::insert_module(ubyte slot_addr, const module_desc& desc) {
    if (this->module_attached(slot_addr)) {
        this->remove_module(slot_addr);
    }
    this->attach_module(slot_addr, desc);
}

//------------------------------------------------------------------------------
inline void
kc85::attach_module(ubyte slot_addr, const module_desc& desc) {
    YAKC_ASSERT(!this->module_attached(slot_addr));
    const int slot_index = this->find_module_by_slot(slot_addr);
    YAKC_ASSERT((slot_index >= 0) && (slot_index < max_num_module_slots));
    auto& mod = this->modules[slot_index];
    mod.desc = desc;
    if ((mod.desc.size > 0) && (nullptr == mod.desc.ptr)) {
        // allocate backing memory
        mod.allocated = true;
        mod.desc.ptr = (ubyte*) YAKC_MALLOC(mod.desc.size);
        YAKC_MEMSET(mod.desc.ptr, 0, mod.desc.size);
    }
}

//------------------------------------------------------------------------------
inline void
kc85::remove_module(ubyte slot_addr) {
    YAKC_ASSERT(this->module_attached(slot_addr));
    const int slot_index = this->find_module_by_slot(slot_addr);
    YAKC_ASSERT((slot_index >= 0) && (slot_index < max_num_module_slots));
    auto& mod = this->modules[slot_index];
    // if module has memory, unmap it
    if (mod.desc.size && mod.desc.ptr) {
        cpu.mem.unmap_layer(slot_to_memory_layer(slot_addr));
    }
    // if memory was allocated for the module, free it
    if (mod.allocated) {
        YAKC_ASSERT(mod.desc.ptr);
        YAKC_FREE(mod.desc.ptr);
        mod.allocated = false;
    }
    mod.desc = module_desc();
    mod.addr = 0x0000;
}

//------------------------------------------------------------------------------
inline bool
kc85::module_attached(ubyte slot_addr) const {
    const int slot_index = this->find_module_by_slot(slot_addr);
    if (slot_index >= 0) {
        return this->modules[slot_index].desc.type != 0xFF;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
inline const kc85::module_slot&
kc85::get_module_slot(ubyte slot_addr) const {
    const int slot_index = this->find_module_by_slot(slot_addr);
    YAKC_ASSERT((slot_index >= 0) && (slot_index < max_num_module_slots));
    return this->modules[slot_index];
}

//------------------------------------------------------------------------------
inline void
kc85::update_module(ubyte slot_addr) {
    // private method is only called from update_bank_switching()
    const int slot_index = this->find_module_by_slot(slot_addr);
    auto& mod = this->modules[slot_index];
    if (0xFF != mod.desc.type) {
        const int memory_layer_index = slot_to_memory_layer(slot_addr);
        this->cpu.mem.unmap_layer(memory_layer_index);

        // compute module start address from control-byte
        mod.addr = (mod.control_byte & 0xF0)<<8;
        YAKC_ASSERT((mod.addr & (mod.desc.size-1)) == 0);
        if (mod.desc.size && mod.desc.ptr) {
            if (mod.control_byte & 0x01) {
                // activate the module
                bool writable = (mod.control_byte & 0x02) && mod.desc.writable;
                this->cpu.mem.map(memory_layer_index, mod.addr, mod.desc.size, mod.desc.ptr, writable);
            }
            else {
                // deactivate the module
                this->cpu.mem.unmap_layer(memory_layer_index);
            }
        }
    }
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
inline kc85::kc_model
kc85::model() const {
    return this->cur_model;
}

//------------------------------------------------------------------------------
inline void
kc85::onframe(int micro_secs) {
    // at 1.75 MHz:
    // per micro-second, 1750000 / 1000000 T-states are executed
    // thus: num T-states to execute is (micro_secs * 17500000) / 1000000)
    // or: (micro_secs * 175) / 100
    this->handle_keyboard_input();
    if (!this->paused) {
        // KC85/3 is 1.75MHz, KC85/4 is 1.77MHz
        const int khz = this->cur_model == kc_model::kc85_3 ? 1750 : 1770;
        const unsigned int num_cycles = (micro_secs * khz) / 1000;

        // step CPU and CTC
        unsigned int cycles_executed = 0;
        while (cycles_executed < num_cycles) {
            if (this->breakpoint_enabled) {
                if (cpu.state.PC == this->breakpoint_address) {
                    this->paused = true;
                    break;
                }
            }
            unsigned int cycles_opcode = cpu.step();
            ctc.update(cycles_opcode);
            // FIXME: CTC channels 0 and 1 seem to be triggered per video scanline

            // CTC2 and CTC3 are triggered per 50 Hz video frame
            if (this->vsync_counter <= 0) {
                this->ctc.trigger(z80ctc::CTC2);
                this->ctc.trigger(z80ctc::CTC3);
                this->vsync_counter = (khz * 1000) / 50;
            }
            this->vsync_counter -= cycles_opcode;
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
        cpu.step();
    }
    while ((pc == cpu.state.PC) && !cpu.state.INV);
}

//------------------------------------------------------------------------------
inline void
kc85::step() {
    this->cpu.step();
}

//------------------------------------------------------------------------------
inline bool
kc85::blink_state() const {
    if (this->pio.read(z80pio::B) & PIO_B_BLINK_ENABLED) {
        return this->video_blink_flag;
    }
    else {
        return true;
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
    // NOTE: this is a shortcut and doesn't emulate the hardware's
    // tricky serial keyboard input. Instead we're directly poking
    // the ASCII value into the right memory location

    // don't do anything if interrupts disabled, IX might point
    // to the wrong base address!
    if (!this->cpu.state.IFF1) {
        return;
    }

    // don't do anything if previous key hasn't been read yet
    // NOTE: IX is usually set to 0x01F0
    const uword ix = this->cpu.state.IX;
    if (0 != (this->cpu.mem.r8(ix+0x08) & 1)) {
        return;
    }

    // write key to special OS locations
    if (0 == this->key_code) {
        // clear ascii code location
        this->cpu.mem.w8(ix+0x0D, 0);
        // clear repeat count
        this->cpu.mem.w8(ix+0x0A, 0);
    }
    else {
        bool overwrite = true;
        // FIXME: handle repeat
        if (this->cpu.mem.r8(ix+0x0D) == this->key_code) {
            // FIXME!
        }
        if (overwrite) {
            this->cpu.mem.w8(ix+0x0D, this->key_code);
            this->cpu.mem.w8(ix+0x08, this->cpu.mem.r8(ix+0x08)|1);
        }
        this->key_code = 0;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::out_cb(void* userdata, uword port, ubyte val) {
    kc85* self = (kc85*)userdata;
    switch (port & 0xFF) {
        case 0x80:
            {
                int slot_index = self->find_module_by_slot(port>>8);
                if (slot_index >= 0) {
                    self->modules[slot_index].control_byte = val;
                    self->update_bank_switching();
                }
            }
            break;
        case 0x88:
            self->pio.write(z80pio::A, val);
            self->update_bank_switching();
            break;
        case 0x89:
            self->pio.write(z80pio::B, val);
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
            YAKC_ASSERT(false);
            break;
    }
}

//------------------------------------------------------------------------------
inline ubyte
kc85::in_cb(void* userdata, uword port) {
    kc85* self = (kc85*)userdata;
    switch (port & 0xFF) {
        case 0x80:
            {
                int slot_index = self->find_module_by_slot(port>>8);
                if (slot_index >= 0) {
                    return self->modules[slot_index].desc.type;
                }
                else {
                    return 0xFF;
                }
            }
            break;
        case 0x88:
            return self->pio.read(z80pio::A);
        case 0x89:
            return self->pio.read(z80pio::B);
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
    cpu.mem.unmap_layer(0);
    // 16 KByte RAM
    if (pio_a & PIO_A_RAM) {
        cpu.mem.map(0, 0x0000, 0x4000, ram0, true);
    }
    // 16 KByte video memory
    if (pio_a & PIO_A_IRM) {
        cpu.mem.map(0, 0x8000, 0x4000, irm0, true);
    }
    // 8 KByte BASIC ROM
    if (pio_a & PIO_A_BASIC_ROM) {
        cpu.mem.map(0, 0xC000, 0x2000, rom_basic_c0, false);
    }
    // 8 KByte CAOS ROM
    if (pio_a & PIO_A_CAOS_ROM) {
        cpu.mem.map(0, 0xE000, 0x2000, caos_rom(), false);
    }

    // map modules in base-device expansion slots
    this->update_module(0x08);
    this->update_module(0x0C);
}

//------------------------------------------------------------------------------
inline void
kc85::ctc_pulse_callback(void* userdata, z80ctc::channel c) {
    YAKC_ASSERT(userdata);
    kc85* self = (kc85*)userdata;

    // CTC channel 2 controls the forward color blink state
    if (z80ctc::CTC2 == c) {
        self->video_blink_flag = !self->video_blink_flag;
    }
    else {
        YAKC_PRINTF("FIXME: PULSE CALLBACK FOR CTC CHANNEL %d\n", c);
    }
}

} // namespace yakc

