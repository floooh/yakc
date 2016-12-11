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

        // clock state
        struct clock_t {
            int base_freq_khz;
            struct timer_t {
                int freq_hz;
                int count;
                int value;
            } timers[4];
        } clock;
        static_assert((sizeof(clock_t)&3)==0, "clock_t odd size!");

        // central KC85 state
        struct kc_t {
            ubyte model;
            ubyte caos;
            ubyte io84;
            ubyte io86;
            ubyte pio_a;
            ubyte pio_b;
            ubyte pad[2];
        } kc;
        static_assert((sizeof(kc_t)&3)==0, "kc_t odd size!");

        // cpu state
        struct cpu_t {
            uword AF, BC, DE, HL, IX, IY, WZ;
            uword AF_, BC_, DE_, HL_, WZ_;
            uword SP, PC;
            ubyte I, R, IM;
            ubyte HALT, IFF1, IFF2;

            ubyte INV;
            ubyte irq_received;
            ubyte enable_interrupt;
            ubyte padding[3];
        } cpu;
        static_assert((sizeof(cpu_t)&3)==0, "cpu_t odd size!");

        // interrupt controller data
        struct intctrl_t {
            ubyte enabled;
            ubyte requested;
            ubyte request_data;
            ubyte pending;
        };

        // ctc state
        struct ctc_t {
            struct chn_t {
                int down_counter;
                ubyte mode;
                ubyte constant;
                ubyte waiting_for_trigger;
                ubyte interrupt_vector;
                intctrl_t intctrl;
            } chn[4];
        } ctc;
        static_assert((sizeof(ctc_t)&3)==0, "ctc_t odd size!");

        // pio state
        struct pio_t {
            z80pio::port_t port[z80pio::num_ports];
            ubyte pad[2];
            intctrl_t intctrl;
        };
        static_assert((sizeof(pio_t)&3)==0, "pio_t odd size!");
        pio_t pio1;
        pio_t pio2;

        // video state
        struct video_t {
            uword cur_pal_line;
            ubyte irm_control;
            ubyte pio_blink_flag;
            ubyte ctc_blink_flag;
            ubyte padding[3];
        } video;
        static_assert((sizeof(video_t)&3)==0, "video_t odd size!");

        // audio state
        struct audio_t {
            int volume;
            struct channel_t {
                ubyte ctc_mode;
                ubyte ctc_constant;
            } chn[2];
        } audio;
        static_assert((sizeof(audio_t)&3)==0, "audio_t odd size!");

        // expansion module system
        struct exp_t {
            struct slot_t {
                ubyte slot_addr;        // 0x08 or 0x0C
                ubyte module_type;      // kc85_exp::module_type
                ubyte control_byte;
                ubyte padding;
            } slots[2];
        } exp;
        static_assert((sizeof(exp_t)&3)==0, "exp_t odd size!");

        // system RAM banks
        ubyte ram[4][0x4000];
        // IRM RAM banks
        ubyte irm[4][0x4000];
        // optional slot 0x08 RAM banks
        ubyte ram8[4][0x4000];
        // optional slot 0x0C RAM banks
        ubyte ramC[4][0x4000];
    };
    static_assert((sizeof(state_t)&3)==0, "state odd size!");
    #pragma pack(pop)

    /// record a snapshot into a memory buffer
    static void take_snapshot(const yakc& emu, state_t& state);
    /// apply a snapshot from memory buffer
    static void apply_snapshot(const state_t& state, yakc& emu);
    /// test if state_t contains a valid snapshot
    static bool is_snapshot(const state_t& state);

    /// write the clock state
    static void write_clock_state(const yakc& emu, state_t& state);
    /// apply clock state
    static void apply_clock_state(const state_t& state, yakc& emu);
    /// write the toplevel KC state
    static void write_kc_state(const yakc& emu, state_t& state);
    /// apply toplevel KC state
    static void apply_kc_state(const state_t& state, yakc& emu);
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
    /// write the video state
    static void write_video_state(const yakc& emu, state_t& state);
    /// apply video state
    static void apply_video_state(const state_t& state, yakc& emu);
    /// write the audio state
    static void write_audio_state(const yakc& emu, state_t& state);
    /// apply audio state
    static void apply_audio_state(const state_t& state, yakc& emu);
    /// write the expansion system state
    static void write_exp_state(const yakc& emu, state_t& state);
    /// apply expansion system state
    static void apply_exp_state(const state_t& state, yakc& emu);
    /// write memory state
    static void write_memory_state(const yakc& emu, state_t& state);
    /// apply memory state
    static void apply_memory_state(const state_t& state, yakc& emu);
};

} // namespace YAKC
