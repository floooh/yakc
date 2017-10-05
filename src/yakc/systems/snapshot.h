#pragma once
//------------------------------------------------------------------------------
/**
    @class snapshot
    @brief helper functions for taking and applying machine state snapshots
*/
#include "yakc/yakc.h"

namespace YAKC {

class snapshot {
public:
    /// snapshot state
    #pragma pack(push,1)
    struct state_t {
        int magic;
        int version;

        // general emulator state
        struct emu_t {
            uint16_t model;
            uint16_t os;
        } emu;
        static_assert((sizeof(emu_t)&3)==0, "emu_t odd size!");

        // clock state
        struct clock_t {
            int base_freq_khz;
            struct timer_t {
                int period;
                int count;
                int value;
            } timers[4];
        } clock;
        static_assert((sizeof(clock_t)&3)==0, "clock_t odd size!");

        // KC85 system state
        struct kc_t {
            uint16_t model;
            uint8_t on;
            uint8_t caos;
            uint8_t io84;
            uint8_t io86;
            uint8_t pio_a;
            uint8_t pio_b;
            uint16_t cur_scanline;
            uint8_t irm_control;
            uint8_t pio_blink_flag;
            uint8_t ctc_blink_flag;
            uint8_t pad[3];
            struct channel_t {
                uint8_t ctc_mode;
                uint8_t ctc_constant;
            } chn[2];
            struct slot_t {
                uint8_t slot_addr;        // 0x08 or 0x0C
                uint8_t module_type;      // kc85_exp::module_type
                uint8_t control_byte;
                uint8_t padding;
            } slots[2];
        } kc;
        static_assert((sizeof(kc_t)&3)==0, "kc_t odd size!");

        // Z1013 system state
        struct z1013_t {
            uint16_t model;
            uint8_t on;
            uint8_t os;
            uint8_t kbd_column_nr_requested = 0;      // requested keyboard matrix column number (0..7)
            uint8_t kbd_8x8_requested = false;         // bit 4 in PIO-B written
            uint8_t pad[2];
            uint64_t next_kbd_column_bits = 0;
            uint64_t kbd_column_bits = 0;
        } z1013;
        static_assert((sizeof(z1013_t)&3)==0, "z1013_t odd size!");

        // Z9001 system state
        struct z9001_t {
            uint16_t model;
            uint8_t on;
            uint8_t os;
            uint8_t ctc0_mode;
            uint8_t kbd_column_mask;
            uint8_t kbd_line_mask;
            uint8_t blink_flipflop;
            uint8_t brd_color;
            uint8_t pad[3];
            uint32_t blink_counter;
            uint8_t ctc0_constant;
            uint8_t pad1[3];
            uint64_t key_mask;
        } z9001;
        static_assert((sizeof(z9001_t)&3)==0, "z9001_t odd size!");

        // cpu state
        struct cpu_t {
            uint16_t AF, BC, DE, HL, IX, IY, WZ;
            uint16_t AF_, BC_, DE_, HL_, WZ_;
            uint16_t SP, PC;
            uint8_t I, R, IM;
            uint8_t HALT, IFF1, IFF2;

            uint8_t INV;
            uint8_t int_active;
            uint8_t int_enable;
            uint8_t padding[3];
        } cpu;
        static_assert((sizeof(cpu_t)&3)==0, "cpu_t odd size!");

        // interrupt controller data
        struct intctrl_t {
            uint8_t enabled;
            uint8_t requested;
            uint8_t request_data;
            uint8_t pending;
        };

        // ctc state
        struct ctc_t {
            struct chn_t {
                int down_counter;
                uint8_t mode;
                uint8_t constant;
                uint8_t waiting_for_trigger;
                uint8_t interrupt_vector;
                intctrl_t intctrl;
            } chn[4];
        } ctc;
        static_assert((sizeof(ctc_t)&3)==0, "ctc_t odd size!");

        // pio state
        struct pio_t {
            z80pio::port_t port[z80pio::num_ports];
            uint8_t pad[2];
            intctrl_t intctrl;
        };
        static_assert((sizeof(pio_t)&3)==0, "pio_t odd size!");
        pio_t pio1;
        pio_t pio2;

        // system RAM banks
        uint8_t ram[8][0x4000];
        // optional slot 0x08 RAM banks
        uint8_t ram8[4][0x4000];
        // optional slot 0x0C RAM banks
        uint8_t ramC[4][0x4000];
    };
    static_assert((sizeof(state_t)&3)==0, "state odd size!");
    #pragma pack(pop)

    /// record a snapshot into a memory buffer
    static void take_snapshot(const yakc& emu, state_t& state);
    /// apply a snapshot from memory buffer
    static void apply_snapshot(const state_t& state, yakc& emu);
    /// test if state_t contains a valid snapshot
    static bool is_snapshot(const state_t& state);

    /// write generic emu state
    static void write_emu_state(const yakc& emu, state_t& state);
    /// apply generic emu state
    static void apply_emu_state(const state_t& state, yakc& emu);
    /// write the clock state
    static void write_clock_state(const yakc& emu, state_t& state);
    /// apply clock state
    static void apply_clock_state(const state_t& state, yakc& emu);
    /// write the KC system state
    static void write_kc_state(const yakc& emu, state_t& state);
    /// apply toplevel KC state
    static void apply_kc_state(const state_t& state, yakc& emu);
    /// write the Z1013 system state
    static void write_z1013_state(const yakc& emu, state_t& state);
    /// apply Z1013 system state
    static void apply_z1013_state(const state_t& state, yakc& emu);
    /// write the z9001 system state
    static void write_z9001_state(const yakc& emu, state_t& state);
    /// apply z9001 system state
    static void apply_z9001_state(const state_t& state, yakc& emu);
    /// write the cpu state
    static void write_cpu_state(const yakc& emu, state_t& state);
    /// apply cpu state
    static void apply_cpu_state(const state_t& state, yakc& emu);
    /// write interrupt controller state
    static void write_intctrl_state(const z80int& src, state_t::intctrl_t& dst);
    /// apply interrupt controller state
    static void apply_intctrl_state(const state_t::intctrl_t& src, z80int& dst);
    /// write the ctc state
    static void write_ctc_state(const yakc& emu, state_t& state);
    /// apply ctc state
    static void apply_ctc_state(const state_t& state, yakc& emu);
    /// write the pio state
    static void write_pio_state(const yakc& emu, state_t& state);
    /// apply pio state
    static void apply_pio_state(const state_t& state, yakc& emu);
    /// write memory state
    static void write_memory_state(const yakc& emu, state_t& state);
    /// apply memory state
    static void apply_memory_state(const state_t& state, yakc& emu);
};

} // namespace YAKC
